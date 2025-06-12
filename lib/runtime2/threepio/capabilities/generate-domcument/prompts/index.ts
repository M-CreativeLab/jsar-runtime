import { PLANNER_PROMPT as JSONL_PLANNER_PROMPT } from './jsonl/planner.prompt';
import { HTML_FRAGMENT_TEMPLATE_PROMPT as JSONL_WORKER_PROMPT } from './jsonl/worker.prompt';

export function getPlanPrompt(): string {
  return JSONL_PLANNER_PROMPT;
}

export function getWorkerPrompt(): string {
  return JSONL_WORKER_PROMPT;
}
