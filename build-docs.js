const fs = require('fs');
const path = require('path');

class JSARDocsBuilder {
  constructor() {
    this.outputDir = path.join(__dirname, 'build', 'website');
    this.docsDir = path.join(__dirname, 'docs');
    this.compatDataDir = path.join(this.docsDir, 'api', 'browser-compat-data');
    this.jsarVersions = {};
    this.apiData = {};
    this.htmlElementsData = {};
  }

  async build() {
    console.log('Building JSAR documentation website...');
    
    await this.createDirectories();
    await this.loadCompatibilityData();
    await this.generateCSS();
    await this.generateJS();
    await this.generateIndexPage();
    await this.generateAPICompatibilityPage();
    await this.generateVersionHistoryPage();
    
    console.log('Documentation website built successfully!');
    console.log(`Output directory: ${this.outputDir}`);
  }

  async createDirectories() {
    const dirs = [
      this.outputDir,
      path.join(this.outputDir, 'css'),
      path.join(this.outputDir, 'js'),
      path.join(this.outputDir, 'assets')
    ];
    
    for (const dir of dirs) {
      if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
      }
    }
  }

  async loadCompatibilityData() {
    const jsarBrowserFile = path.join(this.compatDataDir, 'browsers', 'jsar.json');
    if (fs.existsSync(jsarBrowserFile)) {
      const browserData = JSON.parse(fs.readFileSync(jsarBrowserFile, 'utf8'));
      this.jsarVersions = browserData.browsers.jsar.releases;
    }

    const apiFiles = fs.readdirSync(path.join(this.compatDataDir, 'api'))
      .filter(file => file.endsWith('.json'));
    
    for (const file of apiFiles) {
      const filePath = path.join(this.compatDataDir, 'api', file);
      const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
      this.apiData[file.replace('.json', '')] = data.api;
    }

    const htmlElementsFile = path.join(this.compatDataDir, 'html', 'global_attributes.json');
    if (fs.existsSync(htmlElementsFile)) {
      const data = JSON.parse(fs.readFileSync(htmlElementsFile, 'utf8'));
      this.htmlElementsData['global_attributes'] = data.html;
    }

    const htmlElementsDir = path.join(this.compatDataDir, 'html', 'elements');
    if (fs.existsSync(htmlElementsDir)) {
      const elementFiles = fs.readdirSync(htmlElementsDir).filter(file => file.endsWith('.json'));
      for (const file of elementFiles) {
        const filePath = path.join(htmlElementsDir, file);
        const data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
        this.htmlElementsData[file.replace('.json', '')] = data.html;
      }
    }
  }

  async generateCSS() {
    const css = `
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  line-height: 1.6;
  color: #333;
  background-color: #f8f9fa;
}

.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 20px;
}

header {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 2rem 0;
  text-align: center;
}

header h1 {
  font-size: 3rem;
  margin-bottom: 0.5rem;
  font-weight: 700;
}

header p {
  font-size: 1.2rem;
  opacity: 0.9;
}

nav {
  background: white;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  padding: 1rem 0;
  position: sticky;
  top: 0;
  z-index: 100;
}

nav ul {
  list-style: none;
  display: flex;
  justify-content: center;
  gap: 2rem;
}

nav a {
  text-decoration: none;
  color: #333;
  font-weight: 500;
  padding: 0.5rem 1rem;
  border-radius: 4px;
  transition: background-color 0.2s;
}

nav a:hover {
  background-color: #f0f0f0;
}

main {
  padding: 2rem 0;
}

.hero {
  background: white;
  border-radius: 8px;
  padding: 2rem;
  margin-bottom: 2rem;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.features {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 2rem;
  margin: 2rem 0;
}

.feature-card {
  background: white;
  padding: 1.5rem;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.feature-card h3 {
  color: #667eea;
  margin-bottom: 1rem;
}

.compatibility-section {
  background: white;
  border-radius: 8px;
  padding: 2rem;
  margin-bottom: 2rem;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.search-filter {
  margin-bottom: 2rem;
  display: flex;
  gap: 1rem;
  flex-wrap: wrap;
}

.search-filter input,
.search-filter select {
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 1rem;
}

.search-filter input {
  flex: 1;
  min-width: 200px;
}

.api-table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 1rem;
}

.api-table th,
.api-table td {
  padding: 0.75rem;
  text-align: left;
  border-bottom: 1px solid #eee;
}

.api-table th {
  background-color: #f8f9fa;
  font-weight: 600;
  color: #495057;
}

.api-table tr:hover {
  background-color: #f8f9fa;
}

.support-status {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  font-size: 0.875rem;
  font-weight: 500;
}

.support-status.supported {
  background-color: #d4edda;
  color: #155724;
}

.support-status.not-supported {
  background-color: #f8d7da;
  color: #721c24;
}

.support-status.partial {
  background-color: #fff3cd;
  color: #856404;
}

.version-badge {
  background-color: #e9ecef;
  color: #495057;
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  font-size: 0.75rem;
  font-weight: 500;
}

.mdn-link {
  color: #667eea;
  text-decoration: none;
}

.mdn-link:hover {
  text-decoration: underline;
}

.category-header {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 1rem;
  margin: 2rem 0 1rem 0;
  border-radius: 8px;
  font-size: 1.25rem;
  font-weight: 600;
}

.stats-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
  margin: 2rem 0;
}

.stat-card {
  background: white;
  padding: 1.5rem;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
  text-align: center;
}

.stat-number {
  font-size: 2rem;
  font-weight: 700;
  color: #667eea;
}

.stat-label {
  color: #666;
  font-size: 0.875rem;
  margin-top: 0.5rem;
}

footer {
  background: #333;
  color: white;
  text-align: center;
  padding: 2rem 0;
  margin-top: 4rem;
}

@media (max-width: 768px) {
  header h1 {
    font-size: 2rem;
  }
  
  nav ul {
    flex-direction: column;
    gap: 0.5rem;
  }
  
  .search-filter {
    flex-direction: column;
  }
  
  .api-table {
    font-size: 0.875rem;
  }
  
  .api-table th,
  .api-table td {
    padding: 0.5rem;
  }
}
`;

    fs.writeFileSync(path.join(this.outputDir, 'css', 'styles.css'), css);
  }

  async generateJS() {
    const js = `
class JSARDocsApp {
  constructor() {
    this.initializeSearch();
    this.initializeFilters();
  }

  initializeSearch() {
    const searchInput = document.getElementById('api-search');
    if (searchInput) {
      searchInput.addEventListener('input', (e) => {
        this.filterAPIs(e.target.value);
      });
    }
  }

  initializeFilters() {
    const statusFilter = document.getElementById('status-filter');
    const categoryFilter = document.getElementById('category-filter');
    
    if (statusFilter) {
      statusFilter.addEventListener('change', () => {
        this.applyFilters();
      });
    }
    
    if (categoryFilter) {
      categoryFilter.addEventListener('change', () => {
        this.applyFilters();
      });
    }
  }

  filterAPIs(searchTerm) {
    const rows = document.querySelectorAll('.api-table tbody tr');
    const term = searchTerm.toLowerCase();
    
    rows.forEach(row => {
      const apiName = row.querySelector('td:first-child').textContent.toLowerCase();
      const description = row.querySelector('td:nth-child(2)').textContent.toLowerCase();
      
      if (apiName.includes(term) || description.includes(term)) {
        row.style.display = '';
      } else {
        row.style.display = 'none';
      }
    });
    
    this.updateVisibleCount();
  }

  applyFilters() {
    const statusFilter = document.getElementById('status-filter');
    const categoryFilter = document.getElementById('category-filter');
    const rows = document.querySelectorAll('.api-table tbody tr');
    
    const statusValue = statusFilter ? statusFilter.value : '';
    const categoryValue = categoryFilter ? categoryFilter.value : '';
    
    rows.forEach(row => {
      let showRow = true;
      
      if (statusValue) {
        const statusElement = row.querySelector('.support-status');
        if (statusElement && !statusElement.classList.contains(statusValue)) {
          showRow = false;
        }
      }
      
      if (categoryValue && showRow) {
        const category = row.dataset.category;
        if (category !== categoryValue) {
          showRow = false;
        }
      }
      
      row.style.display = showRow ? '' : 'none';
    });
    
    this.updateVisibleCount();
  }

  updateVisibleCount() {
    const visibleRows = document.querySelectorAll('.api-table tbody tr:not([style*="display: none"])');
    const countElement = document.getElementById('visible-count');
    if (countElement) {
      countElement.textContent = visibleRows.length;
    }
  }
}

document.addEventListener('DOMContentLoaded', () => {
  new JSARDocsApp();
});
`;

    fs.writeFileSync(path.join(this.outputDir, 'js', 'app.js'), js);
  }

  async generateIndexPage() {
    const readmePath = path.join(__dirname, 'README.md');
    let readmeContent = '';
    if (fs.existsSync(readmePath)) {
      readmeContent = fs.readFileSync(readmePath, 'utf8');
    }

    const html = `<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>JSAR Runtime - 面向元宇宙的浏览器引擎库</title>
    <link rel="stylesheet" href="css/styles.css">
</head>
<body>
    <header>
        <div class="container">
            <h1>JSAR Runtime</h1>
            <p>面向元宇宙的革命性浏览器引擎库</p>
        </div>
    </header>

    <nav>
        <div class="container">
            <ul>
                <li><a href="index.html">首页</a></li>
                <li><a href="api-compatibility.html">API 兼容性</a></li>
                <li><a href="version-history.html">版本历史</a></li>
            </ul>
        </div>
    </nav>

    <main>
        <div class="container">
            <section class="hero">
                <h2>✨ 将 Web 技术带入元宇宙</h2>
                <p>JSAR 是一个革命性的<strong>浏览器引擎库</strong>，专为下一代计算时代（元宇宙）而设计。它使您能够使用现代 Web 技术（HTML5、CSS3、TypeScript）创建沉浸式 Web 内容，并将其无缝集成到主流 3D 引擎中，如 Unity、UnrealEngine 和 RealityKit。</p>
            </section>

            <section class="features">
                <div class="feature-card">
                    <h3>🎯 3D 优先设计</h3>
                    <p>与传统浏览器不同，JSAR 从底层针对 3D 和空间计算进行了优化</p>
                </div>
                <div class="feature-card">
                    <h3>🔧 易于集成</h3>
                    <p>支持多种渲染后端和平台集成</p>
                </div>
                <div class="feature-card">
                    <h3>📱 跨平台兼容</h3>
                    <p>支持 Android 和 macOS 平台</p>
                </div>
                <div class="feature-card">
                    <h3>⚡ 高性能</h3>
                    <p>内置 ECS 实现 GPU 驱动的元素渲染器</p>
                </div>
                <div class="feature-card">
                    <h3>🌐 Web 标准兼容</h3>
                    <p>支持 WebXR、WebGL、Web API 和其他现代 Web 标准</p>
                </div>
            </section>

            <section class="compatibility-section">
                <h2>支持的 URL 格式</h2>
                <p>与经典 Web 浏览器一样，JSAR 不仅支持 HTML 文档，还支持多种格式：</p>
                <table class="api-table">
                    <thead>
                        <tr>
                            <th>格式</th>
                            <th>状态</th>
                            <th>推荐用例</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                            <td>HTML</td>
                            <td><span class="support-status partial">🔄 部分支持</span></td>
                            <td>浏览经典 Web 页面</td>
                        </tr>
                        <tr>
                            <td>GLTF</td>
                            <td><span class="support-status supported">✅ 支持</span></td>
                            <td>显示 3D 模型</td>
                        </tr>
                        <tr>
                            <td>JPEG/PNG</td>
                            <td><span class="support-status supported">✅ 支持</span></td>
                            <td>查看图像</td>
                        </tr>
                        <tr>
                            <td>SVG</td>
                            <td><span class="support-status not-supported">❌ 未开始</span></td>
                            <td>显示 SVG 图像</td>
                        </tr>
                    </tbody>
                </table>
            </section>

            <section class="compatibility-section">
                <h2>快速开始</h2>
                <p>要开始使用 JSAR，请查看我们的 <a href="api-compatibility.html" class="mdn-link">API 兼容性页面</a>，了解哪些 Web API 已经支持，哪些正在开发中。</p>
                <p>您也可以查看 <a href="version-history.html" class="mdn-link">版本历史</a> 了解每个版本的新功能和改进。</p>
            </section>
        </div>
    </main>

    <footer>
        <div class="container">
            <p>&copy; 2024 JSAR Runtime. 基于 MIT 许可证开源。</p>
        </div>
    </footer>

    <script src="js/app.js"></script>
</body>
</html>`;

    fs.writeFileSync(path.join(this.outputDir, 'index.html'), html);
  }

  async generateAPICompatibilityPage() {
    const apiStats = this.calculateAPIStats();
    
    const html = `<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>API 兼容性 - JSAR Runtime</title>
    <link rel="stylesheet" href="css/styles.css">
</head>
<body>
    <header>
        <div class="container">
            <h1>API 兼容性</h1>
            <p>了解 JSAR 中支持的 Web API</p>
        </div>
    </header>

    <nav>
        <div class="container">
            <ul>
                <li><a href="index.html">首页</a></li>
                <li><a href="api-compatibility.html">API 兼容性</a></li>
                <li><a href="version-history.html">版本历史</a></li>
            </ul>
        </div>
    </nav>

    <main>
        <div class="container">
            <section class="stats-grid">
                <div class="stat-card">
                    <div class="stat-number">${apiStats.total}</div>
                    <div class="stat-label">总 API 数量</div>
                </div>
                <div class="stat-card">
                    <div class="stat-number">${apiStats.supported}</div>
                    <div class="stat-label">已支持</div>
                </div>
                <div class="stat-card">
                    <div class="stat-number">${apiStats.partial}</div>
                    <div class="stat-label">部分支持</div>
                </div>
                <div class="stat-card">
                    <div class="stat-number">${apiStats.notSupported}</div>
                    <div class="stat-label">未支持</div>
                </div>
            </section>

            <section class="compatibility-section">
                <div class="search-filter">
                    <input type="text" id="api-search" placeholder="搜索 API...">
                    <select id="status-filter">
                        <option value="">所有状态</option>
                        <option value="supported">已支持</option>
                        <option value="partial">部分支持</option>
                        <option value="not-supported">未支持</option>
                    </select>
                    <select id="category-filter">
                        <option value="">所有类别</option>
                        <option value="dom">DOM API</option>
                        <option value="webgl">WebGL</option>
                        <option value="html">HTML 元素</option>
                        <option value="worker">Web Workers</option>
                        <option value="canvas">Canvas</option>
                    </select>
                </div>
                
                <p>显示 <span id="visible-count">${apiStats.total}</span> 个 API</p>

                ${this.generateDOMAPIsSection()}
                ${this.generateWebGLSection()}
                ${this.generateHTMLElementsSection()}
                ${this.generateWorkersSection()}
                ${this.generateCanvasSection()}
            </section>
        </div>
    </main>

    <footer>
        <div class="container">
            <p>&copy; 2024 JSAR Runtime. 基于 MIT 许可证开源。</p>
        </div>
    </footer>

    <script src="js/app.js"></script>
</body>
</html>`;

    fs.writeFileSync(path.join(this.outputDir, 'api-compatibility.html'), html);
  }

  generateDOMAPIsSection() {
    const domAPIs = ['Document', 'DocumentFragment', 'HTMLDocument', 'HTMLElement', 'MutationObserver'];
    let html = '<div class="category-header">DOM API</div>';
    html += '<table class="api-table"><thead><tr><th>API</th><th>描述</th><th>支持状态</th><th>版本</th><th>MDN 链接</th></tr></thead><tbody>';
    
    for (const apiName of domAPIs) {
      if (this.apiData[apiName]) {
        const apiInfo = this.apiData[apiName][apiName];
        if (apiInfo && apiInfo.__compat) {
          const support = apiInfo.__compat.support.jsar;
          const status = this.getSupportStatus(support);
          const version = this.getSupportVersion(support);
          const mdnUrl = apiInfo.__compat.mdn_url || '';
          
          html += `<tr data-category="dom">
            <td><strong>${apiName}</strong></td>
            <td>${this.getAPIDescription(apiName)}</td>
            <td>${status}</td>
            <td>${version}</td>
            <td>${mdnUrl ? `<a href="${mdnUrl}" class="mdn-link" target="_blank">查看文档</a>` : ''}</td>
          </tr>`;
        }
      }
    }
    
    html += '</tbody></table>';
    return html;
  }

  generateWebGLSection() {
    const webglAPIs = ['WebGLRenderingContext', 'WebGL2RenderingContext'];
    let html = '<div class="category-header">WebGL API</div>';
    html += '<table class="api-table"><thead><tr><th>API</th><th>描述</th><th>支持状态</th><th>版本</th><th>MDN 链接</th></tr></thead><tbody>';
    
    for (const apiName of webglAPIs) {
      if (this.apiData[apiName]) {
        const apiInfo = this.apiData[apiName][apiName];
        if (apiInfo && apiInfo.__compat) {
          const support = apiInfo.__compat.support.jsar;
          const status = this.getSupportStatus(support);
          const version = this.getSupportVersion(support);
          const mdnUrl = apiInfo.__compat.mdn_url || '';
          
          html += `<tr data-category="webgl">
            <td><strong>${apiName}</strong></td>
            <td>${this.getAPIDescription(apiName)}</td>
            <td>${status}</td>
            <td>${version}</td>
            <td>${mdnUrl ? `<a href="${mdnUrl}" class="mdn-link" target="_blank">查看文档</a>` : ''}</td>
          </tr>`;
        }
      }
    }
    
    html += '</tbody></table>';
    return html;
  }

  generateHTMLElementsSection() {
    let html = '<div class="category-header">HTML 元素</div>';
    html += '<table class="api-table"><thead><tr><th>元素</th><th>描述</th><th>支持状态</th><th>版本</th><th>MDN 链接</th></tr></thead><tbody>';
    
    const elements = Object.keys(this.htmlElementsData).filter(key => key !== 'global_attributes');
    
    for (const elementName of elements) {
      const elementData = this.htmlElementsData[elementName];
      if (elementData && elementData[elementName] && elementData[elementName].__compat) {
        const support = elementData[elementName].__compat.support.jsar;
        const status = this.getSupportStatus(support);
        const version = this.getSupportVersion(support);
        const mdnUrl = elementData[elementName].__compat.mdn_url || '';
        
        html += `<tr data-category="html">
          <td><strong>&lt;${elementName}&gt;</strong></td>
          <td>${this.getElementDescription(elementName)}</td>
          <td>${status}</td>
          <td>${version}</td>
          <td>${mdnUrl ? `<a href="${mdnUrl}" class="mdn-link" target="_blank">查看文档</a>` : ''}</td>
        </tr>`;
      }
    }
    
    html += '</tbody></table>';
    return html;
  }

  generateWorkersSection() {
    let html = '<div class="category-header">Web Workers</div>';
    html += '<table class="api-table"><thead><tr><th>API</th><th>描述</th><th>支持状态</th><th>版本</th><th>MDN 链接</th></tr></thead><tbody>';
    
    if (this.apiData['Worker']) {
      const workerInfo = this.apiData['Worker']['Worker'];
      if (workerInfo && workerInfo.__compat) {
        const support = workerInfo.__compat.support.jsar;
        const status = this.getSupportStatus(support);
        const version = this.getSupportVersion(support);
        const mdnUrl = workerInfo.__compat.mdn_url || '';
        
        html += `<tr data-category="worker">
          <td><strong>Worker</strong></td>
          <td>Web Workers API 用于在后台线程中运行脚本</td>
          <td>${status}</td>
          <td>${version}</td>
          <td>${mdnUrl ? `<a href="${mdnUrl}" class="mdn-link" target="_blank">查看文档</a>` : ''}</td>
        </tr>`;
      }
    }
    
    html += '</tbody></table>';
    return html;
  }

  generateCanvasSection() {
    const canvasAPIs = ['HTMLCanvasElement', 'OffscreenCanvas'];
    let html = '<div class="category-header">Canvas API</div>';
    html += '<table class="api-table"><thead><tr><th>API</th><th>描述</th><th>支持状态</th><th>版本</th><th>MDN 链接</th></tr></thead><tbody>';
    
    for (const apiName of canvasAPIs) {
      if (this.apiData[apiName]) {
        const apiInfo = this.apiData[apiName][apiName];
        if (apiInfo && apiInfo.__compat) {
          const support = apiInfo.__compat.support.jsar;
          const status = this.getSupportStatus(support);
          const version = this.getSupportVersion(support);
          const mdnUrl = apiInfo.__compat.mdn_url || '';
          
          html += `<tr data-category="canvas">
            <td><strong>${apiName}</strong></td>
            <td>${this.getAPIDescription(apiName)}</td>
            <td>${status}</td>
            <td>${version}</td>
            <td>${mdnUrl ? `<a href="${mdnUrl}" class="mdn-link" target="_blank">查看文档</a>` : ''}</td>
          </tr>`;
        }
      }
    }
    
    html += '</tbody></table>';
    return html;
  }

  getSupportStatus(support) {
    if (!support) {
      return '<span class="support-status not-supported">❌ 未支持</span>';
    }
    
    if (support.version_added === false) {
      return '<span class="support-status not-supported">❌ 未支持</span>';
    } else if (support.version_added === true || support.version_added) {
      return '<span class="support-status supported">✅ 支持</span>';
    } else {
      return '<span class="support-status partial">🔄 部分支持</span>';
    }
  }

  getSupportVersion(support) {
    if (!support || support.version_added === false) {
      return '';
    }
    
    if (support.version_added === true) {
      return '<span class="version-badge">未知版本</span>';
    }
    
    if (support.version_added) {
      return `<span class="version-badge">${support.version_added}</span>`;
    }
    
    return '';
  }

  getAPIDescription(apiName) {
    const descriptions = {
      'Document': 'Document 接口表示任何在浏览器中载入的网页',
      'DocumentFragment': 'DocumentFragment 接口表示一个没有父级文件的最小文档对象',
      'HTMLDocument': 'HTMLDocument 接口是 HTML 文档的根接口',
      'HTMLElement': 'HTMLElement 接口表示所有的 HTML 元素',
      'MutationObserver': 'MutationObserver 接口提供了监视对 DOM 树所做更改的能力',
      'WebGLRenderingContext': 'WebGL 1.0 渲染上下文',
      'WebGL2RenderingContext': 'WebGL 2.0 渲染上下文',
      'HTMLCanvasElement': 'HTML Canvas 元素接口',
      'OffscreenCanvas': '离屏 Canvas 接口',
      'Worker': 'Web Workers API'
    };
    
    return descriptions[apiName] || '';
  }

  getElementDescription(elementName) {
    const descriptions = {
      'html': 'HTML 文档的根元素',
      'head': '包含文档元数据的容器',
      'body': '文档的主体内容',
      'div': '通用容器元素',
      'span': '通用内联元素',
      'p': '段落元素',
      'img': '图像元素',
      'canvas': '画布元素',
      'script': '脚本元素',
      'audio': '音频元素',
      'video': '视频元素',
      'meta': '元数据元素',
      'link': '外部资源链接元素',
      'header': '页眉元素',
      'footer': '页脚元素',
      'main': '主要内容元素',
      'section': '章节元素',
      'template': '模板元素'
    };
    
    return descriptions[elementName] || '';
  }

  calculateAPIStats() {
    let total = 0;
    let supported = 0;
    let partial = 0;
    let notSupported = 0;
    
    for (const apiName of Object.keys(this.apiData)) {
      const apiInfo = this.apiData[apiName][apiName];
      if (apiInfo && apiInfo.__compat && apiInfo.__compat.support && apiInfo.__compat.support.jsar) {
        total++;
        const support = apiInfo.__compat.support.jsar;
        
        if (support.version_added === false) {
          notSupported++;
        } else if (support.version_added === true || support.version_added) {
          supported++;
        } else {
          partial++;
        }
      }
    }
    
    const elements = Object.keys(this.htmlElementsData).filter(key => key !== 'global_attributes');
    for (const elementName of elements) {
      const elementData = this.htmlElementsData[elementName];
      if (elementData && elementData[elementName] && elementData[elementName].__compat) {
        total++;
        const support = elementData[elementName].__compat.support.jsar;
        
        if (!support || support.version_added === false) {
          notSupported++;
        } else if (support.version_added === true || support.version_added) {
          supported++;
        } else {
          partial++;
        }
      }
    }
    
    return { total, supported, partial, notSupported };
  }

  async generateVersionHistoryPage() {
    const changelogPath = path.join(this.docsDir, 'changelogs', 'alpha.md');
    let changelogContent = '';
    if (fs.existsSync(changelogPath)) {
      changelogContent = fs.readFileSync(changelogPath, 'utf8');
    }

    const html = `<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>版本历史 - JSAR Runtime</title>
    <link rel="stylesheet" href="css/styles.css">
</head>
<body>
    <header>
        <div class="container">
            <h1>版本历史</h1>
            <p>JSAR Runtime 的发布历史和更新日志</p>
        </div>
    </header>

    <nav>
        <div class="container">
            <ul>
                <li><a href="index.html">首页</a></li>
                <li><a href="api-compatibility.html">API 兼容性</a></li>
                <li><a href="version-history.html">版本历史</a></li>
            </ul>
        </div>
    </nav>

    <main>
        <div class="container">
            <section class="compatibility-section">
                <h2>发布版本</h2>
                ${this.generateVersionTable()}
            </section>

            <section class="compatibility-section">
                <h2>详细更新日志</h2>
                <div class="changelog-content">
                    ${this.convertMarkdownToHTML(changelogContent)}
                </div>
            </section>
        </div>
    </main>

    <footer>
        <div class="container">
            <p>&copy; 2024 JSAR Runtime. 基于 MIT 许可证开源。</p>
        </div>
    </footer>

    <script src="js/app.js"></script>
</body>
</html>`;

    fs.writeFileSync(path.join(this.outputDir, 'version-history.html'), html);
  }

  generateVersionTable() {
    let html = '<table class="api-table"><thead><tr><th>版本</th><th>发布日期</th><th>发布说明</th></tr></thead><tbody>';
    
    const versions = Object.keys(this.jsarVersions).reverse();
    for (const version of versions) {
      const versionInfo = this.jsarVersions[version];
      const releaseDate = versionInfo.release_date || '';
      const releaseNotes = versionInfo.release_notes || '';
      
      html += `<tr>
        <td><span class="version-badge">${version}</span></td>
        <td>${releaseDate}</td>
        <td>${releaseNotes ? `<a href="${releaseNotes}" class="mdn-link" target="_blank">查看说明</a>` : ''}</td>
      </tr>`;
    }
    
    html += '</tbody></table>';
    return html;
  }

  convertMarkdownToHTML(markdown) {
    return markdown
      .replace(/^# (.*$)/gim, '<h2>$1</h2>')
      .replace(/^## (.*$)/gim, '<h3>$1</h3>')
      .replace(/^### (.*$)/gim, '<h4>$1</h4>')
      .replace(/^\- (.*$)/gim, '<li>$1</li>')
      .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>')
      .replace(/\*(.*?)\*/g, '<em>$1</em>')
      .replace(/`(.*?)`/g, '<code>$1</code>')
      .replace(/\n\n/g, '</p><p>')
      .replace(/^(.*)$/gim, '<p>$1</p>')
      .replace(/<p><li>/g, '<ul><li>')
      .replace(/<\/li><\/p>/g, '</li></ul>')
      .replace(/<p><h/g, '<h')
      .replace(/<\/h([1-6])><\/p>/g, '</h$1>');
  }
}

const builder = new JSARDocsBuilder();
builder.build().catch(console.error);
