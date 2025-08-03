<template>
  <div class="browser-compat-container">
    <header class="compat-header">
      <h1>JSAR Browser Compatibility Data</h1>
      <p class="description">Explore the Web APIs supported in JSAR Runtime across different versions</p>
    </header>

    <div class="compat-card">
      <div class="search-filter-container">
        <div class="search-box">
          <input type="text" v-model="searchQuery" placeholder="Search APIs..." @input="filterAPIs"
            class="search-input">
          <button @click="searchQuery = ''; filterAPIs()" class="clear-button" v-if="searchQuery">
            ✕
          </button>
        </div>

        <div class="filter-container">
          <div class="filter-group">
            <label for="category-filter">Category:</label>
            <select id="category-filter" v-model="selectedCategory" @change="filterAPIs" class="filter-select">
              <option value="all">All Categories</option>
              <option value="api">Web APIs</option>
              <option value="html">HTML Elements</option>
              <option value="global">Global Attributes</option>
            </select>
          </div>

          <div class="filter-group">
            <label for="version-filter">Version:</label>
            <select id="version-filter" v-model="selectedVersion" @change="filterAPIs" class="filter-select">
              <option value="all">All Versions</option>
              <option v-for="version in versions" :key="version" :value="version">{{ version }}</option>
            </select>
          </div>
        </div>
      </div>

      <div class="stats-container">
        <div class="stat-item">
          <div class="stat-value">{{ totalAPIs }}</div>
          <div class="stat-label">Total APIs</div>
        </div>
        <div class="stat-item">
          <div class="stat-value">{{ stableAPIs }}</div>
          <div class="stat-label">Stable</div>
        </div>
        <div class="stat-item">
          <div class="stat-value">{{ experimentalAPIs }}</div>
          <div class="stat-label">Experimental</div>
        </div>
        <div class="stat-item">
          <div class="stat-value">{{ deprecatedAPIs }}</div>
          <div class="stat-label">Deprecated</div>
        </div>
      </div>

      <div id="version-chart" class="version-chart"></div>

      <div v-if="loading" class="loading-container">
        <div class="loading-spinner"></div>
        <p>Loading compatibility data...</p>
      </div>

      <div v-else-if="error" class="error-container">
        <div class="error-icon">⚠️</div>
        <h3>Failed to load compatibility data</h3>
        <p>Unable to fetch API compatibility information</p>
        <button @click="initializeData" class="retry-button">🔄 Retry</button>
      </div>

      <div v-else-if="filteredAPIs.length === 0" class="empty-state">
        <div class="empty-icon">🔍</div>
        <h3>No APIs found</h3>
        <p>Try adjusting your search or filters</p>
        <button @click="resetFilters" class="reset-button">Reset Filters</button>
      </div>

      <div v-else class="api-list">
        <div v-for="(group, groupName) in groupedAPIs" :key="groupName" class="api-group">
          <div class="api-group-header" :class="getCategoryColor(group[0].category)">
            <span class="group-icon">{{ getCategoryIcon(group[0].category) }}</span>
            <h3 class="group-name">{{ groupName }}</h3>
            <span class="group-count">{{ group.length }}</span>
          </div>

          <div class="api-items">
            <div v-for="api in group" :key="api.id" class="api-item">
              <div class="api-item-row">
                <div class="api-main-info">
                  <div class="api-name-section">
                    <h4 class="api-name">{{ api.name }}</h4>
                    <div class="api-badges">
                      <span v-if="api.deprecated" class="badge bg-red-100 text-error">⚠️ Deprecated</span>
                      <span v-else-if="api.experimental" class="badge bg-yellow-100 text-warning">🧪 Experimental</span>
                      <span v-else-if="api.standardTrack" class="badge bg-green-100 text-success">✅ Stable</span>
                      <span class="version-badge">{{ api.version }}</span>
                    </div>
                  </div>
                  <div class="api-links">
                    <a v-if="api.mdnUrl" :href="api.mdnUrl" target="_blank" class="doc-link mdn-link"
                      title="MDN Documentation">📚 MDN</a>
                    <a v-if="api.specUrl" :href="api.specUrl" target="_blank" class="doc-link spec-link"
                      title="Official Specification">📋 Spec</a>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'BrowserCompatData',
  data() {
    return {
      compatData: [],
      filteredAPIs: [],
      groupedAPIs: {},
      browserInfo: null,
      versions: [],
      loading: true,
      error: false,
      searchQuery: '',
      selectedCategory: 'all',
      selectedVersion: 'all',
      totalAPIs: 0,
      stableAPIs: 0,
      experimentalAPIs: 0,
      deprecatedAPIs: 0
    }
  },
  mounted() {
    this.initializeData()
  },
  methods: {
    async initializeData() {
      this.loading = true
      this.error = false

      try {
        // Load browser info and runtime versions
        await this.loadRuntimeVersions()

        // Load compatibility data
        await this.loadCompatibilityData()

        // Setup event listeners and theme toggle
        this.setupThemeToggle()

        // Update statistics
        this.updateStats()

        // Filter and render APIs
        this.filterAPIs()

        // Render version chart
        this.$nextTick(() => {
          this.renderVersionChart()
        })

        this.loading = false
      } catch (error) {
        console.error('Failed to initialize compatibility data:', error)
        this.loading = false
        this.error = true
      }
    },

    async loadRuntimeVersions() {
      try {
        // Try to load from GitHub API first
        const response = await fetch('https://api.github.com/repos/M-CreativeLab/jsar-runtime/releases')
        if (response.ok) {
          const releases = await response.json()
          this.versions = releases
            .filter(release => !release.draft && !release.prerelease)
            .map(release => release.tag_name.replace(/^v/, ''))
            .sort(this.compareVersions)
            .reverse()
          return
        }
      } catch (error) {
        console.warn('Failed to load versions from GitHub API:', error)
      }

      try {
        // Fallback to package.json
        const response = await fetch('/package.json')
        if (response.ok) {
          const packageJson = await response.json()
          this.versions = [packageJson.version]
          return
        }
      } catch (error) {
        console.warn('Failed to load version from package.json:', error)
      }

      // Final fallback: extract from browser compatibility data
      this.versions = ['0.1.0', '0.2.0', '0.3.0'] // Default fallback versions
    },

    async loadCompatibilityData() {
      try {
        // Load the merged single JSON file
        const allData = await this.loadJSON('/api/browser-compat-data/all.json')
        // 设置浏览器信息
        this.browserInfo = allData.browsers.jsar

        // 初始化 APIs 数组
        const apis = []

        // 处理 API 数据
        if (allData.api) {
          Object.entries(allData.api).forEach(([apiName, apiData]) => {
            this.processAPIData(apis, apiName, apiData, 'api')
          })
        }

        // 处理 HTML 元素数据
        if (allData.html && allData.html.elements) {
          Object.entries(allData.html.elements).forEach(([elementName, elementData]) => {
            this.processAPIData(apis, elementName, elementData, 'html')
          })
        }

        // 处理全局属性数据
        if (allData.html && allData.html.global_attributes) {
          Object.entries(allData.html.global_attributes).forEach(([attrName, attrData]) => {
            this.processAPIData(apis, attrName, attrData, 'attribute')
          })
        }

        // 处理 CSS 属性数据
        if (allData.css && allData.css.properties) {
          Object.entries(allData.css.properties).forEach(([propName, propData]) => {
            this.processAPIData(apis, propName, propData, 'css')
          })
        }

        // 存储处理后的数据
        this.compatData = apis
      } catch (error) {
        console.error('Error loading compatibility data:', error)
        throw error
      }
    },

    async loadJSON(path) {
      const response = await fetch(path)
      if (!response.ok) {
        throw new Error(`Failed to load ${path}: ${response.status} ${response.statusText}`)
      }
      return await response.json()
    },



    processAPIData(apis, name, data, category) {
      if (data.__compat) {
        const compat = data.__compat
        const support = compat.support?.jsar

        if (support) {
          apis.push(this.createAPIItem(name, compat, support, category))
        }
      }

      // Process nested APIs
      Object.entries(data).forEach(([subName, subData]) => {
        if (subName !== '__compat' && typeof subData === 'object') {
          this.processAPIData(apis, `${name}.${subName}`, subData, category)
        }
      })
    },

    createAPIItem(name, compat, support, category) {
      const version = support.version_added || 'unknown'
      const experimental = support.flags?.length > 0 || compat.status?.experimental === true
      const deprecated = compat.status?.deprecated === true
      const standardTrack = compat.status?.standard_track !== false

      return {
        id: `${category}-${name}`,
        name,
        version,
        experimental,
        deprecated,
        standardTrack,
        category,
        parent: name.includes('.') ? name.split('.')[0] : category,
        mdnUrl: compat.mdn_url,
        specUrl: compat.spec_url
      }
    },

    setupThemeToggle() {
      // This method is implemented in the original JS file
      // but we'll use VitePress's built-in theme toggle instead
    },

    filterAPIs() {
      const query = this.searchQuery.toLowerCase().trim()

      this.filteredAPIs = this.compatData.filter(api => {
        // Filter by search query
        const matchesSearch = query === '' || api.name.toLowerCase().includes(query)

        // Filter by category
        const matchesCategory = this.selectedCategory === 'all' || api.category === this.selectedCategory

        // Filter by version
        const matchesVersion = this.selectedVersion === 'all' ||
          (api.version !== 'unknown' && this.compareVersions(api.version, this.selectedVersion) <= 0)

        return matchesSearch && matchesCategory && matchesVersion
      })

      // Group APIs by parent or category
      this.groupedAPIs = this.groupAPIsByParent(this.filteredAPIs)
    },

    compareVersions(a, b) {
      const partsA = a.split('.').map(Number)
      const partsB = b.split('.').map(Number)

      for (let i = 0; i < Math.max(partsA.length, partsB.length); i++) {
        const partA = partsA[i] || 0
        const partB = partsB[i] || 0
        if (partA !== partB) return partA - partB
      }

      return 0
    },

    updateStats() {
      this.totalAPIs = this.compatData.length
      this.stableAPIs = this.compatData.filter(api => api.standardTrack && !api.experimental && !api.deprecated).length
      this.experimentalAPIs = this.compatData.filter(api => api.experimental && !api.deprecated).length
      this.deprecatedAPIs = this.compatData.filter(api => api.deprecated).length
    },

    groupAPIsByParent(apis) {
      const groups = {}

      apis.forEach(api => {
        const groupKey = api.parent
        if (!groups[groupKey]) {
          groups[groupKey] = []
        }
        groups[groupKey].push(api)
      })

      // Sort groups by name
      return Object.fromEntries(
        Object.entries(groups).sort((a, b) => a[0].localeCompare(b[0]))
      )
    },

    getCategoryColor(category) {
      switch (category) {
        case 'api': return 'border-accent'
        case 'html': return 'border-success'
        case 'global': return 'border-warning'
        default: return 'border-gray-500'
      }
    },

    getCategoryIcon(category) {
      switch (category) {
        case 'api': return '⚙️'
        case 'html': return '🏗️'
        case 'global': return '🌐'
        default: return '📁'
      }
    },

    renderVersionChart() {
      if (!this.browserInfo || !this.browserInfo.jsar?.releases) {
        console.warn('Browser info not available for chart rendering')
        return
      }

      const chartContainer = document.getElementById('version-chart')
      if (!chartContainer) {
        console.warn('Chart container not found')
        return
      }

      // Get all versions and sort them
      const versions = Object.keys(this.browserInfo.jsar.releases).sort(this.compareVersions)

      // Calculate API counts per version
      const versionData = versions.map(version => {
        const stableCount = this.compatData.filter(api =>
          api.version <= version && (api.standardTrack && !api.experimental && !api.deprecated)
        ).length

        const experimentalCount = this.compatData.filter(api =>
          api.version <= version && api.experimental && !api.deprecated
        ).length

        return {
          version,
          stable: stableCount,
          experimental: experimentalCount,
          total: stableCount + experimentalCount
        }
      })

      // Find max count for scaling
      const maxCount = Math.max(...versionData.map(d => d.total))
      const chartWidth = chartContainer.offsetWidth || 800
      const chartHeight = 250
      const padding = { top: 20, right: 40, bottom: 60, left: 40 }
      const innerWidth = chartWidth - padding.left - padding.right
      const innerHeight = chartHeight - padding.top - padding.bottom

      // Create line chart HTML with SVG
      const chartHTML = `
        <div class="chart-legend">
          <div class="legend-item">
            <div class="legend-color" style="background-color: #10b981;"></div>
            <span>Stable APIs</span>
          </div>
          <div class="legend-item">
            <div class="legend-color" style="background-color: #f59e0b;"></div>
            <span>Experimental APIs</span>
          </div>
        </div>
        <div class="chart-container" style="height: ${chartHeight}px; position: relative;">
          <svg width="${chartWidth}" height="${chartHeight}" style="display: block;">
            <!-- Grid lines -->
            ${Array.from({ length: 6 }, (_, i) => {
        const y = padding.top + (innerHeight / 5) * i
        const value = Math.round(maxCount - (maxCount / 5) * i)
        return `
                <line x1="${padding.left}" y1="${y}" x2="${chartWidth - padding.right}" y2="${y}" 
                      stroke="var(--border-color)" stroke-width="1" opacity="0.3"/>
                <text x="${padding.left - 10}" y="${y + 4}" fill="var(--text-tertiary)" 
                      font-size="12" text-anchor="end">${value}</text>
              `
      }).join('')}
            
            <!-- X-axis labels -->
            ${versionData.map((data, i) => {
        const x = padding.left + (innerWidth / (versionData.length - 1)) * i
        return `
                <text x="${x}" y="${chartHeight - 10}" fill="var(--text-tertiary)" 
                      font-size="11" text-anchor="middle">v${data.version}</text>
              `
      }).join('')}
            
            <!-- Stable APIs line -->
            <polyline
              fill="none"
              stroke="#10b981"
              stroke-width="3"
              points="${versionData.map((data, i) => {
        const x = padding.left + (innerWidth / (versionData.length - 1)) * i
        const y = padding.top + innerHeight - (data.stable / maxCount) * innerHeight
        return `${x},${y}`
      }).join(' ')}"
            />
            
            <!-- Experimental APIs line -->
            <polyline
              fill="none"
              stroke="#f59e0b"
              stroke-width="3"
              points="${versionData.map((data, i) => {
        const x = padding.left + (innerWidth / (versionData.length - 1)) * i
        const y = padding.top + innerHeight - (data.experimental / maxCount) * innerHeight
        return `${x},${y}`
      }).join(' ')}"
            />
            
            <!-- Data points for stable APIs -->
            ${versionData.map((data, i) => {
        const x = padding.left + (innerWidth / (versionData.length - 1)) * i
        const y = padding.top + innerHeight - (data.stable / maxCount) * innerHeight
        return `
                <circle cx="${x}" cy="${y}" r="4" fill="#10b981" stroke="#ffffff" stroke-width="2"
                        class="chart-point" data-tooltip="v${data.version}: ${data.stable} stable APIs" 
                        style="cursor: pointer;"/>
              `
      }).join('')}
            
            <!-- Data points for experimental APIs -->
            ${versionData.map((data, i) => {
        const x = padding.left + (innerWidth / (versionData.length - 1)) * i
        const y = padding.top + innerHeight - (data.experimental / maxCount) * innerHeight
        return `
                <circle cx="${x}" cy="${y}" r="4" fill="#f59e0b" stroke="#ffffff" stroke-width="2"
                        class="chart-point" data-tooltip="v${data.version}: ${data.experimental} experimental APIs" 
                        style="cursor: pointer;"/>
              `
      }).join('')}
          </svg>
        </div>
      `

      chartContainer.innerHTML = chartHTML

      // Add tooltip functionality
      const tooltip = document.createElement('div')
      tooltip.className = 'chart-tooltip'
      chartContainer.appendChild(tooltip)

      chartContainer.querySelectorAll('.chart-point').forEach(point => {
        point.addEventListener('mouseenter', (e) => {
          tooltip.innerHTML = e.target.getAttribute('data-tooltip')
          tooltip.style.display = 'block'
        })

        point.addEventListener('mousemove', (e) => {
          const rect = chartContainer.getBoundingClientRect()
          tooltip.style.left = (e.clientX - rect.left + 10) + 'px'
          tooltip.style.top = (e.clientY - rect.top - 10) + 'px'
        })

        point.addEventListener('mouseleave', () => {
          tooltip.style.display = 'none'
        })
      })
    },

    resetFilters() {
      this.searchQuery = ''
      this.selectedCategory = 'all'
      this.selectedVersion = 'all'
      this.filterAPIs()
    },

    escapeHtml(text) {
      const div = document.createElement('div')
      div.textContent = text
      return div.innerHTML
    }
  }
}
</script>

<style scoped>
.browser-compat-container {
  width: 100%;
  max-width: 1200px;
  margin: 0 auto;
  padding: 1rem;
  font-family: var(--vp-font-family-base);
}

.compat-header {
  margin-bottom: 2rem;
  text-align: center;
}

.compat-header h1 {
  font-size: 2rem;
  font-weight: 700;
  margin-bottom: 0.5rem;
  color: var(--vp-c-text-1);
}

.compat-header .description {
  font-size: 1.1rem;
  color: var(--vp-c-text-2);
}

.compat-card {
  background-color: var(--vp-c-bg-soft);
  border-radius: 8px;
  box-shadow: var(--vp-shadow-2);
  padding: 1.5rem;
  margin-bottom: 2rem;
}

/* Search and Filter Styles */
.search-filter-container {
  display: flex;
  flex-wrap: wrap;
  gap: 1rem;
  margin-bottom: 1.5rem;
  align-items: center;
  justify-content: space-between;
}

.search-box {
  position: relative;
  flex: 1;
  min-width: 250px;
}

.search-input {
  width: 100%;
  padding: 0.75rem 1rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  background-color: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-size: 1rem;
  transition: border-color 0.2s, box-shadow 0.2s;
}

.search-input:focus {
  outline: none;
  border-color: var(--vp-c-brand);
  box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.2);
}

.clear-button {
  position: absolute;
  right: 10px;
  top: 50%;
  transform: translateY(-50%);
  background: none;
  border: none;
  color: var(--vp-c-text-3);
  cursor: pointer;
  font-size: 1rem;
  padding: 0.25rem;
}

.filter-container {
  display: flex;
  gap: 1rem;
  flex-wrap: wrap;
}

.filter-group {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.filter-group label {
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
  white-space: nowrap;
}

.filter-select {
  padding: 0.5rem 2rem 0.5rem 0.75rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  background-color: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-size: 0.9rem;
  appearance: none;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'%3E%3Cpolyline points='6 9 12 15 18 9'%3E%3C/polyline%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: right 0.5rem center;
  background-size: 1rem;
}

.filter-select:focus {
  outline: none;
  border-color: var(--vp-c-brand);
}

/* Stats Container */
.stats-container {
  display: flex;
  justify-content: space-around;
  flex-wrap: wrap;
  gap: 1rem;
  margin-bottom: 2rem;
  padding: 1rem;
  background-color: var(--vp-c-bg);
  border-radius: 8px;
  box-shadow: var(--vp-shadow-1);
}

.stat-item {
  text-align: center;
  padding: 0.5rem 1rem;
}

.stat-value {
  font-size: 1.8rem;
  font-weight: 700;
  color: var(--vp-c-brand);
  margin-bottom: 0.25rem;
}

.stat-label {
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
}

/* Version Chart */
.version-chart {
  margin-bottom: 2rem;
  padding: 1rem;
  background-color: var(--vp-c-bg);
  border-radius: 8px;
  box-shadow: var(--vp-shadow-1);
}

.chart-legend {
  display: flex;
  justify-content: center;
  gap: 2rem;
  margin-bottom: 1rem;
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
}

.legend-color {
  width: 16px;
  height: 16px;
  border-radius: 4px;
}

.chart-tooltip {
  position: absolute;
  display: none;
  background-color: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
  padding: 0.5rem 0.75rem;
  border-radius: 4px;
  font-size: 0.85rem;
  box-shadow: var(--vp-shadow-2);
  z-index: 10;
  pointer-events: none;
  white-space: nowrap;
}

/* API List */
.api-list {
  display: flex;
  flex-direction: column;
  gap: 2rem;
}

.api-group {
  background-color: var(--vp-c-bg);
  border-radius: 8px;
  overflow: hidden;
  box-shadow: var(--vp-shadow-1);
}

.api-group-header {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  padding: 1rem 1.5rem;
  background-color: var(--vp-c-bg-soft);
  border-left: 4px solid;
}

.border-accent {
  border-color: var(--vp-c-brand);
}

.border-success {
  border-color: var(--vp-c-green);
}

.border-warning {
  border-color: var(--vp-c-yellow);
}

.border-gray-500 {
  border-color: var(--vp-c-gray);
}

.group-icon {
  font-size: 1.25rem;
}

.group-name {
  font-size: 1.1rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
  margin: 0;
  flex: 1;
}

.group-count {
  background-color: var(--vp-c-bg);
  color: var(--vp-c-text-2);
  font-size: 0.85rem;
  padding: 0.25rem 0.5rem;
  border-radius: 999px;
  min-width: 1.5rem;
  text-align: center;
}

.api-items {
  padding: 0.5rem;
}

.api-item {
  padding: 0.75rem 1rem;
  border-bottom: 1px solid var(--vp-c-divider);
}

.api-item:last-child {
  border-bottom: none;
}

.api-item-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
}

.api-main-info {
  flex: 1;
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 1rem;
}

.api-name-section {
  display: flex;
  flex-direction: column;
  gap: 0.25rem;
}

.api-name {
  font-size: 1rem;
  font-weight: 500;
  color: var(--vp-c-text-1);
  margin: 0;
  word-break: break-word;
}

.api-badges {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  margin-top: 0.25rem;
}

.badge {
  display: inline-flex;
  align-items: center;
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  font-size: 0.75rem;
  font-weight: 500;
}

.bg-red-100 {
  background-color: rgba(239, 68, 68, 0.1);
}

.bg-yellow-100 {
  background-color: rgba(245, 158, 11, 0.1);
}

.bg-green-100 {
  background-color: rgba(16, 185, 129, 0.1);
}

.text-error {
  color: var(--vp-c-danger);
}

.text-warning {
  color: var(--vp-c-warning);
}

.text-success {
  color: var(--vp-c-green);
}

.version-badge {
  display: inline-flex;
  align-items: center;
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  font-size: 0.75rem;
  font-weight: 500;
  background-color: var(--vp-c-bg-soft);
  color: var(--vp-c-text-2);
}

.api-links {
  display: flex;
  gap: 0.75rem;
}

.doc-link {
  display: inline-flex;
  align-items: center;
  gap: 0.25rem;
  font-size: 0.85rem;
  color: var(--vp-c-brand);
  text-decoration: none;
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  transition: background-color 0.2s;
}

.doc-link:hover {
  background-color: var(--vp-c-bg-soft);
  text-decoration: none;
}

/* Loading, Error, and Empty States */
.loading-container,
.error-container,
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 3rem 1rem;
  text-align: center;
}

.loading-spinner {
  width: 40px;
  height: 40px;
  border: 4px solid rgba(59, 130, 246, 0.1);
  border-left-color: var(--vp-c-brand);
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin-bottom: 1rem;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

.error-icon,
.empty-icon {
  font-size: 3rem;
  margin-bottom: 1rem;
}

.error-container h3,
.empty-state h3 {
  font-size: 1.25rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
  margin-bottom: 0.5rem;
}

.error-container p,
.empty-state p {
  color: var(--vp-c-text-2);
  margin-bottom: 1.5rem;
}

.retry-button,
.reset-button {
  padding: 0.5rem 1rem;
  background-color: var(--vp-c-brand);
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 0.9rem;
  cursor: pointer;
  transition: background-color 0.2s;
}

.retry-button:hover,
.reset-button:hover {
  background-color: var(--vp-c-brand-dark);
}

/* Responsive Adjustments */
@media (max-width: 768px) {
  .search-filter-container {
    flex-direction: column;
    align-items: stretch;
  }

  .search-box {
    width: 100%;
  }

  .filter-container {
    justify-content: space-between;
  }

  .api-main-info {
    flex-direction: column;
    align-items: flex-start;
  }

  .api-links {
    margin-top: 0.5rem;
  }

  .stats-container {
    gap: 0.5rem;
  }

  .stat-item {
    padding: 0.5rem;
    min-width: 40%;
  }
}

@media (max-width: 480px) {
  .compat-card {
    padding: 1rem;
  }

  .filter-container {
    flex-direction: column;
    align-items: stretch;
  }

  .filter-group {
    justify-content: space-between;
  }

  .filter-select {
    flex: 1;
  }

  .stat-item {
    min-width: 45%;
  }

  .stat-value {
    font-size: 1.5rem;
  }
}
</style>
