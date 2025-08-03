<script setup>
import { ref, onMounted } from 'vue'
import { marked } from 'marked'
import { withBase } from 'vitepress';

// Default value in case of fetch failure
const versions = ref([
  {
    version: '0.8.2',
    content: '',
    renderedContent: ''
  },
  {
    version: '0.8.1',
    content: '',
    renderedContent: ''
  },
  {
    version: '0.8.0',
    content: '',
    renderedContent: ''
  }
]);

onMounted(async () => {
  try {
    // Get changelog information
    const changelogResponse = await fetch(withBase('/changelogs/alpha.md'));
    if (changelogResponse.ok) {
      const changelogText = await changelogResponse.text();

      // Use regex to extract information for the first 3 versions
      const versionBlocks = changelogText.match(/## v[\d.]+[\s\S]*?(?=## v[\d.]+|$)/g);

      if (versionBlocks && versionBlocks.length > 0) {
        // Process at most 3 versions
        const maxVersions = Math.min(3, versionBlocks.length);

        for (let i = 0; i < maxVersions; i++) {
          const block = versionBlocks[i];

          // Extract version number
          const versionMatch = block.match(/## v([\d.]+)/);
          if (versionMatch && versionMatch[1]) {
            const versionNumber = versionMatch[1];

            // Extract content (excluding version title line)
            let content = block.replace(/## v[\d.]+\s*/, '');

            // Limit content length to 200 characters
            if (content.length > 200) {
              content = content.substring(0, 200) + '...';
            }

            // Use marked to render markdown content
            const renderedContent = marked.parse(content);

            // Update version information
            versions.value[i] = {
              version: versionNumber,
              content: content,
              renderedContent: renderedContent
            };
          }
        }
      }
    }
  } catch (error) {
    console.error('Failed to load changelog information:', error);
    // Keep default values
  }
});

</script>

<template>
  <div class="changelog-container">
    <h3 class="changelog-title">Latest Releases</h3>
    <div v-for="(version, index) in versions" :key="index" class="version-block">
      <h4 class="version-title">v{{ version.version }}</h4>
      <div class="version-content" v-html="version.renderedContent"></div>
    </div>
    <p class="view-more">
      <a href="/changelogs/alpha.html">View Full Changelog</a>
    </p>
  </div>
</template>

<style scoped>
.changelog-container {
  background-color: var(--vp-c-bg-soft);
  border-radius: 8px;
  padding: 16px;
  margin: 16px 0;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
}

.changelog-title {
  font-size: 1.2rem;
  margin-top: 0;
  margin-bottom: 16px;
  color: var(--vp-c-brand);
}

.version-block {
  margin-bottom: 20px;
  padding-bottom: 16px;
  border-bottom: 1px solid var(--vp-c-divider);
}

.version-block:last-child {
  border-bottom: none;
}

.version-title {
  font-size: 1.1rem;
  margin-top: 0;
  margin-bottom: 8px;
  color: var(--vp-c-text-1);
}

.version-content {
  font-size: 0.95rem;
  line-height: 1.5;
  color: var(--vp-c-text-2);
}

.version-content :deep(h3) {
  font-size: 1rem;
  margin-top: 12px;
  margin-bottom: 8px;
}

.version-content :deep(ul) {
  padding-left: 20px;
  margin: 8px 0;
}

.version-content :deep(li) {
  margin-bottom: 4px;
}

.view-more {
  text-align: right;
  margin-top: 16px;
  margin-bottom: 0;
}

.view-more a {
  color: var(--vp-c-brand);
  text-decoration: none;
  font-weight: 500;
  font-size: 0.9rem;
}

.view-more a:hover {
  text-decoration: underline;
}

@media (max-width: 640px) {
  .changelog-container {
    padding: 12px;
  }

  .version-content {
    font-size: 0.9rem;
  }
}
</style>
