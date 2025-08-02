# JSAR Website - Nextra Documentation

This directory contains the JSAR documentation website built with [Nextra](https://nextra.site/), a Next.js-based documentation framework that provides native MDX support and excellent developer experience.

## Structure

```
website/
├── pages/           # All documentation pages (MDX files)
│   ├── _meta.json   # Navigation configuration
│   ├── index.mdx    # Homepage
│   ├── introduction.mdx
│   ├── quick-start/
│   ├── basic-concepts/
│   ├── features/
│   ├── developer-tools/
│   ├── runtime/
│   ├── references/
│   └── tutorials/
├── public/          # Static assets (images, etc.)
├── theme.config.tsx # Nextra theme configuration
├── next.config.js   # Next.js configuration
└── package.json     # Dependencies
```

## Development

To start the development server:

```bash
# From project root
npm run dev:website

# Or from website directory
cd website
npm run dev
```

This will start the Nextra development server with hot reloading at `http://localhost:3000`.

## Building

To build the website:

```bash
# From project root
npm run build:website

# Or from website directory
cd website
npm run build
```

The built website will be output to the `.website/` directory in the project root, ready for GitHub Pages deployment.

## Features

- **Native MDX Support**: Write documentation in MDX with full React component support
- **Automatic Navigation**: Navigation is automatically generated from the file structure and `_meta.json` files
- **Search**: Built-in full-text search functionality
- **Responsive Design**: Mobile-friendly responsive layout
- **Dark Mode**: Automatic dark/light mode support
- **SEO Optimized**: Proper meta tags and SEO optimization
- **Table Support**: Full markdown table rendering with proper styling
- **Code Highlighting**: Syntax highlighting for code blocks
- **Version Support**: Ready for versioned documentation (can be extended to support multiple versions)

## Adding New Pages

1. Create a new `.mdx` file in the appropriate directory under `pages/`
2. Add the page to the corresponding `_meta.json` file for navigation
3. The page will automatically be included in the build

## Configuration

- **Theme Config**: Edit `theme.config.tsx` to customize the site theme, logo, links, etc.
- **Next.js Config**: Edit `next.config.js` for build configuration, output settings, etc.
- **Navigation**: Edit `_meta.json` files in each directory to configure navigation order and labels

## Migration from Custom Build System

This replaces the previous custom MDX-to-HTML build system (`tools/build-docs.js`) with a modern, maintainable solution that provides:

- Better developer experience with hot reloading
- Native MDX support without custom parsing
- Professional documentation theme
- Built-in search and navigation
- Better performance and SEO
- Easier maintenance and extensibility