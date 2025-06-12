import { Parser } from './interface';
import { StreamPlannerParser as JsonlStreamPlannerParser } from './jsonl/StreamPlannerParser';
import { StreamHtmlParser as JsonlStreamHTMLParser } from './jsonl/StreamHtmlParser';

export function getPlanParser(): Parser {
  return new JsonlStreamPlannerParser() as Parser;
}

export function getWorkerParser(taskid: string): Parser {
  return new JsonlStreamHTMLParser(taskid) as Parser;
}
