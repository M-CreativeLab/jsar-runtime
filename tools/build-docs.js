#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

// Simple MDX to HTML converter
function convertMdxToHtml(content, title = 'JSAR Documentation') {
  // Basic markdown to HTML conversion
  let html = content
    // Headers
    .replace(/^# (.+)$/gm, '<h1>$1</h1>')
    .replace(/^## (.+)$/gm, '<h2>$1</h2>')
    .replace(/^### (.+)$/gm, '<h3>$1</h3>')
    .replace(/^#### (.+)$/gm, '<h4>$1</h4>')
    // Bold and italic
    .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
    .replace(/\*(.+?)\*/g, '<em>$1</em>')
    // Code blocks
    .replace(/```(\w*)\n([\s\S]*?)```/g, '<pre><code class="language-$1">$2</code></pre>')
    .replace(/`([^`]+)`/g, '<code>$1</code>')
    // Links
    .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>')
    .replace(/\[([^\]]+)\]\[([^\]]*)\]/g, '<a href="#$2">$1</a>')
    // Lists
    .replace(/^- (.+)$/gm, '<li>$1</li>')
    // Paragraphs (simple approach)
    .split('\n\n')
    .map(paragraph => {
      paragraph = paragraph.trim();
      if (!paragraph) return '';
      if (paragraph.startsWith('<h') || paragraph.startsWith('<pre') || 
          paragraph.startsWith('<li') || paragraph.startsWith('<ul') || 
          paragraph.startsWith('<ol')) {
        return paragraph;
      }
      return `<p>${paragraph.replace(/\n/g, ' ')}</p>`;
    })
    .join('\n');

  // Wrap list items in ul tags
  html = html.replace(/(<li>.*<\/li>)/gs, '<ul>$1</ul>');
  
  return createHtmlTemplate(html, title);
}

function createHtmlTemplate(content, title) {
  return `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>${title} - JSAR</title>
  <link rel="stylesheet" href="../shared/global.css">
  <style>
    .manual-container {
      max-width: 1200px;
      margin: 0 auto;
      padding: 6rem 2rem 2rem;
      display: grid;
      grid-template-columns: 250px 1fr;
      gap: 2rem;
    }
    
    .manual-sidebar {
      position: sticky;
      top: 6rem;
      height: fit-content;
      background: var(--bg-card);
      border: 1px solid var(--border-color);
      border-radius: 0.5rem;
      padding: 1.5rem;
    }
    
    .manual-content {
      background: var(--bg-card);
      border: 1px solid var(--border-color);
      border-radius: 0.5rem;
      padding: 2rem;
      line-height: 1.7;
    }
    
    .manual-content h1 {
      color: var(--text-primary);
      margin-bottom: 1.5rem;
      border-bottom: 1px solid var(--border-color);
      padding-bottom: 0.5rem;
    }
    
    .manual-content h2 {
      color: var(--text-primary);
      margin: 2rem 0 1rem;
    }
    
    .manual-content h3 {
      color: var(--text-primary);
      margin: 1.5rem 0 1rem;
    }
    
    .manual-content p {
      margin-bottom: 1rem;
      color: var(--text-secondary);
    }
    
    .manual-content code {
      background: var(--bg-secondary);
      padding: 0.2rem 0.4rem;
      border-radius: 0.25rem;
      font-family: 'Courier New', monospace;
      font-size: 0.9em;
    }
    
    .manual-content pre {
      background: var(--bg-secondary);
      padding: 1rem;
      border-radius: 0.5rem;
      overflow-x: auto;
      margin: 1rem 0;
    }
    
    .manual-content ul {
      margin: 1rem 0;
      padding-left: 1.5rem;
    }
    
    .manual-content li {
      margin-bottom: 0.5rem;
      color: var(--text-secondary);
    }
    
    .version-switcher {
      margin-bottom: 1.5rem;
      padding: 1rem;
      background: var(--bg-secondary);
      border-radius: 0.375rem;
    }
    
    .version-select {
      width: 100%;
      padding: 0.5rem;
      border: 1px solid var(--border-color);
      border-radius: 0.25rem;
      background: var(--bg-primary);
      color: var(--text-primary);
    }
    
    @media (max-width: 768px) {
      .manual-container {
        grid-template-columns: 1fr;
        padding: 6rem 1rem 2rem;
      }
      
      .manual-sidebar {
        position: static;
      }
    }
  </style>
</head>
<body data-theme="light">
  <!-- Navigation -->
  <div id="navbar-container"></div>
  
  <div class="manual-container">
    <aside class="manual-sidebar">
      <div class="version-switcher">
        <label for="version-select" style="display: block; margin-bottom: 0.5rem; font-weight: 600; color: var(--text-primary);">Version:</label>
        <select id="version-select" class="version-select">
          <option value="current">Current (v0.8.2)</option>
        </select>
      </div>
      
      <nav class="manual-nav">
        <h3 style="margin-bottom: 1rem; color: var(--text-primary);">Documentation</h3>
        <ul style="list-style: none; padding: 0;">
          <li><a href="introduction.html" class="nav-link">Introduction</a></li>
          <li><a href="quick-start.html" class="nav-link">Quick Start</a></li>
          <li><a href="basic-concepts.html" class="nav-link">Basic Concepts</a></li>
          <li><a href="features.html" class="nav-link">Features</a></li>
          <li><a href="runtime.html" class="nav-link">Runtime</a></li>
          <li><a href="delivery.html" class="nav-link">Delivery</a></li>
          <li><a href="developer-tools.html" class="nav-link">Developer Tools</a></li>
          <li><a href="tutorials.html" class="nav-link">Tutorials</a></li>
          <li><a href="references.html" class="nav-link">References</a></li>
        </ul>
      </nav>
    </aside>
    
    <main class="manual-content">
      ${content}
    </main>
  </div>
  
  <!-- Footer -->
  <div id="footer-container"></div>
  
  <script type="module">
    import { createNavbar } from '../shared/navbar.js';
    import { createFooter } from '../shared/footer.js';
    import { initThemeToggle, initMobileMenu } from '../shared/common.js';

    document.addEventListener('DOMContentLoaded', () => {
      // Insert navbar
      const navContainer = document.getElementById('navbar-container');
      if (navContainer) {
        navContainer.innerHTML = createNavbar();
      }

      // Insert footer
      const footerContainer = document.getElementById('footer-container');
      if (footerContainer) {
        footerContainer.innerHTML = createFooter();
      }

      // Initialize functionality
      initThemeToggle();
      initMobileMenu();
      
      initVersionSwitcher();
    });
    
    function initVersionSwitcher() {
      const versionSelect = document.getElementById('version-select');
      if (!versionSelect) return;
      
      // Load available versions
      loadAvailableVersions().then(versions => {
        versions.forEach(version => {
          if (version !== 'current') {
            const option = document.createElement('option');
            option.value = version;
            option.textContent = version;
            versionSelect.appendChild(option);
          }
        });
      });
      
      versionSelect.addEventListener('change', (e) => {
        const selectedVersion = e.target.value;
        if (selectedVersion !== 'current') {
          // Redirect to versioned docs (to be implemented)
          console.log('Switch to version:', selectedVersion);
        }
      });
    }
    
    async function loadAvailableVersions() {
      // This will be populated with actual version data
      return ['amidala-v0.8.2', 'amidala-v0.8.1', 'amidala-v0.8.0'];
    }
  </script>
</body>
</html>`;
}

function getAllMdxFiles(dir) {
  const files = [];
  const items = fs.readdirSync(dir);
  
  for (const item of items) {
    const fullPath = path.join(dir, item);
    const stat = fs.statSync(fullPath);
    
    if (stat.isDirectory()) {
      files.push(...getAllMdxFiles(fullPath));
    } else if (item.endsWith('.mdx')) {
      files.push(fullPath);
    }
  }
  
  return files;
}

function getAvailableVersions() {
  try {
    const tags = execSync('git tag | grep amidala-v', { encoding: 'utf8' });
    return tags.trim().split('\n').filter(tag => tag.startsWith('amidala-v'));
  } catch (error) {
    console.warn('Could not fetch git tags:', error.message);
    return [];
  }
}

function buildManualPages() {
  const docsDir = path.join(__dirname, '..', 'docs');
  const manualDir = path.join(docsDir, 'manual');
  const outputDir = path.join(docsDir, 'manual-html');
  
  // Create output directory
  if (!fs.existsSync(outputDir)) {
    fs.mkdirSync(outputDir, { recursive: true });
  }
  
  // Get all MDX files
  const mdxFiles = getAllMdxFiles(manualDir);
  
  console.log(`Found ${mdxFiles.length} MDX files to convert`);
  
  for (const filePath of mdxFiles) {
    try {
      const content = fs.readFileSync(filePath, 'utf8');
      const relativePath = path.relative(manualDir, filePath);
      const outputPath = path.join(outputDir, relativePath.replace('.mdx', '.html'));
      
      // Extract title from first header or filename
      const titleMatch = content.match(/^# (.+)$/m);
      const title = titleMatch ? titleMatch[1] : path.basename(filePath, '.mdx');
      
      // Create output directory if needed
      const outputDirPath = path.dirname(outputPath);
      if (!fs.existsSync(outputDirPath)) {
        fs.mkdirSync(outputDirPath, { recursive: true });
      }
      
      const html = convertMdxToHtml(content, title);
      fs.writeFileSync(outputPath, html);
      
      console.log(`Converted: ${relativePath} -> ${path.relative(docsDir, outputPath)}`);
    } catch (error) {
      console.error(`Error converting ${filePath}:`, error.message);
    }
  }
  
  console.log('Manual pages build complete!');
}

if (require.main === module) {
  buildManualPages();
}

module.exports = { buildManualPages, getAvailableVersions };