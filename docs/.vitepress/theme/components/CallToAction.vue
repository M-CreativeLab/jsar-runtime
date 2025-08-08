<template>
  <div class="call-to-action">
    <div class="cta-content">
      <h3 class="cta-title">Ready to try Spatial Web?</h3>
      <p class="cta-text">
        JSAR browser engine brings all these Spatial Web capabilities to your fingertips.
      </p>
      <div class="cta-actions">
        <a href="manual/quick-start.html" class="cta-button primary">
          Get Started
          <svg class="arrow-icon" viewBox="0 0 20 20" fill="currentColor">
            <path fill-rule="evenodd" d="M7.293 14.707a1 1 0 010-1.414L10.586 10 7.293 6.707a1 1 0 011.414-1.414l4 4a1 1 0 010 1.414l-4 4a1 1 0 01-1.414 0z" clip-rule="evenodd" />
          </svg>
        </a>
        <a href="api/browser-compat-data.html" class="cta-button secondary">View API Docs</a>
      </div>
    </div>
  </div>
</template>

<style scoped>
.call-to-action {
  min-height: 60vh;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 4rem 0;
}

.cta-content {
  max-width: 800px;
  padding: 0 2rem;
}

.cta-title {
  font-size: 2.5rem;
  font-weight: 700;
  margin-bottom: 1rem;
  color: var(--vp-c-text-1);
}

.cta-text {
  font-size: 1.2rem;
  color: var(--vp-c-text-2);
  margin-bottom: 2rem;
  line-height: 1.6;
}

.cta-actions {
  display: flex;
  gap: 1rem;
  justify-content: center;
  flex-wrap: wrap;
}

.cta-button {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 1rem 2rem;
  border-radius: 12px;
  font-weight: 600;
  text-decoration: none;
  transition: all 0.3s ease;
  font-size: 1.1rem;
}

.cta-button:hover {
  text-decoration: none;
}

.cta-button.primary {
  background: linear-gradient(135deg, #3b82f6, #8b5cf6);
  color: white;
  box-shadow: 0 4px 15px rgba(59, 130, 246, 0.3);
}

.cta-button.primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
}

.cta-button.secondary {
  background: var(--vp-c-bg-alt);
  color: var(--vp-c-text-1);
  border: 2px solid var(--vp-c-divider);
}

.cta-button.secondary:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}

.arrow-icon {
  width: 20px;
  height: 20px;
  transition: transform 0.3s ease;
}

.cta-button:hover .arrow-icon {
  transform: translateX(3px);
}

/* Responsive Design */
@media (max-width: 768px) {
  .cta-title {
    font-size: 2rem;
  }
  
  .cta-actions {
    flex-direction: column;
    align-items: center;
  }
  
  .cta-button {
    width: 100%;
    max-width: 300px;
    justify-content: center;
  }
}

/* Dark Theme */
.dark .cta-button.secondary {
  background: var(--vp-c-bg-alt);
  border-color: var(--vp-c-divider);
}
</style>