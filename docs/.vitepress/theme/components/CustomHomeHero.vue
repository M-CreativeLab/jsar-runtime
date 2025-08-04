<script setup>
import { useData } from 'vitepress/dist/client/theme-default/composables/data'
import VPHero from 'vitepress/dist/client/theme-default/components/VPHero.vue'
import Hero3DEffect from './Hero3DEffect.vue'
import { ref, onMounted, onUnmounted } from 'vue'

const { frontmatter: fm } = useData()
const heroRef = ref(null)
const mouseX = ref(0)
const mouseY = ref(0)
const scrollY = ref(0)

const handleMouseMove = (e) => {
  mouseX.value = (e.clientX / window.innerWidth - 0.5) * 2
  mouseY.value = (e.clientY / window.innerHeight - 0.5) * 2
}

const handleScroll = () => {
  scrollY.value = window.scrollY
}

onMounted(() => {
  window.addEventListener('mousemove', handleMouseMove)
  window.addEventListener('scroll', handleScroll)
})

onUnmounted(() => {
  window.removeEventListener('mousemove', handleMouseMove)
  window.removeEventListener('scroll', handleScroll)
})
</script>

<template>
  <div class="custom-hero-container">
    <div class="hero-3d-effect-wrapper">
      <Hero3DEffect :mouse-x="mouseX" :mouse-y="mouseY" />
    </div>
    <div ref="heroRef" class="hero-content-wrapper">
      <VPHero v-if="fm.hero" class="VPHomeHero glassmorphism" :name="fm.hero.name" :text="fm.hero.text"
        :tagline="fm.hero.tagline" :image="fm.hero.image" :actions="fm.hero.actions">
        <template #home-hero-info-before>
          <slot name="home-hero-info-before" />
        </template>
        <template #home-hero-info>
          <slot name="home-hero-info" />
        </template>
        <template #home-hero-info-after>
          <slot name="home-hero-info-after" />
        </template>
        <template #home-hero-actions>
          <div class="VPHeroActions">
            <div class="action">
              <a class="VPButton medium brand" href="/manual/quick-start">Get Started</a>
            </div>
            <div class="action">
              <a class="VPButton medium alt" href="/manual/introduction">Learn More</a>
            </div>
            <div class="action">
              <a class="VPButton medium alt github-button" href="https://github.com/M-CreativeLab/jsar-runtime"
                target="_blank" rel="noreferrer">
                <svg class="github-icon" viewBox="0 0 24 24" width="16" height="16">
                  <path fill="currentColor"
                    d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z" />
                </svg>
                View on GitHub
              </a>
            </div>
          </div>
        </template>
        <template #home-hero-actions-after>
          <slot name="home-hero-actions-after" />
        </template>
        <template #home-hero-image>
          <slot name="home-hero-image" />
        </template>
      </VPHero>
    </div>
  </div>
</template>

<style scoped>
.custom-hero-container {
  position: relative;
  overflow: hidden;
  height: 100%;
  perspective: 1000px;
}

.hero-3d-effect-wrapper {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 0;
}

.hero-content-wrapper {
  position: relative;
  z-index: 1;
  transition: transform 0.1s ease-out;
  will-change: transform;
}

.VPHomeHero.glassmorphism {
  position: relative;
  margin: 20px;
  padding: 40px;
  transition: all 0.3s ease;
  will-change: transform;
}

.VPHomeHero.glassmorphism :deep(.name) {
  font-weight: bold;
}

.VPHomeHero.glassmorphism :deep(.text) {
  font-weight: bold;
}

:deep(.github-button) {
  display: inline-flex !important;
  align-items: center;
  gap: 8px;
}

:deep(.github-icon) {
  flex-shrink: 0;
}

.VPHomeHero.glassmorphism :deep(.tagline) {
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.05);
  color: var(--vp-c-text-2);
}

/* Tagline effect in dark mode */
html.dark .VPHomeHero.glassmorphism :deep(.tagline) {
  color: rgba(255, 255, 255, 0.7);
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.2);
}


/* 按钮增强效果 */
.VPHomeHero.glassmorphism :deep(.VPButton) {
  text-shadow: 0 1px 3px rgba(0, 0, 0, 0.3);
  font-weight: 500;
  transition: all 0.3s ease;
}

.VPHomeHero.glassmorphism :deep(.VPButton):hover {
  transform: translateY(-2px);
  text-shadow:
    0 2px 4px rgba(0, 0, 0, 0.4),
    0 0 15px rgba(255, 255, 255, 0.2);
}

/* 响应式设计 */
@media (max-width: 768px) {
  .VPHomeHero.glassmorphism {
    margin: 10px;
    padding: 20px;
    border-radius: 15px;
  }

  .hero-content-wrapper {
    transform: none !important;
  }
}
</style>
