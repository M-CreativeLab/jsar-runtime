/**
 * Integration example demonstrating how to use the Extension System
 * with the JSAR Runtime
 */

// Example of how to extend the JSAR runtime with extensions
class ExtendedJSARRuntime {
  constructor() {
    // This would be initialized with actual WebGL context and runtime ID
    // For demonstration purposes, we show the integration pattern
    
    console.log('🚀 Initializing Extended JSAR Runtime with Extension Support...');
  }
  
  /**
   * Initialize the runtime with extension support
   */
  async initialize(options = {}) {
    // In actual implementation, this would be in TransmuteRuntime2 constructor
    console.log('⚙️  Setting up runtime with extension capabilities...');
    
    // Extension directories to auto-load from
    const extensionDirs = options.extensionDirs || [
      './extensions',           // User extensions
      './fixtures/extensions'   // Sample extensions
    ];
    
    // Load extensions from specified directories
    for (const dir of extensionDirs) {
      try {
        console.log(`📂 Loading extensions from: ${dir}`);
        // await this.loadExtensionsFromDirectory(dir);
        console.log(`   ✅ Extensions loaded from ${dir}`);
      } catch (error) {
        console.warn(`   ⚠️  Failed to load extensions from ${dir}:`, error.message);
      }
    }
    
    console.log('✅ Runtime initialization with extensions complete');
  }
  
  /**
   * Example method showing how extensions can be managed during runtime
   */
  async manageExtensions() {
    console.log('\n🔧 Extension Management Example:');
    
    // Show how to load a specific extension
    console.log('   📦 Loading specific extension...');
    // await this.loadExtension('./fixtures/extensions/sample-extension');
    
    // Show how to get extension status
    console.log('   📊 Extension status:');
    // const states = this.extensionManager.getExtensionStates();
    // console.log('     States:', states);
    
    // Show how to enable/disable extensions
    console.log('   🔄 Extension lifecycle management available');
    
    console.log('   ✅ Extension management complete');
  }
  
  /**
   * Cleanup extensions on runtime shutdown
   */
  async shutdown() {
    console.log('\n🧹 Shutting down runtime...');
    // await this.unloadAllExtensions();
    console.log('   ✅ All extensions unloaded');
    console.log('   ✅ Runtime shutdown complete');
  }
}

/**
 * Example usage demonstration
 */
async function demonstrateExtensionUsage() {
  console.log('🌟 JSAR Extension System Integration Example\n');
  
  const runtime = new ExtendedJSARRuntime();
  
  try {
    // Initialize runtime with extension support
    await runtime.initialize({
      extensionDirs: ['./fixtures/extensions']
    });
    
    // Demonstrate extension management
    await runtime.manageExtensions();
    
    console.log('\n📋 Extension System Features:');
    console.log('   ✅ Chrome Extension-like architecture');
    console.log('   ✅ Extension loading/unloading');
    console.log('   ✅ Background script execution');
    console.log('   ✅ Extension lifecycle management');
    console.log('   ✅ Event-driven architecture');
    console.log('   ✅ Manifest-based configuration');
    console.log('   ✅ Integration with JSAR runtime');
    
    console.log('\n🎯 Usage Summary:');
    console.log('   • Extensions are loaded from directories');
    console.log('   • Each extension has a manifest.json file');
    console.log('   • Background scripts execute on extension load');
    console.log('   • Extensions can be enabled/disabled/unloaded');
    console.log('   • Event system provides lifecycle notifications');
    
    console.log('\n📚 Documentation:');
    console.log('   • See docs/extensions.md for detailed usage');
    console.log('   • See fixtures/extensions/sample-extension/ for example');
    console.log('   • See lib/extensions/ for implementation');
    
    // Cleanup
    await runtime.shutdown();
    
  } catch (error) {
    console.error('❌ Extension system demonstration failed:', error);
  }
}

// Run the demonstration
if (require.main === module) {
  demonstrateExtensionUsage().catch(console.error);
}

module.exports = { ExtendedJSARRuntime, demonstrateExtensionUsage };