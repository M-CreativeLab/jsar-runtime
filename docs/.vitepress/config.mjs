import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'JSAR',
  description: 'Revolutionary Browser Engine for Spatial Web',
  ignoreDeadLinks: true, // Temporarily ignore dead links while we migrate
  
  head: [
    ['link', { rel: 'icon', href: '/favicon.ico' }],
    ['meta', { name: 'theme-color', content: '#3b82f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'JSAR | Revolutionary Browser Engine for Spatial Web' }],
    ['meta', { property: 'og:site_name', content: 'JSAR' }],
    ['meta', { property: 'og:image', content: '/JSAR_POSTER.png' }],
  ],

  themeConfig: {
    
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Documentation', link: '/manual/introduction' },
      { text: 'Blog', link: '/blog' },
      { text: 'API Reference', link: '/api/browser-compat-data' },
      { text: 'GitHub', link: 'https://github.com/M-CreativeLab/jsar-runtime' }
    ],

    sidebar: {
      '/manual/': [
        {
          text: 'Getting Started',
          collapsed: false,
          items: [
            { text: 'Introduction', link: '/manual/introduction' },
            { text: 'Quick Start', link: '/manual/quick-start' },
            { text: 'Basic Concepts', link: '/manual/basic-concepts' }
          ]
        },
        {
          text: 'Features',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/manual/features' },
            { text: 'Runtime Integration', link: '/manual/runtime' },
            { text: 'Delivery', link: '/manual/delivery' }
          ]
        },
        {
          text: 'Developer Tools',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/manual/developer-tools' },
            { text: 'Debugging', link: '/manual/developer-tools/debugging' },
            { text: 'Preview', link: '/manual/developer-tools/preview' }
          ]
        },
        {
          text: 'Tutorials',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/manual/tutorials' }
          ]
        },
        {
          text: 'References',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/manual/references' }
          ]
        }
      ],
      '/api/': [
        {
          text: 'API Reference',
          items: [
            { text: 'Embedder API', link: '/api/embedder' },
            { text: 'Browser Compatibility', link: '/api/browser-compat-data' }
          ]
        }
      ],
      '/contributing/': [
        {
          text: 'Contributing',
          items: [
            { text: 'Coding Style Guides', link: '/contributing/coding_style_guides' },
            { text: 'Publishing WebAPIs', link: '/contributing/publish_webapis' },
            { text: 'Deferred Composition', link: '/contributing/defferred_composition' },
            { text: 'Programming Languages', link: '/contributing/programming_languages' }
          ]
        }
      ]
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/M-CreativeLab/jsar-runtime' }
    ],

    footer: {
      message: 'Released under the Apache 2.0 License.',
      copyright: 'Copyright © 2024 M-CreativeLab'
    },

    editLink: {
      pattern: 'https://github.com/M-CreativeLab/jsar-runtime/edit/main/docs/:path'
    },

    lastUpdated: {
      text: 'Updated at',
      formatOptions: {
        dateStyle: 'full',
        timeStyle: 'medium'
      }
    }
  },

  markdown: {
    image: {
      lazyLoading: true
    }
  }
})