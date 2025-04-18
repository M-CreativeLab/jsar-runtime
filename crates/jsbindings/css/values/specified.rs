use style::values::{
  computed as StyleComputedValues, generics as StyleGenericsValues,
  specified as StyleSpecifiedValues, CSSFloat,
};
use style_traits::{CssWriter, ToCss};

#[derive(Clone, Debug)]
pub(crate) struct Number(StyleSpecifiedValues::Number);

impl Number {
  #[inline]
  pub fn was_calc(&self) -> bool {
    self.0.was_calc()
  }

  #[inline]
  pub fn get_value(&self) -> f32 {
    self.0.get()
  }

  #[inline]
  pub fn to_percentage(&self) -> Percentage {
    self.0.to_percentage().into()
  }
}

impl From<StyleSpecifiedValues::Number> for Number {
  fn from(number: StyleSpecifiedValues::Number) -> Self {
    Self(number)
  }
}

impl From<&StyleSpecifiedValues::Number> for Number {
  fn from(number: &StyleSpecifiedValues::Number) -> Self {
    number.clone().into()
  }
}

#[derive(Clone, Debug)]
pub(crate) struct Angle(StyleSpecifiedValues::Angle);

impl From<StyleSpecifiedValues::Angle> for Angle {
  fn from(angle: StyleSpecifiedValues::Angle) -> Self {
    Self(angle)
  }
}

impl From<&StyleSpecifiedValues::Angle> for Angle {
  fn from(angle: &StyleSpecifiedValues::Angle) -> Self {
    angle.clone().into()
  }
}

#[derive(Clone, Debug)]
pub(crate) struct Percentage(StyleSpecifiedValues::Percentage);

impl Default for Percentage {
  fn default() -> Self {
    Self::new(0.0)
  }
}

impl From<StyleSpecifiedValues::Percentage> for Percentage {
  fn from(percentage: StyleSpecifiedValues::Percentage) -> Self {
    Self(percentage)
  }
}

impl From<StyleComputedValues::Percentage> for Percentage {
  fn from(value: StyleComputedValues::Percentage) -> Self {
    Self::new(value.0)
  }
}

impl ToCss for Percentage {
  fn to_css<W>(&self, dest: &mut CssWriter<W>) -> std::fmt::Result
  where
    W: std::fmt::Write,
  {
    self.0.to_css(dest)
  }
}

impl Percentage {
  #[inline]
  pub fn new(percent: f32) -> Self {
    Self(StyleSpecifiedValues::Percentage::new(percent))
  }

  #[inline]
  pub fn get_percent(&self) -> CSSFloat {
    self.0.get()
  }
}

pub(crate) struct NoCalcLength(StyleSpecifiedValues::NoCalcLength);

impl Default for NoCalcLength {
  fn default() -> Self {
    Self(StyleSpecifiedValues::NoCalcLength::Absolute(
      StyleSpecifiedValues::AbsoluteLength::Px(0.0),
    ))
  }
}

impl From<StyleSpecifiedValues::NoCalcLength> for NoCalcLength {
  fn from(length: StyleSpecifiedValues::NoCalcLength) -> Self {
    Self(length)
  }
}

impl NoCalcLength {
  pub fn unitless_value(&self) -> CSSFloat {
    self.0.unitless_value()
  }

  pub fn unit(&self) -> &str {
    use StyleSpecifiedValues::{
      AbsoluteLength, FontRelativeLength, NoCalcLength, ViewportPercentageLength,
    };

    match self.0 {
      NoCalcLength::Absolute(ref value) => match value {
        AbsoluteLength::Px(..) => "px",
        AbsoluteLength::Pt(..) => "pt",
        AbsoluteLength::Pc(..) => "pc",
        AbsoluteLength::Mm(..) => "mm",
        AbsoluteLength::Cm(..) => "cm",
        AbsoluteLength::In(..) => "in",
        AbsoluteLength::Q(..) => "q",
      },
      NoCalcLength::FontRelative(ref value) => match value {
        FontRelativeLength::Em(..) => "em",
        FontRelativeLength::Ex(..) => "ex",
        FontRelativeLength::Ch(..) => "ch",
        FontRelativeLength::Cap(..) => "cap",
        FontRelativeLength::Ic(..) => "ic",
        FontRelativeLength::Rem(..) => "rem",
        FontRelativeLength::Lh(..) => "lh",
        FontRelativeLength::Rlh(..) => "rlh",
      },
      NoCalcLength::ViewportPercentage(ref value) => match value {
        ViewportPercentageLength::Vw(..) => "vw",
        ViewportPercentageLength::Vh(..) => "vh",
        ViewportPercentageLength::Vmin(..) => "vmin",
        ViewportPercentageLength::Vmax(..) => "vmax",
        _ => "",
      },
      _ => "",
    }
  }

  pub fn is_zero(&self) -> bool {
    self.0.is_zero()
  }

  pub fn is_infinite(&self) -> bool {
    self.0.is_infinite()
  }

  pub fn is_nan(&self) -> bool {
    self.0.is_nan()
  }

  pub fn should_zoom_text(&self) -> bool {
    self.0.should_zoom_text()
  }
}

#[derive(Clone, Debug)]
pub(crate) struct Length(StyleSpecifiedValues::Length);

impl From<StyleSpecifiedValues::Length> for Length {
  fn from(length: StyleSpecifiedValues::Length) -> Self {
    Self(length)
  }
}

impl From<&StyleSpecifiedValues::Length> for Length {
  fn from(length: &StyleSpecifiedValues::Length) -> Self {
    length.clone().into()
  }
}

impl Length {
  pub fn is_calc(&self) -> bool {
    matches!(self.0, StyleSpecifiedValues::Length::Calc(..))
  }

  pub fn to_no_calc_length(&self) -> NoCalcLength {
    match &self.0 {
      StyleSpecifiedValues::Length::NoCalc(value) => value.clone().into(),
      _ => NoCalcLength::default(),
    }
  }
}

#[derive(Clone, Debug)]
pub(crate) struct LengthPercentage(StyleSpecifiedValues::LengthPercentage);

impl From<StyleSpecifiedValues::LengthPercentage> for LengthPercentage {
  fn from(style_value: StyleSpecifiedValues::LengthPercentage) -> Self {
    Self(style_value)
  }
}

impl From<&StyleSpecifiedValues::LengthPercentage> for LengthPercentage {
  fn from(style_value: &StyleSpecifiedValues::LengthPercentage) -> Self {
    style_value.clone().into()
  }
}

impl LengthPercentage {
  #[inline]
  pub fn is_calc(&self) -> bool {
    matches!(self.0, StyleSpecifiedValues::LengthPercentage::Calc(..))
  }

  #[inline]
  pub fn is_no_calc_length(&self) -> bool {
    matches!(self.0, StyleSpecifiedValues::LengthPercentage::Length(..))
  }

  pub fn is_percentage(&self) -> bool {
    matches!(
      self.0,
      StyleSpecifiedValues::LengthPercentage::Percentage(..)
    )
  }

  pub fn to_no_calc_length(&self) -> NoCalcLength {
    match &self.0 {
      StyleSpecifiedValues::LengthPercentage::Length(value) => value.clone().into(),
      _ => NoCalcLength::default(),
    }
  }

  pub fn to_percentage(&self) -> Percentage {
    match &self.0 {
      StyleSpecifiedValues::LengthPercentage::Percentage(value) => value.clone().into(),
      _ => Percentage::default(),
    }
  }
}

#[derive(Clone, Debug)]
pub(crate) struct NumberMatrix(pub StyleGenericsValues::transform::Matrix<Number>);

impl NumberMatrix {
  #[inline]
  pub fn a(&self) -> &Number {
    &self.0.a
  }

  #[inline]
  pub fn b(&self) -> &Number {
    &self.0.b
  }

  #[inline]
  pub fn c(&self) -> &Number {
    &self.0.c
  }

  #[inline]
  pub fn d(&self) -> &Number {
    &self.0.d
  }

  #[inline]
  pub fn e(&self) -> &Number {
    &self.0.e
  }

  #[inline]
  pub fn f(&self) -> &Number {
    &self.0.f
  }

  #[inline]
  pub fn item(&self, index: usize) -> &Number {
    match index {
      0 => &self.a(),
      1 => &self.b(),
      2 => &self.c(),
      3 => &self.d(),
      4 => &self.e(),
      5 => &self.f(),
      _ => panic!("Invalid index"),
    }
  }
}

impl From<&StyleGenericsValues::transform::Matrix<StyleSpecifiedValues::Number>> for NumberMatrix {
  fn from(matrix: &StyleGenericsValues::transform::Matrix<StyleSpecifiedValues::Number>) -> Self {
    Self(StyleGenericsValues::transform::Matrix {
      a: matrix.a.clone().into(),
      b: matrix.b.clone().into(),
      c: matrix.c.clone().into(),
      d: matrix.d.clone().into(),
      e: matrix.e.clone().into(),
      f: matrix.f.clone().into(),
    })
  }
}

#[derive(Clone, Debug)]
pub(crate) struct NumberMatrix3D(pub StyleGenericsValues::transform::Matrix3D<Number>);

impl NumberMatrix3D {
  pub fn item(&self, row: usize, col: usize) -> &Number {
    match row {
      0 => panic!("Row starts from 1"),
      1 => match col {
        0 => panic!("Column starts from 1"),
        1 => &self.0.m11,
        2 => &self.0.m12,
        3 => &self.0.m13,
        4 => &self.0.m14,
        _ => panic!("Column index out of range"),
      },
      2 => match col {
        0 => panic!("Column starts from 1"),
        1 => &self.0.m21,
        2 => &self.0.m22,
        3 => &self.0.m23,
        4 => &self.0.m24,
        _ => panic!("Column index out of range"),
      },
      3 => match col {
        0 => panic!("Column starts from 1"),
        1 => &self.0.m31,
        2 => &self.0.m32,
        3 => &self.0.m33,
        4 => &self.0.m34,
        _ => panic!("Column index out of range"),
      },
      4 => match col {
        0 => panic!("Column starts from 1"),
        1 => &self.0.m41,
        2 => &self.0.m42,
        3 => &self.0.m43,
        4 => &self.0.m44,
        _ => panic!("Column index out of range"),
      },
      _ => panic!("Row index out of range"),
    }
  }
}

impl From<&StyleGenericsValues::transform::Matrix3D<StyleSpecifiedValues::Number>>
  for NumberMatrix3D
{
  fn from(
    matrix3d: &StyleGenericsValues::transform::Matrix3D<StyleSpecifiedValues::Number>,
  ) -> Self {
    Self(StyleGenericsValues::transform::Matrix3D {
      m11: matrix3d.m11.clone().into(),
      m12: matrix3d.m12.clone().into(),
      m13: matrix3d.m13.clone().into(),
      m14: matrix3d.m14.clone().into(),
      m21: matrix3d.m21.clone().into(),
      m22: matrix3d.m22.clone().into(),
      m23: matrix3d.m23.clone().into(),
      m24: matrix3d.m24.clone().into(),
      m31: matrix3d.m31.clone().into(),
      m32: matrix3d.m32.clone().into(),
      m33: matrix3d.m33.clone().into(),
      m34: matrix3d.m34.clone().into(),
      m41: matrix3d.m41.clone().into(),
      m42: matrix3d.m42.clone().into(),
      m43: matrix3d.m43.clone().into(),
      m44: matrix3d.m44.clone().into(),
    })
  }
}

#[derive(Clone, Debug)]
pub(crate) enum TransformOperation {
  Invalid,
  Matrix(NumberMatrix),
  Matrix3D(NumberMatrix3D),
  Skew(),
  SkewX(),
  SkewY(),
  Translate(LengthPercentage, LengthPercentage),
  TranslateX(LengthPercentage),
  TranslateY(LengthPercentage),
  TranslateZ(Length),
  Translate3D(LengthPercentage, LengthPercentage, Length),
  Scale(Number, Number),
  ScaleX(Number),
  ScaleY(Number),
  ScaleZ(Number),
  Scale3D(Number, Number, Number),
  Rotate(Angle),
  RotateX(Angle),
  RotateY(Angle),
  RotateZ(Angle),
  Rotate3D(Number, Number, Number, Angle),
  // TODO: Add more operations
}

#[allow(unused)]
impl TransformOperation {
  pub fn new(handle: &StyleSpecifiedValues::transform::TransformOperation) -> Self {
    match handle {
      StyleSpecifiedValues::transform::TransformOperation::Matrix(matrix) => {
        TransformOperation::Matrix(matrix.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Matrix3D(matrix3d) => {
        TransformOperation::Matrix3D(matrix3d.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Translate(x, y) => {
        TransformOperation::Translate(x.into(), y.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::TranslateX(value) => {
        TransformOperation::TranslateX(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::TranslateY(value) => {
        TransformOperation::TranslateY(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::TranslateZ(value) => {
        TransformOperation::TranslateZ(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Translate3D(x, y, z) => {
        TransformOperation::Translate3D(x.into(), y.into(), z.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Scale(x, y) => {
        TransformOperation::Scale(x.into(), y.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::ScaleX(value) => {
        TransformOperation::ScaleX(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::ScaleY(value) => {
        TransformOperation::ScaleY(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::ScaleZ(value) => {
        TransformOperation::ScaleZ(value.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Scale3D(x, y, z) => {
        TransformOperation::Scale3D(x.into(), y.into(), z.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Rotate(angle) => {
        TransformOperation::Rotate(angle.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::RotateX(angle) => {
        TransformOperation::RotateX(angle.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::RotateY(angle) => {
        TransformOperation::RotateY(angle.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::RotateZ(angle) => {
        TransformOperation::RotateZ(angle.into())
      }
      StyleSpecifiedValues::transform::TransformOperation::Rotate3D(x, y, z, angle) => {
        TransformOperation::Rotate3D(x.into(), y.into(), z.into(), angle.into())
      }
      _ => TransformOperation::Invalid,
    }
  }

  pub fn try_get_matrix(&self) -> Option<&NumberMatrix> {
    match self {
      TransformOperation::Matrix(matrix) => Some(matrix),
      _ => None,
    }
  }

  pub fn try_get_matrix3d(&self) -> Option<&NumberMatrix3D> {
    match self {
      TransformOperation::Matrix3D(matrix3d) => Some(matrix3d),
      _ => None,
    }
  }

  pub fn try_into_translate(&self) -> Option<(&LengthPercentage, &LengthPercentage)> {
    match self {
      TransformOperation::Translate(x, y) => Some((x, y)),
      _ => None,
    }
  }

  pub fn try_get_translate_x(&self) -> Option<&LengthPercentage> {
    match self {
      TransformOperation::TranslateX(x) => Some(x),
      _ => None,
    }
  }

  pub fn try_into_translate_x(&self) -> Option<LengthPercentage> {
    match self {
      TransformOperation::TranslateX(x) => Some(x.clone()),
      _ => None,
    }
  }

  pub fn try_get_translate_y(&self) -> Option<&LengthPercentage> {
    match self {
      TransformOperation::TranslateY(y) => Some(y),
      _ => None,
    }
  }

  pub fn try_into_translate_y(&self) -> Option<LengthPercentage> {
    match self {
      TransformOperation::TranslateY(y) => Some(y.clone()),
      _ => None,
    }
  }

  pub fn try_get_translate_z(&self) -> Option<&Length> {
    match self {
      TransformOperation::TranslateZ(z) => Some(z),
      _ => None,
    }
  }

  pub fn try_into_translate_z(&self) -> Option<Length> {
    match self {
      TransformOperation::TranslateZ(z) => Some(z.clone()),
      _ => None,
    }
  }

  pub fn try_into_translate3d(&self) -> Option<(LengthPercentage, LengthPercentage, Length)> {
    match self {
      TransformOperation::Translate3D(x, y, z) => Some((x.clone(), y.clone(), z.clone())),
      _ => None,
    }
  }

  pub fn show(&self) {
    println!("TransformOperation({})", "");
  }
}

#[derive(Clone, Debug)]
pub(crate) struct Transform {
  operations: Vec<TransformOperation>,
}

impl Transform {
  pub fn new(handle: StyleSpecifiedValues::transform::Transform) -> Self {
    Self {
      operations: handle
        .0
        .iter()
        .map(|op| TransformOperation::new(op))
        .collect(),
    }
  }

  pub fn len(&self) -> usize {
    self.operations.len()
  }

  pub fn get_operation_ref(&self, index: usize) -> &TransformOperation {
    &self.operations[index]
  }
}

#[derive(Clone, Debug)]
pub(crate) struct SpecifiedProperty<T> {
  handle: T,
}

impl<T> SpecifiedProperty<T>
where
  T: ToCss,
{
  pub fn from(handle: T) -> Self {
    Self { handle }
  }

  pub fn as_handle(&self) -> &T {
    &self.handle
  }

  #[allow(unused)]
  pub fn to_css(&self) -> String {
    self.handle.to_css_string()
  }
}

pub(crate) type GridTemplateComponent =
  SpecifiedProperty<StyleSpecifiedValues::GridTemplateComponent>;
pub(crate) type GridLine = SpecifiedProperty<StyleSpecifiedValues::GridLine>;
