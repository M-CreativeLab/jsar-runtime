// Shared Footer Component
export function createFooter() {
  return `
    <footer class="py-12 border-t border-color">
      <div class="container">
        <div class="grid grid-cols-1 md:grid-cols-3 gap-8">
          <div>
            <div class="flex items-center gap-2 mb-4">
              <div class="text-2xl font-bold text-accent">JSAR</div>
              <div class="version-badge">v0.8.2</div>
            </div>
            <p class="text-secondary">
              Revolutionary Browser Engine Library designed for The Spatial Web.
            </p>
          </div>
          
          <div>
            <h4 class="font-semibold text-primary mb-4">Resources</h4>
            <div class="space-y-2">
              <a href="manual/introduction.mdx" class="block text-secondary hover:text-accent transition-colors">Documentation</a>
              <a href="browser-compat-data.html" class="block text-secondary hover:text-accent transition-colors">API Compatibility</a>
              <a href="changelogs/alpha.md" class="block text-secondary hover:text-accent transition-colors">Changelog</a>
              <a href="https://github.com/M-CreativeLab/jsar-runtime" class="block text-secondary hover:text-accent transition-colors" target="_blank">GitHub Repository</a>
            </div>
          </div>
          
          <div>
            <h4 class="font-semibold text-primary mb-4">Community</h4>
            <div class="space-y-2">
              <a href="https://github.com/M-CreativeLab/jsar-runtime/issues" class="block text-secondary hover:text-accent transition-colors" target="_blank">Report Issues</a>
              <a href="https://github.com/M-CreativeLab/jsar-runtime/discussions" class="block text-secondary hover:text-accent transition-colors" target="_blank">Discussions</a>
              <a href="contributing/coding_style_guides.md" class="block text-secondary hover:text-accent transition-colors">Contributing</a>
            </div>
          </div>
        </div>
        
        <div class="border-t border-color mt-8 pt-8 text-center">
          <p class="text-tertiary text-sm">
            © 2024 M-CreativeLab. Licensed under the Apache 2.0 License.
          </p>
        </div>
      </div>
    </footer>
  `;
}