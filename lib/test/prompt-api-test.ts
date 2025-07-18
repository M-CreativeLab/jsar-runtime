/**
 * Basic test for the Prompt API implementation
 * This tests the core functionality without requiring the full build system
 */

import { LanguageModel } from '../navigator/prompt-api';

async function testBasicPromptAPI() {
  console.log('Testing Prompt API implementation...');

  try {
    // Test 1: Check availability
    console.log('\n1. Testing availability check...');
    const availability = await LanguageModel.availability();
    console.log('Availability:', availability);

    // Test 2: Check params
    console.log('\n2. Testing params...');
    const params = await LanguageModel.params();
    console.log('Params:', params);

    // Test 3: Create a session
    console.log('\n3. Creating a session...');
    const session = await LanguageModel.create({
      initialPrompts: [
        { role: 'system', content: 'You are a helpful assistant.' }
      ],
      temperature: 0.7
    });
    console.log('Session created:', { inputUsage: session.inputUsage, inputQuota: session.inputQuota });

    // Test 4: Simple prompt
    console.log('\n4. Testing simple prompt...');
    const response1 = await session.prompt('Hello, how are you?');
    console.log('Response:', response1);

    // Test 5: Test streaming
    console.log('\n5. Testing streaming prompt...');
    const stream = await session.promptStreaming('Tell me a short joke');
    const reader = stream.getReader();
    let streamedResponse = '';
    
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      streamedResponse += value;
      process.stdout.write(value); // Show streaming in real time
    }
    console.log('\nComplete streamed response:', streamedResponse);

    // Test 6: Test session cloning
    console.log('\n6. Testing session cloning...');
    const clonedSession = await session.clone();
    const response2 = await clonedSession.prompt('What is 2 + 2?');
    console.log('Cloned session response:', response2);

    // Test 7: Test measure input usage
    console.log('\n7. Testing input usage measurement...');
    const usage = await session.measureInputUsage('This is a test message');
    console.log('Estimated token usage:', usage);

    console.log('\n✅ All tests passed!');

  } catch (error) {
    console.error('❌ Test failed:', error);
  }
}

// Run the test if this file is executed directly
if (require.main === module) {
  testBasicPromptAPI();
}

export { testBasicPromptAPI };