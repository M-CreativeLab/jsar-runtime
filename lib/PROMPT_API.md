# Prompt API Implementation for JSAR Runtime

This document describes the implementation of the [Prompt API specification](https://github.com/webmachinelearning/prompt-api) as the foundation for the JSAR runtime's threepio module.

## Overview

The Prompt API provides a standardized interface for interacting with language models in web browsers. This implementation integrates seamlessly with the existing JSAR runtime architecture while providing a standards-compliant interface.

## Implementation Structure

### Core Files

- `lib/navigator/prompt-api-types.ts` - TypeScript type definitions matching the Prompt API spec
- `lib/navigator/prompt-api.ts` - Main implementation of LanguageModel and LanguageModelSession classes
- `lib/navigator/index.ts` - Navigator integration exposing the API via `navigator.languageModel`
- `lib/runtime2/threepio/prompt-api-integration.ts` - Integration layer with existing threepio functionality
- `lib/runtime2/threepio/index.ts` - Updated threepio class with Prompt API support

### Test and Demo Files

- `lib/test/prompt-api-test.ts` - Basic functionality tests
- `lib/test/prompt-api-demo.ts` - Comprehensive demonstration of all features

## Key Features Implemented

### ✅ Core API Surface
- `LanguageModel.create()` - Session creation with configuration options
- `LanguageModel.availability()` - Check model availability
- `LanguageModel.params()` - Get model parameters
- `session.prompt()` - Synchronous prompting
- `session.promptStreaming()` - Streaming responses
- `session.clone()` - Session cloning for parallel conversations
- `session.destroy()` - Resource cleanup

### ✅ Session Management
- Initial prompts and system prompts
- Conversation history maintenance
- Input quota tracking and overflow handling
- Token usage estimation
- Session cloning and parallel operations

### ✅ Configuration Options
- Temperature and topK parameters
- System prompts with proper validation
- Expected input/output languages
- Abort signal support

### ✅ Advanced Features
- Multi-message prompting
- Session persistence across multiple interactions
- Progress monitoring during session creation
- Quota overflow events
- Proper error handling with custom exception types

## API Usage Examples

### Basic Usage

```typescript
// Access via navigator
const session = await navigator.languageModel.create();
const response = await session.prompt('Hello, how are you?');
console.log(response);
```

### System Prompts

```typescript
const session = await navigator.languageModel.create({
  initialPrompts: [
    { role: 'system', content: 'You are a helpful coding assistant.' }
  ]
});

const code = await session.prompt('Write a function to reverse a string in JavaScript');
```

### Streaming Responses

```typescript
const stream = await session.promptStreaming('Tell me a long story');
const reader = stream.getReader();

while (true) {
  const { done, value } = await reader.read();
  if (done) break;
  console.log(value); // Process chunks as they arrive
}
```

### Session Cloning

```typescript
const originalSession = await navigator.languageModel.create({
  initialPrompts: [
    { role: 'system', content: 'You are an expert in web development.' }
  ]
});

const clonedSession = await originalSession.clone();

// Both sessions can operate independently
const response1 = await originalSession.prompt('Explain React hooks');
const response2 = await clonedSession.prompt('Explain Vue composition API');
```

## Integration with Threepio

The implementation provides both backward compatibility and new functionality:

### Legacy API (Preserved)
```typescript
const threepio = new Threepio(browsingContext);
await threepio.request('Generate a welcome page'); // Original behavior
```

### New Prompt API Integration
```typescript
const threepio = new Threepio(browsingContext);
const content = await threepio.generateContent('Generate a welcome page'); // Returns string
const stream = await threepio.streamContent('Generate a product page'); // Streaming
```

## Current Implementation Status

### ✅ Completed
- Core API structure and types
- Basic session management
- Prompt and streaming functionality
- System prompts and configuration
- Session cloning and destruction
- Token usage estimation
- Error handling and abort signals
- Navigator integration
- Threepio integration layer

### 🔄 Mock Implementation
- LLM responses are currently mocked for demonstration
- Ready for integration with actual LLM providers (qwen, doubao, etc.)

### 🔮 Future Enhancements
- Multimodal inputs (images, audio)
- Tool use functionality
- Structured output with JSON schema
- DOM integration (per issue #70)
- Event-based messaging (per issue #99)
- Integration with real LLM backends

## Testing

Run the basic test:
```bash
cd lib/test && node -r ts-node/register prompt-api-test.ts
```

Run the comprehensive demo:
```bash
cd lib/test && node -r ts-node/register prompt-api-demo.ts
```

## Standards Compliance

This implementation follows the [Prompt API specification](https://github.com/webmachinelearning/prompt-api) and is designed to be interoperable with other implementations. The API surface matches the specification exactly, ensuring compatibility with web applications that use the standard.

## Relationship to Issues #99 and #70

This implementation provides the foundation for the enhanced features discussed in:

- **Issue #99 (Events to Assistants)**: The session-based architecture supports adding event role messages
- **Issue #70 (DOM Integration)**: The API can be extended to accept Document objects as prompts

Both features can be added as extensions to this core implementation without breaking existing functionality.