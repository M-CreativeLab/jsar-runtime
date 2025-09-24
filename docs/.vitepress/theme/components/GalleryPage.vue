<template>
  <div class="gallery-container">
    <header class="gallery-header">
      <h1>JSAR Examples Gallery</h1>
      <p class="description">Browse and discover {{ fixturesData?.total || 0 }} HTML examples and test pages.</p>
    </header>

    <div class="gallery-controls">
      <div class="search-box">
        <input 
          type="text" 
          v-model="searchQuery" 
          placeholder="Search examples by name, title, or description..." 
          @input="filterFixtures"
          class="search-input">
        <button @click="clearSearch" class="clear-button" v-if="searchQuery">
          ✕
        </button>
      </div>

      <div class="filter-container">
        <div class="filter-group">
          <label for="category-filter">Category:</label>
          <select id="category-filter" v-model="selectedCategory" @change="filterFixtures" class="filter-select">
            <option value="all">All Categories</option>
            <option v-for="category in categories" :key="category.name" :value="category.name">
              {{ category.name }} ({{ category.count }})
            </option>
          </select>
        </div>
      </div>
    </div>

    <div class="stats-container">
      <div class="stat-item">
        <div class="stat-value">{{ filteredFixtures.length }}</div>
        <div class="stat-label">{{ filteredFixtures.length === fixturesData?.total ? 'Total' : 'Filtered' }} Examples</div>
      </div>
      <div class="stat-item">
        <div class="stat-value">{{ categories.length }}</div>
        <div class="stat-label">Categories</div>
      </div>
    </div>

    <!-- Loading state -->
    <div v-if="loading" class="loading-state">
      <div class="loading-spinner"></div>
      <p>Loading examples...</p>
    </div>

    <!-- Error state -->
    <div v-else-if="error" class="error-state">
      <p>Failed to load examples: {{ error }}</p>
    </div>

    <!-- No results state -->
    <div v-else-if="filteredFixtures.length === 0 && searchQuery" class="no-results">
      <p>No examples found matching "{{ searchQuery }}"</p>
      <button @click="clearSearch" class="clear-search-btn">Clear search</button>
    </div>

    <!-- Examples grid -->
    <div v-else class="examples-grid">
      <div 
        v-for="fixture in filteredFixtures" 
        :key="fixture.filename"
        class="example-card"
      >
        <div class="card-header">
          <h3 class="example-title" :title="fixture.title">{{ fixture.title }}</h3>
          <span class="example-category">{{ fixture.category }}</span>
        </div>
        
        <div class="card-body">
          <div class="example-filename" :title="fixture.filename">
            <code>{{ fixture.filename }}</code>
          </div>
          <p v-if="fixture.description" class="example-description">
            {{ fixture.description }}
          </p>
        </div>

        <div class="card-actions">
          <a :href="fixture.rawUrl" target="_blank" class="action-btn primary">
            Open
          </a>
          <a :href="fixture.githubUrl" target="_blank" class="action-btn secondary">
            View Source
          </a>
        </div>
      </div>
    </div>

    <!-- Categories overview -->
    <div class="categories-section">
      <h2>Example Categories</h2>
      <div class="categories-grid">
        <div v-for="category in categories" :key="category.name" class="category-card" @click="filterByCategory(category.name)">
          <h3>{{ category.name }}</h3>
          <p>{{ category.count }} example{{ category.count === 1 ? '' : 's' }}</p>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'GalleryPage',
  data() {
    return {
      fixturesData: null,
      searchQuery: '',
      selectedCategory: 'all',
      loading: true,
      error: null,
      filteredFixtures: []
    }
  },
  computed: {
    categories() {
      return this.fixturesData?.categories || [];
    },
    allFixtures() {
      return this.fixturesData?.categories.flatMap(cat => cat.fixtures) || [];
    }
  },
  async mounted() {
    await this.loadFixturesData();
    this.filterFixtures();
  },
  methods: {
    async loadFixturesData() {
      try {
        this.loading = true;
        // Load the fixtures data JSON from public directory with correct base path
        const response = await fetch('/jsar-runtime/fixtures-data.json');
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        this.fixturesData = await response.json();
      } catch (error) {
        console.error('Failed to load fixtures data:', error);
        this.error = error.message;
      } finally {
        this.loading = false;
      }
    },
    filterFixtures() {
      const query = this.searchQuery.toLowerCase().trim();
      let fixtures = this.allFixtures;

      // Filter by category
      if (this.selectedCategory !== 'all') {
        fixtures = fixtures.filter(fixture => fixture.category === this.selectedCategory);
      }

      // Filter by search query
      if (query) {
        fixtures = fixtures.filter(fixture => 
          fixture.filename.toLowerCase().includes(query) ||
          fixture.title.toLowerCase().includes(query) ||
          (fixture.description && fixture.description.toLowerCase().includes(query))
        );
      }

      this.filteredFixtures = fixtures;
    },
    clearSearch() {
      this.searchQuery = '';
      this.selectedCategory = 'all';
      this.filterFixtures();
    },
    filterByCategory(categoryName) {
      this.selectedCategory = categoryName;
      this.searchQuery = '';
      this.filterFixtures();
      // Scroll to top
      this.$el.scrollIntoView({ behavior: 'smooth' });
    }
  }
}
</script>

<style scoped>
.gallery-container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 2rem;
}

.gallery-header {
  text-align: center;
  margin-bottom: 2rem;
}

.gallery-header h1 {
  color: var(--vp-c-text-1);
  font-size: 2.5rem;
  font-weight: 700;
  margin-bottom: 1rem;
}

.description {
  color: var(--vp-c-text-2);
  font-size: 1.1rem;
  margin-bottom: 0;
}

.gallery-controls {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  margin-bottom: 2rem;
  padding: 1.5rem;
  background: var(--vp-c-bg-soft);
  border-radius: 8px;
  border: 1px solid var(--vp-c-border);
}

.search-box {
  position: relative;
  flex: 1;
}

.search-input {
  width: 100%;
  padding: 0.75rem 1rem;
  border: 1px solid var(--vp-c-border);
  border-radius: 6px;
  background: var(--vp-c-bg);
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
  transition: color 0.2s;
}

.clear-button:hover {
  color: var(--vp-c-text-1);
}

.filter-container {
  display: flex;
  flex-wrap: wrap;
  gap: 1rem;
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
  padding: 0.5rem;
  border: 1px solid var(--vp-c-border);
  border-radius: 4px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
}

.stats-container {
  display: flex;
  justify-content: center;
  gap: 2rem;
  margin-bottom: 2rem;
  padding: 1rem;
  background: var(--vp-c-bg-alt);
  border-radius: 8px;
}

.stat-item {
  text-align: center;
}

.stat-value {
  font-size: 2rem;
  font-weight: bold;
  color: var(--vp-c-brand);
}

.stat-label {
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
  margin-top: 0.25rem;
}

.loading-state,
.error-state,
.no-results {
  text-align: center;
  padding: 3rem;
  color: var(--vp-c-text-2);
}

.loading-spinner {
  width: 40px;
  height: 40px;
  border: 4px solid var(--vp-c-border);
  border-top: 4px solid var(--vp-c-brand);
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin: 0 auto 1rem;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.clear-search-btn {
  margin-top: 1rem;
  padding: 0.5rem 1rem;
  background: var(--vp-c-brand);
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.clear-search-btn:hover {
  background: var(--vp-c-brand-darker);
}

.examples-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(350px, 1fr));
  gap: 1.5rem;
  margin-bottom: 3rem;
}

.example-card {
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-border);
  border-radius: 8px;
  padding: 1.5rem;
  transition: transform 0.2s, box-shadow 0.2s, border-color 0.2s;
}

.example-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
  border-color: var(--vp-c-brand);
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 1rem;
}

.example-title {
  color: var(--vp-c-text-1);
  font-size: 1.1rem;
  font-weight: 600;
  margin: 0;
  line-height: 1.3;
  flex: 1;
  white-space: nowrap;
  overflow-x: hidden;
  text-overflow: ellipsis;
}

.example-category {
  background: var(--vp-c-brand);
  color: white;
  font-size: 0.75rem;
  padding: 0.25rem 0.5rem;
  border-radius: 12px;
  white-space: nowrap;
  margin-left: 0.5rem;
}

.card-body {
  margin-bottom: 1.5rem;
  height: 6rem;
}

.example-filename {
  padding: 0.5rem 0;
  margin-bottom: 0.75rem;
  width: 100%;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.example-filename code {
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
  padding: 0.25rem 0.5rem;
  border-radius: 4px;
  font-size: 0.85rem;
  border: 1px solid var(--vp-c-border);
}

.example-description {
  color: var(--vp-c-text-2);
  font-size: 0.9rem;
  line-height: 1.4;
  height: 2.5rem;
  overflow-y: auto;
  margin: 0;
}

.card-actions {
  display: flex;
  gap: 0.75rem;
}

.action-btn {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.6rem 1rem;
  border-radius: 6px;
  text-decoration: none;
  white-space: nowrap;
  font-size: 1rem;
  font-weight: 500;
  transition: all 0.2s;
  flex: 1;
  justify-content: center;
}

.action-btn.primary {
  background: var(--vp-c-brand);
  color: white;
}

.action-btn.primary:hover {
  transform: translateY(-1px);
}

.action-btn.secondary {
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
  border: 1px solid var(--vp-c-border);
}

.action-btn.secondary:hover {
  background: var(--vp-c-bg-alt);
  border-color: var(--vp-c-brand);
}

.btn-icon {
  font-size: 0.8rem;
}

.categories-section {
  margin-top: 3rem;
  padding-top: 2rem;
  border-top: 1px solid var(--vp-c-border);
}

.categories-section h2 {
  color: var(--vp-c-text-1);
  font-size: 1.5rem;
  margin-bottom: 1rem;
}

.categories-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
}

.category-card {
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-border);
  border-radius: 6px;
  padding: 1rem;
  cursor: pointer;
  transition: all 0.2s;
  text-align: center;
}

.category-card:hover {
  background: var(--vp-c-bg-alt);
  border-color: var(--vp-c-brand);
  transform: translateY(-1px);
}

.category-card h3 {
  color: var(--vp-c-text-1);
  font-size: 1rem;
  margin: 0 0 0.5rem 0;
}

.category-card p {
  color: var(--vp-c-text-2);
  font-size: 0.9rem;
  margin: 0;
}

@media (max-width: 768px) {
  .gallery-container {
    padding: 1rem;
  }
  
  .gallery-header h1 {
    font-size: 2rem;
  }
  
  .examples-grid {
    grid-template-columns: 1fr;
    gap: 1rem;
  }
  
  .stats-container {
    gap: 1rem;
  }
  
  .filter-container {
    flex-direction: column;
  }
  
  .card-actions {
    flex-direction: column;
  }
}
</style>