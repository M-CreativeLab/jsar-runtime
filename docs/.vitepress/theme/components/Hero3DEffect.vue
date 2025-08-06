<template>
  <div class="hero-3d-effect" ref="container">
    <canvas ref="canvas" class="hero-3d-canvas"></canvas>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount, computed } from 'vue'
import * as THREE from 'three'

// Receive mouse position props
const props = defineProps({
  mouseX: {
    type: Number,
    default: 0
  },
  mouseY: {
    type: Number,
    default: 0
  }
})

const container = ref(null)
const canvas = ref(null)
let animationFrameId = null
let scene, camera, renderer, particles
let baseCameraPosition = { x: 0, y: 0, z: 50 }

// Detect theme mode
const isDark = computed(() => {
  if (typeof document !== 'undefined') {
    return document.documentElement.classList.contains('dark')
  }
  return false
})

// Theme-related color configuration
const themeColors = computed(() => {
  if (isDark.value) {
    return {
      nodeColor: 0x6366f1,
      connectionColor: 0x8b5cf6,
      ringColors: [0x8b5cf6, 0x06b6d4, 0x10b981],
      particleColors: [
        new THREE.Color(0x6366f1),
        new THREE.Color(0x8b5cf6),
        new THREE.Color(0x06b6d4),
        new THREE.Color(0x10b981)
      ]
    }
  } else {
    return {
      nodeColor: 0x4338ca,
      connectionColor: 0x6366f1,
      ringColors: [0x7c3aed, 0x0891b2, 0x059669],
      particleColors: [
        new THREE.Color(0x4338ca),
        new THREE.Color(0x7c3aed),
        new THREE.Color(0x0891b2),
        new THREE.Color(0x059669)
      ]
    }
  }
})

onMounted(() => {
  initThree()
  animate()

  window.addEventListener('resize', onResize)

  // Listen for theme changes
  const observer = new MutationObserver(() => {
    updateThemeColors()
  })

  if (typeof document !== 'undefined') {
    observer.observe(document.documentElement, {
      attributes: true,
      attributeFilter: ['class']
    })
  }

  // Also remove observer in cleanup function
  onBeforeUnmount(() => {
    observer.disconnect()
  })
})

onBeforeUnmount(() => {
  if (animationFrameId) {
    cancelAnimationFrame(animationFrameId)
  }
  window.removeEventListener('resize', onResize)
})

function initThree() {
  // Create scene
  scene = new THREE.Scene()

  // 创建相机
  camera = new THREE.PerspectiveCamera(75, container.value.clientWidth / container.value.clientHeight, 0.1, 1000)
  camera.position.set(0, 0, 50)

  // Create renderer
  renderer = new THREE.WebGLRenderer({
    canvas: canvas.value,
    alpha: true,
    antialias: true
  })
  renderer.setSize(container.value.clientWidth, container.value.clientHeight)
  renderer.setClearColor(0x000000, 0) // Transparent background

  // Create main container group
  particles = new THREE.Group()

  // 1. Create network nodes (representing spatial web browser nodes)
  createNetworkNodes()

  // 2. Create connection lines (representing data transmission)
  createConnectionLines()

  // 3. Create orbital rings (representing spatial layers of metaverse)
  createOrbitalRings()

  // 4. Create data flow particles (subtle data transmission effects)
  createDataFlowParticles()

  scene.add(particles)
}

function createNetworkNodes() {
  const nodeGeometry = new THREE.SphereGeometry(0.3, 8, 6)
  const nodeMaterial = new THREE.MeshBasicMaterial({
    color: themeColors.value.nodeColor,
    transparent: true,
    opacity: 0.7
  })

  // Create main nodes
  const mainNodes = [
    { x: 0, y: 0, z: 0, scale: 1.5 }, // Center node
    { x: 15, y: 8, z: -5, scale: 1.0 },
    { x: -12, y: -6, z: 8, scale: 1.0 },
    { x: 8, y: -10, z: 12, scale: 1.0 },
    { x: -18, y: 5, z: -8, scale: 1.0 },
    { x: 10, y: 15, z: 3, scale: 1.0 }
  ]

  mainNodes.forEach(nodeData => {
    const node = new THREE.Mesh(nodeGeometry, nodeMaterial.clone())
    node.position.set(nodeData.x, nodeData.y, nodeData.z)
    node.scale.setScalar(nodeData.scale)
    node.userData = { originalScale: nodeData.scale, type: 'node' }
    particles.add(node)
  })
}

function createConnectionLines() {
  const lineMaterial = new THREE.LineBasicMaterial({
    color: themeColors.value.connectionColor,
    transparent: true,
    opacity: 0.25
  })

  // Lines connecting main nodes
  const connections = [
    [0, 1], [0, 2], [0, 3], [0, 4], [0, 5], // Connect from center to other nodes
    [1, 3], [2, 4], [3, 5], [1, 5] // Connections between nodes
  ]

  const nodePositions = [
    new THREE.Vector3(0, 0, 0),
    new THREE.Vector3(15, 8, -5),
    new THREE.Vector3(-12, -6, 8),
    new THREE.Vector3(8, -10, 12),
    new THREE.Vector3(-18, 5, -8),
    new THREE.Vector3(10, 15, 3)
  ]

  connections.forEach(([start, end]) => {
    const geometry = new THREE.BufferGeometry().setFromPoints([
      nodePositions[start],
      nodePositions[end]
    ])
    const line = new THREE.Line(geometry, lineMaterial)
    line.userData = { type: 'connection' }
    particles.add(line)
  })
}

function createOrbitalRings() {
  // Create three orbital rings of different sizes
  const ringData = [
    { radius: 25, segments: 64, opacity: 0.12 },
    { radius: 35, segments: 80, opacity: 0.08 },
    { radius: 45, segments: 96, opacity: 0.06 }
  ]

  ringData.forEach((ring, index) => {
    const geometry = new THREE.RingGeometry(ring.radius - 0.5, ring.radius + 0.5, ring.segments)
    const material = new THREE.MeshBasicMaterial({
      color: themeColors.value.ringColors[index],
      transparent: true,
      opacity: ring.opacity,
      side: THREE.DoubleSide
    })

    const ringMesh = new THREE.Mesh(geometry, material)
    ringMesh.rotation.x = Math.PI / 2 + (index * 0.3)
    ringMesh.rotation.z = index * 0.5
    ringMesh.userData = { type: 'ring', index, ringIndex: index }
    particles.add(ringMesh)
  })
}

function createDataFlowParticles() {
  const particleGeometry = new THREE.BufferGeometry()
  const particleCount = 200 // Reduce particle count for more refinement

  const positions = new Float32Array(particleCount * 3)
  const colors = new Float32Array(particleCount * 3)
  const sizes = new Float32Array(particleCount)

  const colorPalette = themeColors.value.particleColors

  for (let i = 0; i < particleCount; i++) {
    // Distribute particles near orbits
    const angle = (i / particleCount) * Math.PI * 2
    const radius = 20 + Math.random() * 30
    const height = (Math.random() - 0.5) * 10

    positions[i * 3] = Math.cos(angle) * radius + (Math.random() - 0.5) * 5
    positions[i * 3 + 1] = height
    positions[i * 3 + 2] = Math.sin(angle) * radius + (Math.random() - 0.5) * 5

    sizes[i] = Math.random() * 0.8 + 0.2

    const color = colorPalette[Math.floor(Math.random() * colorPalette.length)]
    colors[i * 3] = color.r
    colors[i * 3 + 1] = color.g
    colors[i * 3 + 2] = color.b
  }

  particleGeometry.setAttribute('position', new THREE.BufferAttribute(positions, 3))
  particleGeometry.setAttribute('color', new THREE.BufferAttribute(colors, 3))
  particleGeometry.setAttribute('size', new THREE.BufferAttribute(sizes, 1))

  const particleMaterial = new THREE.PointsMaterial({
    size: 1.2,
    sizeAttenuation: true,
    vertexColors: true,
    transparent: true,
    opacity: 0.5,
    blending: THREE.AdditiveBlending
  })

  const dataParticles = new THREE.Points(particleGeometry, particleMaterial)
  dataParticles.userData = { type: 'dataFlow' }
  particles.add(dataParticles)
}

function animate() {
  animationFrameId = requestAnimationFrame(animate)

  // Slightly change camera direction based on mouse position
  if (camera) {
    const mouseInfluence = 0.5 // Influence strength, adjustable
    camera.position.x = baseCameraPosition.x + props.mouseX * mouseInfluence
    camera.position.y = baseCameraPosition.y - props.mouseY * mouseInfluence
    camera.lookAt(0, 0, 0) // Always look at scene center
  }

  if (particles) {
    const time = Date.now() * 0.001

    // Overall slow rotation
    particles.rotation.y += 0.002

    // Animate all child objects
    particles.children.forEach((child, index) => {
      if (child.userData.type === 'node') {
        // Node pulsing effect
        const pulse = Math.sin(time * 2 + index) * 0.2 + 1
        child.scale.setScalar(child.userData.originalScale * pulse)

        // Node glowing effect (through opacity changes)
        child.material.opacity = 0.6 + Math.sin(time * 3 + index) * 0.2
      } else if (child.userData.type === 'ring') {
        // Orbital ring rotation
        const ringIndex = child.userData.index
        child.rotation.z += (0.001 + ringIndex * 0.0005)

        // 环形轨道透明度变化
        child.material.opacity = child.material.opacity * 0.99 + (0.05 + Math.sin(time + ringIndex) * 0.03) * 0.01
      } else if (child.userData.type === 'connection') {
        // 连接线透明度脉动
        child.material.opacity = 0.2 + Math.sin(time * 1.5 + index * 0.5) * 0.1
      } else if (child.userData.type === 'dataFlow') {
        // 数据流粒子动画
        if (child.geometry && child.geometry.attributes) {
          const positions = child.geometry.attributes.position.array
          const sizes = child.geometry.attributes.size.array

          for (let i = 0; i < positions.length; i += 3) {
            const particleIndex = i / 3

            // Particles flow along orbits
            const angle = time * 0.5 + particleIndex * 0.1
            const radius = 20 + Math.sin(time + particleIndex) * 10

            positions[i] = Math.cos(angle) * radius + Math.sin(time * 2 + particleIndex) * 2
            positions[i + 1] = Math.sin(time * 0.8 + particleIndex) * 5
            positions[i + 2] = Math.sin(angle) * radius + Math.cos(time * 1.5 + particleIndex) * 2

            // Particle size changes
            sizes[particleIndex] = 0.5 + Math.sin(time * 4 + particleIndex) * 0.3
          }

          child.geometry.attributes.position.needsUpdate = true
          child.geometry.attributes.size.needsUpdate = true
        }
      }
    })
  }

  if (renderer && scene && camera) {
    renderer.render(scene, camera)
  }
}

function onResize() {
  if (camera && renderer && container.value) {
    camera.aspect = container.value.clientWidth / container.value.clientHeight
    camera.updateProjectionMatrix()
    renderer.setSize(container.value.clientWidth, container.value.clientHeight)
  }
}

function updateThemeColors() {
  if (!scene) return

  // Update node colors
  scene.traverse((child) => {
    if (child.isMesh && child.material && child.geometry.type === 'SphereGeometry') {
      child.material.color.setHex(themeColors.value.nodeColor)
    }

    // Update connection line colors
    if (child.isLine && child.material) {
      child.material.color.setHex(themeColors.value.connectionColor)
    }

    // Update orbital ring colors
    if (child.isMesh && child.material && child.geometry.type === 'RingGeometry') {
      const ringIndex = child.userData.ringIndex || 0
      child.material.color.setHex(themeColors.value.ringColors[ringIndex])
    }

    // Update particle colors
    if (child.isPoints && child.geometry.attributes.color) {
      const colors = child.geometry.attributes.color.array
      const colorPalette = themeColors.value.particleColors

      for (let i = 0; i < colors.length; i += 3) {
        const colorIndex = Math.floor((i / 3) % colorPalette.length)
        const color = colorPalette[colorIndex]
        colors[i] = color.r
        colors[i + 1] = color.g
        colors[i + 2] = color.b
      }

      child.geometry.attributes.color.needsUpdate = true
    }
  })
}
</script>

<style scoped>
.hero-3d-effect {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  overflow: hidden;
  z-index: 0;
  pointer-events: none;
  opacity: 0.4;
  background: radial-gradient(ellipse at center, rgba(79, 70, 229, 0.05) 0%, transparent 70%);
}

.hero-3d-canvas {
  width: 100%;
  height: 100%;
}
</style>
