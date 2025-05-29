// src/interfaces/index.ts

import EventEmitter from 'events';

// --- Planner Stage (Phase 1) ---

export interface ParsedHeader {
  appName: string;
  overallTheme: string;
  layout: string;
}

export interface ParsedModule {
  name: string;
  layout: string;
  description: string;
  parentId?: string;
}

export interface EmitData {
  type: FragmentType;
  fragment: Fragment;
}

export interface Fragment { }

export interface HeaderFragment extends Fragment {
  content: string;
}

export interface MoudleFragment extends Fragment {
  content: string;
}

export interface HtmlFragment extends Fragment {
  parentId: string;
  content: string;
}

export interface CssFragment extends Fragment {
  content: string;
  id?: string;
}

export const EmitterEventType = {
  append: 'append',
} as const;

export const ParsedPlannerFields = {
  name: 'Name',
  layout: 'Layout',
  description: 'Description',
  theme: 'Theme',
  id: 'ID',
} as const;

// --- Fragment Generation Stage (Phase 2) ---

export enum FragmentType {
  Header = 'header',
  Moudle = 'moudle',
  HTML = 'html',
  CSS = 'css',
}

export interface LLMFragmentTask {
  id: string;
  fragmentType: FragmentType;
  path: string;
  moudle: ParsedModule;
  context: {
    pageGoal: string;
    designSystemInfo: string;
  };
}

export interface LLMMoudleFragmentTask {
  fragmentType: FragmentType;
  moudle: ParsedModule;
  context: {
    pageGoal: string;
    designSystemInfo: string;
  };
}

export interface LLMTaskResult {
  id: string;
  generatedCode: string;
}

export const PerformanceType = {
  planner: 'planner',
  moudle: 'moudle',
  parseHeader: 'parseHeader',
  parseMoudle: 'parseMoudle',
  total: 'total',
} as const;

export interface LLMAPI {
  sendPromptStream(input: string): EventEmitter;
}