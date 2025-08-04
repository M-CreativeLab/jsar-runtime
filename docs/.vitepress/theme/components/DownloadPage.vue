<script setup>
import { ref, onMounted } from 'vue'
import { useData } from 'vitepress'

const currentSlide = ref(0)
const screenshots = ref([
  {
    src: 'https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-00.png',
    alt: 'Mixed 2D and 3D Elements'
  },
  {
    src: 'https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-01.png',
    alt: '2D Elements'
  },
  {
    src: 'https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-02.png',
    alt: '3D Particles'
  },
])

const nextSlide = () => {
  currentSlide.value = (currentSlide.value + 1) % screenshots.value.length
}

const prevSlide = () => {
  currentSlide.value = currentSlide.value === 0 ? screenshots.value.length - 1 : currentSlide.value - 1
}

const goToSlide = (index) => {
  currentSlide.value = index
}

let autoPlayInterval = null
const startAutoPlay = () => {
  autoPlayInterval = setInterval(nextSlide, 5000)
}

const stopAutoPlay = () => {
  if (autoPlayInterval) {
    clearInterval(autoPlayInterval)
    autoPlayInterval = null
  }
}

onMounted(() => {
  startAutoPlay()
})

const downloads = ref([
  {
    title: 'Transmute Browser',
    subtitle: 'Try JSAR browser engine on your desktop.',
    platforms: [
      {
        name: 'macOS',
        url: 'https://github.com/M-CreativeLab/jsar-runtime/releases/latest/download/TransmuteBrowser.zip',
        requirements: 'macOS 14 or later',
      },
    ]
  },
])

const scrollToSection = (id) => {
  document.getElementById(id)?.scrollIntoView({ behavior: 'smooth' })
}
</script>

<template>
  <div class="download-page">
    <!-- Hero Section -->
    <section class="hero-section">
      <div class="hero-content">
        <h1 class="hero-title">JSAR Downloads</h1>
        <p class="hero-subtitle">
          Experience spatial Web browsing with JSAR.
        </p>
        <div class="hero-carousel" @mouseenter="stopAutoPlay" @mouseleave="startAutoPlay">
          <div class="carousel-container">
            <div class="carousel-slides" :style="{ transform: `translateX(-${currentSlide * 100}%)` }">
              <div v-for="(screenshot, index) in screenshots" :key="index" class="carousel-slide">
                <img :src="screenshot.src" :alt="screenshot.alt" class="screenshot-image" />
              </div>
            </div>
            
            <button class="carousel-btn carousel-btn-prev" @click="prevSlide" aria-label="Previous">
              <svg width="24" height="24" viewBox="0 0 24 24" fill="none">
                <path d="M15 18L9 12L15 6" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
              </svg>
            </button>
            <button class="carousel-btn carousel-btn-next" @click="nextSlide" aria-label="Next">
              <svg width="24" height="24" viewBox="0 0 24 24" fill="none">
                <path d="M9 18L15 12L9 6" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
              </svg>
            </button>
          </div>
          
          <div class="carousel-indicators">
            <button 
              v-for="(screenshot, index) in screenshots" 
              :key="index"
              class="carousel-indicator"
              :class="{ active: currentSlide === index }"
              @click="goToSlide(index)"
              :aria-label="`Go to slide ${index + 1}`"
            ></button>
          </div>
        </div>
      </div>
    </section>

    <!-- Download Sections -->
    <section class="downloads-container">
      <div class="downloads-wrapper">
        <div v-for="(section, index) in downloads" :key="index" class="download-section">
          <div class="section-header">
            <h2 class="section-title">{{ section.title }}</h2>
            <p class="section-subtitle">{{ section.subtitle }}</p>
          </div>

          <div class="platforms-grid">
            <div v-for="platform in section.platforms" :key="platform.name" class="platform-card">
              <div class="platform-icon">{{ platform.icon }}</div>
              <div class="platform-info">
                <h3 class="platform-name">{{ platform.name }}</h3>
                <p class="platform-requirements">{{ platform.requirements }}</p>
              </div>
              <div class="download-button-wrapper">
                <a 
                  v-if="platform.url" 
                  :href="platform.url" 
                  class="download-button" 
                  target="_blank" 
                  rel="noopener noreferrer"
                >
                  Download
                </a>
                <button 
                  v-else 
                  class="download-button download-button-disabled" 
                  disabled
                  :title="'Download not available for ' + platform.name"
                >
                  Download
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- Help Section -->
    <section class="help-section">
      <div class="help-container">
        <h2 class="help-title">Need Help?</h2>
        <div class="help-links">
          <a href="manual/quick-start" class="help-link">
            <span class="help-icon">🚀</span>
            <span>Quick Start Guide</span>
          </a>
          <a href="https://github.com/M-CreativeLab/jsar-runtime/issues" class="help-link" target="_blank">
            <span class="help-icon">🐛</span>
            <span>GitHub Issues</span>
          </a>
          <a href="https://github.com/M-CreativeLab/jsar-runtime/discussions" class="help-link" target="_blank">
            <span class="help-icon">💬</span>
            <span>Community Discussions</span>
          </a>
        </div>
      </div>
    </section>
  </div>
</template>

<style scoped>
.download-page {
  min-height: 100vh;
  background: linear-gradient(180deg,
      rgba(59, 130, 246, 0.03) 0%,
      rgba(255, 255, 255, 0) 50%,
      rgba(59, 130, 246, 0.02) 100%);
}

.dark .download-page {
  background: linear-gradient(180deg,
      rgba(59, 130, 246, 0.05) 0%,
      rgba(0, 0, 0, 0) 50%,
      rgba(59, 130, 246, 0.03) 100%);
}

/* Hero Section */
.hero-section {
  padding: 120px 24px 80px;
  text-align: center;
  max-width: 1200px;
  margin: 0 auto;
}

.hero-title {
  font-size: 3.5rem;
  font-weight: 700;
  background: linear-gradient(135deg, #3B82F6, #1D4ED8);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin-bottom: 24px;
  letter-spacing: -0.02em;
  line-height: 1;
}

.hero-subtitle {
  font-size: 1.5rem;
  color: var(--vp-c-text-2);
  max-width: 600px;
  margin: 0 auto 40px;
  line-height: 1.6;
}

/* 轮播图样式 */
.hero-carousel {
  margin: 40px auto 0;
  max-width: 900px;
  width: 100%;
}

.carousel-container {
  position: relative;
  overflow: hidden;
  border-radius: 12px;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
}

.carousel-slides {
  display: flex;
  transition: transform 0.5s ease-in-out;
}

.carousel-slide {
  width: 100%;
  flex-shrink: 0;
}

.screenshot-image {
  width: 100%;
  height: auto;
  display: block;
}

/* 导航按钮 */
.carousel-btn {
  position: absolute;
  top: 50%;
  transform: translateY(-50%);
  background: rgba(255, 255, 255, 0.9);
  border: none;
  border-radius: 50%;
  width: 48px;
  height: 48px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: all 0.3s ease;
  z-index: 2;
  color: #333;
}

.carousel-btn:hover {
  background: rgba(255, 255, 255, 1);
  transform: translateY(-50%) scale(1.1);
}

.carousel-btn-prev {
  left: 16px;
}

.carousel-btn-next {
  right: 16px;
}

.dark .carousel-btn {
  background: rgba(0, 0, 0, 0.7);
  color: #fff;
}

.dark .carousel-btn:hover {
  background: rgba(0, 0, 0, 0.9);
}

/* 指示器 */
.carousel-indicators {
  display: flex;
  justify-content: center;
  gap: 8px;
  margin-top: 20px;
}

.carousel-indicator {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  border: none;
  background: rgba(59, 130, 246, 0.3);
  cursor: pointer;
  transition: all 0.3s ease;
}

.carousel-indicator.active {
  background: #3B82F6;
  transform: scale(1.2);
}

.carousel-indicator:hover {
  background: rgba(59, 130, 246, 0.6);
}

.dark .carousel-container {
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
}

/* 响应式设计 */
@media (max-width: 768px) {
  .hero-carousel {
    max-width: 100%;
    margin: 30px auto 0;
    padding: 0 20px;
  }
  
  .carousel-container {
    border-radius: 8px;
  }
  
  .carousel-btn {
    width: 40px;
    height: 40px;
  }
  
  .carousel-btn-prev {
    left: 8px;
  }
  
  .carousel-btn-next {
    right: 8px;
  }
}

/* 添加截图样式 */
.hero-screenshot {
  margin: 40px auto 0;
  max-width: 900px;
  width: 100%;
  display: flex;
  justify-content: center;
}

.screenshot-image {
  width: 100%;
  height: auto;
  border-radius: 12px;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
  transition: transform 0.3s ease, box-shadow 0.3s ease;
}

.screenshot-image:hover {
  transform: translateY(-4px);
  box-shadow: 0 25px 50px rgba(0, 0, 0, 0.15);
}

.dark .screenshot-image {
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
}

.dark .screenshot-image:hover {
  box-shadow: 0 25px 50px rgba(0, 0, 0, 0.4);
}

/* 响应式设计 */
@media (max-width: 768px) {
  .hero-screenshot {
    max-width: 100%;
    margin: 30px auto 0;
    padding: 0 20px;
  }
  
  .screenshot-image {
    border-radius: 8px;
  }
}

/* Downloads Container */
.downloads-container {
  padding: 0 24px 80px;
}

.downloads-wrapper {
  max-width: 1200px;
  margin: 0 auto;
}

.download-section {
  margin-bottom: 80px;
}

.section-header {
  text-align: center;
  margin-bottom: 48px;
}

.section-title {
  font-size: 2.5rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
  margin-bottom: 16px;
  letter-spacing: -0.01em;
}

.section-subtitle {
  font-size: 1.25rem;
  color: var(--vp-c-text-2);
  max-width: 800px;
  margin: 0 auto;
  line-height: 1.6;
}

/* Platform Cards */
.platforms-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
  gap: 24px;
  margin-top: 48px;
}

.platform-card {
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider-light);
  border-radius: 16px;
  padding: 32px;
  display: flex;
  align-items: center;
  gap: 24px;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  /* 如果有图片，调整为垂直布局 */
  flex-direction: row;
}

/* 添加图片样式 */
.platform-image {
  flex-shrink: 0;
  width: 120px;
  height: 80px;
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

.platform-image img {
  width: 100%;
  height: 100%;
  object-fit: cover;
  transition: transform 0.3s ease;
}

.platform-card:hover .platform-image img {
  transform: scale(1.05);
}

/* 响应式调整 */
@media (max-width: 768px) {
  .platform-card {
    flex-direction: column;
    text-align: center;
    gap: 16px;
  }
  
  .platform-image {
    width: 100%;
    height: 120px;
  }
}
.platform-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 20px 40px rgba(59, 130, 246, 0.1);
  border-color: var(--vp-c-brand-1);
}

.platform-icon {
  font-size: 2.5rem;
  flex-shrink: 0;
}

.platform-info {
  flex: 1;
}

.platform-name {
  font-size: 1.5rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
  margin-bottom: 8px;
}

.platform-requirements {
  color: var(--vp-c-text-2);
  font-size: 0.95rem;
  line-height: 1.5;
}

.download-button {
  display: inline-block;
  padding: 12px 24px;
  background: linear-gradient(135deg, #3B82F6, #1D4ED8);
  color: white;
  text-decoration: none;
  border-radius: 8px;
  font-weight: 600;
  transition: all 0.3s ease;
  border: none;
  cursor: pointer;
  font-size: 14px;
}

.download-button:hover {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(59, 130, 246, 0.3);
}

/* 禁用状态样式 */
.download-button-disabled {
  background: #9CA3AF !important;
  cursor: not-allowed !important;
  opacity: 0.6;
}

.download-button-disabled:hover {
  transform: none !important;
  box-shadow: none !important;
}

.download-button-wrapper {
  position: relative;
}

/* Tooltip 样式 */
.download-button-disabled[title]:hover::after {
  content: attr(title);
  position: absolute;
  bottom: 100%;
  left: 50%;
  transform: translateX(-50%);
  background: rgba(0, 0, 0, 0.8);
  color: white;
  padding: 8px 12px;
  border-radius: 4px;
  font-size: 12px;
  white-space: nowrap;
  z-index: 1000;
  margin-bottom: 5px;
}

.download-button-disabled[title]:hover::before {
  content: '';
  position: absolute;
  bottom: 100%;
  left: 50%;
  transform: translateX(-50%);
  border: 5px solid transparent;
  border-top-color: rgba(0, 0, 0, 0.8);
  z-index: 1000;
}

.dark .download-button-disabled[title]:hover::after {
  background: rgba(255, 255, 255, 0.9);
  color: #000;
}

.dark .download-button-disabled[title]:hover::before {
  border-top-color: rgba(255, 255, 255, 0.9);
}

/* Requirements Section */
.requirements-section {
  padding: 80px 24px;
  background: var(--vp-c-bg-alt);
}

.requirements-container {
  max-width: 1200px;
  margin: 0 auto;
}

.requirements-title {
  font-size: 2.5rem;
  font-weight: 600;
  text-align: center;
  margin-bottom: 48px;
  color: var(--vp-c-text-1);
}

.requirements-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 32px;
}

.requirement-card {
  background: var(--vp-c-bg);
  border-radius: 16px;
  padding: 32px;
  border: 1px solid var(--vp-c-divider-light);
}

.requirement-card h3 {
  font-size: 1.5rem;
  font-weight: 600;
  margin-bottom: 20px;
  color: var(--vp-c-text-1);
}

.requirement-card ul {
  list-style: none;
  padding: 0;
  margin: 0;
}

.requirement-card li {
  padding: 8px 0;
  color: var(--vp-c-text-2);
  border-bottom: 1px solid var(--vp-c-divider-light);
}

.requirement-card li:last-child {
  border-bottom: none;
}

/* Help Section */
.help-section {
  padding: 80px 24px;
}

.help-container {
  max-width: 1200px;
  margin: 0 auto;
  text-align: center;
}

.help-title {
  font-size: 2.5rem;
  font-weight: 600;
  margin-bottom: 48px;
  color: var(--vp-c-text-1);
}

.help-links {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 24px;
  max-width: 800px;
  margin: 0 auto;
}

.help-link {
  display: flex;
  align-items: center;
  gap: 16px;
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider-light);
  border-radius: 12px;
  text-decoration: none;
  color: var(--vp-c-text-1);
  font-weight: 500;
  transition: all 0.3s ease;
}

.help-link:hover {
  transform: translateY(-2px);
  box-shadow: 0 8px 24px rgba(59, 130, 246, 0.1);
  border-color: var(--vp-c-brand-1);
}

.help-icon {
  font-size: 1.5rem;
}

/* Responsive Design */
@media (max-width: 768px) {
  .hero-title {
    font-size: 2.5rem;
  }

  .hero-subtitle {
    font-size: 1.25rem;
  }

  .section-title {
    font-size: 2rem;
  }

  .platform-card {
    flex-direction: column;
    text-align: center;
    gap: 16px;
  }

  .platforms-grid {
    grid-template-columns: 1fr;
  }

  .requirements-grid {
    grid-template-columns: 1fr;
  }

  .help-links {
    grid-template-columns: 1fr;
  }
}
</style>