/**
 * Comprehensive example of the Prompt API implementation
 * Demonstrates various features according to the specification
 */

import { LanguageModel } from '../navigator/prompt-api';

async function demonstratePromptAPI() {
  console.log('🚀 Comprehensive Prompt API Demonstration\n');

  try {
    // 1. Basic API availability and capabilities
    console.log('1. 📊 Checking API availability and parameters...');
    
    const availability = await LanguageModel.availability();
    console.log('   Availability:', availability);
    
    const params = await LanguageModel.params();
    console.log('   Parameters:', params);

    // 2. Zero-shot prompting
    console.log('\n2. 💬 Zero-shot prompting...');
    
    const session = await LanguageModel.create();
    const poem = await session.prompt('Write me a poem about coding.');
    console.log('   Poem:', poem);

    // 3. System prompts
    console.log('\n3. 🤖 System prompts...');
    
    const systemSession = await LanguageModel.create({
      initialPrompts: [
        { role: 'system', content: 'Pretend to be an eloquent hamster.' }
      ]
    });
    
    const hamsterResponse = await systemSession.prompt('What is your favorite food?');
    console.log('   Hamster response:', hamsterResponse);

    // 4. N-shot prompting with examples
    console.log('\n4. 🎯 N-shot prompting with examples...');
    
    const fewShotSession = await LanguageModel.create({
      initialPrompts: [
        { role: 'system', content: 'Predict up to 5 emojis as a response to a comment. Output emojis, comma-separated.' },
        { role: 'user', content: 'This is amazing!' },
        { role: 'assistant', content: '❤️, ➕' },
        { role: 'user', content: 'LGTM' },
        { role: 'assistant', content: '👍, 🚢' }
      ]
    });
    
    const emojiResponse = await fewShotSession.prompt('Back to the drawing board');
    console.log('   Emoji prediction:', emojiResponse);

    // 5. Streaming responses
    console.log('\n5. 🌊 Streaming responses...');
    
    const streamingSession = await LanguageModel.create();
    const stream = await streamingSession.promptStreaming('Tell me a story about AI and humans working together');
    
    console.log('   Story (streaming):');
    const reader = stream.getReader();
    let storyContent = '';
    
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      storyContent += value;
      process.stdout.write(value);
    }
    console.log('\n   Story complete!');

    // 6. Session cloning for parallel conversations
    console.log('\n6. 👯 Session cloning...');
    
    const originalSession = await LanguageModel.create({
      initialPrompts: [
        { role: 'system', content: 'You are a helpful assistant specialized in web development.' }
      ]
    });
    
    const clonedSession = await originalSession.clone();
    
    const [originalResponse, clonedResponse] = await Promise.all([
      originalSession.prompt('What is React?'),
      clonedSession.prompt('What is Vue.js?')
    ]);
    
    console.log('   Original session (React):', originalResponse);
    console.log('   Cloned session (Vue.js):', clonedResponse);

    // 7. Session management and usage tracking
    console.log('\n7. 📈 Session management and usage tracking...');
    
    const managedSession = await LanguageModel.create({
      temperature: 0.8,
      topK: 50
    });
    
    console.log('   Initial usage:', managedSession.inputUsage, '/', managedSession.inputQuota);
    
    await managedSession.prompt('This is a test message to measure token usage.');
    console.log('   After prompt usage:', managedSession.inputUsage, '/', managedSession.inputQuota);
    
    const estimatedUsage = await managedSession.measureInputUsage('Another test message');
    console.log('   Estimated usage for next message:', estimatedUsage, 'tokens');

    // 8. Multiple message format
    console.log('\n8. 💬 Multiple message format...');
    
    const multiSession = await LanguageModel.create();
    const multiResponse = await multiSession.prompt([
      { role: 'user', content: 'I need help with JavaScript' },
      { role: 'user', content: 'Specifically about async/await' }
    ]);
    
    console.log('   Multi-message response:', multiResponse);

    // 9. Error handling and abort signals
    console.log('\n9. ⚠️ Error handling and abort signals...');
    
    const abortController = new AbortController();
    const abortSession = await LanguageModel.create();
    
    // Start a prompt but abort it quickly
    setTimeout(() => abortController.abort(), 100);
    
    try {
      await abortSession.prompt('This will be aborted', { signal: abortController.signal });
    } catch (error) {
      console.log('   Successfully caught aborted operation:', error.name);
    }

    // 10. Cleanup
    console.log('\n10. 🧹 Cleanup...');
    
    session.destroy();
    systemSession.destroy();
    fewShotSession.destroy();
    streamingSession.destroy();
    originalSession.destroy();
    clonedSession.destroy();
    managedSession.destroy();
    multiSession.destroy();
    abortSession.destroy();
    
    console.log('   All sessions destroyed.');

    console.log('\n✅ Comprehensive Prompt API demonstration completed successfully!');
    console.log('\nThe Prompt API is now ready to serve as the basis for the threepio module.');

  } catch (error) {
    console.error('❌ Demonstration failed:', error);
  }
}

// Export the demonstration function
export { demonstratePromptAPI };

// Run the demo if this file is executed directly
if (require.main === module) {
  demonstratePromptAPI();
}