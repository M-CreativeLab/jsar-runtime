class JSARCompatibilityViewer {
  constructor() {
    this.compatData = [];
    this.filteredData = [];
    this.browserInfo = null;
    this.currentVersion = null;
    this.init();
  }

  async init() {
    try {
      await this.loadRuntimeVersion();
      await this.loadCompatibilityData();
      this.setupEventListeners();
      this.setupThemeToggle();
      this.updateStats();
      this.renderAPIs();
      this.hideLoading();
    } catch (error) {
      console.error('Failed to load compatibility data:', error);
      this.showError();
    }
  }

  async loadRuntimeVersion() {
    try {
      // Try to load package.json to get the current version
      const response = await fetch('../package.json');
      if (response.ok) {
        const packageData = await response.json();
        this.currentVersion = packageData.version;
      } else {
        // Fallback: try to get from browser compat data
        this.browserInfo = await this.loadJSON('./api/browser-compat-data/browsers/jsar.json');
        if (this.browserInfo && this.browserInfo.browsers && this.browserInfo.browsers.jsar) {
          const versions = Object.keys(this.browserInfo.browsers.jsar.releases || {});
          this.currentVersion = versions.sort((a, b) => this.compareVersions(b, a))[0] || '0.8.2';
        } else {
          this.currentVersion = '0.8.2'; // Default fallback
        }
      }
    } catch (error) {
      console.warn('Could not load runtime version:', error);
      this.currentVersion = '0.8.2'; // Default fallback
    }

    // Update the UI with the current version
    document.getElementById('runtime-version').textContent = this.currentVersion;
  }

  async loadCompatibilityData() {
    // Load browser info if not already loaded
    if (!this.browserInfo) {
      this.browserInfo = await this.loadJSON('./api/browser-compat-data/browsers/jsar.json');
    }
    
    // Define all compatibility data files
    const files = [
      // API files
      './api/browser-compat-data/api/Document.json',
      './api/browser-compat-data/api/DocumentFragment.json',
      './api/browser-compat-data/api/HTMLCanvasElement.json',
      './api/browser-compat-data/api/HTMLDocument.json',
      './api/browser-compat-data/api/HTMLElement.json',
      './api/browser-compat-data/api/MutationObserver.json',
      './api/browser-compat-data/api/OffscreenCanvas.json',
      './api/browser-compat-data/api/WebGL2RenderingContext.json',
      './api/browser-compat-data/api/WebGLRenderingContext.json',
      './api/browser-compat-data/api/Worker.json',
      
      // HTML element files
      './api/browser-compat-data/html/elements/audio.json',
      './api/browser-compat-data/html/elements/body.json',
      './api/browser-compat-data/html/elements/canvas.json',
      './api/browser-compat-data/html/elements/div.json',
      './api/browser-compat-data/html/elements/footer.json',
      './api/browser-compat-data/html/elements/head.json',
      './api/browser-compat-data/html/elements/header.json',
      './api/browser-compat-data/html/elements/html.json',
      './api/browser-compat-data/html/elements/img.json',
      './api/browser-compat-data/html/elements/link.json',
      './api/browser-compat-data/html/elements/main.json',
      './api/browser-compat-data/html/elements/meta.json',
      './api/browser-compat-data/html/elements/p.json',
      './api/browser-compat-data/html/elements/script.json',
      './api/browser-compat-data/html/elements/section.json',
      './api/browser-compat-data/html/elements/span.json',
      './api/browser-compat-data/html/elements/template.json',
      './api/browser-compat-data/html/elements/video.json',
      
      // Global attributes
      './api/browser-compat-data/html/global_attributes.json'
    ];

    // Load all files in parallel
    const promises = files.map(file => this.loadJSON(file));
    const results = await Promise.all(promises);
    
    // Process and flatten the data
    this.compatData = this.processCompatibilityData(results);
    this.filteredData = [...this.compatData];
  }

  async loadJSON(url) {
    try {
      const response = await fetch(url);
      if (!response.ok) {
        throw new Error(`Failed to load ${url}: ${response.status}`);
      }
      return await response.json();
    } catch (error) {
      console.warn(`Could not load ${url}:`, error);
      return null;
    }
  }

  processCompatibilityData(results) {
    const processed = [];
    
    results.forEach(data => {
      if (!data) return;
      
      // Process API data
      if (data.api) {
        Object.keys(data.api).forEach(apiName => {
          const apiData = data.api[apiName];
          if (apiData.__compat) {
            processed.push(this.createAPIItem(apiName, apiData.__compat, 'api', apiName));
          }
          
          // Process nested methods/properties
          Object.keys(apiData).forEach(key => {
            if (key !== '__compat' && apiData[key].__compat) {
              processed.push(this.createAPIItem(
                `${apiName}.${key}`, 
                apiData[key].__compat, 
                'api', 
                apiName
              ));
            }
          });
        });
      }
      
      // Process HTML data
      if (data.html) {
        if (data.html.elements) {
          Object.keys(data.html.elements).forEach(elementName => {
            const elementData = data.html.elements[elementName];
            if (elementData.__compat) {
              processed.push(this.createAPIItem(
                `<${elementName}>`, 
                elementData.__compat, 
                'html', 
                'HTML Elements'
              ));
            }
          });
        }
        
        if (data.html.global_attributes) {
          Object.keys(data.html.global_attributes).forEach(attrName => {
            const attrData = data.html.global_attributes[attrName];
            if (attrData.__compat) {
              processed.push(this.createAPIItem(
                attrName, 
                attrData.__compat, 
                'global', 
                'Global Attributes'
              ));
            }
          });
        }
      }
    });
    
    return processed.sort((a, b) => a.name.localeCompare(b.name));
  }

  createAPIItem(name, compat, category, parentName) {
    const support = compat.support?.jsar;
    const version = support?.version_added || 'Unknown';
    
    return {
      name,
      category,
      parentName,
      version,
      mdnUrl: compat.mdn_url,
      specUrl: compat.spec_url,
      experimental: compat.status?.experimental || false,
      standardTrack: compat.status?.standard_track || false,
      deprecated: compat.status?.deprecated || false
    };
  }

  setupEventListeners() {
    const searchInput = document.getElementById('search');
    const categorySelect = document.getElementById('category');
    const versionSelect = document.getElementById('version');
    
    searchInput.addEventListener('input', () => this.filterAPIs());
    categorySelect.addEventListener('change', () => this.filterAPIs());
    versionSelect.addEventListener('change', () => this.filterAPIs());
  }

  setupThemeToggle() {
    const themeToggle = document.getElementById('theme-toggle');
    const themeIcon = document.getElementById('theme-icon');
    const body = document.body;
    
    // Initialize theme from localStorage or default to light
    const savedTheme = localStorage.getItem('theme') || 'light';
    body.setAttribute('data-theme', savedTheme);
    themeIcon.textContent = savedTheme === 'dark' ? '🌙' : '☀️';
    
    themeToggle.addEventListener('click', () => {
      const currentTheme = body.getAttribute('data-theme');
      const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
      
      body.setAttribute('data-theme', newTheme);
      themeIcon.textContent = newTheme === 'dark' ? '🌙' : '☀️';
      localStorage.setItem('theme', newTheme);
    });
  }

  filterAPIs() {
    const searchTerm = document.getElementById('search').value.toLowerCase();
    const categoryFilter = document.getElementById('category').value;
    const versionFilter = document.getElementById('version').value;
    
    this.filteredData = this.compatData.filter(api => {
      // Search filter - improved to include more comprehensive search
      const matchesSearch = !searchTerm || 
        api.name.toLowerCase().includes(searchTerm) ||
        api.parentName.toLowerCase().includes(searchTerm) ||
        (api.mdnUrl && api.mdnUrl.toLowerCase().includes(searchTerm));
      
      // Category filter
      const matchesCategory = categoryFilter === 'all' || api.category === categoryFilter;
      
      // Version filter
      const matchesVersion = versionFilter === 'all' || 
        this.compareVersions(api.version, versionFilter) >= 0;
      
      return matchesSearch && matchesCategory && matchesVersion;
    });
    
    this.renderAPIs();
  }

  compareVersions(a, b) {
    if (a === 'Unknown' || b === 'Unknown') return 0;
    
    const parseVersion = v => v.split('.').map(n => parseInt(n, 10));
    const versionA = parseVersion(a);
    const versionB = parseVersion(b);
    
    for (let i = 0; i < Math.max(versionA.length, versionB.length); i++) {
      const numA = versionA[i] || 0;
      const numB = versionB[i] || 0;
      if (numA !== numB) return numA - numB;
    }
    return 0;
  }

  updateStats() {
    const totalAPIs = this.compatData.length;
    const stableAPIs = this.compatData.filter(api => !api.experimental && !api.deprecated).length;
    const experimentalAPIs = this.compatData.filter(api => api.experimental).length;
    const latestVersion = this.getLatestVersion();
    
    document.getElementById('total-apis').textContent = totalAPIs;
    document.getElementById('stable-apis').textContent = stableAPIs;
    document.getElementById('experimental-apis').textContent = experimentalAPIs;
    document.getElementById('latest-version').textContent = latestVersion;
  }

  getLatestVersion() {
    const versions = this.compatData
      .map(api => api.version)
      .filter(v => v !== 'Unknown')
      .sort((a, b) => this.compareVersions(b, a));
    
    return versions[0] || this.currentVersion || 'Unknown';
  }

  renderAPIs() {
    const container = document.getElementById('api-container');
    const emptyState = document.getElementById('empty-state');
    
    if (this.filteredData.length === 0) {
      container.innerHTML = '';
      emptyState.classList.remove('hidden');
      return;
    }
    
    emptyState.classList.add('hidden');
    
    // Group APIs by parent/category for better organization
    const grouped = this.groupAPIsByParent(this.filteredData);
    
    container.innerHTML = Object.keys(grouped).map(groupName => {
      const apis = grouped[groupName];
      return this.renderAPIGroup(groupName, apis);
    }).join('');
  }

  groupAPIsByParent(apis) {
    const grouped = {};
    
    apis.forEach(api => {
      const key = api.parentName || api.category;
      if (!grouped[key]) {
        grouped[key] = [];
      }
      grouped[key].push(api);
    });
    
    return grouped;
  }

  renderAPIGroup(groupName, apis) {
    const categoryColor = this.getCategoryColor(apis[0].category);
    
    return `
      <div class="api-item card rounded-lg shadow-sm ${categoryColor} overflow-hidden" style="border-left: 4px solid;">
        <div class="p-6">
          <div class="flex items-center justify-between mb-4">
            <h3 class="text-lg font-semibold text-primary">${groupName}</h3>
            <span class="badge bg-secondary text-secondary">
              ${apis.length} API${apis.length > 1 ? 's' : ''}
            </span>
          </div>
          
          <div class="space-y-3">
            ${apis.map(api => this.renderAPIItem(api)).join('')}
          </div>
        </div>
      </div>
    `;
  }

  renderAPIItem(api) {
    const statusBadge = this.getStatusBadge(api);
    const versionBadge = `<span class="badge bg-accent text-white">v${api.version}</span>`;
    
    return `
      <div class="flex items-center justify-between p-4 bg-secondary rounded" style="padding: 0.75rem;">
        <div class="flex-1">
          <div class="flex items-center gap-2 mb-1">
            <code class="text-sm font-mono text-primary">${api.name}</code>
            ${statusBadge}
            ${versionBadge}
          </div>
          <div class="flex gap-2 text-xs">
            ${api.mdnUrl ? `<a href="${api.mdnUrl}" target="_blank" class="text-accent">📚 MDN Docs</a>` : ''}
            ${api.specUrl ? `<a href="${api.specUrl}" target="_blank" class="text-success">📋 Specification</a>` : ''}
          </div>
        </div>
      </div>
    `;
  }

  getCategoryColor(category) {
    switch (category) {
      case 'api': return 'border-accent';
      case 'html': return 'border-success';
      case 'global': return 'border-warning';
      default: return 'border-gray-500';
    }
  }

  getStatusBadge(api) {
    if (api.deprecated) {
      return '<span class="badge bg-red-100 text-error">⚠️ Deprecated</span>';
    }
    if (api.experimental) {
      return '<span class="badge bg-yellow-100 text-warning">🧪 Experimental</span>';
    }
    if (api.standardTrack) {
      return '<span class="badge bg-green-100 text-success">✅ Stable</span>';
    }
    return '';
  }

  hideLoading() {
    document.getElementById('loading').classList.add('hidden');
    document.getElementById('content').classList.remove('hidden');
  }

  showError() {
    document.getElementById('loading').innerHTML = `
      <div class="text-center py-12">
        <div class="text-error" style="font-size: 4rem; margin-bottom: 1rem;">⚠️</div>
        <h3 class="text-lg font-medium text-primary mb-2">Failed to load compatibility data</h3>
        <p class="text-secondary">Please check the console for more details or try refreshing the page</p>
      </div>
    `;
  }
}

// Initialize the app when the DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
  new JSARCompatibilityViewer();
});