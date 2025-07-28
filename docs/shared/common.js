// Shared Common JavaScript Functionality
import { createNavbar } from './navbar.js';
import { createFooter } from './footer.js';

// Theme Toggle Functionality
export function initThemeToggle() {
  const themeToggle = document.getElementById('theme-toggle');
  const themeToggleMobile = document.getElementById('theme-toggle-mobile');
  const sunIcon = document.getElementById('sun-icon');
  const moonIcon = document.getElementById('moon-icon');
  const sunIconMobile = document.getElementById('sun-icon-mobile');
  const moonIconMobile = document.getElementById('moon-icon-mobile');
  const body = document.body;
  
  // Check for saved theme preference or default to 'light'
  const currentTheme = localStorage.getItem('theme') || 'light';
  body.setAttribute('data-theme', currentTheme);
  
  // Update icon based on current theme
  updateThemeIcon(currentTheme);
  
  function updateThemeIcon(theme) {
    if (theme === 'dark') {
      // Desktop icons
      if (sunIcon && moonIcon) {
        sunIcon.style.display = 'none';
        moonIcon.style.display = 'block';
      }
      // Mobile icons
      if (sunIconMobile && moonIconMobile) {
        sunIconMobile.style.display = 'none';
        moonIconMobile.style.display = 'block';
      }
    } else {
      // Desktop icons
      if (sunIcon && moonIcon) {
        sunIcon.style.display = 'block';
        moonIcon.style.display = 'none';
      }
      // Mobile icons
      if (sunIconMobile && moonIconMobile) {
        sunIconMobile.style.display = 'block';
        moonIconMobile.style.display = 'none';
      }
    }
  }
  
  function toggleTheme() {
    const currentTheme = body.getAttribute('data-theme');
    const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
    
    body.setAttribute('data-theme', newTheme);
    localStorage.setItem('theme', newTheme);
    updateThemeIcon(newTheme);
  }
  
  // Add event listeners to both buttons
  if (themeToggle) {
    themeToggle.addEventListener('click', toggleTheme);
  }
  if (themeToggleMobile) {
    themeToggleMobile.addEventListener('click', toggleTheme);
  }
}

// Mobile Menu Functionality
export function initMobileMenu() {
  const mobileMenuToggle = document.getElementById('mobile-menu-toggle');
  const mobileMenu = document.getElementById('mobile-menu');
  const menuIcon = document.getElementById('menu-icon');
  
  if (mobileMenuToggle && mobileMenu) {
    mobileMenuToggle.addEventListener('click', () => {
      const isActive = mobileMenu.classList.contains('active');
      
      if (isActive) {
        mobileMenu.classList.remove('active');
        menuIcon.textContent = '☰';
      } else {
        mobileMenu.classList.add('active');
        menuIcon.textContent = '✕';
      }
    });
    
    // Close mobile menu when clicking on a link
    const mobileMenuLinks = mobileMenu.querySelectorAll('a');
    mobileMenuLinks.forEach(link => {
      link.addEventListener('click', () => {
        mobileMenu.classList.remove('active');
        menuIcon.textContent = '☰';
      });
    });
    
    // Close mobile menu when clicking outside
    document.addEventListener('click', (event) => {
      if (!mobileMenuToggle.contains(event.target) && !mobileMenu.contains(event.target)) {
        mobileMenu.classList.remove('active');
        menuIcon.textContent = '☰';
      }
    });
  }
}

// Initialize shared components
export function initSharedComponents() {
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
}

// Simple markdown to HTML parser for blog content
export function parseMarkdown(markdown) {
  let html = markdown;
  
  // Headers
  html = html.replace(/^### (.*$)/gim, '<h3 class="text-xl font-semibold text-primary mb-4 mt-6">$1</h3>');
  html = html.replace(/^## (.*$)/gim, '<h2 class="text-2xl font-bold text-primary mb-6 mt-8">$1</h2>');
  html = html.replace(/^# (.*$)/gim, '<h1 class="text-3xl font-bold text-primary mb-8">$1</h1>');
  
  // Bold
  html = html.replace(/\*\*(.*?)\*\*/g, '<strong class="font-semibold text-primary">$1</strong>');
  
  // Italic in published date format
  html = html.replace(/\*([^*]+)\*/g, '<em class="italic text-tertiary">$1</em>');
  
  // Code blocks
  html = html.replace(/```[\s\S]*?```/g, (match) => {
    const code = match.replace(/```/g, '').trim();
    return `<pre class="bg-secondary border border-color rounded-lg p-4 mb-4 overflow-x-auto"><code class="text-sm font-mono">${escapeHtml(code)}</code></pre>`;
  });
  
  // Inline code
  html = html.replace(/`([^`]+)`/g, '<code class="bg-secondary px-2 py-1 rounded text-sm font-mono">$1</code>');
  
  // Links
  html = html.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2" class="text-accent hover:underline" target="_blank">$1</a>');
  
  // Lists (convert lines starting with - to list items)
  html = html.replace(/^- (.+)$/gm, '<li class="text-secondary mb-2 ml-4">• $1</li>');
  
  // Wrap consecutive list items in ul tags
  html = html.replace(/(<li[^>]*>.*?<\/li>\s*)+/g, (match) => {
    return `<ul class="mb-4 space-y-1">${match}</ul>`;
  });
  
  // Convert double line breaks to paragraphs
  const paragraphs = html.split(/\n\s*\n/);
  html = paragraphs.map(p => {
    p = p.trim();
    if (!p) return '';
    if (p.startsWith('<h') || p.startsWith('<ul') || p.startsWith('<pre')) {
      return p;
    }
    return `<p class="text-lg text-secondary mb-4 leading-relaxed">${p}</p>`;
  }).join('\n');
  
  return html;
}

// HTML escape function
function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}