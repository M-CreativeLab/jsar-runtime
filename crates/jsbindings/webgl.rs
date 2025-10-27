use glsl_lang::ast;
use glsl_lang::visitor::{Host, HostMut, Visit, Visitor, VisitorMut};
use serde::{Deserialize, Serialize};
use std::path::Path;

use crate::glsl_transpiler;

struct MyGLSLPatcher {}

impl MyGLSLPatcher {
  fn create_model_view_matrix_expr(&self) -> ast::Expr {
    let new_lhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData(ast::SmolStr::new_inline("viewMatrix")).into())
        .into();
    let new_rhs: ast::Expr =
      ast::ExprData::Variable(ast::IdentifierData(ast::SmolStr::new_inline("modelMatrix")).into())
        .into();
    let new_binary_expr: ast::Expr = ast::ExprData::Binary(
      ast::BinaryOpData::Mult.into(),
      Box::new(new_lhs),
      Box::new(new_rhs),
    )
    .into();
    new_binary_expr
  }

  fn handle_expr(&self, expr: &mut ast::Expr) -> bool {
    match &mut expr.content {
      ast::ExprData::Variable(identifier) => {
        if identifier.content.0 == "modelViewMatrix" {
          *expr = self.create_model_view_matrix_expr();
          true
        } else {
          false
        }
      }
      ast::ExprData::Unary(_, operand) => self.handle_expr(operand),
      ast::ExprData::Binary(_, lhs, rhs) => {
        let r1 = self.handle_expr(lhs);
        let r2 = self.handle_expr(rhs);
        r1 || r2
      }
      ast::ExprData::Assignment(_, _, rhs) => self.handle_expr(rhs),
      ast::ExprData::FunCall(_, args) => {
        let mut changed = false;
        for arg in args {
          changed |= self.handle_expr(arg);
        }
        changed
      }
      _ => false,
    }
  }
}

impl VisitorMut for MyGLSLPatcher {
  fn visit_expr(&mut self, expr: &mut ast::Expr) -> Visit {
    if self.handle_expr(expr) {
      Visit::Parent
    } else {
      Visit::Children
    }
  }
}

fn patch_glsl_source_from_str(s: &str) -> String {
  use glsl_lang::{
    ast::TranslationUnit, lexer::full::fs::PreprocessorExt, parse::IntoParseBuilderExt,
  };

  let mut processor = glsl_lang_pp::processor::fs::StdProcessor::new();
  let mut tu: TranslationUnit = processor
    .open_source(s, Path::new("."))
    .builder()
    .parse()
    .map(|(mut tu, _, iter)| {
      iter.into_directives().inject(&mut tu);
      tu
    })
    .expect(format!("Failed to parse GLSL source: \n{}\n", s).as_str());

  let mut my_glsl_patcher = MyGLSLPatcher {};
  tu.visit_mut(&mut my_glsl_patcher);

  {
    /*
     * This reorders the preprocessor directives in the GLSL source code.
     *
     * 1. Move the #version directive to the top.
     * 2. Move the #extension directives to the top after the #version directive if exists.
     */
    let mut versions_list = Vec::new();
    let mut extensions_list = Vec::new();
    tu.0.retain(|decl| match &decl.content {
      ast::ExternalDeclarationData::Preprocessor(processor) => match processor.content {
        ast::PreprocessorData::Version(_) => {
          versions_list.push(decl.clone());
          false
        }
        ast::PreprocessorData::Extension(_) => {
          extensions_list.push(decl.clone());
          false
        }
        _ => true,
      },
      _ => true,
    });
    tu.0.splice(0..0, extensions_list);
    tu.0.splice(0..0, versions_list);
  }

  let mut s = String::new();
  glsl_transpiler::glsl::show_translation_unit(
    &mut s,
    &tu,
    glsl_transpiler::glsl::FormattingState::default(),
  )
  .expect("Failed to show GLSL");
  s
}

/// Visitor to collect all variable references in the shader
struct ReferenceCollector {
  referenced_names: std::collections::HashSet<String>,
  /// Track if gl_InstanceID or gl_VertexID are used
  uses_gl_instance_id: bool,
  uses_gl_vertex_id: bool,
  /// Track local variable assignments: local_var -> source_var
  /// For example, when we see "local_foo = foo[0]", we store "local_foo" -> "foo[0]"
  variable_aliases: std::collections::HashMap<String, Vec<String>>,
}

impl ReferenceCollector {
  fn new() -> Self {
    Self {
      referenced_names: std::collections::HashSet::new(),
      uses_gl_instance_id: false,
      uses_gl_vertex_id: false,
      variable_aliases: std::collections::HashMap::new(),
    }
  }
  
  /// Track a reference, expanding through variable aliases
  fn track_reference(&mut self, var_name: String) {
    self.track_reference_internal(var_name, &mut std::collections::HashSet::new());
  }
  
  /// Internal helper that tracks visited variables to prevent infinite recursion
  fn track_reference_internal(&mut self, var_name: String, visited: &mut std::collections::HashSet<String>) {
    // Prevent infinite recursion from circular aliases
    if visited.contains(&var_name) {
      return;
    }
    visited.insert(var_name.clone());
    
    self.referenced_names.insert(var_name.clone());
    
    // Check if this is a field access on an aliased variable (e.g., local_foo.f1 -> foo[0].f1)
    if let Some(dot_pos) = var_name.find('.') {
      let base_var = &var_name[..dot_pos];
      let field_path = &var_name[dot_pos..]; // includes the dot
      
      // If the base variable has aliases, track the expanded names
      if let Some(sources) = self.variable_aliases.get(base_var).cloned() {
        for source in sources {
          let expanded = format!("{}{}", source, field_path);
          self.track_reference_internal(expanded, visited);
        }
      }
    } else {
      // Simple variable reference - check for aliases
      if let Some(sources) = self.variable_aliases.get(&var_name).cloned() {
        for source in sources {
          self.track_reference_internal(source, visited);
        }
      }
    }
  }
}

impl Visitor for ReferenceCollector {
  fn visit_expr(&mut self, expr: &ast::Expr) -> Visit {
    match &expr.content {
      // Direct variable reference
      ast::ExprData::Variable(identifier) => {
        let var_name = identifier.content.0.to_string();
        
        // Track built-in GL variables
        if var_name == "gl_InstanceID" {
          self.uses_gl_instance_id = true;
        } else if var_name == "gl_VertexID" {
          self.uses_gl_vertex_id = true;
        }
        
        self.track_reference(var_name);
      }
      // Field access: uniform.field or uniform[0].field
      ast::ExprData::Dot(base_expr, field_ident) => {
        // Build the full path for struct member access
        let full_path = self.build_member_path(base_expr, &field_ident.content.0);
        self.track_reference(full_path);
      }
      // Array access: uniform[0]
      ast::ExprData::Bracket(base_expr, index_expr) => {
        // Try to extract the index if it's a constant
        let index_str = if let ast::ExprData::IntConst(val) = &index_expr.content {
          format!("[{}]", val)
        } else {
          // For non-constant indices, we can't determine exact element
          // Mark the base as referenced
          "[0]".to_string() // Default to [0] for tracking
        };
        
        if let ast::ExprData::Variable(ident) = &base_expr.content {
          let full_name = format!("{}{}", ident.content.0, index_str);
          self.track_reference(full_name);
        }
      }
      // Assignment: track variable aliases (local_var = uniform_var)
      ast::ExprData::Assignment(lhs, _, rhs) => {
        // Extract the left-hand side variable name
        let lhs_name = self.extract_var_name(lhs);
        // Extract all variables referenced on the right-hand side
        let rhs_vars = self.extract_all_var_names(rhs);
        
        if let Some(lhs_var) = lhs_name {
          if !rhs_vars.is_empty() {
            self.variable_aliases.insert(lhs_var, rhs_vars);
          }
        }
      }
      _ => {}
    }
    Visit::Children
  }
}

impl ReferenceCollector {
  /// Build the full member path for nested field access
  fn build_member_path(&self, expr: &ast::Expr, field: &str) -> String {
    match &expr.content {
      // Simple variable: var.field
      ast::ExprData::Variable(ident) => {
        format!("{}.{}", ident.content.0, field)
      }
      // Nested access: var[0].field
      ast::ExprData::Bracket(base_expr, index_expr) => {
        let base_path = self.get_base_path(base_expr);
        let index_str = if let ast::ExprData::IntConst(val) = &index_expr.content {
          format!("[{}]", val)
        } else {
          "[0]".to_string()
        };
        format!("{}{}.{}", base_path, index_str, field)
      }
      // Nested field: var.field1.field2
      ast::ExprData::Dot(nested_base, nested_field) => {
        let base_path = self.build_member_path(nested_base, &nested_field.content.0);
        format!("{}.{}", base_path, field)
      }
      _ => field.to_string(),
    }
  }

  /// Get the base path from an expression
  fn get_base_path(&self, expr: &ast::Expr) -> String {
    match &expr.content {
      ast::ExprData::Variable(ident) => ident.content.0.to_string(),
      ast::ExprData::Dot(base, field) => {
        self.build_member_path(base, &field.content.0)
      }
      _ => String::new(),
    }
  }
  
  /// Extract the variable name from an expression (for LHS of assignments)
  fn extract_var_name(&self, expr: &ast::Expr) -> Option<String> {
    match &expr.content {
      ast::ExprData::Variable(ident) => Some(ident.content.0.to_string()),
      _ => None,
    }
  }
  
  /// Extract all variable names referenced in an expression (for RHS of assignments)
  fn extract_all_var_names(&self, expr: &ast::Expr) -> Vec<String> {
    let mut vars = Vec::new();
    self.collect_var_names(expr, &mut vars);
    vars
  }
  
  /// Recursively collect all variable names from an expression
  fn collect_var_names(&self, expr: &ast::Expr, vars: &mut Vec<String>) {
    match &expr.content {
      ast::ExprData::Variable(ident) => {
        vars.push(ident.content.0.to_string());
      }
      ast::ExprData::Bracket(base_expr, index_expr) => {
        // For array access like foo[0], build the full name
        if let ast::ExprData::Variable(ident) = &base_expr.content {
          let index_str = if let ast::ExprData::IntConst(val) = &index_expr.content {
            format!("[{}]", val)
          } else {
            "[0]".to_string()
          };
          vars.push(format!("{}{}", ident.content.0, index_str));
        } else {
          self.collect_var_names(base_expr, vars);
        }
        self.collect_var_names(index_expr, vars);
      }
      ast::ExprData::Dot(base_expr, field) => {
        // For field access like foo.bar, build the full path
        let full_path = self.build_member_path(base_expr, &field.content.0);
        vars.push(full_path);
      }
      ast::ExprData::Unary(_, operand) => {
        self.collect_var_names(operand, vars);
      }
      ast::ExprData::Binary(_, lhs, rhs) => {
        self.collect_var_names(lhs, vars);
        self.collect_var_names(rhs, vars);
      }
      ast::ExprData::Ternary(cond, if_true, if_false) => {
        self.collect_var_names(cond, vars);
        self.collect_var_names(if_true, vars);
        self.collect_var_names(if_false, vars);
      }
      // Note: Assignment is intentionally NOT handled here to avoid infinite recursion.
      // Assignments are handled specially in visit_expr() to track variable aliases.
      // If we recursively process assignments here, it creates a loop since visit_expr
      // calls extract_all_var_names which calls this function.
      ast::ExprData::FunCall(_, args) => {
        for arg in args {
          self.collect_var_names(arg, vars);
        }
      }
      _ => {}
    }
  }
}

/// GL type constants matching WebGL spec
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u32)]
pub enum GLType {
  Float = 0x1406,
  FloatVec2 = 0x8B50,
  FloatVec3 = 0x8B51,
  FloatVec4 = 0x8B52,
  Int = 0x1404,
  IntVec2 = 0x8B53,
  IntVec3 = 0x8B54,
  IntVec4 = 0x8B55,
  UnsignedInt = 0x1405,
  Bool = 0x8B56,
  BoolVec2 = 0x8B57,
  BoolVec3 = 0x8B58,
  BoolVec4 = 0x8B59,
  FloatMat2 = 0x8B5A,
  FloatMat3 = 0x8B5B,
  FloatMat4 = 0x8B5C,
  FloatMat2x3 = 0x8B65,
  FloatMat2x4 = 0x8B66,
  FloatMat3x2 = 0x8B67,
  FloatMat3x4 = 0x8B68,
  FloatMat4x2 = 0x8B69,
  FloatMat4x3 = 0x8B6A,
  Sampler2D = 0x8B5E,
  SamplerCube = 0x8B60,
  Unknown = 0,
}

/// Represents metadata about a vertex attribute in GLSL
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GLSLAttribute {
  /// The name of the attribute (e.g., "position", "normal")
  pub name: String,
  /// The GL type constant (e.g., GL_FLOAT_VEC3, GL_FLOAT_VEC4)
  #[serde(rename = "type")]
  pub gl_type: u32,
  /// The size of the attribute (number of elements, usually 1 for single values)
  pub size: i32,
  /// The assigned location of the attribute (0-based index)
  pub location: i32,
  /// Whether the attribute is active (referenced in the shader)
  pub active: bool,
}

/// Represents metadata about a uniform variable in GLSL
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GLSLUniform {
  /// The name of the uniform (e.g., "modelViewMatrix", "lightColor")
  pub name: String,
  /// The GL type constant (e.g., GL_FLOAT_MAT4, GL_FLOAT_VEC3, GL_SAMPLER_2D)
  #[serde(rename = "type")]
  pub gl_type: u32,
  /// The size of the uniform (number of elements, 1 for non-arrays)
  pub size: i32,
  /// Whether the uniform is active (referenced in the shader)
  pub active: bool,
}

/// Combined result of shader parsing containing both attributes and uniforms
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GLSLShaderVariables {
  /// List of vertex attributes
  pub attributes: Vec<GLSLAttribute>,
  /// List of uniform variables
  pub uniforms: Vec<GLSLUniform>,
}

/// Struct field definition
#[derive(Debug, Clone)]
struct StructField {
  name: String,
  gl_type: GLType,
  is_struct: bool,
  struct_name: Option<String>,
}

/// Analyzer for extracting shader variables (attributes, uniforms) from GLSL source code
pub struct GLSLShaderAnalyzer {
  attributes: Vec<GLSLAttribute>,
  uniforms: Vec<GLSLUniform>,
  next_location: i32,
  referenced_names: std::collections::HashSet<String>,
  /// Map of struct names to their field definitions
  struct_definitions: std::collections::HashMap<String, Vec<StructField>>,
  /// Whether to include gl_InstanceID/gl_VertexID as active attributes if used
  include_builtin_attributes: bool,
}

impl GLSLShaderAnalyzer {
  /// Create a new analyzer
  pub fn new() -> Self {
    Self {
      attributes: Vec::new(),
      uniforms: Vec::new(),
      next_location: 0,
      referenced_names: std::collections::HashSet::new(),
      struct_definitions: std::collections::HashMap::new(),
      include_builtin_attributes: false,
    }
  }

  /// Create a new analyzer with configuration for built-in attributes
  pub fn with_builtin_attributes(include_builtin: bool) -> Self {
    Self {
      attributes: Vec::new(),
      uniforms: Vec::new(),
      next_location: 0,
      referenced_names: std::collections::HashSet::new(),
      struct_definitions: std::collections::HashMap::new(),
      include_builtin_attributes: include_builtin,
    }
  }

  /// Parse GLSL source and extract all attribute and uniform declarations
  pub fn parse(&mut self, source: &str) -> Result<(), String> {
    use glsl_lang::{
      ast::TranslationUnit, lexer::full::fs::PreprocessorExt, parse::IntoParseBuilderExt,
    };

    let mut processor = glsl_lang_pp::processor::fs::StdProcessor::new();
    let tu: TranslationUnit = processor
      .open_source(source, Path::new("."))
      .builder()
      .parse()
      .map(|(mut tu, _, iter)| {
        iter.into_directives().inject(&mut tu);
        tu
      })
      .map_err(|e| format!("Failed to parse GLSL source: {:?}", e))?;

    // First pass: collect all variable references
    let mut ref_collector = ReferenceCollector::new();
    tu.visit(&mut ref_collector);
    self.referenced_names = ref_collector.referenced_names;

    // Second pass: visit the AST to find attributes and uniforms
    use glsl_lang::visitor::Host;
    tu.visit(self);

    // Mark inactive variables instead of filtering them out
    for attr in &mut self.attributes {
      attr.active = self.referenced_names.contains(&attr.name);
    }
    for uniform in &mut self.uniforms {
      uniform.active = self.referenced_names.contains(&uniform.name);
    }

    // Add built-in attributes if requested and they're used in the shader
    if self.include_builtin_attributes {
      if ref_collector.uses_gl_instance_id {
        self.attributes.push(GLSLAttribute {
          name: "gl_InstanceID".to_string(),
          gl_type: GLType::Int as u32,
          size: 1,
          location: -1, // Built-in attributes don't have explicit locations
          active: true,
        });
      }
      if ref_collector.uses_gl_vertex_id {
        self.attributes.push(GLSLAttribute {
          name: "gl_VertexID".to_string(),
          gl_type: GLType::Int as u32,
          size: 1,
          location: -1, // Built-in attributes don't have explicit locations
          active: true,
        });
      }
    }

    Ok(())
  }

  /// Get the parsed attributes
  pub fn get_attributes(&self) -> &[GLSLAttribute] {
    &self.attributes
  }

  /// Get the parsed uniforms
  pub fn get_uniforms(&self) -> &[GLSLUniform] {
    &self.uniforms
  }

  /// Find attribute location by name
  pub fn get_attrib_location(&self, name: &str) -> Option<i32> {
    self
      .attributes
      .iter()
      .find(|attr| attr.name == name)
      .map(|attr| attr.location)
  }

  /// Extract GL type from TypeSpecifierNonArray
  fn type_to_gl_type(&self, ty: &ast::TypeSpecifierNonArray) -> GLType {
    match &ty.content {
      ast::TypeSpecifierNonArrayData::Float => GLType::Float,
      ast::TypeSpecifierNonArrayData::Double => GLType::Float, // Map double to float
      ast::TypeSpecifierNonArrayData::Int => GLType::Int,
      ast::TypeSpecifierNonArrayData::UInt => GLType::UnsignedInt,
      ast::TypeSpecifierNonArrayData::Bool => GLType::Bool,
      ast::TypeSpecifierNonArrayData::Vec2 => GLType::FloatVec2,
      ast::TypeSpecifierNonArrayData::Vec3 => GLType::FloatVec3,
      ast::TypeSpecifierNonArrayData::Vec4 => GLType::FloatVec4,
      ast::TypeSpecifierNonArrayData::DVec2 => GLType::FloatVec2,
      ast::TypeSpecifierNonArrayData::DVec3 => GLType::FloatVec3,
      ast::TypeSpecifierNonArrayData::DVec4 => GLType::FloatVec4,
      ast::TypeSpecifierNonArrayData::BVec2 => GLType::BoolVec2,
      ast::TypeSpecifierNonArrayData::BVec3 => GLType::BoolVec3,
      ast::TypeSpecifierNonArrayData::BVec4 => GLType::BoolVec4,
      ast::TypeSpecifierNonArrayData::IVec2 => GLType::IntVec2,
      ast::TypeSpecifierNonArrayData::IVec3 => GLType::IntVec3,
      ast::TypeSpecifierNonArrayData::IVec4 => GLType::IntVec4,
      ast::TypeSpecifierNonArrayData::UVec2 => GLType::IntVec2, // Map uint to int
      ast::TypeSpecifierNonArrayData::UVec3 => GLType::IntVec3,
      ast::TypeSpecifierNonArrayData::UVec4 => GLType::IntVec4,
      ast::TypeSpecifierNonArrayData::Mat2 => GLType::FloatMat2,
      ast::TypeSpecifierNonArrayData::Mat3 => GLType::FloatMat3,
      ast::TypeSpecifierNonArrayData::Mat4 => GLType::FloatMat4,
      ast::TypeSpecifierNonArrayData::Mat23 => GLType::FloatMat2x3,
      ast::TypeSpecifierNonArrayData::Mat24 => GLType::FloatMat2x4,
      ast::TypeSpecifierNonArrayData::Mat32 => GLType::FloatMat3x2,
      ast::TypeSpecifierNonArrayData::Mat34 => GLType::FloatMat3x4,
      ast::TypeSpecifierNonArrayData::Mat42 => GLType::FloatMat4x2,
      ast::TypeSpecifierNonArrayData::Mat43 => GLType::FloatMat4x3,
      ast::TypeSpecifierNonArrayData::DMat2 => GLType::FloatMat2,
      ast::TypeSpecifierNonArrayData::DMat3 => GLType::FloatMat3,
      ast::TypeSpecifierNonArrayData::DMat4 => GLType::FloatMat4,
      ast::TypeSpecifierNonArrayData::DMat23 => GLType::FloatMat2x3,
      ast::TypeSpecifierNonArrayData::DMat24 => GLType::FloatMat2x4,
      ast::TypeSpecifierNonArrayData::DMat32 => GLType::FloatMat3x2,
      ast::TypeSpecifierNonArrayData::DMat34 => GLType::FloatMat3x4,
      ast::TypeSpecifierNonArrayData::DMat42 => GLType::FloatMat4x2,
      ast::TypeSpecifierNonArrayData::DMat43 => GLType::FloatMat4x3,
      ast::TypeSpecifierNonArrayData::Sampler2D => GLType::Sampler2D,
      ast::TypeSpecifierNonArrayData::SamplerCube => GLType::SamplerCube,
      _ => GLType::Unknown,
    }
  }

  /// Extract explicit location from layout qualifier if present
  fn extract_layout_location(&self, qualifiers: &[ast::LayoutQualifierSpec]) -> Option<i32> {
    for qualifier in qualifiers {
      match &qualifier.content {
        ast::LayoutQualifierSpecData::Identifier(ident, Some(expr)) => {
          if ident.content.0.as_str() == "location" {
            // Try to extract the constant expression value
            if let ast::ExprData::IntConst(val) = &expr.content {
              return Some(*val);
            }
          }
        }
        _ => {}
      }
    }
    None
  }

  /// Check if a declaration has an attribute/in qualifier
  fn has_attribute_qualifier(&self, qualifiers: &[ast::TypeQualifierSpec]) -> (bool, Option<i32>) {
    let mut is_attribute = false;
    let mut layout_location = None;

    for qualifier in qualifiers {
      match &qualifier.content {
        ast::TypeQualifierSpecData::Storage(storage) => match &storage.content {
          ast::StorageQualifierData::Attribute | ast::StorageQualifierData::In => {
            is_attribute = true;
          }
          _ => {}
        },
        ast::TypeQualifierSpecData::Layout(layout) => {
          layout_location = self.extract_layout_location(&layout.content.ids);
        }
        _ => {}
      }
    }

    (is_attribute, layout_location)
  }

  /// Check if a declaration has a uniform qualifier
  fn has_uniform_qualifier(&self, qualifiers: &[ast::TypeQualifierSpec]) -> bool {
    for qualifier in qualifiers {
      if let ast::TypeQualifierSpecData::Storage(storage) = &qualifier.content {
        if let ast::StorageQualifierData::Uniform = &storage.content {
          return true;
        }
      }
    }
    false
  }

  /// Expand a struct uniform into its individual members
  /// Returns a list of (name, gl_type, size) tuples
  /// 
  /// # Arguments
  /// * `base_name` - The base name of the uniform
  /// * `struct_name` - The name of the struct type
  /// * `array_size` - The size of the array
  /// * `is_array_decl` - Whether this was declared as an array (even if size is 1)
  fn expand_struct_uniform(
    &self,
    base_name: &str,
    struct_name: &str,
    array_size: i32,
    is_array_decl: bool,
  ) -> Vec<(String, u32, i32)> {
    let mut expanded = Vec::new();
    
    if let Some(fields) = self.struct_definitions.get(struct_name) {
      if is_array_decl {
        // Array of structs: expand as uniformName[i].field
        // Note: even for single-element arrays (declared as [1]), we use [0] notation
        for i in 0..array_size {
          for field in fields {
            let member_name = format!("{}[{}].{}", base_name, i, field.name);
            if field.is_struct {
              if let Some(ref nested_struct) = field.struct_name {
                // Recursively expand nested structs (nested structs are not arrays)
                let nested = self.expand_struct_uniform(&member_name, nested_struct, 1, false);
                expanded.extend(nested);
              }
            } else {
              expanded.push((member_name, field.gl_type as u32, 1));
            }
          }
        }
      } else {
        // Non-array struct: expand as uniformName.field
        for field in fields {
          let member_name = format!("{}.{}", base_name, field.name);
          if field.is_struct {
            if let Some(ref nested_struct) = field.struct_name {
              // Recursively expand nested structs
              let nested = self.expand_struct_uniform(&member_name, nested_struct, 1, false);
              expanded.extend(nested);
            }
          } else {
            expanded.push((member_name, field.gl_type as u32, 1));
          }
        }
      }
    }
    
    expanded
  }

  /// Check if declaration has an array specifier (even if size is 1)
  fn is_array(&self, array_specifier: &Option<ast::ArraySpecifier>) -> bool {
    array_specifier.is_some()
  }

  /// Extract array size from array specifier if present
  fn get_array_size(&self, array_specifier: &Option<ast::ArraySpecifier>) -> i32 {
    if let Some(spec) = array_specifier {
      if let Some(first_dim) = spec.content.dimensions.first() {
        if let ast::ArraySpecifierDimensionData::ExplicitlySized(ref expr) = first_dim.content {
          if let ast::ExprData::IntConst(val) = expr.content {
            return val;
          }
        }
      }
    }
    1  // Default to 1 if no array or can't determine size
  }

  /// Check if a type specifier is a struct type
  fn is_struct_type(&self, ty: &ast::TypeSpecifierNonArray) -> Option<String> {
    match &ty.content {
      ast::TypeSpecifierNonArrayData::TypeName(type_name) => {
        Some(type_name.content.0.to_string())
      }
      _ => None,
    }
  }
}

impl Visitor for GLSLShaderAnalyzer {
  /// Visit struct specifier to collect struct definitions
  fn visit_struct_specifier(&mut self, spec: &ast::StructSpecifier) -> Visit {
    if let Some(ref name) = spec.name {
      let struct_name = name.content.0.to_string();
      let mut fields = Vec::new();
      
      // Collect all fields from the struct
      for field_decl in &spec.fields {
        // field_decl.content is StructFieldSpecifier
        // .ty is FullySpecifiedType
        // .ty.ty is DIRECTLY TypeSpecifierNonArray (not TypeSpecifier!)
        // This is different from SingleDeclaration where ty.ty.ty is needed
        let type_spec_non_array = &field_decl.content.ty.ty;
        let field_name_base = &field_decl.content.identifiers;
        
        for field in field_name_base {
          let field_name = field.content.ident.content.0.to_string();
          
          // Check if field is itself a struct
          if let Some(nested_struct_name) = self.is_struct_type(type_spec_non_array) {
            fields.push(StructField {
              name: field_name,
              gl_type: GLType::Unknown,
              is_struct: true,
              struct_name: Some(nested_struct_name),
            });
          } else {
            let gl_type = self.type_to_gl_type(type_spec_non_array);
            fields.push(StructField {
              name: field_name,
              gl_type,
              is_struct: false,
              struct_name: None,
            });
          }
        }
      }
      
      self.struct_definitions.insert(struct_name, fields);
    }
    
    Visit::Children
  }

  fn visit_single_declaration(&mut self, declaration: &ast::SingleDeclaration) -> Visit {
    // Check if this declaration has qualifiers
    if let Some(ref qualifier) = declaration.ty.qualifier {
      let (is_attribute, layout_location) =
        self.has_attribute_qualifier(&qualifier.content.qualifiers);
      let is_uniform = self.has_uniform_qualifier(&qualifier.content.qualifiers);

      if is_attribute {
        if let Some(ref name) = declaration.name {
          // Extract GL type from the type specifier
          let gl_type = self.type_to_gl_type(&declaration.ty.ty.ty) as u32;

          let location = layout_location.unwrap_or_else(|| {
            let loc = self.next_location;
            self.next_location += 1;
            loc
          });

          // Get array size
          let size = self.get_array_size(&declaration.array_specifier);

          self.attributes.push(GLSLAttribute {
            name: name.content.0.to_string(),
            gl_type,
            size,
            location,
            active: false, // Will be set later after reference collection
          });
        }
      } else if is_uniform {
        if let Some(ref name) = declaration.name {
          let uniform_name = name.content.0.to_string();
          
          // Check if this is a struct type
          if let Some(struct_name) = self.is_struct_type(&declaration.ty.ty.ty) {
            // Get array size and check if it's declared as an array
            let is_array_decl = self.is_array(&declaration.array_specifier);
            let array_size = self.get_array_size(&declaration.array_specifier);
            
            // Expand the struct into individual member uniforms
            let expanded = self.expand_struct_uniform(&uniform_name, &struct_name, array_size, is_array_decl);
            
            for (member_name, member_gl_type, member_size) in expanded {
              self.uniforms.push(GLSLUniform {
                name: member_name,
                gl_type: member_gl_type,
                size: member_size,
                active: false, // Will be set later after reference collection
              });
            }
          } else {
            // Regular uniform (not a struct)
            let gl_type = self.type_to_gl_type(&declaration.ty.ty.ty) as u32;
            let size = self.get_array_size(&declaration.array_specifier);

            self.uniforms.push(GLSLUniform {
              name: uniform_name,
              gl_type,
              size,
              active: false, // Will be set later after reference collection
            });
          }
        }
      }
    }

    Visit::Children
  }
}

#[cxx::bridge(namespace = "holocron::webgl")]
mod ffi {
  extern "Rust" {
    /// Patch GLSL source code from string
    #[cxx_name = "patchGLSLSourceFromStr"]
    fn patch_glsl_source_from_str(input: &str) -> String;

    /// Parse GLSL shader source and extract both attributes and uniforms as JSON
    /// @param source The GLSL shader source code
    /// @param include_builtin_attributes Whether to include gl_InstanceID/gl_VertexID as active attributes if used
    #[cxx_name = "parseGLSLShader"]
    fn parse_glsl_shader(source: &str, include_builtin_attributes: bool) -> String;

    /// Parse GLSL shader source and extract attributes as JSON (deprecated, use parseGLSLShader)
    /// @param source The GLSL shader source code
    /// @param include_builtin_attributes Whether to include gl_InstanceID/gl_VertexID as active attributes if used
    #[cxx_name = "parseGLSLAttributes"]
    fn parse_glsl_attributes(source: &str, include_builtin_attributes: bool) -> String;

    /// Parse GLSL shader source and extract uniforms as JSON (deprecated, use parseGLSLShader)
    #[cxx_name = "parseGLSLUniforms"]
    fn parse_glsl_uniforms(source: &str) -> String;
  }
}

/// Parse GLSL source and return both attributes and uniforms as JSON string
fn parse_glsl_shader(source: &str, include_builtin_attributes: bool) -> String {
  let mut analyzer = GLSLShaderAnalyzer::with_builtin_attributes(include_builtin_attributes);

  match analyzer.parse(source) {
    Ok(_) => {
      let variables = GLSLShaderVariables {
        attributes: analyzer.get_attributes().to_vec(),
        uniforms: analyzer.get_uniforms().to_vec(),
      };
      serde_json::to_string(&variables).unwrap_or_else(|_| r#"{"attributes":[],"uniforms":[]}"#.to_string())
    }
    Err(_) => r#"{"attributes":[],"uniforms":[]}"#.to_string(),
  }
}

/// Parse GLSL source and return all attributes as JSON string
fn parse_glsl_attributes(source: &str, include_builtin_attributes: bool) -> String {
  let mut analyzer = GLSLShaderAnalyzer::with_builtin_attributes(include_builtin_attributes);

  match analyzer.parse(source) {
    Ok(_) => {
      let attributes = analyzer.get_attributes();
      serde_json::to_string(attributes).unwrap_or_else(|_| "[]".to_string())
    }
    Err(_) => "[]".to_string(),
  }
}

/// Parse GLSL source and return all uniforms as JSON string
fn parse_glsl_uniforms(source: &str) -> String {
  let mut analyzer = GLSLShaderAnalyzer::new();
  
  match analyzer.parse(source) {
    Ok(_) => {
      let uniforms = analyzer.get_uniforms();
      serde_json::to_string(uniforms).unwrap_or_else(|_| "[]".to_string())
    }
    Err(_) => "[]".to_string(),
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_patch_glsl_source() {
    let source_str = r#"
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;

#version 300 es
precision highp float;
highp float a = 1.0;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 0) out highp vec4 glFragColor;
#extension GL_OES_standard_derivatives : enable

void main() { 
  gl_FragColor = vec4(1, 1, 1, 1); 
}"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
#extension GL_OVR_multiview2 : enable
#extension GL_OES_standard_derivatives : enable
layout(num_views = 2) in;
precision highp float;
highp float a = 1.;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 0) out highp vec4 glFragColor;
void main() {
    gl_FragColor = vec4(1, 1, 1, 1);
}
"#
    )
  }

  #[test]
  fn test_patch_glsl_source_threejs() {
    let source_str = r#"
#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;
#define VIEW_ID gl_ViewID_OVR

uniform mat4 modelMatrix;
uniform mat4 viewMatrices[2];
uniform mat4 modelViewMatrices[2];

#define viewMatrix viewMatrices[VIEW_ID]
#define modelViewMatrix modelMatrix * viewMatrix

in vec3 position;
void main() {
  gl_Position = modelViewMatrix * vec4(position, 1.0);
}
  "#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;
uniform mat4 modelMatrix;
uniform mat4 viewMatrices[2];
uniform mat4 modelViewMatrices[2];
in vec3 position;
void main() {
    gl_Position = modelMatrix * viewMatrices[gl_ViewID_OVR] * vec4(position, 1.);
}
"#
    )
  }

  #[test]
  #[ignore]
  fn test_patch_glsl_source_elif_expand() {
    let source_str = r#"
#version 300 es
#define CS1
#define CS2
#define CS3

vec3 test() {
#if defined(CS1)
  return vec3(1.0, 0.0, 0.0);
#elif defined(CS2)
  return vec3(2.0, 0.0, 0.0);
#elif defined(CS3)
  return vec3(3.0, 1.0, 0.0);
#else
  return vec3(0.0, 0.0, 1.0);
#endif
}
"#;
    let patched_source_str = patch_glsl_source_from_str(source_str);
    assert_eq!(
      patched_source_str,
      r#"#version 300 es
vec3 test() {
    return vec3(1., 0., 0.);
}
"#
    )
  }

  #[test]
  fn test_parse_glsl_attributes_basic() {
    let source_str = r#"
#version 300 es
precision highp float;

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 modelViewMatrix;

out vec3 vNormal;
out vec2 vUv;

void main() {
  gl_Position = modelViewMatrix * vec4(position, 1.0);
  vNormal = normal;
  vUv = uv;
}
"#;
    let mut parser = GLSLShaderAnalyzer::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    // Check first attribute
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(attributes[0].size, 1);
    assert_eq!(attributes[0].location, 0);
    assert_eq!(attributes[0].active, true);

    // Check second attribute
    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(attributes[1].size, 1);
    assert_eq!(attributes[1].location, 1);
    assert_eq!(attributes[1].active, true);

    // Check third attribute
    assert_eq!(attributes[2].name, "uv");
    assert_eq!(attributes[2].gl_type, GLType::FloatVec2 as u32);
    assert_eq!(attributes[2].size, 1);
    assert_eq!(attributes[2].location, 2);
    assert_eq!(attributes[2].active, true);
  }

  #[test]
  fn test_parse_glsl_attributes_with_layout() {
    let source_str = r#"
#version 300 es
precision highp float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;

void main() {
  gl_Position = vec4(aPos, 1.0);
  vNormal = aNormal;
  vTexCoord = aTexCoord;
}
"#;
    let mut parser = GLSLShaderAnalyzer::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    assert_eq!(attributes[0].name, "aPos");
    assert_eq!(attributes[0].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(attributes[0].location, 0);

    assert_eq!(attributes[1].name, "aNormal");
    assert_eq!(attributes[1].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(attributes[1].location, 1);

    assert_eq!(attributes[2].name, "aTexCoord");
    assert_eq!(attributes[2].gl_type, GLType::FloatVec2 as u32);
    assert_eq!(attributes[2].location, 2);
  }

  #[test]
  fn test_parse_glsl_attributes_glsl100() {
    let source_str = r#"
precision highp float;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoord;

varying vec3 vNormal;
varying vec2 vTexCoord;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
  vTexCoord = texCoord;
}
"#;
    let mut parser = GLSLShaderAnalyzer::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    let attributes = parser.get_attributes();
    assert_eq!(attributes.len(), 3);

    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].gl_type, GLType::FloatVec3 as u32);

    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].gl_type, GLType::FloatVec3 as u32);

    assert_eq!(attributes[2].name, "texCoord");
    assert_eq!(attributes[2].gl_type, GLType::FloatVec2 as u32);
  }

  #[test]
  fn test_get_attrib_location() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 vNormal;
out vec2 vUv;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
  vUv = uv;
}
"#;
    let mut parser = GLSLShaderAnalyzer::new();
    parser.parse(source_str).expect("Failed to parse GLSL");

    assert_eq!(parser.get_attrib_location("position"), Some(0));
    assert_eq!(parser.get_attrib_location("normal"), Some(1));
    assert_eq!(parser.get_attrib_location("uv"), Some(2));
    assert_eq!(parser.get_attrib_location("nonexistent"), None);
  }

  #[test]
  fn test_parse_glsl_attributes_ffi() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;

out vec3 vNormal;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
}
"#;
    let result = parse_glsl_attributes(source_str, false);

    // Parse the JSON result
    let attributes: Vec<GLSLAttribute> = serde_json::from_str(&result).expect("Failed to parse JSON");
    assert_eq!(attributes.len(), 2);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[1].name, "normal");
  }

  #[test]
  fn test_parse_glsl_attributes_marks_inactive() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;
in vec2 unused_attr;

out vec3 vNormal;

void main() {
  gl_Position = vec4(position, 1.0);
  vNormal = normal;
}
"#;
    let mut parser = GLSLShaderAnalyzer::new();
    parser.parse(source_str).expect("Failed to parse GLSL");
    
    let attributes = parser.get_attributes();
    // All attributes should be returned, but unused_attr should be marked inactive
    assert_eq!(attributes.len(), 3);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].active, true);
    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].active, true);
    assert_eq!(attributes[2].name, "unused_attr");
    assert_eq!(attributes[2].active, false); // This one is inactive
  }

  #[test]
  fn test_parse_glsl_uniforms_basic() {
    let source_str = r#"
#version 300 es
precision highp float;

in vec3 position;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;

out vec3 vPosition;

void main() {
  vec4 worldPos = modelViewMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix * worldPos;
  vPosition = lightPosition;
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse GLSL");

    let uniforms = analyzer.get_uniforms();
    assert_eq!(uniforms.len(), 3);

    assert_eq!(uniforms[0].name, "modelViewMatrix");
    assert_eq!(uniforms[0].gl_type, GLType::FloatMat4 as u32);

    assert_eq!(uniforms[1].name, "projectionMatrix");
    assert_eq!(uniforms[1].gl_type, GLType::FloatMat4 as u32);

    assert_eq!(uniforms[2].name, "lightPosition");
    assert_eq!(uniforms[2].gl_type, GLType::FloatVec3 as u32);
  }

  #[test]
  fn test_parse_glsl_uniforms_marks_inactive() {
    let source_str = r#"
#version 300 es
precision highp float;

in vec3 position;
uniform mat4 mvpMatrix;
uniform vec3 unusedColor;
uniform float unusedScale;

void main() {
  gl_Position = mvpMatrix * vec4(position, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse GLSL");

    let uniforms = analyzer.get_uniforms();
    // All uniforms should be returned, but only mvpMatrix should be active
    assert_eq!(uniforms.len(), 3);
    assert_eq!(uniforms[0].name, "mvpMatrix");
    assert_eq!(uniforms[0].gl_type, GLType::FloatMat4 as u32);
    assert_eq!(uniforms[0].active, true);
    assert_eq!(uniforms[1].name, "unusedColor");
    assert_eq!(uniforms[1].active, false);
    assert_eq!(uniforms[2].name, "unusedScale");
    assert_eq!(uniforms[2].active, false);
  }

  #[test]
  fn test_parse_glsl_uniforms_ffi() {
    let source_str = r#"
#version 300 es
in vec3 position;
uniform mat4 transform;
uniform vec4 color;

out vec4 vColor;

void main() {
  gl_Position = transform * vec4(position, 1.0);
  vColor = color;
}
"#;
    let result = parse_glsl_uniforms(source_str);
    
    // Parse the JSON result
    let uniforms: Vec<GLSLUniform> = serde_json::from_str(&result).expect("Failed to parse JSON");
    assert_eq!(uniforms.len(), 2);
    assert_eq!(uniforms[0].name, "transform");
    assert_eq!(uniforms[0].gl_type, GLType::FloatMat4 as u32);
    assert_eq!(uniforms[1].name, "color");
    assert_eq!(uniforms[1].gl_type, GLType::FloatVec4 as u32);
  }

  #[test]
  fn test_parse_glsl_shader_combined() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 normal;
in vec2 unused_attr;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform vec3 unusedColor;

out vec3 vNormal;

void main() {
  gl_Position = modelMatrix * viewMatrix * vec4(position, 1.0);
  vNormal = normal;
}
"#;
    let result = parse_glsl_shader(source_str, false);

    // Parse the JSON result
    let variables: GLSLShaderVariables = serde_json::from_str(&result).expect("Failed to parse JSON");
    
    // Check attributes
    assert_eq!(variables.attributes.len(), 3);
    assert_eq!(variables.attributes[0].name, "position");
    assert_eq!(variables.attributes[0].active, true);
    assert_eq!(variables.attributes[1].name, "normal");
    assert_eq!(variables.attributes[1].active, true);
    assert_eq!(variables.attributes[2].name, "unused_attr");
    assert_eq!(variables.attributes[2].active, false);
    
    // Check uniforms
    assert_eq!(variables.uniforms.len(), 3);
    assert_eq!(variables.uniforms[0].name, "modelMatrix");
    assert_eq!(variables.uniforms[0].active, true);
    assert_eq!(variables.uniforms[1].name, "viewMatrix");
    assert_eq!(variables.uniforms[1].active, true);
    assert_eq!(variables.uniforms[2].name, "unusedColor");
    assert_eq!(variables.uniforms[2].active, false);
  }

  #[test]
  fn test_parse_glsl_structured_uniforms() {
    let source_str = r#"
#version 300 es
struct DirectionalLight {
  vec3 direction;
  vec3 color;
  float intensity;
};

in vec3 position;
uniform DirectionalLight directionalLights[2];
uniform vec3 ambient;

out vec4 fragColor;

void main() {
  gl_Position = vec4(position, 1.0);
  vec3 lighting = ambient;
  lighting += directionalLights[0].color * directionalLights[0].intensity;
  lighting += directionalLights[1].direction * 0.5;
  fragColor = vec4(lighting, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse");
    
    let uniforms = analyzer.get_uniforms();
    
    // Uniforms should be in declaration order: directionalLights[0].*, directionalLights[1].*, ambient
    // Should have 7 total: 6 from the 2-element array of structs + 1 ambient
    assert_eq!(uniforms.len(), 7);
    
    // Check expanded struct uniforms (directionalLights declared first)
    assert_eq!(uniforms[0].name, "directionalLights[0].direction");
    assert_eq!(uniforms[0].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[0].active, false); // Not used in the shader
    
    assert_eq!(uniforms[1].name, "directionalLights[0].color");
    assert_eq!(uniforms[1].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[1].active, true); // Used in shader
    
    assert_eq!(uniforms[2].name, "directionalLights[0].intensity");
    assert_eq!(uniforms[2].gl_type, GLType::Float as u32);
    assert_eq!(uniforms[2].active, true); // Used in shader
    
    assert_eq!(uniforms[3].name, "directionalLights[1].direction");
    assert_eq!(uniforms[3].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[3].active, true); // Used in shader
    
    assert_eq!(uniforms[4].name, "directionalLights[1].color");
    assert_eq!(uniforms[4].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[4].active, false); // Not used in the shader
    
    assert_eq!(uniforms[5].name, "directionalLights[1].intensity");
    assert_eq!(uniforms[5].gl_type, GLType::Float as u32);
    assert_eq!(uniforms[5].active, false); // Not used in the shader
    
    // Check ambient uniform (declared after directionalLights)
    assert_eq!(uniforms[6].name, "ambient");
    assert_eq!(uniforms[6].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[6].active, true);
  }

  #[test]
  fn test_parse_glsl_nested_struct_uniforms() {
    let source_str = r#"
#version 300 es
struct Material {
  vec3 ambient;
  vec3 diffuse;
};

struct Light {
  vec3 position;
  Material material;
};

in vec3 vertPosition;
uniform Light light;

out vec4 fragColor;

void main() {
  gl_Position = vec4(vertPosition, 1.0);
  fragColor = vec4(light.material.diffuse, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse");
    
    let uniforms = analyzer.get_uniforms();
    
    // Should have: light.position, light.material.ambient, light.material.diffuse
    assert_eq!(uniforms.len(), 3);
    
    assert_eq!(uniforms[0].name, "light.position");
    assert_eq!(uniforms[0].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[0].active, false); // Not used
    
    assert_eq!(uniforms[1].name, "light.material.ambient");
    assert_eq!(uniforms[1].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[1].active, false); // Not used
    
    assert_eq!(uniforms[2].name, "light.material.diffuse");
    assert_eq!(uniforms[2].gl_type, GLType::FloatVec3 as u32);
    assert_eq!(uniforms[2].active, true); // Used!
  }

  #[test]
  fn test_parse_glsl_with_builtin_attributes() {
    let source_str = r#"
#version 300 es
in vec3 position;
in vec3 offset;

uniform mat4 mvpMatrix;

void main() {
  // Use gl_InstanceID to index into transforms
  vec3 instanceOffset = offset * float(gl_InstanceID);
  vec4 worldPos = vec4(position + instanceOffset, 1.0);
  gl_Position = mvpMatrix * worldPos;
  
  // Use gl_VertexID for something
  float fade = float(gl_VertexID) * 0.01;
}
"#;
    
    // Test WITHOUT including builtin attributes
    let mut analyzer_no_builtin = GLSLShaderAnalyzer::new();
    analyzer_no_builtin.parse(source_str).expect("Failed to parse");
    let attributes_no_builtin = analyzer_no_builtin.get_attributes();
    
    // Should only have position and offset (no gl_InstanceID or gl_VertexID)
    assert_eq!(attributes_no_builtin.len(), 2);
    assert_eq!(attributes_no_builtin[0].name, "position");
    assert_eq!(attributes_no_builtin[1].name, "offset");
    
    // Test WITH including builtin attributes
    let mut analyzer_with_builtin = GLSLShaderAnalyzer::with_builtin_attributes(true);
    analyzer_with_builtin.parse(source_str).expect("Failed to parse");
    let attributes_with_builtin = analyzer_with_builtin.get_attributes();
    
    // Should have position, offset, gl_InstanceID, and gl_VertexID
    assert_eq!(attributes_with_builtin.len(), 4);
    assert_eq!(attributes_with_builtin[0].name, "position");
    assert_eq!(attributes_with_builtin[0].active, true);
    assert_eq!(attributes_with_builtin[1].name, "offset");
    assert_eq!(attributes_with_builtin[1].active, true);
    assert_eq!(attributes_with_builtin[2].name, "gl_InstanceID");
    assert_eq!(attributes_with_builtin[2].gl_type, GLType::Int as u32);
    assert_eq!(attributes_with_builtin[2].active, true);
    assert_eq!(attributes_with_builtin[2].location, -1); // Builtin has no explicit location
    assert_eq!(attributes_with_builtin[3].name, "gl_VertexID");
    assert_eq!(attributes_with_builtin[3].gl_type, GLType::Int as u32);
    assert_eq!(attributes_with_builtin[3].active, true);
    assert_eq!(attributes_with_builtin[3].location, -1); // Builtin has no explicit location
  }

  #[test]
  fn test_parse_glsl_builtin_attributes_ffi() {
    let source_str = r#"
#version 300 es
in vec3 position;

void main() {
  gl_Position = vec4(position * float(gl_InstanceID), 1.0);
}
"#;
    
    // Test FFI with builtin attributes enabled
    let result = parse_glsl_shader(source_str, true);
    let variables: GLSLShaderVariables = serde_json::from_str(&result).expect("Failed to parse JSON");
    
    // Should have position and gl_InstanceID
    assert_eq!(variables.attributes.len(), 2);
    assert_eq!(variables.attributes[0].name, "position");
    assert_eq!(variables.attributes[1].name, "gl_InstanceID");
    assert_eq!(variables.attributes[1].active, true);
  }

  #[test]
  fn test_parse_glsl_with_preprocessor_defines() {
    // Test that preprocessor directives are handled before analysis
    let source_str = r#"
#version 300 es
#define USE_NORMAL 1
#define USE_UV 1

in vec3 position;
#if USE_NORMAL
in vec3 normal;
#endif
#if USE_UV
in vec2 uv;
#endif

uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(position, 1.0);
#if USE_NORMAL
  vec3 n = normal;
#endif
#if USE_UV
  vec2 texCoord = uv;
#endif
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse GLSL with preprocessor");
    
    let attributes = analyzer.get_attributes();
    // Should include all three attributes (position, normal, uv) since preprocessor enables them
    assert_eq!(attributes.len(), 3);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].active, true);
    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].active, true);
    assert_eq!(attributes[2].name, "uv");
    assert_eq!(attributes[2].active, true);
    
    let uniforms = analyzer.get_uniforms();
    assert_eq!(uniforms.len(), 1);
    assert_eq!(uniforms[0].name, "mvp");
    assert_eq!(uniforms[0].active, true);
  }

  #[test]
  fn test_parse_glsl_conditional_compilation() {
    // Test that #ifdef and #if directives properly control attribute/uniform declarations
    let source_str = r#"
#version 300 es
#define ENABLE_LIGHTING
#define ENABLE_TEXTURES

in vec3 position;

// These should be included (defined and enabled)
#ifdef ENABLE_LIGHTING
in vec3 normal;
uniform vec3 lightDir;
#endif

#ifdef ENABLE_TEXTURES
in vec2 texCoord;
uniform vec4 texColor;
#endif

// This should NOT be included (not defined)
#ifdef ENABLE_SHADOWS
in vec4 shadowCoord;
uniform mat4 shadowMatrix;
#endif

// Conditional uniform inside #if with expression
#if defined(ENABLE_LIGHTING) && defined(ENABLE_TEXTURES)
uniform float shininess;
#endif

out vec4 fragColor;

void main() {
  gl_Position = vec4(position, 1.0);
  fragColor = vec4(1.0);
#ifdef ENABLE_LIGHTING
  vec3 n = normal;
  vec3 light = lightDir;
  fragColor.rgb *= dot(n, light);
#endif
#ifdef ENABLE_TEXTURES
  vec2 tc = texCoord;
  fragColor *= texColor;
#endif
#if defined(ENABLE_LIGHTING) && defined(ENABLE_TEXTURES)
  float s = shininess;
  fragColor.a *= s;
#endif
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse shader with conditional compilation");
    
    let attributes = analyzer.get_attributes();
    // Should have: position, normal, texCoord (shadowCoord should NOT be included)
    assert_eq!(attributes.len(), 3, "Should include only conditionally enabled attributes");
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].active, true);
    assert_eq!(attributes[1].name, "normal");
    assert_eq!(attributes[1].active, true);
    assert_eq!(attributes[2].name, "texCoord");
    assert_eq!(attributes[2].active, true);
    
    let uniforms = analyzer.get_uniforms();
    // Should have: lightDir, diffuseMap, shininess (shadowMatrix should NOT be included)
    assert_eq!(uniforms.len(), 3, "Should include only conditionally enabled uniforms");
    
    let light_dir = uniforms.iter().find(|u| u.name == "lightDir");
    assert!(light_dir.is_some(), "lightDir should be included (ENABLE_LIGHTING defined)");
    assert_eq!(light_dir.unwrap().active, true);
    
    let tex_color = uniforms.iter().find(|u| u.name == "texColor");
    assert!(tex_color.is_some(), "texColor should be included (ENABLE_TEXTURES defined)");
    assert_eq!(tex_color.unwrap().active, true);
    
    let shininess = uniforms.iter().find(|u| u.name == "shininess");
    assert!(shininess.is_some(), "shininess should be included (both flags defined)");
    assert_eq!(shininess.unwrap().active, true);
    
    // Verify that ENABLE_SHADOWS items are NOT included
    let shadow_coord = attributes.iter().find(|a| a.name == "shadowCoord");
    assert!(shadow_coord.is_none(), "shadowCoord should NOT be included (ENABLE_SHADOWS not defined)");
    
    let shadow_matrix = uniforms.iter().find(|u| u.name == "shadowMatrix");
    assert!(shadow_matrix.is_none(), "shadowMatrix should NOT be included (ENABLE_SHADOWS not defined)");
  }

  #[test]
  fn test_parse_glsl_array_sizes() {
    // Test that array sizes are correctly extracted
    let source_str = r#"
#version 300 es
precision highp float;

in vec3 position;
uniform vec4 colors[4];
uniform mat4 transforms[3];
uniform float values[10];

void main() {
  vec4 color = colors[0] + colors[1];
  gl_Position = transforms[0] * vec4(position, 1.0);
  float val = values[0];
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse GLSL with arrays");
    
    let uniforms = analyzer.get_uniforms();
    assert_eq!(uniforms.len(), 3);
    
    // Check colors array
    assert_eq!(uniforms[0].name, "colors");
    assert_eq!(uniforms[0].gl_type, GLType::FloatVec4 as u32);
    assert_eq!(uniforms[0].size, 4);
    assert_eq!(uniforms[0].active, true);
    
    // Check transforms array
    assert_eq!(uniforms[1].name, "transforms");
    assert_eq!(uniforms[1].gl_type, GLType::FloatMat4 as u32);
    assert_eq!(uniforms[1].size, 3);
    assert_eq!(uniforms[1].active, true);
    
    // Check values array
    assert_eq!(uniforms[2].name, "values");
    assert_eq!(uniforms[2].gl_type, GLType::Float as u32);
    assert_eq!(uniforms[2].size, 10);
    assert_eq!(uniforms[2].active, true);
  }

  #[test]
  fn test_parse_glsl_with_macro_array_size() {
    // Test array size defined by preprocessor macro
    // Note: This tests whether the current implementation handles this case
    let source_str = r#"
#version 300 es
precision highp float;

#define NUM_LIGHTS 5

in vec3 position;
uniform vec3 lightPositions[NUM_LIGHTS];

void main() {
  vec3 lighting = vec3(0.0);
  for (int i = 0; i < NUM_LIGHTS; i++) {
    lighting += lightPositions[i];
  }
  gl_Position = vec4(position + lighting * 0.01, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    let result = analyzer.parse(source_str);
    
    // The preprocessor should expand NUM_LIGHTS to 5 before parsing
    assert!(result.is_ok(), "Should successfully parse shader with macro-defined array size");
    
    let uniforms = analyzer.get_uniforms();
    assert_eq!(uniforms.len(), 1);
    assert_eq!(uniforms[0].name, "lightPositions");
    assert_eq!(uniforms[0].gl_type, GLType::FloatVec3 as u32);
    // After preprocessing, the array size should be 5
    assert_eq!(uniforms[0].size, 5, "Preprocessor should expand macro array size");
    assert_eq!(uniforms[0].active, true);
  }

  #[test]
  fn test_single_element_array_uses_bracket_notation() {
    // Test that single-element arrays still use [0] notation
    let source_str = r#"
#version 300 es
precision highp float;

struct HemisphereLight {
  vec3 skyColor;
  vec3 groundColor;
};

in vec3 position;
uniform HemisphereLight hemisphereLights[1];

void main() {
  vec3 color = hemisphereLights[0].skyColor + hemisphereLights[0].groundColor;
  gl_Position = vec4(position + color * 0.01, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse single-element array shader");
    
    let uniforms = analyzer.get_uniforms();
    // Should expand to hemisphereLights[0].skyColor and hemisphereLights[0].groundColor
    assert_eq!(uniforms.len(), 2);
    
    // Both should use [0] notation, not just "hemisphereLights.skyColor"
    assert_eq!(uniforms[0].name, "hemisphereLights[0].skyColor");
    assert_eq!(uniforms[0].active, true);
    
    assert_eq!(uniforms[1].name, "hemisphereLights[0].groundColor");
    assert_eq!(uniforms[1].active, true);
  }

  #[test]
  fn test_variable_expansion_tracking() {
    // Test that when a local variable is assigned from a uniform,
    // references to the local variable mark the uniform as active
    let source_str = r#"
#version 300 es
precision highp float;

struct Foo {
  vec3 f1;
  vec3 f2;
};

in vec3 position;
uniform Foo foo[2];

out vec3 vColor;

void main() {
  Foo local_foo;
  local_foo = foo[0];  // Assignment: local_foo is an alias of foo[0]
  
  // Reference to local_foo.f1 should mark foo[0].f1 as active
  vColor = local_foo.f1;
  
  gl_Position = vec4(position, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    analyzer.parse(source_str).expect("Failed to parse variable expansion shader");
    
    let uniforms = analyzer.get_uniforms();
    // Should have foo[0].f1, foo[0].f2, foo[1].f1, foo[1].f2
    assert_eq!(uniforms.len(), 4);
    
    // foo[0].f1 should be active (referenced via local_foo.f1)
    let foo0_f1 = uniforms.iter().find(|u| u.name == "foo[0].f1");
    assert!(foo0_f1.is_some());
    assert_eq!(foo0_f1.unwrap().active, true, "foo[0].f1 should be active through local_foo.f1");
    
    // foo[0].f2 should be inactive (not referenced)
    let foo0_f2 = uniforms.iter().find(|u| u.name == "foo[0].f2");
    assert!(foo0_f2.is_some());
    assert_eq!(foo0_f2.unwrap().active, false, "foo[0].f2 should be inactive");
    
    // foo[1].* should all be inactive
    let foo1_f1 = uniforms.iter().find(|u| u.name == "foo[1].f1");
    assert!(foo1_f1.is_some());
    assert_eq!(foo1_f1.unwrap().active, false, "foo[1].f1 should be inactive");
    
    let foo1_f2 = uniforms.iter().find(|u| u.name == "foo[1].f2");
    assert!(foo1_f2.is_some());
    assert_eq!(foo1_f2.unwrap().active, false, "foo[1].f2 should be inactive");
  }

  #[test]
  fn test_parse_glsl_no_stack_overflow_on_reassignment() {
    // This test verifies the fix for the stack overflow bug where
    // reassignments like "transformedPos = normalMatrix * transformedPos"
    // caused infinite recursion in collect_var_names
    let source_str = r#"
in vec3 position;
uniform mat3 normalMatrix;

void main() {
    vec3 transformedPos = position;
    transformedPos = normalMatrix * transformedPos;
    gl_Position = vec4(transformedPos, 1.0);
}
"#;
    let mut analyzer = GLSLShaderAnalyzer::new();
    // This should not cause a stack overflow
    analyzer.parse(source_str).expect("Failed to parse shader with reassignment");
    
    // Verify that the uniform is correctly marked as active
    let uniforms = analyzer.get_uniforms();
    assert_eq!(uniforms.len(), 1);
    assert_eq!(uniforms[0].name, "normalMatrix");
    assert_eq!(uniforms[0].active, true, "normalMatrix should be active");
    
    // Verify that the attribute is correctly found
    let attributes = analyzer.get_attributes();
    assert_eq!(attributes.len(), 1);
    assert_eq!(attributes[0].name, "position");
    assert_eq!(attributes[0].active, true, "position should be active");
  }
}
