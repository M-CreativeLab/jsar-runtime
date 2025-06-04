/**
 * Event types for the emitter.
 */
export type MoudleParserEventType = 'append' | 'streamEnd' | 'error';

export type PlanParserEventType = 'headerParsed' | 'moduleParsed' | 'parseEnd' | 'error';

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
export interface Fragment {
  content: string;         // HTML content string
}

/**
 * Fragment representing module content.
 */
export interface MoudleFragment extends Fragment {
}

/**
 * Fragment representing HTML content.
 */
export interface HtmlFragment extends Fragment {
  parentId: string;        // The parent element ID for this HTML fragment
}

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

// Callbacks for the StreamHtmlParser to handle parsed data
export interface StreamHtmlParserCallbacks {
  onData?: (eventType: MoudleParserEventType, data: EmitData) => void;
  onError?: (error: Error) => void;
}
