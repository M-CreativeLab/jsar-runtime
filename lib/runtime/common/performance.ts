/**
 * This function returns the correct `Performance` instance in the current environment.
 *
 * @returns The performance object.
 */
export function getPerformanceInCurrentEnv(): Performance {
    if (performance !== undefined) {
        return performance;
    } else {
        return require('perf_hooks').performance;
    }
}
