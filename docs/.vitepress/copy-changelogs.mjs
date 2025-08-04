import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Copy changelogs files to dist directory
 */
export const copyChangelogs = () => {
  try {
    const sourceDir = path.resolve(__dirname, '../changelogs');
    const distDir = path.resolve(__dirname, './dist/changelogs');

    // Check if source directory exists
    if (!fs.existsSync(sourceDir)) {
      console.log('⚠️ Changelogs source directory not found, skipping copy');
      return;
    }

    // Ensure target directory exists
    if (!fs.existsSync(distDir)) {
      fs.mkdirSync(distDir, { recursive: true });
    }

    // Read all .md files from source directory
    const files = fs.readdirSync(sourceDir).filter(file => file.endsWith('.md'));

    if (files.length === 0) {
      console.log('⚠️ No markdown files found in changelogs directory');
      return;
    }

    // Copy each .md file
    files.forEach(file => {
      const sourcePath = path.join(sourceDir, file);
      const destPath = path.join(distDir, file);

      fs.copyFileSync(sourcePath, destPath);
      console.log(`✅ Copied ${file} to dist/changelogs/`);
    });

    console.log(`✅ Successfully copied ${files.length} changelog file(s) to dist/changelogs/`);
  } catch (error) {
    console.error('❌ Failed to copy changelogs:', error);
  }
};

/**
 * Copy browser-compat-data/all.json file to dist directory
 */
export const copyBrowserCompatData = () => {
  try {
    const sourceFile = path.resolve(__dirname, '../api/browser-compat-data/all.json');
    const distDir = path.resolve(__dirname, './dist/api/browser-compat-data');
    const destFile = path.join(distDir, 'all.json');

    // Check if source file exists
    if (!fs.existsSync(sourceFile)) {
      console.log('⚠️ browser-compat-data/all.json file not found, skipping copy');
      return;
    }

    // Ensure target directory exists
    if (!fs.existsSync(distDir)) {
      fs.mkdirSync(distDir, { recursive: true });
    }

    // Copy file
    fs.copyFileSync(sourceFile, destFile);
    console.log('✅ Copied all.json to dist/api/browser-compat-data/');

  } catch (error) {
    console.error('❌ Failed to copy browser-compat-data/all.json:', error);
  }
};

/**
   * Copy inspector HTML files to dist directory
   */
export const copyInspector = () => {
  try {
    const sourceDir = path.resolve(__dirname, '../inspector');
    const distDir = path.resolve(__dirname, './dist/inspector');

    // Check if source directory exists
    if (!fs.existsSync(sourceDir)) {
      console.log('⚠️ inspector directory not found, skipping copy');
      return;
    }

    // Ensure target directory exists
    if (!fs.existsSync(distDir)) {
      fs.mkdirSync(distDir, { recursive: true });
    }

    // Read all files in the inspector directory
    const files = fs.readdirSync(sourceDir);

    // Filter and copy only HTML files
    const htmlFiles = files.filter(file => file.endsWith('.html'));

    if (htmlFiles.length === 0) {
      console.log('⚠️ No HTML files found in inspector directory');
      return;
    }

    htmlFiles.forEach(file => {
      const sourcePath = path.join(sourceDir, file);
      const destPath = path.join(distDir, file);

      fs.copyFileSync(sourcePath, destPath);
      console.log(`✅ Copied ${file} to dist/inspector/`);
    });

    console.log(`✅ Copied ${htmlFiles.length} HTML files to dist/inspector/`);

  } catch (error) {
    console.error('❌ Failed to copy inspector files:', error);
  }
};

/**
 * Copy image files and README.md to dist directory
 */
export const copyStaticAssets = () => {
  try {
    const docsDir = path.resolve(__dirname, '..');
    const distDir = path.resolve(__dirname, './dist');

    // Ensure target directory exists
    if (!fs.existsSync(distDir)) {
      fs.mkdirSync(distDir, { recursive: true });
    }

    // Copy README.md
    const readmePath = path.join(docsDir, 'README.md');
    if (fs.existsSync(readmePath)) {
      const destReadmePath = path.join(distDir, 'README.md');
      fs.copyFileSync(readmePath, destReadmePath);
      console.log('✅ Copied README.md to dist/');
    } else {
      console.log('⚠️ README.md not found, skipping copy');
    }

    // Copy image files (jpg, png, webp, svg)
    const imageExtensions = ['.jpg', '.png', '.webp', '.svg'];
    const files = fs.readdirSync(docsDir);

    const imageFiles = files.filter(file => {
      const ext = path.extname(file).toLowerCase();
      return imageExtensions.includes(ext);
    });

    if (imageFiles.length === 0) {
      console.log('⚠️ No image files found in docs directory');
      return;
    }

    imageFiles.forEach(file => {
      const sourcePath = path.join(docsDir, file);
      const destPath = path.join(distDir, file);

      fs.copyFileSync(sourcePath, destPath);
      console.log(`✅ Copied ${file} to dist/`);
    });

    console.log(`✅ Successfully copied ${imageFiles.length} image file(s) to dist/`);

  } catch (error) {
    console.error('❌ Failed to copy static assets:', error);
  }
};
