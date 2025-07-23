#!/usr/bin/env node

/**
 * Test script for JSAR Extension System
 * This script demonstrates loading and managing extensions
 */

const { ExtensionManager, ExtensionState } = require('../lib/extensions');
const path = require('path');

async function testExtensionSystem() {
  console.log('🚀 Testing JSAR Extension System...\n');
  
  const extensionManager = new ExtensionManager();
  const sampleExtensionPath = path.join(__dirname, '../fixtures/extensions/sample-extension');
  
  try {
    console.log('📦 Loading sample extension...');
    const extension = await extensionManager.loadExtension(sampleExtensionPath);
    
    console.log(`✅ Extension loaded successfully!`);
    console.log(`   ID: ${extension.id}`);
    console.log(`   Name: ${extension.manifest.name}`);
    console.log(`   Version: ${extension.manifest.version}`);
    console.log(`   State: ${extension.state}`);
    
    // Test extension states
    console.log('\n🔄 Testing extension state management...');
    
    await extensionManager.disableExtension(extension.id);
    console.log(`   Extension disabled. State: ${extension.state}`);
    
    await extensionManager.enableExtension(extension.id);
    console.log(`   Extension enabled. State: ${extension.state}`);
    
    // Show extension summary
    console.log('\n📊 Extension Summary:');
    const states = extensionManager.getExtensionStates();
    console.log('   States:', states);
    
    const runningExtensions = extensionManager.getExtensionsByState(ExtensionState.RUNNING);
    console.log(`   Running extensions: ${runningExtensions.length}`);
    
    // Cleanup
    console.log('\n🧹 Cleaning up...');
    await extensionManager.unloadAllExtensions();
    console.log('   All extensions unloaded');
    
    console.log('\n✅ Extension system test completed successfully!');
    
  } catch (error) {
    console.error('❌ Extension system test failed:', error);
    process.exit(1);
  }
}

// Run the test
if (require.main === module) {
  testExtensionSystem().catch(console.error);
}

module.exports = { testExtensionSystem };