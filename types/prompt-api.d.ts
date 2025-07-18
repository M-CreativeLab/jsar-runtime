/**
 * TypeScript declarations for the Prompt API extension to Navigator
 * Extends the standard Navigator interface with the languageModel property
 */

import { LanguageModelStatic } from '../lib/navigator/prompt-api-types';

declare global {
  interface Navigator {
    /**
     * The Prompt API provides access to language models for text generation.
     * Based on the specification: https://github.com/webmachinelearning/prompt-api
     */
    readonly languageModel: LanguageModelStatic;
  }
}

export {};