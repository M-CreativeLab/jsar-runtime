<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import ThreeDFirstDemo from './features/ThreeDFirstDemo.vue'
import SpatialPhotosDemo from './features/SpatialPhotosDemo.vue'
import AudioVisualizerDemo from './features/AudioVisualizerDemo.vue'

const features = [
  {
    id: '3d-design',
    icon: '🎨',
    title: '3D-First Design',
    subtitle: 'Content designed for 3D space',
    description: 'Unlike traditional flat web pages, spatial web content is inherently three-dimensional, allowing for depth, perspective, and immersive layouts that feel natural in 3D environments.',
    codeExample: `// Spatialized elements
<div style="transform: translateZ(100)">
  <h1>Welcome to Spatial Web</h1>
</div>`,
    color: 'from-blue-500 to-purple-600',
    bgColor: 'bg-blue-50 dark:bg-blue-950/20'
  },
  {
    id: 'spatial-image',
    icon: '🖼️',
    title: 'Spatial Photos',
    subtitle: 'Presenting spatial photos easily',
    description: 'Rendering spatial photos has never been easier - just use the <img> element for creator',
    codeExample: `<img src="example.jpg" spatial="stereo" />`,
    color: 'from-green-500 to-teal-600',
    bgColor: 'bg-green-50 dark:bg-green-950/20'
  },
  {
    id: 'spatial-audio',
    icon: '🔊',
    title: 'Audio Spatialization',
    subtitle: 'Natively support audio spatialization',
    description: '<audio> elements with spatialization are automatically enabled, the only thing for the creator is to add the audio element and set the source.',
    codeExample: `// Audio spatialization
<audio src="example.mp3"></audio>
`,
    color: 'from-orange-500 to-red-600',
    bgColor: 'bg-orange-50 dark:bg-orange-950/20'
  }
]

const visibleFeatures = ref(new Set())
const activeFeature = ref(0)

const observeFeatures = () => {
  const observer = new IntersectionObserver(
    (entries) => {
      entries.forEach((entry) => {
        const index = parseInt(entry.target.dataset.index)
        if (entry.isIntersecting) {
          visibleFeatures.value.add(index)
          activeFeature.value = index
        }
      })
    },
    {
      threshold: 0.3,
      rootMargin: '-10% 0px -10% 0px'
    }
  )

  document.querySelectorAll('.feature-section').forEach((el, index) => {
    el.dataset.index = index
    observer.observe(el)
  })

  return observer
}

let observer = null

onMounted(() => {
  setTimeout(() => {
    observer = observeFeatures()
  }, 100)
})

onUnmounted(() => {
  if (observer) observer.disconnect()
})
</script>

<template>
  <div class="spatial-web-features">
    <div class="features-container">
      <div v-for="(feature, index) in features" :key="feature.id" class="feature-section" :class="{
        'reverse': index % 2 === 1,
        'visible': visibleFeatures.has(index)
      }">
        <div class="feature-content">
          <div class="feature-left">
            <div class="feature-header">
              <div class="feature-icon-large" :class="`bg-gradient-to-br ${feature.color}`">
                <span class="icon-emoji-large">{{ feature.icon }}</span>
              </div>
              <div class="feature-text">
                <h3 class="feature-title">{{ feature.title }}</h3>
                <p class="feature-subtitle">{{ feature.subtitle }}</p>
              </div>
            </div>
            <p class="feature-description">{{ feature.description }}</p>

            <!-- Interactive Demo -->
            <div v-if="feature.codeExample" class="feature-demo">
              <div class="demo-window">
                <div class="demo-header">
                  <div class="demo-controls">
                    <span class="demo-dot red"></span>
                    <span class="demo-dot yellow"></span>
                    <span class="demo-dot green"></span>
                  </div>
                  <span class="demo-title">{{ feature.title }}</span>
                </div>
                <div class="demo-content">
                  <pre><code>{{ feature.codeExample }}</code></pre>
                </div>
              </div>
            </div>
          </div>
          <div class="feature-right">
            <div class="feature-visual">
              <div class="visual-container">
                <!-- 3D Design Demo -->
                <ThreeDFirstDemo v-if="feature.id === '3d-design'" />

                <!-- Spatial Photos Demo -->
                <SpatialPhotosDemo v-else-if="feature.id === 'spatial-image'" />

                <!-- Audio Visualizer Demo -->
                <AudioVisualizerDemo v-else-if="feature.id === 'spatial-audio'" />
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.spatial-web-features {
  position: relative;
  overflow: hidden;
}

/* Introduction Section */
.intro-section {
  min-height: 60vh;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 4rem 0;
  border-bottom: 1px solid var(--vp-c-divider);
}

.intro-content {
  max-width: 800px;
  padding: 0 2rem;
}

.intro-title {
  font-size: 3rem;
  font-weight: 700;
  margin-bottom: 2rem;
  color: var(--vp-c-text-1);
  text-align: center;
  border-top: none;
  line-height: 1.2;
}

.intro-description {
  font-size: 1.2rem;
  color: var(--vp-c-text-2);
  line-height: 1.6;
  text-align: center;
}

/* Feature Sections */
.features-container {
  position: relative;
}

.feature-section {
  min-height: 60vh;
  display: flex;
  align-items: center;
  padding: 2rem 0;
  margin: 1.5rem auto;
  opacity: 0.3;
  transform: translateY(50px);
  transition: all 0.8s cubic-bezier(0.4, 0, 0.2, 1);
}

.feature-section:nth-child(even) > .feature-content {
  direction: rtl;
}

.feature-section:not(:last-of-type) {
  border-bottom: 1px solid var(--vp-c-divider);
}

.feature-section.visible {
  opacity: 1;
  transform: translateY(-20px);
}

.feature-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 2rem;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 4rem;
  align-items: center;
}

.feature-content-area {
  display: flex;
  flex-direction: column;
  gap: 2rem;
}

.feature-text-content {
  display: flex;
  flex-direction: column;
  gap: 2rem;
}

.demo-top {
  order: -1;
}

.demo-bottom {
  order: 1;
}

.demo-inline {
  margin-top: 1rem;
}

.demo-inline .demo-window {
  max-width: 400px;
}

.feature-content pre {
  text-align: left !important;
}

.feature-content code {
  background-color: transparent;
  border: none;
  padding: 0;
}

.feature-left {
  direction: ltr;
  animation: slideInLeft 0.8s ease-out;
}

.feature-right {
  direction: ltr;
  animation: slideInRight 0.8s ease-out;
}

.feature-header {
  display: flex;
  align-items: center;
  gap: 1.5rem;
  margin-bottom: 2rem;
}

.feature-icon-large {
  width: 80px;
  height: 80px;
  border-radius: 20px;
  display: flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.15);
  flex-shrink: 0;
}

.icon-emoji-large {
  font-size: 2.5rem;
  filter: drop-shadow(0 2px 4px rgba(0, 0, 0, 0.1));
}

.feature-title {
  font-size: 2.5rem;
  font-weight: 700;
  margin-bottom: 0.5rem;
  color: var(--vp-c-text-1);
  line-height: 1.2;
}

.feature-subtitle {
  font-size: 1.1rem;
  color: var(--vp-c-text-2);
  font-weight: 500;
}

.feature-description {
  font-size: 1.1rem;
  line-height: 1.7;
  color: var(--vp-c-text-2);
  margin-bottom: 2rem;
}

/* Demo Window */
.feature-demo {
  margin-top: 2rem;
}

.demo-window {
  background: var(--vp-c-bg-alt);
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
  border: 1px solid var(--vp-c-divider);
}

.demo-header {
  background: var(--vp-c-bg-soft);
  padding: 0.75rem 1rem;
  display: flex;
  align-items: center;
  gap: 1rem;
  border-bottom: 1px solid var(--vp-c-divider);
}

.demo-controls {
  display: flex;
  gap: 0.5rem;
}

.demo-dot {
  width: 12px;
  height: 12px;
  border-radius: 50%;
}

.demo-dot.red {
  background: #ff5f57;
}

.demo-dot.yellow {
  background: #ffbd2e;
}

.demo-dot.green {
  background: #28ca42;
}

.demo-title {
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
  font-weight: 500;
}

.demo-content {
  padding: 1.5rem;
}

.demo-content pre {
  margin: 0;
  font-family: 'Monaco', 'Menlo', monospace;
  font-size: 0.9rem;
  line-height: 1.5;
  color: var(--vp-c-text-1);
}

/* Visual Container */
.feature-visual {
  position: relative;
  height: 400px;
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}

/* Adjust visual container size to match larger canvas */
.visual-container {
  width: 100%;
  height: 400px;
  position: relative;
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}

/* Keep original gradient background styles */
.gradient-0 {
  background: linear-gradient(135deg, #3b82f6, #8b5cf6);
}

.gradient-1 {
  background: linear-gradient(135deg, #10b981, #14b8a6);
}

.gradient-2 {
  background: linear-gradient(135deg, #f59e0b, #ef4444);
}

.gradient-3 {
  background: linear-gradient(135deg, #ec4899, #f43f5e);
}

/* Floating element animation (for non-3D features) */
.floating-elements {
  position: absolute;
  width: 100%;
  height: 100%;
}

.floating-element {
  position: absolute;
  font-size: 3rem;
  animation: float 6s ease-in-out infinite;
  filter: drop-shadow(0 4px 8px rgba(0, 0, 0, 0.2));
}

.floating-element:nth-child(1) {
  top: 20%;
  left: 20%;
}

.floating-element:nth-child(2) {
  top: 60%;
  right: 20%;
}

.floating-element:nth-child(3) {
  bottom: 20%;
  left: 50%;
  transform: translateX(-50%);
}

@keyframes float {

  0%,
  100% {
    transform: translateY(0px) rotate(0deg);
  }

  33% {
    transform: translateY(-20px) rotate(1deg);
  }

  66% {
    transform: translateY(10px) rotate(-1deg);
  }
}

/* Responsive Design */
@media (max-width: 1024px) {
  .feature-content {
    grid-template-columns: 1fr;
    gap: 2rem;
    text-align: center;
  }

  .feature-header {
    justify-content: center;
  }

  .feature-title {
    font-size: 2rem;
  }
}

@media (max-width: 768px) {
  .feature-title {
    font-size: 2rem;
  }

  .feature-description {
    font-size: 1.1rem;
  }

  .feature-content {
    padding: 0 1rem;
    text-align: left;
  }

  .feature-icon-large {
    display: none;
  }

  .summary-actions {
    flex-direction: column;
    align-items: center;
  }

  .cta-button {
    width: 100%;
    max-width: 300px;
    justify-content: center;
  }

  .feature-visual {
    height: 300px;
  }
}

/* Dark Theme */
.dark .demo-window {
  background: var(--vp-c-bg-alt);
  border-color: var(--vp-c-divider);
}

.dark .demo-header {
  background: var(--vp-c-bg-soft);
}

.dark .cta-button.secondary {
  background: var(--vp-c-bg-alt);
  border-color: var(--vp-c-divider);
}
</style>