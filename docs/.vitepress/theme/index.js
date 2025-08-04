import DefaultTheme from 'vitepress/theme'
import './custom.css'

import LatestRelease from './components/LatestRelease.vue'
import BrowserCompatData from './components/BrowserCompatData.vue'
import Hero3DEffect from './components/Hero3DEffect.vue'
import DownloadPage from './components/DownloadPage.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('LatestRelease', LatestRelease)
    app.component('BrowserCompatData', BrowserCompatData)
    app.component('Hero3DEffect', Hero3DEffect)
    app.component('DownloadPage', DownloadPage)
  }
}
