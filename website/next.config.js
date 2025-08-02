const withNextra = require('nextra')({
  theme: 'nextra-theme-docs',
  themeConfig: './theme.config.tsx'
})

module.exports = withNextra({
  output: 'export',
  trailingSlash: true,
  distDir: '../.website',
  images: {
    unoptimized: true
  }
})