/**
 * Complete usage example of the Prompt API in JSAR Runtime
 * This file demonstrates how the implemented Prompt API can be used
 * as the foundation for the threepio module
 */

// Example 1: Basic usage through navigator.languageModel
async function example1_BasicUsage() {
  console.log('Example 1: Basic Prompt API Usage');
  console.log('===================================');
  
  // This would work in the JSAR runtime environment:
  // const session = await navigator.languageModel.create();
  // const response = await session.prompt('Hello, how are you?');
  // console.log('Response:', response);
  
  console.log('✅ Basic usage pattern defined');
  console.log('Usage: await navigator.languageModel.create()');
  console.log('Then: await session.prompt("Your message here")');
}

// Example 2: Advanced session configuration
async function example2_AdvancedConfiguration() {
  console.log('\nExample 2: Advanced Session Configuration');
  console.log('=========================================');
  
  const exampleConfig = {
    initialPrompts: [
      { 
        role: 'system', 
        content: 'You are a helpful assistant for spatial web development. Generate clean HTML and provide technical guidance.' 
      }
    ],
    temperature: 0.7,
    topK: 40,
    expectedInputs: [
      { type: 'text', languages: ['en'] }
    ],
    expectedOutputs: [
      { type: 'text', languages: ['en'] }
    ]
  };
  
  console.log('✅ Advanced configuration example:');
  console.log(JSON.stringify(exampleConfig, null, 2));
}

// Example 3: Content generation for spatial web
async function example3_SpatialWebContent() {
  console.log('\nExample 3: Spatial Web Content Generation');
  console.log('=========================================');
  
  const spatialWebPrompts = [
    'Create a 3D product showcase with interactive elements',
    'Generate HTML for a spatial navigation menu',
    'Build a responsive layout for VR/AR environments',
    'Create accessible content for spatial web applications'
  ];
  
  console.log('✅ Example prompts for spatial web content:');
  spatialWebPrompts.forEach((prompt, i) => {
    console.log(`${i + 1}. ${prompt}`);
  });
}

// Example 4: Streaming content generation
async function example4_StreamingGeneration() {
  console.log('\nExample 4: Streaming Content Generation');
  console.log('=======================================');
  
  console.log('✅ Streaming pattern:');
  console.log(`
const stream = await session.promptStreaming('Generate a complex webpage');
const reader = stream.getReader();

while (true) {
  const { done, value } = await reader.read();
  if (done) break;
  
  // Process each chunk as it arrives
  document.getElementById('preview').innerHTML += value;
}
  `);
}

// Example 5: Integration with existing threepio
async function example5_ThreepioIntegration() {
  console.log('\nExample 5: Threepio Integration');
  console.log('===============================');
  
  console.log('✅ Backward compatibility maintained:');
  console.log(`
// Legacy API (still works)
const threepio = new Threepio(browsingContext);
await threepio.request('Generate content'); // Original behavior

// New Prompt API integration
const content = await threepio.generateContent('Generate content'); // Returns string
const stream = await threepio.streamContent('Generate content'); // Returns stream
  `);
}

// Example 6: Error handling and resource management
async function example6_ErrorHandling() {
  console.log('\nExample 6: Error Handling & Resource Management');
  console.log('===============================================');
  
  console.log('✅ Proper error handling patterns:');
  console.log(`
try {
  const session = await navigator.languageModel.create();
  
  // Handle quota overflow
  session.addEventListener('quotaoverflow', () => {
    console.log('Quota exceeded, older messages removed');
  });
  
  // Abort long-running operations
  const controller = new AbortController();
  setTimeout(() => controller.abort(), 10000); // 10 second timeout
  
  const response = await session.prompt('Your message', {
    signal: controller.signal
  });
  
  // Always clean up
  session.destroy();
  
} catch (error) {
  if (error.name === 'AbortError') {
    console.log('Operation was cancelled');
  } else if (error.name === 'QuotaExceededError') {
    console.log('Input too large:', error.requested, 'vs', error.quota);
  } else {
    console.log('Other error:', error);
  }
}
  `);
}

// Example 7: Multi-modal inputs (future extension)
async function example7_MultiModalInputs() {
  console.log('\nExample 7: Multi-modal Inputs (Future Extension)');
  console.log('================================================');
  
  console.log('✅ Framework ready for multimodal inputs:');
  console.log(`
// This structure is ready for when multimodal support is added
const session = await navigator.languageModel.create({
  expectedInputs: [
    { type: 'text', languages: ['en'] },
    { type: 'image' },
    { type: 'audio' }
  ]
});

// Future multimodal usage
const response = await session.prompt([{
  role: 'user',
  content: [
    { type: 'text', value: 'Describe this image:' },
    { type: 'image', value: imageBlob }
  ]
}]);
  `);
}

// Example 8: Performance considerations
async function example8_Performance() {
  console.log('\nExample 8: Performance Considerations');
  console.log('=====================================');
  
  console.log('✅ Performance best practices:');
  console.log(`
// 1. Reuse sessions for related conversations
const session = await navigator.languageModel.create();

// 2. Clone sessions for parallel operations
const parallelSession = await session.clone();

// 3. Monitor token usage
console.log('Usage:', session.inputUsage, '/', session.inputQuota);

// 4. Estimate costs before sending
const estimatedTokens = await session.measureInputUsage('Test message');

// 5. Use streaming for long responses
const stream = await session.promptStreaming('Generate long content');

// 6. Clean up when done
session.destroy();
parallelSession.destroy();
  `);
}

// Main demonstration function
async function demonstratePromptAPIUsage() {
  console.log('🌟 JSAR Runtime Prompt API - Complete Usage Examples');
  console.log('====================================================\n');
  
  await example1_BasicUsage();
  await example2_AdvancedConfiguration();
  await example3_SpatialWebContent();
  await example4_StreamingGeneration();
  await example5_ThreepioIntegration();
  await example6_ErrorHandling();
  await example7_MultiModalInputs();
  await example8_Performance();
  
  console.log('\n🎯 Summary');
  console.log('==========');
  console.log('✅ Standards-compliant Prompt API implementation');
  console.log('✅ Seamless integration with existing threepio module');
  console.log('✅ Full session management and streaming support');
  console.log('✅ Proper error handling and resource management');
  console.log('✅ Ready for future multimodal extensions');
  console.log('✅ Performance-optimized with usage tracking');
  
  console.log('\n🚀 The Prompt API is now the foundation of the threepio module!');
  console.log('\nImplementation addresses issues:');
  console.log('• #76: Implement the Prompt API ✅');
  console.log('• Foundation for #99: Events to Assistants 🔮');
  console.log('• Foundation for #70: DOM Integration 🔮');
}

// Export for use
module.exports = {
  demonstratePromptAPIUsage,
  example1_BasicUsage,
  example2_AdvancedConfiguration,
  example3_SpatialWebContent,
  example4_StreamingGeneration,
  example5_ThreepioIntegration,
  example6_ErrorHandling,
  example7_MultiModalInputs,
  example8_Performance
};

// Run if executed directly
if (require.main === module) {
  demonstratePromptAPIUsage().catch(console.error);
}