/**
 * TypeScript types for the Prompt API specification
 * Based on: https://github.com/webmachinelearning/prompt-api
 */

export type PromptRole = "system" | "user" | "assistant";

export interface PromptMessage {
  role: PromptRole;
  content: string | PromptContent[];
  prefix?: boolean; // For assistant messages that serve as prefixes
}

export interface PromptContent {
  type: "text" | "image" | "audio";
  value: string | ImageBitmapSource | AudioBuffer | BufferSource | Blob;
}

export interface ExpectedInput {
  type: "text" | "image" | "audio";
  languages?: string[];
}

export interface ExpectedOutput {
  type: "text"; // Only text outputs are currently supported
  languages?: string[];
}

export interface LanguageModelCreateOptions {
  signal?: AbortSignal;
  monitor?: (monitor: LanguageModelMonitor) => void;
  initialPrompts?: PromptMessage[];
  expectedInputs?: ExpectedInput[];
  expectedOutputs?: ExpectedOutput[];
  temperature?: number;
  topK?: number;
  tools?: PromptTool[];
}

export interface LanguageModelCloneOptions {
  signal?: AbortSignal;
}

export interface PromptOptions {
  signal?: AbortSignal;
  responseConstraint?: object | RegExp;
  omitResponseConstraintInput?: boolean;
}

export interface PromptTool {
  name: string;
  description: string;
  inputSchema: object;
  execute: (args: any) => Promise<string>;
}

export interface LanguageModelMonitor extends EventTarget {
  addEventListener(type: "downloadprogress", listener: (event: ProgressEvent) => void): void;
}

export interface LanguageModelSession extends EventTarget {
  readonly inputUsage: number;
  readonly inputQuota: number;
  
  prompt(input: string | PromptMessage[], options?: PromptOptions): Promise<string>;
  promptStreaming(input: string | PromptMessage[], options?: PromptOptions): ReadableStream<string>;
  append(input: string | PromptMessage[], options?: { signal?: AbortSignal }): Promise<void>;
  measureInputUsage(input: string | PromptMessage[], options?: { signal?: AbortSignal }): Promise<number>;
  clone(options?: LanguageModelCloneOptions): Promise<LanguageModelSession>;
  destroy(): void;
  
  // Events
  addEventListener(type: "quotaoverflow", listener: () => void): void;
}

export interface LanguageModelParams {
  defaultTemperature: number;
  maxTemperature: number;
  defaultTopK: number;
  maxTopK: number;
}

export type LanguageModelAvailability = "unavailable" | "downloadable" | "downloading" | "available";

export interface LanguageModelAvailabilityOptions {
  expectedInputs?: ExpectedInput[];
  expectedOutputs?: ExpectedOutput[];
  temperature?: number;
  topK?: number;
}

export interface LanguageModelStatic {
  create(options?: LanguageModelCreateOptions): Promise<LanguageModelSession>;
  availability(options?: LanguageModelAvailabilityOptions): Promise<LanguageModelAvailability>;
  params(): Promise<LanguageModelParams | null>;
}

// Error types
export class QuotaExceededError extends DOMException {
  readonly requested: number;
  readonly quota: number;
  
  constructor(message: string, requested: number, quota: number) {
    super(message, "QuotaExceededError");
    this.requested = requested;
    this.quota = quota;
  }
}