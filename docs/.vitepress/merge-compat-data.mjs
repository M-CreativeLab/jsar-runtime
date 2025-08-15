// Script for merging browser compatibility data
import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Merge browser compatibility data
 */
export const mergeCompatibilityData = () => {
  try {
    console.log('🔄 Starting to merge browser compatibility data...');
    console.log(`Source directory: ${path.resolve(__dirname, '../api/browser-compat-data')}`);
    console.log(`Target directory: ${path.join(__dirname, '../api/browser-compat-data')}`);
    console.log('Starting data processing...');

    // Define source and target directories
    const sourceDir = path.resolve(__dirname, '../api/browser-compat-data');
    const targetDir = path.join(__dirname, '../api/browser-compat-data');

    // Ensure target directory exists
    if (!fs.existsSync(targetDir)) {
      fs.mkdirSync(targetDir, { recursive: true });
    }

    // Initialize merged data object
    const mergedData = {
      browsers: {},
      api: {},
      html: {
        elements: {},
        global_attributes: {}
      },
      css: {
        properties: {}
      }
    };

    // Merge browser data
    const browsersDir = path.join(sourceDir, 'browsers');
    if (fs.existsSync(browsersDir)) {
      const browserFiles = fs.readdirSync(browsersDir).filter(file => file.endsWith('.json'));
      for (const file of browserFiles) {
        const filePath = path.join(browsersDir, file);
        const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
        if (data.browsers) {
          Object.assign(mergedData.browsers, data.browsers);
        }
      }
    }

    // Merge API data
    const apiDir = path.join(sourceDir, 'api');
    if (fs.existsSync(apiDir)) {
      const apiFiles = fs.readdirSync(apiDir).filter(file => file.endsWith('.json'));
      for (const file of apiFiles) {
        const filePath = path.join(apiDir, file);
        const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
        if (data.api) {
          Object.assign(mergedData.api, data.api);
        }
      }
    }

    // Merge HTML element data
    const htmlElementsDir = path.join(sourceDir, 'html/elements');
    if (fs.existsSync(htmlElementsDir)) {
      const htmlFiles = fs.readdirSync(htmlElementsDir).filter(file => file.endsWith('.json'));
      for (const file of htmlFiles) {
        const filePath = path.join(htmlElementsDir, file);
        const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
        if (data.html && data.html.elements) {
          Object.assign(mergedData.html.elements, data.html.elements);
        }
      }
    }

    // Merge global attributes data
    const globalAttributesPath = path.join(sourceDir, 'html/global_attributes.json');
    if (fs.existsSync(globalAttributesPath)) {
      const data = JSON.parse(fs.readFileSync(globalAttributesPath, 'utf8'));
      if (data.html && data.html.global_attributes) {
        Object.assign(mergedData.html.global_attributes, data.html.global_attributes);
      }
    }

    // Merge CSS properties data
    const cssPropertiesDir = path.join(sourceDir, 'css/properties');
    if (fs.existsSync(cssPropertiesDir)) {
      const cssFiles = fs.readdirSync(cssPropertiesDir).filter(file => file.endsWith('.json'));
      for (const file of cssFiles) {
        const filePath = path.join(cssPropertiesDir, file);
        const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
        if (data.css && data.css.properties) {
          Object.assign(mergedData.css.properties, data.css.properties);
        }
      }
    }

    // Write merged data to all.json only
    const allJsonPath = path.join(targetDir, 'all.json');
    console.log(`Writing merged data to: ${allJsonPath}`);
    fs.writeFileSync(
      allJsonPath,
      JSON.stringify(mergedData, null, 2)
    );
    console.log(`Successfully wrote all.json file`);

    console.log('✅ Browser compatibility data merge completed');
    return true;
  } catch (error) {
    console.error('❌ Failed to merge browser compatibility data:', error);
    return false;
  } finally {
    console.log('🏁 Browser compatibility data merge script execution completed');
  }
};

// Immediately invoked function to ensure script execution on import
(async function () {
  console.log('🚀 Auto-executing browser compatibility data merge');
  await mergeCompatibilityData();
})();
