<template>
  <div class="audio-visualizer-demo">
    <canvas ref="canvasRef" class="visualizer-canvas"
            @touchstart="handleTouchStart"
            @touchmove="handleTouchMove"
            @touchend="handleTouchEnd"></canvas>
    <div class="controls">
      <div class="distance-control">
        <label>Distance</label>
        <input type="range" min="2" max="20" step="0.1" v-model="cameraDistance" @input="updateCameraDistance"
          class="distance-slider" />
      </div>
      <div class="audio-control">
        <button @click="toggleAudio" class="audio-button">
          {{ isPlaying ? 'Pause' : 'Play' }}
        </button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import * as THREE from 'three'

// Reactive data
const canvasRef = ref(null)
const cameraDistance = ref(10)
const isPlaying = ref(false)
const audioVolume = ref(0)

// Three.js related variables
let scene, camera, renderer, geometry, material, mesh
let audioContext, analyser, audioBuffer, source
let animationId
let frequencyData
let gainNode = null
// New wave-related variables
let waveRings = [] // Store multiple wave rings
let waveGeometry, waveMaterial

// Add color array
const waveColors = [
  new THREE.Color(0x007AFF), // Apple Blue
  new THREE.Color(0xFF3B30), // Apple Red
  new THREE.Color(0x34C759), // Apple Green
  new THREE.Color(0xFF9500), // Apple Orange
  new THREE.Color(0xAF52DE), // Apple Purple
  new THREE.Color(0xFF2D92), // Apple Pink
  new THREE.Color(0x5AC8FA), // Apple Light Blue
  new THREE.Color(0xFFCC00), // Apple Yellow
]

// Add color change related variables
let colorChangeTimer = 0
let currentColorIndex = 0
let nextColorIndex = 1
let colorTransition = 0

// New function to create wave rings
// Create wave rings function - ensure centered
const createWaveRings = () => {
  const ringCount = 8
  
  for (let i = 0; i < ringCount; i++) {
    const ringGeometry = new THREE.RingGeometry(0.8 + i * 0.15, 1.0 + i * 0.15, 64)
    
    // Wave ring shader material
    const ringMaterial = new THREE.ShaderMaterial({
      uniforms: {
        time: { value: 0 },
        audioData: { value: 0.1 }, // Set initial value to avoid complete transparency
        ringIndex: { value: i },
        opacity: { value: 0.8 - i * 0.1 }, // Increase base transparency
        waveColor: { value: new THREE.Color(0x007AFF) }
      },
      vertexShader: `
        uniform float time;
        uniform float audioData;
        uniform float ringIndex;
        varying vec2 vUv;
        varying float vWave;
        varying vec2 vPosition;
        
        void main() {
          vUv = uv;
          vPosition = position.xy;
          
          vec3 pos = position;
          
          // Create wave effect - based on audio data and time
          float angle = atan(pos.y, pos.x);
          float radius = length(pos.xy);
          
          // Multi-layer wave overlay
          float wave1 = sin(angle * 8.0 + time * 2.0 - ringIndex * 0.5) * audioData;
          float wave2 = cos(angle * 12.0 + time * 1.5 - ringIndex * 0.3) * audioData * 0.5;
          float wave3 = sin(angle * 16.0 + time * 3.0 - ringIndex * 0.8) * audioData * 0.3;
          
          float totalWave = (wave1 + wave2 + wave3) * 0.2; // Reduce deformation amplitude
          vWave = totalWave;
          
          // Apply wave deformation
          pos.xy *= (1.0 + totalWave);
          
          // Outward expansion animation
          float expansionPhase = mod(time * 0.8 + ringIndex * 0.4, 2.0);
          float expansion = smoothstep(0.0, 0.3, expansionPhase) * (1.0 - smoothstep(1.7, 2.0, expansionPhase));
          pos.xy *= (1.0 + expansion * 0.3); // Reduce expansion amplitude
          
          gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
        }
      `,
      fragmentShader: `
        uniform float time;
        uniform float audioData;
        uniform float opacity;
        uniform vec3 waveColor;
        varying vec2 vUv;
        varying float vWave;
        varying vec2 vPosition;
        
        void main() {
          // Color based on wave intensity
          vec3 color = waveColor;
          
          // Adjust color intensity based on audio data
          color *= (0.8 + audioData * 1.5); // Ensure base brightness
          
          // Simplified radial gradient effect
          float distanceFromCenter = length(vPosition);
          float radialGradient = 1.0 - smoothstep(0.5, 1.5, distanceFromCenter);
          
          // Simplified transparency calculation, ensure visibility
          float baseAlpha = opacity * 0.6; // Base transparency
          float waveAlpha = abs(vWave) * 0.4; // Wave enhancement
          float audioAlpha = audioData * 0.3; // Audio response
          
          float alpha = baseAlpha + waveAlpha + audioAlpha;
          alpha = clamp(alpha, 0.1, 1.0); // Ensure minimum visibility
          
          gl_FragColor = vec4(color, alpha * radialGradient);
        }
      `,
      transparent: true,
      side: THREE.DoubleSide,
      blending: THREE.AdditiveBlending,
      depthWrite: false // Avoid depth conflicts
    })

    const ringMesh = new THREE.Mesh(ringGeometry, ringMaterial)
    ringMesh.position.set(0, 0, 0) // Ensure rings are also at origin
    ringMesh.rotation.x = 0
    scene.add(ringMesh)

    waveRings.push({
      mesh: ringMesh,
      material: ringMaterial,
      initialScale: 1 + i * 0.1
    })
  }
}

// Initialize Three.js scene
const initThreeJS = () => {
  // Create scene - transparent background
  scene = new THREE.Scene()

  // Get container dimensions
  const width = canvasRef.value.clientWidth
  const height = canvasRef.value.clientHeight
  const aspect = width / height

  // Create camera - optimize field of view for different resolutions
  const fov = aspect < 1 ? 85 : 75 // Increase field of view for portrait orientation
  camera = new THREE.PerspectiveCamera(fov, aspect, 0.1, 1000)
  
  // Ensure camera always looks at scene center
  camera.position.set(0, 0, cameraDistance.value)
  camera.lookAt(0, 0, 0)

  // Create renderer - transparent background
  renderer = new THREE.WebGLRenderer({
    canvas: canvasRef.value,
    antialias: true,
    alpha: true // Enable transparency
  })
  renderer.setSize(width, height)
  renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2)) // Limit pixel ratio for performance
  renderer.setClearColor(0x000000, 0) // Completely transparent background

  // Enable shadows to enhance 3D effect
  renderer.shadowMap.enabled = true
  renderer.shadowMap.type = THREE.PCFSoftShadowMap

  // Create geometry - use more complex geometry to enhance 3D effect
  geometry = new THREE.IcosahedronGeometry(1, 2) // More detailed sphere

  // Apple-style material - use glass texture and metallic luster
  material = new THREE.ShaderMaterial({
    uniforms: {
      time: { value: 0 },
      audioData: { value: 0 },
      // Apple classic colors: Space Gray, Silver, Gold
      primaryColor: { value: new THREE.Color(0x007AFF) }, // Apple Blue
      secondaryColor: { value: new THREE.Color(0xFF3B30) }, // Apple Red
      accentColor: { value: new THREE.Color(0xFFFFFF) }, // White highlight
      metallic: { value: 0.8 },
      roughness: { value: 0.2 }
    },
    vertexShader: `
      uniform float time;
      uniform float audioData;
      varying vec3 vPosition;
      varying vec3 vNormal;
      varying vec3 vWorldPosition;
      
      void main() {
        vPosition = position;
        vNormal = normalize(normalMatrix * normal);
        
        // Create more natural wave deformation based on audio data
        vec3 newPosition = position;
        float wave1 = sin(position.y * 8.0 + time * 1.5) * audioData * 0.15;
        float wave2 = cos(position.x * 6.0 + time * 2.0) * audioData * 0.1;
        float wave3 = sin(position.z * 10.0 + time * 1.8) * audioData * 0.08;
        
        newPosition += normal * (wave1 + wave2 + wave3);
        
        vec4 worldPosition = modelMatrix * vec4(newPosition, 1.0);
        vWorldPosition = worldPosition.xyz;
        
        gl_Position = projectionMatrix * modelViewMatrix * vec4(newPosition, 1.0);
      }
    `,
    fragmentShader: `
      uniform float time;
      uniform float audioData;
      uniform vec3 primaryColor;
      uniform vec3 secondaryColor;
      uniform vec3 accentColor;
      uniform float metallic;
      uniform float roughness;
      
      varying vec3 vPosition;
      varying vec3 vNormal;
      varying vec3 vWorldPosition;
      
      // Apple-style Fresnel reflection
      float fresnel(vec3 viewDirection, vec3 normal, float power) {
        return pow(1.0 - max(0.0, dot(viewDirection, normal)), power);
      }
      
      void main() {
        vec3 viewDirection = normalize(cameraPosition - vWorldPosition);
        vec3 normal = normalize(vNormal);
        
        // Base color mixing - Apple-style gradient
        float colorMix = sin(vPosition.y * 3.0 + time * 0.5) * 0.5 + 0.5;
        vec3 baseColor = mix(primaryColor, secondaryColor, colorMix + audioData * 0.3);
        
        // Fresnel effect - edge glow
        float fresnelFactor = fresnel(viewDirection, normal, 2.0);
        vec3 fresnelColor = accentColor * fresnelFactor;
        
        // Metallic reflection
        float metallicReflection = metallic * (1.0 - roughness);
        vec3 reflectionColor = mix(baseColor, accentColor, metallicReflection * fresnelFactor);
        
        // Audio-responsive glow effect
        vec3 glowColor = primaryColor * audioData * 2.0;
        
        // Final color composition
        vec3 finalColor = baseColor + fresnelColor * 0.8 + glowColor;
        finalColor = mix(finalColor, reflectionColor, 0.3);
        
        // Apple-style transparency - glass texture
        float alpha = 0.85 + audioData * 0.15 + fresnelFactor * 0.2;
        
        gl_FragColor = vec4(finalColor, alpha);
      }
    `,
    transparent: true,
    side: THREE.DoubleSide
  })

  // Create mesh - ensure position at origin
  mesh = new THREE.Mesh(geometry, material)
  mesh.position.set(0, 0, 0) // Explicitly set to origin
  mesh.castShadow = true
  mesh.receiveShadow = true
  scene.add(mesh)

  // Create outward radiating wave rings - ensure centered
  createWaveRings()

  // Apple-style lighting setup
  // Main light - simulate Apple product photography main light
  const mainLight = new THREE.DirectionalLight(0xffffff, 1.2)
  mainLight.position.set(5, 5, 5)
  mainLight.castShadow = true
  mainLight.shadow.mapSize.width = 2048
  mainLight.shadow.mapSize.height = 2048
  scene.add(mainLight)

  // Fill light - soft ambient light
  const ambientLight = new THREE.AmbientLight(0x404040, 0.4)
  scene.add(ambientLight)

  // Rim light - enhance 3D effect
  const rimLight = new THREE.DirectionalLight(0x007AFF, 0.8)
  rimLight.position.set(-3, 2, -3)
  scene.add(rimLight)

  // Bottom reflection light - simulate Apple display stand effect
  const bottomLight = new THREE.DirectionalLight(0xFFFFFF, 0.3)
  bottomLight.position.set(0, -5, 0)
  scene.add(bottomLight)

  // Set initial volume
  updateCameraDistance()
}

// Initialize audio context
const initAudio = async () => {
  try {
    audioContext = new (window.AudioContext || window.webkitAudioContext)()
    analyser = audioContext.createAnalyser()
    analyser.fftSize = 512 // Increase analysis precision
    frequencyData = new Uint8Array(analyser.frequencyBinCount)
    
    console.info('Audio context initialized successfully, state:', audioContext.state)

    // Load remote audio file
    const audioUrl = 'http://ar.rokidcdn.com/web-assets/pages/sounds/music.wav'
    
    try {
      console.info('Starting to load remote audio:', audioUrl)
      const response = await fetch(audioUrl)
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`)
      }
      const arrayBuffer = await response.arrayBuffer()
      audioBuffer = await audioContext.decodeAudioData(arrayBuffer)
      console.info('Remote audio loaded successfully:', audioBuffer.duration + ' seconds, sample rate:', audioBuffer.sampleRate)
    } catch (fetchError) {
      console.warn('Remote audio loading failed, using backup audio:', fetchError)
      // Backup solution: use original synthesized audio
      const sampleRate = audioContext.sampleRate
      const duration = 15 // 15-second loop
      const frameCount = sampleRate * duration
      
      audioBuffer = audioContext.createBuffer(2, frameCount, sampleRate) // Stereo
      
      for (let channel = 0; channel < audioBuffer.numberOfChannels; channel++) {
        const channelData = audioBuffer.getChannelData(channel)
        
        for (let i = 0; i < frameCount; i++) {
          const t = i / sampleRate
          // Create more complex harmonies
          channelData[i] = 
            Math.sin(2 * Math.PI * 220 * t) * 0.2 + // Base frequency
            Math.sin(2 * Math.PI * 440 * t) * 0.15 + // Octave
            Math.sin(2 * Math.PI * 660 * t) * 0.1 + // Fifth
            Math.sin(2 * Math.PI * 880 * t) * 0.08 + // High octave
            Math.sin(2 * Math.PI * 110 * t) * 0.12 * (channel === 0 ? 1 : 0.8) // Bass, left and right channels slightly different
        }
      }
      console.info('Backup audio generated successfully:', audioBuffer.duration + ' seconds')
    }
  } catch (error) {
    console.error('Audio initialization failed:', error)
  }
}

// Audio playback control
const toggleAudio = async () => {
  if (!audioContext || !audioBuffer) {
    console.error('Audio context or buffer not initialized')
    return
  }

  if (isPlaying.value) {
    // Stop playback
    if (source) {
      source.stop()
      source = null
      gainNode = null
    }
    isPlaying.value = false
    console.info('Audio stopped')
  } else {
    try {
      // Ensure audio context is running
      if (audioContext.state === 'suspended') {
        await audioContext.resume()
      }

      source = audioContext.createBufferSource()
      source.buffer = audioBuffer
      source.loop = true

      // Create gain node to control volume
      gainNode = audioContext.createGain()
      // Use larger volume range for more noticeable changes
      const baseVolume = audioVolume.value
      gainNode.gain.value = baseVolume
      console.info('Set volume:', baseVolume)

      // Connect audio nodes
      source.connect(gainNode)
      gainNode.connect(analyser)
      analyser.connect(audioContext.destination)

      // Add error handling
      source.onended = () => {
        console.info('Audio playback ended')
      }

      source.start()
      console.info('Audio started playing, buffer duration:', audioBuffer.duration, 'seconds')
      isPlaying.value = true
    } catch (error) {
      console.error('Audio playback failed:', error)
      isPlaying.value = false
    }
  }
}

// Update camera distance and volume
const updateCameraDistance = () => {
  if (camera) {
    // Keep camera moving on Z-axis, always looking at origin
    camera.position.set(0, 0, parseFloat(cameraDistance.value))
    camera.lookAt(0, 0, 0) // Ensure always looking at scene center

    // Adjust volume based on distance - more noticeable volume changes
    const maxDistance = 20
    const minDistance = 2
    const normalizedDistance = (cameraDistance.value - minDistance) / (maxDistance - minDistance)
    
    // Use larger volume range: from 0.05 to 2.0 for more noticeable changes
    const minVolume = 0.05  // Lower minimum volume
    const maxVolume = 2.0   // Higher maximum volume
    audioVolume.value = minVolume + (maxVolume - minVolume) * Math.pow(1 - normalizedDistance, 1.5)
    
    console.info('Distance:', cameraDistance.value, 'Volume:', audioVolume.value.toFixed(2))
    
    // Update volume in real-time
    if (isPlaying.value && gainNode) {
      gainNode.gain.setValueAtTime(audioVolume.value, audioContext.currentTime)
      console.info('Real-time volume updated to:', audioVolume.value.toFixed(2))
    }
  }
}

// Animation loop
// Ensure objects are centered in animate function
const animate = () => {
  animationId = requestAnimationFrame(animate)

  if (analyser && frequencyData) {
    analyser.getByteFrequencyData(frequencyData)

    // Analyze audio data
    const bassRange = Math.floor(frequencyData.length * 0.1)
    const midRange = Math.floor(frequencyData.length * 0.5)
    const trebleRange = frequencyData.length

    let bassSum = 0, midSum = 0, trebleSum = 0
    for (let i = 0; i < bassRange; i++) bassSum += frequencyData[i]
    for (let i = bassRange; i < midRange; i++) midSum += frequencyData[i]
    for (let i = midRange; i < trebleRange; i++) trebleSum += frequencyData[i]

    const bassAvg = bassSum / bassRange / 255
    const midAvg = midSum / (midRange - bassRange) / 255
    const trebleAvg = trebleSum / (trebleRange - midRange) / 255

    // Update main mesh shader uniforms
    if (material && material.uniforms) {
      material.uniforms.time.value = performance.now() * 0.001
      material.uniforms.audioData.value = midAvg
    }

    // Update wave rings
    if (waveRings.length > 0) {
      // Color change logic
      colorChangeTimer += 0.016 // Assuming 60fps
      if (colorChangeTimer > 2.0) { // Change color every 2 seconds
        colorChangeTimer = 0
        currentColorIndex = nextColorIndex
        nextColorIndex = (nextColorIndex + 1) % waveColors.length
        colorTransition = 0
      }
      colorTransition = Math.min(colorTransition + 0.02, 1.0) // Smooth transition

      waveRings.forEach((ring, index) => {
        const frequencyIndex = Math.floor((index / waveRings.length) * frequencyData.length)
        const audioResponse = frequencyData[frequencyIndex] / 255

        // Update shader uniforms
        ring.material.uniforms.time.value = performance.now() * 0.001
        ring.material.uniforms.audioData.value = Math.max(0.1, audioResponse)

        // Scaling effect
        const scale = ring.initialScale * (1 + audioResponse * 0.3)
        ring.mesh.scale.setScalar(scale)

        // Dynamic color change
        if (isPlaying.value && audioResponse > 0.1) {
          const currentColor = waveColors[currentColorIndex]
          const nextColor = waveColors[nextColorIndex]
          
          // Color mixing based on audio intensity
          const intensity = 0.5 + audioResponse * 1.5
          const r = THREE.MathUtils.lerp(currentColor.r, nextColor.r, colorTransition) * intensity
          const g = THREE.MathUtils.lerp(currentColor.g, nextColor.g, colorTransition) * intensity
          const b = THREE.MathUtils.lerp(currentColor.b, nextColor.b, colorTransition) * intensity
          
          // Add slight color offset for each ring to create rainbow effect
          const hueOffset = (index * 0.1 + performance.now() * 0.0005) % 1.0
          const offsetColor = new THREE.Color().setHSL(
            (r + g + b) / 3 + hueOffset,
            0.8,
            0.6 + audioResponse * 0.4
          )
          ring.material.uniforms.waveColor.value = offsetColor
        } else {
          // Restore default Apple blue when idle
          const intensity = 0.5
          ring.material.uniforms.waveColor.value = new THREE.Color(0x007AFF).multiplyScalar(intensity)
        }
      })
    }

    // Control main mesh rotation based on bass, but keep position centered
    if (mesh) {
      mesh.rotation.y += bassAvg * 0.02
      mesh.rotation.x += trebleAvg * 0.01
      
      // Control scaling based on mid frequencies
      const scale = 1 + midAvg * 0.3
      mesh.scale.setScalar(scale)
      
      // Adjust position based on audio intensity (slight floating effect), but keep near center
      const floatY = Math.sin(performance.now() * 0.001) * midAvg * 0.3 // Reduce floating range
      mesh.position.set(0, floatY, 0) // Only slight floating on Y-axis, keep X and Z centered
    }
  }

  // Render scene
  if (renderer && scene && camera) {
    renderer.render(scene, camera)
  }
}

// Window resize handling - optimize centering logic
const handleResize = () => {
  if (!camera || !renderer || !canvasRef.value) return

  const width = canvasRef.value.clientWidth
  const height = canvasRef.value.clientHeight
  const aspect = width / height

  // Adjust field of view based on aspect ratio
  const fov = aspect < 1 ? 85 : 75 // Increase field of view for portrait orientation
  camera.fov = fov
  camera.aspect = aspect
  camera.updateProjectionMatrix()

  // Ensure camera position and orientation are correct
  camera.position.set(0, 0, cameraDistance.value)
  camera.lookAt(0, 0, 0)

  // Update renderer size
  renderer.setSize(width, height)
  renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2))

  // Ensure all object positions are correct
  if (mesh) {
    mesh.position.set(0, 0, 0)
  }
  
  // Ensure wave ring positions are correct
  waveRings.forEach(ring => {
    if (ring.mesh) {
      ring.mesh.position.set(0, 0, 0)
    }
  })
}

// Component mounted
onMounted(async () => {
  await initAudio()
  initThreeJS()
  animate()

  window.addEventListener('resize', handleResize)
})

// Component unmounted
onUnmounted(() => {
  if (animationId) {
    cancelAnimationFrame(animationId)
  }

  if (source) {
    source.stop()
  }

  if (audioContext) {
    audioContext.close()
  }

  // Clean up wave rings
  waveRings.forEach(ring => {
    if (ring.mesh) {
      scene.remove(ring.mesh)
      ring.mesh.geometry.dispose()
      ring.mesh.material.dispose()
    }
  })
  waveRings = []

  if (renderer) {
    renderer.dispose()
  }

  window.removeEventListener('resize', handleResize)
})
</script>

<style scoped>
.audio-visualizer-demo {
  position: relative;
  width: 100%;
  height: 400px;
  background: transparent;
  overflow: hidden;
  touch-action: none;
  display: flex;
  align-items: center;
  justify-content: center;
}

.visualizer-canvas {
  width: 100%;
  height: 100%;
  display: block;
  background: transparent;
  touch-action: none;
  object-fit: contain;
}

.controls {
  position: absolute;
  bottom: 20px;
  left: 50%;
  transform: translateX(-50%);
  display: flex;
  gap: 20px;
  align-items: center;
  justify-content: space-between;
  background: rgba(0, 0, 0, 0.7);
  padding: 15px 25px;
  border-radius: 25px;
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  max-width: calc(100vw - 40px);
  box-sizing: border-box;
  flex-wrap: nowrap; /* Keep single line on desktop */
}

.distance-control {
  display: flex;
  align-items: center;
  gap: 10px;
  flex-shrink: 0; /* Prevent compression */
}

.distance-control label {
  color: white;
  font-size: 14px;
  font-weight: 500;
  min-width: 60px;
  white-space: nowrap; /* Prevent text wrapping */
}

.distance-slider {
  width: 120px;
  height: 4px;
  background: rgba(255, 255, 255, 0.3);
  border-radius: 2px;
  outline: none;
  -webkit-appearance: none;
  appearance: none;
  flex-shrink: 0; /* Prevent slider compression */
}

.distance-slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 16px;
  height: 16px;
  background: #007AFF;
  border-radius: 50%;
  cursor: pointer;
  box-shadow: 0 2px 6px rgba(0, 122, 255, 0.3);
}

.distance-slider::-moz-range-thumb {
  width: 16px;
  height: 16px;
  background: #007AFF;
  border-radius: 50%;
  cursor: pointer;
  border: none;
  box-shadow: 0 2px 6px rgba(0, 122, 255, 0.3);
}

.audio-control {
  flex-shrink: 0; /* Prevent button compression */
}

.audio-button {
  background: linear-gradient(135deg, #007AFF, #0056CC);
  color: white;
  border: none;
  padding: 10px 20px;
  border-radius: 20px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 4px 15px rgba(0, 122, 255, 0.3);
  min-height: 44px;
  display: flex;
  align-items: center;
  justify-content: center;
  white-space: nowrap; /* Prevent button text wrapping */
}

.audio-button:hover {
  background: linear-gradient(135deg, #0056CC, #003D99);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(0, 122, 255, 0.4);
}

.audio-button:active {
  transform: translateY(0);
}

/* Tablet adaptation */
@media (max-width: 1024px) {
  .controls {
    gap: 15px;
    padding: 12px 20px;
  }
  
  .distance-slider {
    width: 100px;
  }
}

/* Mobile adaptation - below 768px */
@media (max-width: 768px) {
  .audio-visualizer-demo {
    height: 300px;
  }
  
  .controls {
    bottom: 15px;
    gap: 12px;
    padding: 12px 18px;
    width: 100%;
  }
  
  .distance-control {
    justify-content: center; /* Center distance control */
    gap: 8px;
  }
  
  .distance-control label {
    font-size: 13px;
    min-width: 50px;
  }
  
  .distance-slider {
    width: 120px; /* Keep appropriate width on mobile */
    height: 6px; /* Increase height for touch */
  }
  
  .distance-slider::-webkit-slider-thumb {
    width: 20px;
    height: 20px;
  }
  
  .distance-slider::-moz-range-thumb {
    width: 20px;
    height: 20px;
  }
  
  .audio-control {
    display: flex;
    justify-content: center; /* Center button */
  }
  
  .audio-button {
    padding: 12px 24px;
    font-size: 14px;
    min-width: 120px; /* Ensure minimum button width */
  }
}

/* Small phone adaptation - below 480px */
@media (max-width: 480px) {
  .audio-visualizer-demo {
    height: 250px;
  }
  
  .controls {
    bottom: 10px;
    padding: 10px 15px;
    border-radius: 20px;
    min-width: 260px;
    gap: 10px;
  }
  
  .distance-control label {
    font-size: 12px;
    min-width: 45px;
  }
  
  .distance-slider {
    width: 100px;
  }
  
  .audio-button {
    padding: 10px 20px;
    font-size: 13px;
    min-height: 40px;
    min-width: 100px;
  }
}

/* Extra small screen adaptation - below 360px */
@media (max-width: 360px) {
  .controls {
    min-width: 240px;
    padding: 8px 12px;
  }
  
  .distance-control {
    gap: 6px;
  }
  
  .distance-control label {
    font-size: 11px;
    min-width: 40px;
  }
  
  .distance-slider {
    width: 80px;
  }
  
  .audio-button {
    padding: 8px 16px;
    font-size: 12px;
    min-width: 80px;
  }
}

/* Landscape adaptation */
@media (orientation: landscape) and (max-height: 500px) {
  .audio-visualizer-demo {
    height: 200px;
  }
  
  .controls {
    bottom: 5px;
    padding: 8px 15px;
    flex-direction: row; /* Restore horizontal layout in landscape */
    gap: 10px;
  }
  
  .distance-control {
    gap: 6px;
  }
  
  .audio-button {
    padding: 6px 12px;
    font-size: 12px;
  }
}

/* High-resolution desktop optimization */
@media (min-width: 1200px) {
  .controls {
    gap: 25px;
    padding: 18px 30px;
  }
  
  .distance-slider {
    width: 140px;
  }
  
  .audio-button {
    padding: 12px 24px;
    font-size: 15px;
  }
}

/* Ensure centering at all resolutions */
@media (max-width: 768px) {
  .audio-visualizer-demo {
    min-height: 250px;
  }
}

@media (max-width: 480px) {
  .audio-visualizer-demo {
    min-height: 200px;
  }
}

/* Ultra-wide screen adaptation */
@media (min-aspect-ratio: 2/1) {
  .audio-visualizer-demo {
    height: 300px;
  }
}

/* Ultra-tall screen adaptation */
@media (max-aspect-ratio: 1/2) {
  .audio-visualizer-demo {
    height: 400px;
  }
}
</style>
