// Generate fixtures summary data during build
import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Function to extract title from HTML content
function extractTitle(htmlContent, filename) {
  const titleMatch = htmlContent.match(/<title[^>]*>\s*(.*?)\s*<\/title>/i);
  if (titleMatch && titleMatch[1].trim()) {
    return titleMatch[1].trim()
      .replace(/&lt;/g, '<')
      .replace(/&gt;/g, '>')
      .replace(/&amp;/g, '&')
      .replace(/&quot;/g, '"')
      .replace(/&#39;/g, "'");
  }
  
  return path.basename(filename, '.html').replace(/-/g, ' ').replace(/\b\w/g, l => l.toUpperCase());
}

// Function to extract description from HTML content
function extractDescription(htmlContent) {
  // Look for meta description
  const metaDescMatch = htmlContent.match(/<meta\s+name=["']description["']\s+content=["'](.*?)["']/i);
  if (metaDescMatch) {
    return metaDescMatch[1];
  }
  
  // Look for first paragraph or heading
  const pMatch = htmlContent.match(/<p[^>]*>(.*?)<\/p>/i);
  if (pMatch) {
    return pMatch[1].replace(/<[^>]*>/g, '').trim().slice(0, 150);
  }
  
  const h2Match = htmlContent.match(/<h[2-6][^>]*>(.*?)<\/h[2-6]>/i);
  if (h2Match) {
    return h2Match[1].replace(/<[^>]*>/g, '').trim();
  }
  
  return '';
}

// Function to categorize fixtures based on filename and content
function categorizeFixture(filename, title, htmlContent) {
  const name = filename.toLowerCase();
  const content = (title + ' ' + htmlContent.slice(0, 1000)).toLowerCase();
  
  if (name.includes('canvas') || content.includes('canvas')) return 'Canvas/WebGL';
  if (name.includes('three') || content.includes('three.js') || content.includes('webgl')) return 'Canvas/WebGL';
  if (name.includes('babylon') || content.includes('babylon')) return 'Canvas/WebGL';
  if (name.includes('webgl') || content.includes('webgl')) return 'Canvas/WebGL';
  
  if (name.includes('spatial') || name.includes('webxr') || content.includes('webxr') || content.includes('spatial')) return 'Spatial/WebXR';
  
  if (name.includes('layout') || name.includes('flexbox') || name.includes('grid') || 
      content.includes('flexbox') || content.includes('grid') || content.includes('layout')) return 'Layout';
  
  if (name.includes('css') || name.includes('style') || name.includes('background') ||
      name.includes('transform') || content.includes('css ')) return 'CSS';
  
  if (name.includes('text') || name.includes('font') || content.includes('font') || content.includes('text')) return 'Typography';
  
  if (name.includes('element') || name.includes('button') || name.includes('input') || 
      name.includes('template') || content.includes('element')) return 'HTML Elements';
  
  if (name.includes('api') || name.includes('fetch') || name.includes('worker') || 
      name.includes('websocket') || name.includes('wasm')) return 'Web APIs';
  
  if (name.includes('esm') || name.includes('import') || content.includes('import ')) return 'JavaScript/ESM';
  
  if (name.includes('test') || name.includes('demo') || name.includes('example')) return 'Examples';
  
  return 'Other';
}

// Main function to generate fixtures data
export function generateFixturesData() {
  console.log('🔄 Generating fixtures data...');
  
  try {
    const fixturesDir = path.join(__dirname, '../../fixtures/html');
    const outputPath = path.join(__dirname, 'fixtures-data.json');
    
    if (!fs.existsSync(fixturesDir)) {
      console.error('❌ Fixtures directory not found:', fixturesDir);
      return;
    }
    
    const htmlFiles = fs.readdirSync(fixturesDir)
      .filter(file => file.endsWith('.html') && fs.statSync(path.join(fixturesDir, file)).isFile())
      .sort();
    
    console.log(`📁 Found ${htmlFiles.length} HTML fixture files`);
    
    const fixturesData = htmlFiles.map(filename => {
      const filepath = path.join(fixturesDir, filename);
      const htmlContent = fs.readFileSync(filepath, 'utf8');
      const title = extractTitle(htmlContent, filename);
      const description = extractDescription(htmlContent);
      const category = categorizeFixture(filename, title, htmlContent);
      
      // Get file stats
      const stats = fs.statSync(filepath);
      
      return {
        filename,
        title,
        description,
        category,
        githubUrl: `https://github.com/M-CreativeLab/jsar-runtime/blob/main/fixtures/html/${filename}`,
        rawUrl: `https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/main/fixtures/html/${filename}`,
        size: stats.size,
        lastModified: stats.mtime.toISOString()
      };
    });
    
    // Group by category for summary
    const categories = {};
    fixturesData.forEach(fixture => {
      if (!categories[fixture.category]) {
        categories[fixture.category] = [];
      }
      categories[fixture.category].push(fixture);
    });
    
    const summary = {
      total: fixturesData.length,
      categories: Object.keys(categories).map(name => ({
        name,
        count: categories[name].length,
        fixtures: categories[name]
      })),
      generatedAt: new Date().toISOString()
    };
    
    // Write the data file
    fs.writeFileSync(outputPath, JSON.stringify(summary, null, 2));
    
    console.log('✅ Fixtures data generated successfully');
    console.log(`📊 Total fixtures: ${summary.total}`);
    console.log(`📂 Categories: ${Object.keys(categories).join(', ')}`);
    
    return summary;
    
  } catch (error) {
    console.error('❌ Failed to generate fixtures data:', error);
    throw error;
  }
}