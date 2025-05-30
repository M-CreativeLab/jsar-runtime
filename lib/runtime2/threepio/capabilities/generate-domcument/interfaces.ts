import type EventEmitter from 'events';
/**
 * This module defines interfaces and types used in the planner stage of the Threepio project.
 * It includes interfaces for parsed headers, modules, emitted data, and fragment types.
 * It also defines the structure of tasks for LLM fragment generation and performance tracking.
 * The planner stage is responsible for parsing the initial input, generating modules, and preparing data for the fragment generation stage.
 */

/**
 * Interface for the parsed header information.
 * It includes the application name, overall theme, and layout.
 * @interface ParsedHeader
 * @property {string} appName - The name of the application.
 * @property {string} overallTheme - The overall theme of the application.
 * @property {string} layout - The layout of the application.
 */
export interface ParsedHeader {
  appName: string;
  overallTheme: string;
  layout: string;
}

/**
 * Interface for the parsed module information.
 * It includes the module name, layout, description, and an optional parent ID.
 * @interface ParsedModule
 * @property {string} name - The name of the module.
 * @property {string} layout - The layout of the module.
 * @property {string} description - A brief description of the module.
 * @property {string} [parentId] - An optional parent ID for the module, if it is part of a larger structure.
 */
export interface ParsedModule {
  name: string;
  layout: string;
  description: string;
  parentId?: string;
}

/**
 * Data structure for emitted data during the planning or fragment generation process.
 * Includes the fragment type and the fragment content.
 */
export interface EmitData {
  type: FragmentType;      // The type of fragment being emitted
  fragment: Fragment;      // The fragment data
}

/**
 * Base interface for all fragment types.
 */
export interface Fragment { }

/**
 * Fragment representing header content.
 */
export interface HeaderFragment extends Fragment {
  content: string;         // Header content string
}

/**
 * Fragment representing module content.
 */
export interface MoudleFragment extends Fragment {
  content: string;         // Module content string
}

/**
 * Fragment representing HTML content.
 */
export interface HtmlFragment extends Fragment {
  parentId: string;        // The parent element ID for this HTML fragment
  content: string;         // HTML content string
}

/**
 * Fragment representing CSS content.
 */
export interface CssFragment extends Fragment {
  content: string;         // CSS content string
  id?: string;             // Optional ID for the CSS fragment
}

/**
 * Event types for the emitter.
 */
export const EmitterEventType = {
  append: 'append',
} as const;

/**
 * Field names used in the planner for parsing and mapping.
 */
export const ParsedPlannerFields = {
  name: 'Name',
  layout: 'Layout',
  description: 'Description',
  theme: 'Theme',
  id: 'ID',
} as const;

// --- Fragment Generation Stage (Phase 2) ---

/**
 * Enum for fragment types used in the generation stage.
 */
export enum FragmentType {
  Header = 'header',
  Moudle = 'moudle',
  HTML = 'html',
  CSS = 'css',
}

/**
 * Task structure for LLM fragment generation.
 * Contains task ID, fragment type, path, module info, and context.
 */
export interface FragmentTask {
  id: string;              // Unique task identifier
  fragmentType: FragmentType; // Type of fragment to generate
  path: string;            // Path for the fragment
  moudle: ParsedModule;    // Module information
  context: {               // Context for the task
    pageGoal: string;      // The goal of the page
    designSystemInfo: string; // Design system information
  };
}

/**
 * Task structure for LLM module fragment generation.
 */
export interface MoudleFragmentTask {
  fragmentType: FragmentType; // Type of fragment to generate
  moudle: ParsedModule;       // Module information
  context: {                  // Context for the task
    pageGoal: string;         // The goal of the page
    designSystemInfo: string; // Design system information
  };
}

/**
 * Result structure for LLM tasks.
 */
export interface LLMTaskResult {
  id: string;                // Task identifier
  generatedCode: string;     // Generated code string
}

/**
 * Performance type constants for tracking different stages.
 */
export const PerformanceType = {
  planner: 'planner',
  moudle: 'moudle',
  parseHeader: 'parseHeader',
  parseMoudle: 'parseMoudle',
  total: 'total',
} as const;

/**
 * Interface for the LLM API used in the planner stage.
 * Defines the method to send a prompt and receive a stream of responses.
 * @returns {EventEmitter} An EventEmitter that emits events for each chunk of the response.
 */
export interface LLMAPI {
  sendPromptStream(input: string): EventEmitter;
}
