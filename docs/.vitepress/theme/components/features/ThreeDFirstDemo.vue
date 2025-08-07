<template>
  <div ref="containerRef" class="threejs-container"></div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import * as THREE from 'three'

const containerRef = ref(null)
let scene, camera, renderer, animationId
let webElements = []
let mouse = new THREE.Vector2()
let raycaster = new THREE.Raycaster()
let hoveredElement = null

const initThreeJS = () => {
  if (!containerRef.value) return

  // Scene setup
  scene = new THREE.Scene()
  camera = new THREE.PerspectiveCamera(75, 1, 0.1, 1000)
  renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true })

  const size = 400
  renderer.setSize(size, size)
  renderer.setClearColor(0x000000, 0)
  renderer.shadowMap.enabled = true
  renderer.shadowMap.type = THREE.PCFSoftShadowMap

  containerRef.value.appendChild(renderer.domElement)

  // Adjust camera position
  camera.position.set(4, 3, 10)
  camera.lookAt(0, 0, 0)

  // Ambient light
  const ambientLight = new THREE.AmbientLight(0x404040, 0.5)
  scene.add(ambientLight)

  // Directional light
  const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8)
  directionalLight.position.set(8, 8, 8)
  directionalLight.castShadow = true
  directionalLight.shadow.mapSize.width = 2048
  directionalLight.shadow.mapSize.height = 2048
  scene.add(directionalLight)

  // Add mouse event listeners
  renderer.domElement.addEventListener('mousemove', onMouseMove)
  renderer.domElement.addEventListener('click', onMouseClick)

  // Create Web components
  createWebElements()

  // Start animation
  animate()
}

const createWebElements = () => {
  // Use brighter colors, remove thickness (set Z-axis depth to very small value)
  const elements = [
    {
      name: 'Header',
      color: 0x6366f1,  // Brighter purple (brightened from 0x4f46e5)
      initialPosition: [0, 4, 0],
      finalPosition: [0, 4, 0],
      size: [10, 1.8, 0.02],
      textColor: '#ffffff'
    },
    {
      name: 'Navigation',
      color: 0x10b981,  // Brighter green (brightened from 0x059669)
      initialPosition: [0, 2.5, 0],
      finalPosition: [0, 2.5, 0.3],
      size: [10, 1.2, 0.02],
      textColor: '#ffffff'
    },
    {
      name: 'Hero Section',
      color: 0x3b82f6,  // Brighter blue (brightened from 0x0ea5e9)
      initialPosition: [0, 0.5, 0],
      finalPosition: [0, 0.5, 0.6],
      size: [9.5, 3.2, 0.02],
      textColor: '#ffffff'
    },
    {
      name: 'Content Area',
      color: 0xfcd34d,  // Brighter yellow (brightened from 0xfbbf24)
      initialPosition: [-2, -1.5, 0],
      finalPosition: [-2, -1.5, 0.9],
      size: [5.5, 3.5, 0.02],
      textColor: '#ffffff'
    },
    {
      name: 'Sidebar',
      color: 0xf59e0b,  // Brighter orange (changed from 0xf472b6 to orange and brightened)
      initialPosition: [2.8, -1.5, 0],
      finalPosition: [2.8, -1.5, 1.2],
      size: [2.8, 3.5, 0.02],
      textColor: '#ffffff'  // Changed to white text to adapt to orange background
    },
    {
      name: 'Footer',
      color: 0x8b5cf6,  // Brighter indigo (brightened from 0x6366f1)
      initialPosition: [0, -3.8, 0],
      finalPosition: [0, -3.8, 1.5],
      size: [10, 1.5, 0.02],
      textColor: '#ffffff'
    }
  ]

  // In createWebElements function, fix text label settings
  elements.forEach((element, index) => {
    // Create main geometry (plane instead of cube)
    const geometry = new THREE.PlaneGeometry(element.size[0], element.size[1])
    const material = new THREE.MeshLambertMaterial({
      color: element.color,
      transparent: true,
      opacity: 0.99,
      side: THREE.DoubleSide,
      depthTest: false,  // Disable depth testing
      depthWrite: false  // Disable depth writing
    })
    const mesh = new THREE.Mesh(geometry, material)

    // Set render order: panels with larger Z values (farther from screen) have smaller renderOrder (render first)
    mesh.renderOrder = element.finalPosition[2] * 100

    // Set initial position
    mesh.position.set(...element.initialPosition)
    mesh.castShadow = false
    mesh.receiveShadow = false
    scene.add(mesh)

    // Create text label
    const canvas = document.createElement('canvas')
    const context = canvas.getContext('2d')
    canvas.width = 1536
    canvas.height = 384

    // Clear canvas background, ensure transparency
    context.clearRect(0, 0, canvas.width, canvas.height)

    // Set text style
    context.fillStyle = element.textColor
    context.font = 'bold 140px Monospace'
    context.textAlign = 'center'
    context.textBaseline = 'middle'

    // Add text stroke to enhance readability
    context.strokeStyle = element.textColor === '#ffffff' ? '#000000' : '#ffffff'
    context.lineWidth = 3
    context.strokeText(element.name, 768, 192)
    context.fillText(element.name, 768, 192)

    const texture = new THREE.CanvasTexture(canvas)
    texture.needsUpdate = true

    const labelMaterial = new THREE.MeshBasicMaterial({
      map: texture,
      transparent: true,
      opacity: 1.0,  // Set to completely opaque
      alphaTest: 0.01,  // Lower alphaTest value
      depthTest: false,
      depthWrite: false,
      side: THREE.DoubleSide  // Ensure double-sided visibility
    })

    const labelWidth = Math.min(element.size[0] * 0.98, 6.5)
    const labelHeight = labelWidth * 0.25
    const labelGeometry = new THREE.PlaneGeometry(labelWidth, labelHeight)
    const label = new THREE.Mesh(labelGeometry, labelMaterial)

    label.position.copy(mesh.position)
    label.position.z += 0.01
    label.renderOrder = mesh.renderOrder + 0.1  // Text labels render slightly later than panels

    scene.add(label)

    webElements.push({
      mesh,
      label,
      originalPosition: [...element.initialPosition],
      finalPosition: [...element.finalPosition],
      originalColor: element.color,
      name: element.name
    })
  })
}

const onMouseMove = (event) => {
  const rect = renderer.domElement.getBoundingClientRect()
  mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1
  mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1

  // Raycasting
  raycaster.setFromCamera(mouse, camera)
  const intersects = raycaster.intersectObjects(webElements.map(el => el.mesh))

  // Reset all elements
  webElements.forEach(element => {
    element.mesh.material.color.setHex(element.originalColor)
    element.mesh.scale.set(1, 1, 1)
  })

  // Highlight hovered element
  if (intersects.length > 0) {
    const intersectedObject = intersects[0].object
    const element = webElements.find(el => el.mesh === intersectedObject)
    if (element) {
      hoveredElement = element
      // Brighten color
      const color = new THREE.Color(element.originalColor)
      color.multiplyScalar(1.3)
      element.mesh.material.color.copy(color)
      // Slight scaling
      element.mesh.scale.set(1.05, 1.05, 1)
      element.label.scale.set(1.05, 1.05, 1)
      // Change cursor style
      renderer.domElement.style.cursor = 'pointer'
    }
  } else {
    hoveredElement = null
    renderer.domElement.style.cursor = 'default'
  }
}

const onMouseClick = (event) => {
  if (hoveredElement) {
    // Click effect: quick scaling animation
    const element = hoveredElement
    element.mesh.scale.set(0.9, 0.9, 1)
    element.label.scale.set(0.9, 0.9, 1)

    setTimeout(() => {
      element.mesh.scale.set(1.05, 1.05, 1)
      element.label.scale.set(1.05, 1.05, 1)
    }, 100)

    console.log(`Clicked on: ${element.name}`)
  }
}

const animate = () => {
  animationId = requestAnimationFrame(animate)

  const time = Date.now() * 0.001

  // Cyclic expand/collapse animation
  webElements.forEach((element, index) => {
    // Use sine wave to create cyclic animation, each element has different phase offset
    const phaseOffset = index * 0.5  // Phase offset for each element
    const animationSpeed = 0.8  // Animation speed
    const waveValue = Math.sin(time * animationSpeed + phaseOffset)

    // Map sine wave value from [-1, 1] to [0, 1]
    const progress = (waveValue + 1) * 0.5

    // Use easing function to make animation smoother
    const easeProgress = progress < 0.5
      ? 2 * progress * progress
      : 1 - Math.pow(-2 * progress + 2, 2) / 2

    // Interpolate between initial position (flat) and final position (depth)
    const currentX = element.originalPosition[0] + (element.finalPosition[0] - element.originalPosition[0]) * easeProgress
    const currentY = element.originalPosition[1] + (element.finalPosition[1] - element.originalPosition[1]) * easeProgress
    const currentZ = element.originalPosition[2] + (element.finalPosition[2] - element.originalPosition[2]) * easeProgress

    // Update position
    element.mesh.position.set(currentX, currentY, currentZ)
    element.label.position.set(currentX, currentY, currentZ + 0.01)
  })
  renderer.render(scene, camera)
}

const cleanup = () => {
  if (animationId) {
    cancelAnimationFrame(animationId)
  }
  if (renderer && renderer.domElement) {
    renderer.domElement.removeEventListener('mousemove', onMouseMove)
    renderer.domElement.removeEventListener('click', onMouseClick)
  }
  if (renderer) {
    renderer.dispose()
    if (containerRef.value && renderer.domElement) {
      containerRef.value.removeChild(renderer.domElement)
    }
  }
  if (scene) {
    scene.traverse((object) => {
      if (object.geometry) object.geometry.dispose()
      if (object.material) {
        if (Array.isArray(object.material)) {
          object.material.forEach(material => material.dispose())
        } else {
          object.material.dispose()
        }
      }
    })
  }
}

onMounted(() => {
  initThreeJS()
})

onUnmounted(() => {
  cleanup()
})
</script>

<style scoped>
.threejs-container {
  width: 400px;
  height: 400px;
  overflow: hidden;
  display: flex;
  align-items: center;
  justify-content: center;
  background: transparent;
}
</style>