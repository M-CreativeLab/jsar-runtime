import { FragmentType, ParsedModule, MoudleFragmentTask } from './interfaces';

/**
 * Converts a single parsed module into a SkeletonNode and an FragmentTask.
 * This is designed to be called incrementally as modules are parsed.
 * @param module Parsed module information.
 * @param overallDesignTheme The overall design theme from the page header.
 * @param appName The application name from the page header.
 * @returns An object containing the new SkeletonNode and its corresponding FragmentTask.
 */
export function createModuleTask(module: ParsedModule, overallDesignTheme: string, parentId: string): MoudleFragmentTask {
  module.parentId = parentId;
  const htmltask: MoudleFragmentTask = {
    moudle: module,
    fragmentType: FragmentType.HTML,
    context: {
      pageGoal: module.name,
      designSystemInfo: overallDesignTheme,
    },
  };
  return htmltask;
}
