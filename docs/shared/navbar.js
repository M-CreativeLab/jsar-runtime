// Shared Navbar Component
export function createNavbar() {
  return `
    <nav class="bg-primary border-b border-color shadow-sm fixed top-0 left-0 z-50 backdrop-blur-sm bg-opacity-95" style="width: 100vw;">
      <div class="py-0" style="max-width: 1280px; margin: 0 auto; padding-left: 1rem; padding-right: 1rem; position: relative;">
        <div class="flex items-center" style="width: 100%; justify-content: space-between;">
          <!-- Left side: Logo and version -->
          <div class="flex items-center gap-6 py-2">
            <div class="text-2xl font-bold">
              <a href="index.html" class="bg-clip-text text-transparent bg-gradient-to-r from-accent-color to-bg-gradient-end" style="background: linear-gradient(135deg, var(--accent-color), var(--bg-gradient-end)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; background-clip: text; text-decoration: none;">
                <span class="hidden sm:inline">YODAOS JSAR</span>
                <span class="sm:hidden">JSAR</span>
              </a>
            </div>
            <div class="version-badge latest">v0.8.2</div>
          </div>
          
          <!-- Center: Desktop Navigation -->
          <div class="desktop-nav flex items-center gap-5 mx-auto">
            <a href="index.html#features" class="nav-link">Features</a>
            <a href="index.html#documentation" class="nav-link">Documentation</a>
            <a href="blog.html" class="nav-link">Blog</a>
            <a href="index.html#changelog" class="nav-link">Changelog</a>
            <a href="browser-compat-data.html" class="nav-link">Compatibility</a>
            <a href="https://github.com/M-CreativeLab/jsar-runtime" class="btn btn-secondary mr-4 my-2" target="_blank" style="padding: 0.4rem 0.8rem;">
              <span>GitHub</span>
              <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="ml-1"><path d="M7 17L17 7"/><path d="M7 7h10v10"/></svg>
            </a>
          </div>
          
          <!-- Right side: Theme Toggle Button -->
          <div class="flex items-center">
            <button id="theme-toggle" class="theme-toggle-btn" aria-label="Toggle theme">
              <svg id="sun-icon" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="5"/>
                <path d="M12 1v2M12 21v2M4.22 4.22l1.42 1.42M18.36 18.36l1.42 1.42M1 12h2M21 12h2M4.22 19.78l1.42-1.42M18.36 5.64l1.42-1.42"/>
              </svg>
              <svg id="moon-icon" xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="display: none;">
                <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/>
              </svg>
            </button>
          </div>
          
          <!-- Mobile Menu Toggle -->
          <div class="flex items-center gap-4 md:hidden">
            <button class="mobile-menu-toggle" id="mobile-menu-toggle" aria-label="Toggle menu">
              <span id="menu-icon">☰</span>
            </button>
          </div>
          
          <!-- Mobile Menu -->
          <div class="mobile-menu" id="mobile-menu">
            <a href="index.html#features">Features</a>
            <a href="index.html#documentation">Documentation</a>
            <a href="blog.html">Blog</a>
            <a href="index.html#changelog">Changelog</a>
            <a href="browser-compat-data.html">Compatibility</a>
            <a href="https://github.com/M-CreativeLab/jsar-runtime" target="_blank">GitHub</a>
          </div>
        </div>
      </div>
    </nav>
  `;
}