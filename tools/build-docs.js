#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

// Improved MDX to HTML converter with table support
function convertMdxToHtml(content, title = 'JSAR Documentation', currentFile = '', version = '') {
  const lines = content.split('\n');
  const htmlLines = [];
  let inCodeBlock = false;
  let inList = false;
  let inTable = false;
  let currentListItems = [];
  let currentTableRows = [];
  let tableHeaders = [];
  
  for (let i = 0; i < lines.length; i++) {
    let line = lines[i];
    
    // Handle code blocks
    if (line.startsWith('```')) {
      flushList();
      flushTable();
      if (inCodeBlock) {
        htmlLines.push('</code></pre>');
        inCodeBlock = false;
      } else {
        const language = line.replace('```', '').trim();
        htmlLines.push(`<pre><code class="language-${language}">`);
        inCodeBlock = true;
      }
      continue;
    }
    
    if (inCodeBlock) {
      htmlLines.push(escapeHtml(line));
      continue;
    }
    
    // Handle table rows
    if (line.trim().startsWith('|') && line.trim().endsWith('|')) {
      flushList();
      const cells = line.split('|').slice(1, -1).map(cell => cell.trim());
      
      // Check if this is a separator row (|---|---|)
      if (cells.every(cell => /^-+$/.test(cell))) {
        // This is a header separator, mark current row as headers
        if (currentTableRows.length > 0) {
          tableHeaders = currentTableRows.pop();
        }
        continue;
      }
      
      currentTableRows.push(cells);
      inTable = true;
    }
    // Headers
    else if (line.startsWith('#### ')) {
      flushList();
      flushTable();
      htmlLines.push(`<h4>${processInlineMarkdown(line.replace('#### ', ''))}</h4>`);
    } else if (line.startsWith('### ')) {
      flushList();
      flushTable();
      htmlLines.push(`<h3>${processInlineMarkdown(line.replace('### ', ''))}</h3>`);
    } else if (line.startsWith('## ')) {
      flushList();
      flushTable();
      htmlLines.push(`<h2>${processInlineMarkdown(line.replace('## ', ''))}</h2>`);
    } else if (line.startsWith('# ')) {
      flushList();
      flushTable();
      htmlLines.push(`<h1>${processInlineMarkdown(line.replace('# ', ''))}</h1>`);
    }
    // List items
    else if (line.startsWith('- ')) {
      flushTable();
      const listItem = processInlineMarkdown(line.replace('- ', ''));
      currentListItems.push(`<li>${listItem}</li>`);
      inList = true;
    }
    // Empty lines
    else if (line.trim() === '') {
      flushList();
      flushTable();
      htmlLines.push('');
    }
    // Regular paragraphs
    else {
      flushList();
      flushTable();
      if (line.trim()) {
        const processedLine = processInlineMarkdown(line);
        htmlLines.push(`<p>${processedLine}</p>`);
      }
    }
  }
  
  flushList(); // Ensure any remaining list is flushed
  flushTable(); // Ensure any remaining table is flushed
  
  function flushList() {
    if (inList && currentListItems.length > 0) {
      htmlLines.push('<ul>');
      htmlLines.push(...currentListItems);
      htmlLines.push('</ul>');
      currentListItems = [];
      inList = false;
    }
  }
  
  function flushTable() {
    if (inTable && (currentTableRows.length > 0 || tableHeaders.length > 0)) {
      htmlLines.push('<table class="table">');
      
      // Add headers if present
      if (tableHeaders.length > 0) {
        htmlLines.push('<thead>');
        htmlLines.push('<tr>');
        for (const header of tableHeaders) {
          htmlLines.push(`<th>${processInlineMarkdown(header)}</th>`);
        }
        htmlLines.push('</tr>');
        htmlLines.push('</thead>');
      }
      
      // Add body rows
      if (currentTableRows.length > 0) {
        htmlLines.push('<tbody>');
        for (const row of currentTableRows) {
          htmlLines.push('<tr>');
          for (const cell of row) {
            htmlLines.push(`<td>${processInlineMarkdown(cell)}</td>`);
          }
          htmlLines.push('</tr>');
        }
        htmlLines.push('</tbody>');
      }
      
      htmlLines.push('</table>');
      
      currentTableRows = [];
      tableHeaders = [];
      inTable = false;
    }
  }
  
  function processInlineMarkdown(text) {
    return text
      // Bold
      .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
      // Italic
      .replace(/\*(.+?)\*/g, '<em>$1</em>')
      // Code
      .replace(/`([^`]+)`/g, '<code>$1</code>')
      // Links
      .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>')
      // Reference links (simple handling)
      .replace(/\[([^\]]+)\]\[([^\]]*)\]/g, '<a href="#$2">$1</a>');
  }
  
  function escapeHtml(text) {
    return text
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/'/g, '&#39;');
  }
  
  const html = htmlLines.join('\n');
  return createHtmlTemplate(html, title, currentFile, version);
}

function generateManualNavigation(allFiles, currentFile = '') {
  const structure = {
    'introduction.html': 'Introduction',
    'quick-start.html': 'Quick Start',
    'quick-start/setup.html': 'Setup',
    'quick-start/write-html.html': 'Write HTML',
    'quick-start/run.html': 'Run',
    'basic-concepts.html': 'Basic Concepts',
    'basic-concepts/spaces.html': 'Spaces',
    'basic-concepts/input-sources.html': 'Input Sources',
    'basic-concepts/recap-of-html.html': 'Recap of HTML',
    'features.html': 'Features',
    'features/typescript.html': 'TypeScript',
    'features/vanillajs.html': 'Vanilla JavaScript',
    'features/three.html': 'Three.js',
    'features/babylon.html': 'Babylon.js',
    'features/webassembly.html': 'WebAssembly',
    'features/modules.html': 'Modules',
    'features/web-workers.html': 'Web Workers',
    'features/audio.html': 'Audio',
    'features/image.html': 'Image',
    'features/debugging.html': 'Debugging',
    'runtime.html': 'Runtime',
    'runtime/integrate-runtime-with-unity.html': 'Unity Integration',
    'runtime/nodejs-apis.html': 'Node.js APIs',
    'delivery.html': 'Delivery',
    'developer-tools.html': 'Developer Tools',
    'developer-tools/preview.html': 'Preview',
    'developer-tools/debugging.html': 'Debugging',
    'developer-tools/packaging.html': 'Packaging',
    'developer-tools/intro-gui.html': 'GUI Introduction',
    'developer-tools/intro-features.html': 'Features Introduction',
    'tutorials.html': 'Tutorials',
    'tutorials/helloworld-threejs.html': 'Hello World - Three.js',
    'tutorials/helloworld-babylonjs.html': 'Hello World - Babylon.js',
    'tutorials/spatial-images.html': 'Spatial Images',
    'tutorials/create-meshes.html': 'Create Meshes',
    'tutorials/create-materials.html': 'Create Materials',
    'tutorials/https-request.html': 'HTTPS Request',
    'tutorials/connect-websocket-server.html': 'WebSocket Server',
    'references.html': 'References',
    'references/web-apis.html': 'Web APIs',
    'references/jsar-apis.html': 'JSAR APIs',
    'references/webxr.html': 'WebXR',
    'references/webxr-inputsources.html': 'WebXR Input Sources',
    'references/webgl.html': 'WebGL',
    'references/spatial-css.html': 'Spatial CSS',
    'references/xsml.html': 'XSML',
    'references/caching.html': 'Caching',
    'references/graphics-backends.html': 'Graphics Backends',
    'references/runtime-configuration.html': 'Runtime Configuration',
    'references/embedder-guides.html': 'Embedder Guides',
    'references/embedder-guides/native-apis.html': 'Native APIs',
    'references/embedder-guides/unity-apis.html': 'Unity APIs'
  };
  
  let nav = '<nav class="manual-nav">\n';
  nav += '<h3 style="margin-bottom: 1rem; color: var(--text-primary);">Documentation</h3>\n';
  nav += '<ul style="list-style: none; padding: 0; margin: 0;">\n';
  
  const mainSections = ['introduction.html', 'quick-start.html', 'basic-concepts.html', 'features.html', 'runtime.html', 'delivery.html', 'developer-tools.html', 'tutorials.html', 'references.html'];
  
  for (const section of mainSections) {
    const sectionName = structure[section];
    const isActive = currentFile === section;
    const sectionClass = isActive ? 'nav-link active' : 'nav-link';
    
    nav += `  <li style="margin-bottom: 0.5rem;">\n`;
    nav += `    <a href="${section}" class="${sectionClass}" style="font-weight: 600;">${sectionName}</a>\n`;
    
    // Add subsections
    const subsections = Object.keys(structure).filter(key => 
      key.startsWith(section.replace('.html', '/')) && key !== section
    );
    
    if (subsections.length > 0) {
      nav += `    <ul style="list-style: none; padding-left: 1rem; margin: 0.25rem 0 0.5rem 0;">\n`;
      for (const subsection of subsections) {
        const isSubActive = currentFile === subsection;
        const subClass = isSubActive ? 'nav-link active' : 'nav-link';
        nav += `      <li style="margin-bottom: 0.25rem;"><a href="${subsection}" class="${subClass}" style="font-size: 0.9em;">${structure[subsection]}</a></li>\n`;
      }
      nav += `    </ul>\n`;
    }
    nav += `  </li>\n`;
  }
  
  nav += '</ul>\n';
  nav += '</nav>';
  
  return nav;
}

function createHtmlTemplate(content, title, currentFile = '', version = '') {
  const navigation = generateManualNavigation([], currentFile);
  const versions = getAvailableVersions();
  
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
      grid-template-columns: 280px 1fr;
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
      min-height: 70vh;
    }
    
    .manual-content h1 {
      color: var(--text-primary);
      margin-bottom: 1.5rem;
      border-bottom: 2px solid var(--border-color);
      padding-bottom: 0.75rem;
      font-size: 2rem;
    }
    
    .manual-content h2 {
      color: var(--text-primary);
      margin: 2.5rem 0 1rem;
      font-size: 1.5rem;
      border-bottom: 1px solid var(--border-color);
      padding-bottom: 0.5rem;
    }
    
    .manual-content h3 {
      color: var(--text-primary);
      margin: 2rem 0 1rem;
      font-size: 1.25rem;
    }
    
    .manual-content h4 {
      color: var(--text-primary);
      margin: 1.5rem 0 0.75rem;
      font-size: 1.1rem;
    }
    
    .manual-content p {
      margin-bottom: 1rem;
      color: var(--text-secondary);
    }
    
    .manual-content code {
      background: var(--bg-secondary);
      padding: 0.2rem 0.4rem;
      border-radius: 0.25rem;
      font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
      font-size: 0.9em;
      color: var(--text-primary);
    }
    
    .manual-content pre {
      background: var(--bg-secondary);
      padding: 1.5rem;
      border-radius: 0.5rem;
      overflow-x: auto;
      margin: 1.5rem 0;
      border: 1px solid var(--border-color);
    }
    
    .manual-content pre code {
      background: none;
      padding: 0;
      color: var(--text-primary);
    }
    
    .manual-content ul {
      margin: 1rem 0;
      padding-left: 1.5rem;
    }
    
    .manual-content li {
      margin-bottom: 0.5rem;
      color: var(--text-secondary);
    }
    
    .manual-content a {
      color: var(--accent-color);
      text-decoration: none;
    }
    
    .manual-content a:hover {
      text-decoration: underline;
    }
    
    /* Table styling */
    .manual-content .table {
      width: 100%;
      border-collapse: collapse;
      margin: 1.5rem 0;
      background: var(--bg-card);
      border: 1px solid var(--border-color);
      border-radius: 0.5rem;
      overflow: hidden;
    }
    
    .manual-content .table th,
    .manual-content .table td {
      padding: 0.75rem 1rem;
      text-align: left;
      border-bottom: 1px solid var(--border-color);
    }
    
    .manual-content .table th {
      background: var(--bg-secondary);
      font-weight: 600;
      color: var(--text-primary);
    }
    
    .manual-content .table td {
      color: var(--text-secondary);
    }
    
    .manual-content .table tr:hover {
      background: var(--bg-secondary);
    }
    
    .manual-content .table tr:last-child td {
      border-bottom: none;
    }
    
    .version-switcher {
      margin-bottom: 1.5rem;
      padding: 1rem;
      background: var(--bg-secondary);
      border-radius: 0.375rem;
      border: 1px solid var(--border-color);
    }
    
    .version-select {
      width: 100%;
      padding: 0.5rem;
      border: 1px solid var(--border-color);
      border-radius: 0.25rem;
      background: var(--bg-primary);
      color: var(--text-primary);
    }
    
    .nav-link {
      display: block;
      padding: 0.5rem 0;
      color: var(--text-secondary);
      text-decoration: none;
      border-radius: 0.25rem;
      transition: all 0.2s ease;
    }
    
    .nav-link:hover {
      color: var(--accent-color);
      background: var(--bg-secondary);
      padding-left: 0.5rem;
    }
    
    .nav-link.active {
      color: var(--accent-color);
      font-weight: 600;
      background: var(--bg-secondary);
      padding-left: 0.5rem;
    }
    
    @media (max-width: 768px) {
      .manual-container {
        grid-template-columns: 1fr;
        padding: 6rem 1rem 2rem;
      }
      
      .manual-sidebar {
        position: static;
        order: 2;
      }
      
      .manual-content {
        order: 1;
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
          ${versions.map(version => `<option value="${version}">${version}</option>`).join('\n          ')}
        </select>
      </div>
      
      ${navigation}
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
      
      versionSelect.addEventListener('change', (e) => {
        const selectedVersion = e.target.value;
        if (selectedVersion !== 'current') {
          // Construct URL for version-specific docs
          const currentPath = window.location.pathname;
          const filename = currentPath.split('/').pop();
          const versionUrl = \`\${selectedVersion}/\${filename}\`;
          window.location.href = versionUrl;
        }
      });
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

function createManualIndexPage(outputDir) {
  const indexContent = `
<h1>JSAR Documentation</h1>

<p>Welcome to the JSAR (JavaScript Spatial Augmented Reality) documentation. This comprehensive guide will help you get started with developing immersive Web experiences using HTML5, CSS3, and TypeScript.</p>

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 1.5rem; margin: 2rem 0;">
  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="introduction.html" style="color: var(--accent-color); text-decoration: none;">Introduction</a></h3>
    <p>Learn about JSAR's revolutionary approach to spatial web development and its key features.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="quick-start.html" style="color: var(--accent-color); text-decoration: none;">Quick Start</a></h3>
    <p>Get up and running quickly with JSAR. Perfect for beginners who want to start building right away.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="basic-concepts.html" style="color: var(--accent-color); text-decoration: none;">Basic Concepts</a></h3>
    <p>Understanding core concepts like 3D spaces, input sources, and spatial HTML fundamentals.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="features.html" style="color: var(--accent-color); text-decoration: none;">Features</a></h3>
    <p>Explore JSAR's powerful features including TypeScript support, Three.js, Babylon.js integration, and more.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="runtime.html" style="color: var(--accent-color); text-decoration: none;">Runtime</a></h3>
    <p>Learn how to integrate JSAR runtime with Unity and understand the Node.js APIs available.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="developer-tools.html" style="color: var(--accent-color); text-decoration: none;">Developer Tools</a></h3>
    <p>Discover the debugging, preview, and packaging tools that make JSAR development easier.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="tutorials.html" style="color: var(--accent-color); text-decoration: none;">Tutorials</a></h3>
    <p>Step-by-step tutorials covering everything from Hello World to advanced spatial interactions.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="references.html" style="color: var(--accent-color); text-decoration: none;">References</a></h3>
    <p>Complete API reference, WebXR documentation, and technical specifications.</p>
  </div>
</div>

<h2>Getting Started</h2>
<p>New to JSAR? We recommend starting with the <a href="introduction.html">Introduction</a> to understand the core concepts, then moving on to the <a href="quick-start.html">Quick Start</a> guide to build your first application.</p>

<h2>Version Information</h2>
<p>This documentation is for JSAR v0.8.2. Use the version switcher in the sidebar to view documentation for other versions.</p>
`;

  const indexHtml = createHtmlTemplate(indexContent, 'JSAR Documentation', 'index.html');
  fs.writeFileSync(path.join(outputDir, 'index.html'), indexHtml);
  console.log('Created manual index page');
}

function buildManualPages() {
  const docsDir = path.join(__dirname, '..', 'docs');
  const manualDir = path.join(docsDir, 'manual');
  const outputDir = path.join(__dirname, '..', '.website', 'manual');
  
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
      const currentFile = relativePath.replace('.mdx', '.html');
      
      // Extract title from first header or filename
      const titleMatch = content.match(/^# (.+)$/m);
      const title = titleMatch ? titleMatch[1] : path.basename(filePath, '.mdx');
      
      // Create output directory if needed
      const outputDirPath = path.dirname(outputPath);
      if (!fs.existsSync(outputDirPath)) {
        fs.mkdirSync(outputDirPath, { recursive: true });
      }
      
      const html = convertMdxToHtml(content, title, currentFile);
      fs.writeFileSync(outputPath, html);
      
      console.log(`Converted: ${relativePath} -> ${path.relative(docsDir, outputPath)}`);
    } catch (error) {
      console.error(`Error converting ${filePath}:`, error.message);
    }
  }
  
  // Create manual index page
  createManualIndexPage(outputDir);
  
  console.log('Manual pages build complete!');
}

function buildVersionedDocs() {
  const websiteDir = path.join(__dirname, '..', '.website');
  const manualDir = path.join(websiteDir, 'manual');
  const versions = getAvailableVersions();
  
  console.log(`Building documentation for ${versions.length} versions...`);
  
  // Create manual directory
  if (!fs.existsSync(manualDir)) {
    fs.mkdirSync(manualDir, { recursive: true });
  }
  
  const currentBranch = getCurrentBranch();
  
  for (const version of versions.slice(0, 5)) {  // Build only last 5 versions for now
    try {
      console.log(`Building docs for ${version}...`);
      
      const versionDir = path.join(manualDir, version);
      
      // Create version directory
      if (!fs.existsSync(versionDir)) {
        fs.mkdirSync(versionDir, { recursive: true });
      }
      
      // Checkout the version tag
      execSync(`git checkout ${version}`, { stdio: 'ignore' });
      
      // Check if docs/manual exists in this version
      const manualDir = path.join(__dirname, '..', 'docs', 'manual');
      if (fs.existsSync(manualDir)) {
        buildManualPagesForVersion(manualDir, versionDir, version);
      } else {
        console.log(`No docs/manual directory found in ${version}, skipping`);
      }
      
    } catch (error) {
      console.error(`Error building docs for ${version}:`, error.message);
    }
  }
  
  // Return to original branch
  try {
    execSync(`git checkout ${currentBranch}`, { stdio: 'ignore' });
  } catch (error) {
    console.warn(`Could not return to branch ${currentBranch}:`, error.message);
  }
  
  console.log('Versioned documentation build complete!');
}

function getCurrentBranch() {
  try {
    return execSync('git branch --show-current', { encoding: 'utf8' }).trim();
  } catch (error) {
    return 'main'; // fallback
  }
}

function buildManualPagesForVersion(manualDir, outputDir, version) {
  // Get all MDX files for this version
  const mdxFiles = getAllMdxFiles(manualDir);
  
  console.log(`  Found ${mdxFiles.length} MDX files in ${version}`);
  
  for (const filePath of mdxFiles) {
    try {
      const content = fs.readFileSync(filePath, 'utf8');
      const relativePath = path.relative(manualDir, filePath);
      const outputPath = path.join(outputDir, relativePath.replace('.mdx', '.html'));
      const currentFile = relativePath.replace('.mdx', '.html');
      
      // Extract title from first header or filename
      const titleMatch = content.match(/^# (.+)$/m);
      const title = titleMatch ? titleMatch[1] : path.basename(filePath, '.mdx');
      
      // Create output directory if needed
      const outputDirPath = path.dirname(outputPath);
      if (!fs.existsSync(outputDirPath)) {
        fs.mkdirSync(outputDirPath, { recursive: true });
      }
      
      const html = convertMdxToHtml(content, title, currentFile, version);
      fs.writeFileSync(outputPath, html);
      
    } catch (error) {
      console.error(`  Error converting ${filePath}:`, error.message);
    }
  }
  
  // Create version-specific index page
  createVersionIndexPage(outputDir, version);
}

function createVersionIndexPage(outputDir, version) {
  const indexContent = `
<h1>JSAR Documentation (${version})</h1>

<p>Welcome to the JSAR documentation for version ${version}. This version-specific documentation reflects the features and APIs available in this release.</p>

<div style="background: var(--bg-secondary); padding: 1rem; border-radius: 0.5rem; border: 1px solid var(--border-color); margin: 1.5rem 0;">
  <strong>Version Notice:</strong> You are viewing documentation for ${version}. 
  <a href="../index.html">Switch to current version</a> for the latest documentation.
</div>

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 1.5rem; margin: 2rem 0;">
  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="introduction.html" style="color: var(--accent-color); text-decoration: none;">Introduction</a></h3>
    <p>Learn about JSAR's revolutionary approach to spatial web development and its key features.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="quick-start.html" style="color: var(--accent-color); text-decoration: none;">Quick Start</a></h3>
    <p>Get up and running quickly with JSAR. Perfect for beginners who want to start building right away.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="basic-concepts.html" style="color: var(--accent-color); text-decoration: none;">Basic Concepts</a></h3>
    <p>Understanding core concepts like 3D spaces, input sources, and spatial HTML fundamentals.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="features.html" style="color: var(--accent-color); text-decoration: none;">Features</a></h3>
    <p>Explore JSAR's powerful features including TypeScript support, Three.js, Babylon.js integration, and more.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="runtime.html" style="color: var(--accent-color); text-decoration: none;">Runtime</a></h3>
    <p>Learn how to integrate JSAR runtime with Unity and understand the Node.js APIs available.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="developer-tools.html" style="color: var(--accent-color); text-decoration: none;">Developer Tools</a></h3>
    <p>Discover the debugging, preview, and packaging tools that make JSAR development easier.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="tutorials.html" style="color: var(--accent-color); text-decoration: none;">Tutorials</a></h3>
    <p>Step-by-step tutorials covering everything from Hello World to advanced spatial interactions.</p>
  </div>

  <div style="background: var(--bg-secondary); padding: 1.5rem; border-radius: 0.5rem; border: 1px solid var(--border-color);">
    <h3><a href="references.html" style="color: var(--accent-color); text-decoration: none;">References</a></h3>
    <p>Complete API reference, WebXR documentation, and technical specifications.</p>
  </div>
</div>
`;

  const indexHtml = createHtmlTemplate(indexContent, `JSAR Documentation (${version})`, 'index.html', version);
  fs.writeFileSync(path.join(outputDir, 'index.html'), indexHtml);
}

if (require.main === module) {
  buildManualPages();
  
  // Build versioned docs if --versions flag is passed
  if (process.argv.includes('--versions')) {
    buildVersionedDocs();
  }
}

module.exports = { buildManualPages, getAvailableVersions };