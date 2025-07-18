/**
 * Example demonstrating the Prompt API integration with threepio
 * Shows how the Prompt API can serve as the basis for the threepio module
 */

import { LanguageModel } from '../navigator/prompt-api';

export class ThreepioPromptIntegration {
  private session?: any;

  /**
   * Initialize a language model session for threepio operations
   */
  async initialize(): Promise<void> {
    this.session = await LanguageModel.create({
      initialPrompts: [
        { 
          role: 'system', 
          content: `You are a helpful AI assistant that helps generate and modify HTML content for spatial web applications. 
          You understand HTML, CSS, and spatial web concepts. When asked to generate content, provide clean, 
          semantic HTML that works well in 3D spatial environments.` 
        }
      ],
      temperature: 0.7,
      expectedInputs: [
        { type: 'text', languages: ['en'] }
      ],
      expectedOutputs: [
        { type: 'text', languages: ['en'] }
      ]
    });
  }

  /**
   * Generate HTML content using the Prompt API
   * This replaces the existing threepio request method
   */
  async generateContent(userRequest: string): Promise<string> {
    if (!this.session) {
      await this.initialize();
    }

    const prompt = `User request: ${userRequest}

Please generate appropriate HTML content for this request. Focus on:
1. Clean, semantic HTML structure
2. Accessibility considerations
3. Spatial web compatibility
4. Responsive design principles

Return only the HTML content without additional commentary.`;

    return await this.session.prompt(prompt);
  }

  /**
   * Stream content generation for real-time updates
   */
  async streamContent(userRequest: string): Promise<ReadableStream<string>> {
    if (!this.session) {
      await this.initialize();
    }

    const prompt = `User request: ${userRequest}

Please generate appropriate HTML content for this request. Stream the content progressively.`;

    return await this.session.promptStreaming(prompt);
  }

  /**
   * Get conversation history and usage statistics
   */
  getSessionStats() {
    if (!this.session) {
      return null;
    }

    return {
      inputUsage: this.session.inputUsage,
      inputQuota: this.session.inputQuota,
      usagePercentage: (this.session.inputUsage / this.session.inputQuota) * 100
    };
  }

  /**
   * Clone the session for parallel operations
   */
  async createParallelSession() {
    if (!this.session) {
      await this.initialize();
    }

    return await this.session.clone();
  }

  /**
   * Clean up resources
   */
  destroy(): void {
    if (this.session) {
      this.session.destroy();
      this.session = null;
    }
  }
}

// Example usage
export async function demonstrateThreepioIntegration() {
  console.log('Demonstrating Threepio integration with Prompt API...');

  const threepio = new ThreepioPromptIntegration();
  
  try {
    // Generate some content
    console.log('\n1. Generating HTML content...');
    const htmlContent = await threepio.generateContent(
      'Create a welcome page for a spatial web application with a title, description, and navigation menu'
    );
    console.log('Generated HTML:', htmlContent);

    // Check usage stats
    console.log('\n2. Session statistics:');
    console.log(threepio.getSessionStats());

    // Demonstrate streaming
    console.log('\n3. Streaming content generation...');
    const stream = await threepio.streamContent(
      'Create a product showcase with 3 example products'
    );
    
    const reader = stream.getReader();
    let streamedContent = '';
    
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      streamedContent += value;
      process.stdout.write(value);
    }
    
    console.log('\n\nStreaming complete!');

    // Clean up
    threepio.destroy();
    
    console.log('\n✅ Threepio integration demo completed successfully!');

  } catch (error) {
    console.error('❌ Demo failed:', error);
    threepio.destroy();
  }
}

// Export the integration class and demo function
export { LanguageModel };