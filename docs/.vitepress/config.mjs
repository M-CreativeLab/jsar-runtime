import { fileURLToPath } from 'node:url'
import { defineConfig } from 'vitepress'
import { buildStart } from './build-hooks.mjs'
import taskCheckbox from 'markdown-it-task-checkbox'

export default defineConfig({
  base: '/jsar-runtime/',
  vite: {
    resolve: {
      alias: [
        {
          find: /^..*\/VPHomeHero.vue$/,
          replacement: fileURLToPath(new URL('./theme/components/CustomHomeHero.vue', import.meta.url))
        },
      ],
    },
    optimizeDeps: {
      include: ['three']
    },
    ssr: {
      noExternal: ['three']
    }
  },
  title: 'YODAOS JSAR',
  description: 'Open Source Browser Engine for the Spatial Web',
  ignoreDeadLinks: true, // Temporarily ignore dead links while we migrate
  buildEnd: buildStart,

  head: [
    ['link', { rel: 'icon', href: '/favicon.ico' }],
    ['meta', { name: 'theme-color', content: '#3b82f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'JSAR | Open Source Browser Engine for the Spatial Web' }],
    ['meta', { property: 'og:site_name', content: 'JSAR' }],
    ['meta', { property: 'og:image', content: '/JSAR_POSTER.png' }],
  ],

  themeConfig: {

    nav: [
      { text: 'Documentation', link: '/manual/introduction' },
      {
        text: 'API Reference',
        link: '/api/browser-compat-data',
      },
      { text: 'Changelog', link: '/changelogs/alpha' },
      { text: 'Blog', link: '/blog' },
      { text: 'GitHub', link: 'https://github.com/M-CreativeLab/jsar-runtime' }
    ],

    sidebar: {
      '/manual/': [
        {
          text: 'Getting Started',
          collapsed: false,
          items: [
            { text: 'What\' JSAR', link: '/manual/introduction' },
            { text: 'Quick Start', link: '/manual/quick-start' },
            {
              text: 'Basic Concepts',
              link: '/manual/basic-concepts',
              collapsed: true,
              items: [
                { text: 'Spaces', link: '/manual/basic-concepts/spaces' },
                { text: 'Input Sources', link: '/manual/basic-concepts/input-sources' },
                { text: 'Recap of HTML', link: '/manual/basic-concepts/recap-of-html' },
              ]
            }
          ]
        },
        {
          text: 'Features',
          collapsed: false,
          items: [
            { text: 'Modules', link: '/manual/features/modules' },
            { text: 'TypeScript', link: '/manual/features/typescript' },
            {
              text: 'Graphics Libraries',
              collapsed: true,
              items: [
                { text: 'Babylon.js', link: '/manual/features/babylon' },
                { text: 'Three.js', link: '/manual/features/three' },
                { text: 'VanillaJS', link: '/manual/features/vanillajs' },
              ]
            },
            { text: 'Audio', link: '/manual/features/audio' },
            { text: 'Image', link: '/manual/features/image' },
            { text: 'Debugging', link: '/manual/features/debugging' },
            { text: 'WebAssembly', link: '/manual/features/webassembly' },
            { text: 'Web Workers', link: '/manual/features/web-workers' },
          ]
        },
        {
          text: 'Integration',
          collapsed: false,
          items: [
            { text: 'Unity', link: '/manual/runtime/integrate-runtime-with-unity' },
          ]
        },
        {
          text: 'Developer Tools',
          collapsed: false,
          items: [
            { text: 'Debugging', link: '/manual/developer-tools/debugging' },
            { text: 'Preview', link: '/manual/developer-tools/preview' },
            {
              text: 'Developer Tools',
              collapsed: true,
              items: [
                { text: 'Intro Features', link: '/manual/developer-tools/intro-features' },
                { text: 'Intro GUI', link: '/manual/developer-tools/intro-gui' },
                { text: 'Packaging', link: '/manual/developer-tools/packaging' }
              ]
            }
          ]
        },
        {
          text: 'Tutorials',
          collapsed: false,
          items: [
            { text: 'Hello World with Babylon.js', link: '/manual/tutorials/helloworld-babylonjs' },
            { text: 'Hello World with Three.js', link: '/manual/tutorials/helloworld-threejs' },
            { text: 'Create Materials', link: '/manual/tutorials/create-materials' },
            { text: 'Create Meshes', link: '/manual/tutorials/create-meshes' },
            { text: 'Spatial Images', link: '/manual/tutorials/spatial-images' },
            { text: 'HTTPS Request', link: '/manual/tutorials/https-request' },
            { text: 'Connect WebSocket Server', link: '/manual/tutorials/connect-websocket-server' },
          ]
        },
        {
          text: 'References',
          collapsed: true,
          items: [
            { text: 'Overview', link: '/manual/references' },
            { text: 'Caching', link: '/manual/references/caching' },
            { text: 'Graphics Backends', link: '/manual/references/graphics-backends' },
            { text: 'JSAR APIs', link: '/manual/references/jsar-apis' },
            { text: 'Runtime Configuration', link: '/manual/references/runtime-configuration' },
            { text: 'Spatial CSS', link: '/manual/references/spatial-css' },
            { text: 'Web APIs', link: '/manual/references/web-apis' },
            { text: 'WebGL', link: '/manual/references/webgl' },
            { text: 'WebXR', link: '/manual/references/webxr' },
            { text: 'WebXR Input Sources', link: '/manual/references/webxr-inputsources' },
            { text: 'XSML', link: '/manual/references/xsml' },
            {
              text: 'Embedder Guides',
              collapsed: true,
              link: '/manual/references/embedder-guides',
              items: [
                { text: 'Native APIs', link: '/manual/references/embedder-guides/native-apis' },
                { text: 'Unity APIs', link: '/manual/references/embedder-guides/unity-apis' }
              ]
            }
          ]
        }
      ],
      '/api/': [
        {
          text: 'API Reference',
          items: [
            { text: 'Browser Compatibility', link: '/api/browser-compat-data' },
            { text: 'Embedder API', link: '/api/embedder' },
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
      message: 'Apache 2.0 License.<br>Built with ❤️ using <a href="https://www.trae.ai/" target="_blank">Trae 2.0</a>.',
      copyright: '©2025 ROKID MLabs. All rights reserved.'
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
    },
    config: (md) => {
      md.use(taskCheckbox, { disabled: true })
    }
  }
})
