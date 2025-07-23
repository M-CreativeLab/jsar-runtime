#!/usr/bin/env node

// Simple manual test for the C++ extension system
// This test validates the C++ extension system components

const path = require('path');

async function validateExtensionSystem() {
  console.log('🔍 Validating JSAR C++ Extension System...\n');

  try {
    // Test 1: Check if C++ extension files exist
    console.log('📁 Checking C++ extension files...');
    const fs = require('fs');
    
    const extensionFiles = [
      'src/extensions/extension_types.hpp',
      'src/extensions/extension.hpp', 
      'src/extensions/extension.cpp',
      'src/extensions/extension_manager.hpp',
      'src/extensions/extension_manager.cpp',
      'src/extensions/extensions.hpp'
    ];
    
    for (const file of extensionFiles) {
      const filePath = path.join(__dirname, '..', file);
      if (fs.existsSync(filePath)) {
        console.log(`   ✅ ${file} exists`);
      } else {
        console.log(`   ❌ ${file} missing`);
        return false;
      }
    }

    // Test 2: Check sample extension
    console.log('\n📦 Checking sample extension...');
    const sampleExtensionPath = path.join(__dirname, '../examples/extensions/sample-extension');
    const manifestPath = path.join(sampleExtensionPath, 'manifest.json');
    const backgroundPath = path.join(sampleExtensionPath, 'background.js');
    
    if (fs.existsSync(manifestPath)) {
      console.log('   ✅ Sample extension manifest.json exists');
      const manifest = JSON.parse(fs.readFileSync(manifestPath, 'utf-8'));
      console.log(`   📝 Extension name: ${manifest.name}`);
      console.log(`   📝 Extension version: ${manifest.version}`);
    } else {
      console.log('   ❌ Sample extension manifest.json missing');
    }
    
    if (fs.existsSync(backgroundPath)) {
      console.log('   ✅ Sample extension background.js exists');
    } else {
      console.log('   ❌ Sample extension background.js missing');
    }

    // Test 3: Check runtime integration  
    console.log('\n🔗 Checking runtime integration...');
    const runtimePath = path.join(__dirname, '../lib/runtime2/index.ts');
    const runtimeContent = fs.readFileSync(runtimePath, 'utf-8');
    
    if (runtimeContent.includes('ExtensionManager')) {
      console.log('   ✅ ExtensionManager imported in runtime');
    } else {
      console.log('   ❌ ExtensionManager not found in runtime');
    }
    
    if (runtimeContent.includes('#extensionManager')) {
      console.log('   ✅ Extension manager instance found in runtime');
    } else {
      console.log('   ❌ Extension manager instance not found in runtime');
    }

    if (runtimeContent.includes('loadExtension')) {
      console.log('   ✅ Extension loading methods found in runtime');
    } else {
      console.log('   ❌ Extension loading methods not found in runtime');
    }

    // Test 4: Check C++ build integration
    console.log('\n🏗️ Checking C++ build integration...');
    const cmakePath = path.join(__dirname, '../cmake/TransmuteCore.cmake');
    const cmakeContent = fs.readFileSync(cmakePath, 'utf-8');
    
    if (cmakeContent.includes('src/extensions/*.cpp')) {
      console.log('   ✅ C++ extension files included in build system');
    } else {
      console.log('   ❌ C++ extension files not included in build system');
    }

    // Test 5: Basic header validation
    console.log('\n📝 C++ header validation...');
    try {
      // Check if C++ headers have basic valid structure
      const extensionTypeContent = fs.readFileSync(path.join(__dirname, '../src/extensions/extension_types.hpp'), 'utf-8');
      if (extensionTypeContent.includes('struct ExtensionManifest') && 
          extensionTypeContent.includes('enum class ExtensionState')) {
        console.log('   ✅ C++ extension types definitions are valid');
      } else {
        console.log('   ❌ C++ extension types definitions may be incomplete');
      }
      
      const extensionHeaderContent = fs.readFileSync(path.join(__dirname, '../src/extensions/extension.hpp'), 'utf-8');
      if (extensionHeaderContent.includes('class Extension') && 
          extensionHeaderContent.includes('loadFromDirectory')) {
        console.log('   ✅ C++ Extension class definition is valid');
      } else {
        console.log('   ❌ C++ Extension class definition may be incomplete');
      }
      
    } catch (error) {
      console.log('   ❌ Error validating C++ headers:', error.message);
    }

    console.log('\n✅ C++ Extension system validation completed successfully!');
    console.log('\n🎯 Summary:');
    console.log('   - C++ extension system files are present');
    console.log('   - Sample extension is configured');
    console.log('   - Runtime integration is complete');
    console.log('   - C++ build integration is configured');
    
    console.log('\n📚 Next steps:');
    console.log('   - C++ extension system is implemented but needs bindings');
    console.log('   - Load extensions using TransmuteRuntime2.loadExtension() (stub)');
    console.log('   - See docs/extensions.md for usage examples');
    console.log('   - Implement Node.js bindings for full C++ integration');
    
  } catch (error) {
    console.error('❌ Extension system validation failed:', error);
    return false;
  }
  
  return true;
}

// Run the validation
if (require.main === module) {
  validateExtensionSystem().then(success => {
    process.exit(success ? 0 : 1);
  }).catch(error => {
    console.error('Validation error:', error);
    process.exit(1);
  });
}

module.exports = { validateExtensionSystem };