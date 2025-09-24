// Build hooks for updating version information and merging compatibility data during documentation build
import path from 'path';
import { fileURLToPath } from 'url';
import { mergeCompatibilityData } from './merge-compat-data.mjs';
import { copyChangelogs, copyBrowserCompatData, copyInspector, copyStaticAssets, copyFixturesData } from './copy-changelogs.mjs';
import { generateFixturesData } from './generate-fixtures-data.mjs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Update version information before build
 */
export const buildStart = () => {
  try {
    // Merge browser compatibility data
    mergeCompatibilityData();

    // Generate fixtures data
    generateFixturesData();

    // Copy changelogs files
    copyChangelogs();

    // Copy browser-compat-data/all.json file
    copyBrowserCompatData();

    // Copy static assets (images and README.md)
    copyStaticAssets();

    // Copy fixtures data
    copyFixturesData();

    // Copy inspector HTML files
    copyInspector();
  } catch (error) {
    console.error('❌ Failed to update version information:', error);
  }
};
