// src/constants/plannerSeparators.ts

export const PLANNER_HEADER_MARKER = 'H:'; // Marks the start of header info
export const PLANNER_MODULE_MARKER = 'M:'; // Marks the start of a module's info
export const PLANNER_MOUDLEDESC_MARKER = 'MD:';  // Marks the start of a field's value
export const PLANNER_END_MARKER = 'E:';   // Marks the end of the entire plan

// Internal key-value separators (optional, for internal parsing logic)
export const PLANNER_FIELD_SEPARATOR = ' | ';

// 新的、极致简短的节点流协议标记
export const S_HTML_START = 'SH#';    // HTML流结束
export const S_NODE_START = 'N:';     // 节点开始: N:{id}:{parent_id}:{tag}

export const S_CSS_START = 'CS:';   // CSS样式开始
export const S_BASIC_CSS_START = 'BCS:';   // 基础CSS样式开始
export const S_HTML_END = 'EH#';    // HTML流结束
export const NULL_PARENT = "body";