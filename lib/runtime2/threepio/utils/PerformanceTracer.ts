interface TaskTiming {
  startTime: number;
  endTime?: number;
  duration?: number;
  count: number; // Number of task executions
  totalDuration: number; // Total duration for repeated tasks
  invocations: {
    startTime: number;
    endTime?: number;
    duration?: number;
    total?: number; // New field
  }[];
}

class PerformanceTracer {
  private timings: Map<string, TaskTiming> = new Map();
  private concurrentTasks: Map<string, number> = new Map(); // Used to generate unique IDs for concurrent tasks
  private creationTime: number; // Time when the tracer was created

  constructor(private enabled: boolean = true) {
    this.creationTime = this.now() // Initialize creation time
  }

  /**
   * Start timing a task
   * @param taskName Name of the task
   * @param instanceId Optional instance ID to distinguish concurrent executions
   *                   Auto-generated from taskName if not provided
   * @returns Unique task instance ID for subsequent end() calls
   */
  public start(taskName: string, instanceId?: string): string {
    if (!this.enabled) return taskName; // If disabled, return taskName directly

    const uniqueInstanceId = instanceId || this.generateUniqueTaskInstanceId(taskName);

    if (this.timings.has(uniqueInstanceId) && !this.timings.get(uniqueInstanceId)?.endTime) {
      // Allow repeated start calls but only first one records start time
      console.warn(`Task '${uniqueInstanceId}' was already started. Restarting timer.`);
    }

    const now = this.now()
    let taskTiming = this.timings.get(taskName);
    if (!taskTiming) {
      taskTiming = {
        startTime: now, // Represents first occurrence time of this task type
        count: 0,
        totalDuration: 0,
        invocations: [],
      };
      this.timings.set(taskName, taskTiming);
    }

    const invocationDetail = { startTime: now - this.creationTime };
    
    if (uniqueInstanceId === taskName) {
      taskTiming.invocations.push(invocationDetail);
      taskTiming.startTime = now; // Update to most recent start time
    } else {
      if (this.timings.has(uniqueInstanceId)) {
        console.warn(`Concurrent task instance '${uniqueInstanceId}' was already started. Restarting timer.`);
      }
      this.timings.set(uniqueInstanceId, {
        startTime: now,
        count: 1,
        totalDuration: 0,
        invocations: [invocationDetail]
      });
    }
    return uniqueInstanceId;
  }

  /**
   * End timing a task
   * @param taskInstanceId start() method returned unique task instance ID.
   */
  public end(taskInstanceId: string): void {
    if (!this.enabled) return;

    const now = this.now()
    const taskTimingEntry = this.timings.get(taskInstanceId);

    if (!taskTimingEntry || taskTimingEntry.endTime) {
      console.warn(`Task '${taskInstanceId}' was not started or already ended.`);
      return;
    }

    taskTimingEntry.endTime = now - this.creationTime;
    taskTimingEntry.duration = now - taskTimingEntry.startTime;

    // Update invocations corresponding to this entry
    // Find last invocation that is not finished (or based on more complex matching logic if needed)
    const lastInvocation = taskTimingEntry.invocations.find(inv => !inv.endTime);
    if (lastInvocation) {
      lastInvocation.endTime = now;
      lastInvocation.duration = now - lastInvocation.startTime;
      lastInvocation.total = now - this.creationTime; // Update relative time
    }

    // If taskInstanceId is a concurrent instance ID (different from original taskName)
    // We need to add its duration to the statistics of the original taskName
    const baseTaskName = this.getBaseTaskName(taskInstanceId);
    if (baseTaskName !== taskInstanceId) {
      const baseTaskTiming = this.timings.get(baseTaskName);
      if (baseTaskTiming && taskTimingEntry.duration !== undefined) {
        baseTaskTiming.count++;
        baseTaskTiming.totalDuration += taskTimingEntry.duration;
        // Add this concurrent instance invocations to the base task invocations (if needed)
        baseTaskTiming.invocations.push(...taskTimingEntry.invocations);
      }
    } else {
      // If taskInstanceId is the taskName (not concurrent)
      taskTimingEntry.count++;
      if (taskTimingEntry.duration !== undefined) {
        taskTimingEntry.totalDuration += taskTimingEntry.duration;
      }
    }
  }

  private generateUniqueTaskInstanceId(taskName: string): string {
    const count = (this.concurrentTasks.get(taskName) || 0) + 1;
    this.concurrentTasks.set(taskName, count);
    return `${taskName}#${count}`;
  }

  private getBaseTaskName(taskInstanceId: string): string {
    const hashIndex = taskInstanceId.lastIndexOf('#');
    if (hashIndex > 0 && !isNaN(Number(taskInstanceId.substring(hashIndex + 1)))) {
      return taskInstanceId.substring(0, hashIndex);
    }
    return taskInstanceId;
  }

  /**
   * Generate and print performance report
   */
  public report(): void {
    if (!this.enabled) {
      console.log('PerformanceTracer is disabled.');
      return;
    }

    console.log('\n--- Performance Report ---');
    console.log(`Tracer created at: ${this.creationTime.toFixed(2)}ms`); // 报告创建时间
    if (this.timings.size === 0) {
      console.log('No tasks were timed.');
      return;
    }
    this.timings.forEach((timing, taskNameOrInstanceId) => {
      // 我们只报告基础任务的汇总信息，并发实例的计时已合并
      const baseTaskName = this.getBaseTaskName(taskNameOrInstanceId);
      if (baseTaskName !== taskNameOrInstanceId && this.timings.has(baseTaskName)) {
        // 这是并发实例的条目，其信息已汇总到 baseTaskName，跳过单独报告
        return;
      }

      // 对于基础任务 (taskName)
      const taskTiming = this.timings.get(taskNameOrInstanceId);
      if (!taskTiming) return;

      if (taskTiming.count > 0) {
        const averageDuration = taskTiming.totalDuration / taskTiming.count;
        console.log(
          `Task: ${taskNameOrInstanceId} | Count: ${taskTiming.count} | Total: ${taskTiming.totalDuration.toFixed(2)}ms | Avg: ${averageDuration.toFixed(2)}ms`
        );
        if (taskTiming.invocations.length > 0) {
          taskTiming.invocations.forEach((inv, index) => {
            const durationStr = inv.duration !== undefined ? `${inv.duration.toFixed(2)}ms` : 'In progress';
            const endTimeStr = inv.endTime !== undefined ? `, End: ${inv.endTime.toFixed(2)}ms` : '';
            const relativeStartTimeStr = inv.total !== undefined
              ? ` (duration from begin: ${inv.total}ms)`
              : '';
            console.log(`  Invocation ${index + 1}: ${durationStr} (Start: ${inv.startTime.toFixed(2)}ms${endTimeStr}${relativeStartTimeStr})`);
          });
        }
      } else if (taskTiming.startTime && !taskTiming.endTime && taskTiming.invocations.length > 0) {
        // 处理仅开始未结束的任务，并确保有调用信息可供显示
        const firstInvocation = taskTiming.invocations[0];
        const relativeStartTimeStr = firstInvocation?.total !== undefined
          ? ` (duration from begin: ${firstInvocation.total.toFixed(2)}ms)`
          : '';
        console.log(`Task: ${taskNameOrInstanceId} | Status: Started but not ended (Started at: ${firstInvocation.startTime.toFixed(2)}ms${relativeStartTimeStr})`);
      }
    });
    console.log('--- End of Report ---\n');
  }

  /**
   * Reset all timing data.
   */
  public reset(): void {
    this.timings.clear();
    this.concurrentTasks.clear();
    this.creationTime = this.now() // 重置时也更新创建时间，或者保持原有创建时间，取决于需求
    console.log('PerformanceTracer has been reset.');
  }

  private now(): number {
    return performance.now();
    // return Date.now();
  }

  /**
   * Enable tracer.
   */
  public enable(): void {
    this.enabled = true;
    console.log('PerformanceTracer enabled.');
  }

  /**
   * Disable tracer. Call start/end will be invalid.
   */
  public disable(): void {
    this.enabled = false;
    console.log('PerformanceTracer disabled.');
  }
}

export { PerformanceTracer };

