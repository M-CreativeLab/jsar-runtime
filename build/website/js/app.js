
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
