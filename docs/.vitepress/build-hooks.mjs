// Build hooks for updating version information and merging compatibility data during documentation build
import path from 'path';
import { fileURLToPath } from 'url';
import { mergeCompatibilityData } from './merge-compat-data.mjs';
import { copyChangelogs, copyBrowserCompatData, copyInspector } from './copy-changelogs.mjs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Update version information before build
 */
export const buildStart = () => {
  try {
    // Merge browser compatibility data
    mergeCompatibilityData();

    // Copy changelogs files
    copyChangelogs();

    // Copy browser-compat-data/all.json file
    copyBrowserCompatData();

    // Copy inspector HTML files
    copyInspector();
  } catch (error) {
    console.error('❌ Failed to update version information:', error);
  }
};
