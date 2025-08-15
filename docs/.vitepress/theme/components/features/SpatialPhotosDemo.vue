<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import * as THREE from 'three'

const containerRef = ref(null)
let scene, camera, renderer, windowMesh, blurMesh
let mouseX = 0, mouseY = 0
let targetX = 0, targetY = 0
const windowWidth = 3.2
const windowHeight = 4

// Initialize Three.js scene
const initThreeJS = () => {
  // Create scene
  scene = new THREE.Scene()

  // Create camera
  camera = new THREE.PerspectiveCamera(75, containerRef.value.clientWidth / containerRef.value.clientHeight, 0.1, 1000)
  camera.position.z = 3

  // Create renderer
  renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true })
  renderer.setSize(containerRef.value.clientWidth, containerRef.value.clientHeight)
  renderer.setClearColor(0x000000, 0)
  containerRef.value.appendChild(renderer.domElement)

  // Create window geometry
  const windowGeometry = new THREE.PlaneGeometry(windowWidth, windowHeight)

  // Load texture
  const textureLoader = new THREE.TextureLoader()
  const imageTexture = textureLoader.load(
    'https://ar.rokidcdn.com/web-assets/pages/features_spatial_photos.png', (texture) => {
      texture.wrapS = THREE.ClampToEdgeWrapping
      texture.wrapT = THREE.ClampToEdgeWrapping
      texture.minFilter = THREE.LinearFilter
      texture.magFilter = THREE.LinearFilter
    })

  // Simplify window material, ensure basic display first
  const windowMaterial = new THREE.ShaderMaterial({
    uniforms: {
      map: { value: imageTexture },
      cornerRadius: { value: 0.05 },
      blurStrength: { value: 8.0 }, // Increase blur strength
      focusRadius: { value: 0.47 },  // Reduce clear area to 10%
      maxBlurRadius: { value: 0.9 } // Expand blur area
    },
    vertexShader: `
      varying vec2 vUv;
      void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,
    fragmentShader: `
      uniform sampler2D map;
      uniform float cornerRadius;
      uniform float blurStrength;
      uniform float focusRadius;
      uniform float maxBlurRadius;
      varying vec2 vUv;
      
      // Rounded corner mask function
      float roundedRectMask(vec2 uv, float radius) {
        vec2 center = vec2(0.5, 0.5);
        vec2 pos = abs(uv - center);
        vec2 size = vec2(0.5, 0.5);
        
        vec2 corner = max(pos - size + radius, 0.0);
        float dist = length(corner) - radius;
        return 1.0 - smoothstep(-0.01, 0.01, dist);
      }
      
      // Apple spatial photo style depth of field blur
      vec4 appleSpacialBlur(sampler2D tex, vec2 uv, float strength) {
        vec2 center = vec2(0.5, 0.5);
        float distToCenter = distance(uv, center);
        
        // Create Apple-style depth mapping
        float depthBlur = 0.0;
        
        if (distToCenter <= focusRadius) {
          // Focus area: completely clear
          depthBlur = 0.0;
        } else if (distToCenter <= maxBlurRadius) {
          // Transition area: progressive blur, using rounded rectangle area
          float normalizedDist = (distToCenter - focusRadius) / (maxBlurRadius - focusRadius);
          
          // Use rounded rectangle to define blur area
          vec2 rectPos = abs(uv - center);
          vec2 rectSize = vec2(focusRadius, focusRadius);
          vec2 rectCorner = max(rectPos - rectSize + cornerRadius * 0.5, 0.0);
          float rectDist = length(rectCorner) - cornerRadius * 0.5;
          
          // Combine circular distance and rectangular distance
          float blendFactor = smoothstep(-0.05, 0.05, rectDist);
          depthBlur = smoothstep(0.0, 1.0, normalizedDist) * blendFactor;
        } else {
          // Background area: maximum blur
          depthBlur = 1.0;
        }
        
        // If in focus area, return original image
        if (depthBlur < 0.02) {
          return texture2D(tex, uv);
        }
        
        // Apple-style dynamic blur radius
        float dynamicBlurSize = depthBlur * strength * 0.015;
        
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
        float totalWeight = 0.0;
        
        // Use 7x7 sampling kernel to simulate Apple's high-quality blur
        for (int x = -3; x <= 3; x++) {
          for (int y = -3; y <= 3; y++) {
            vec2 offset = vec2(float(x), float(y)) * dynamicBlurSize;
            vec2 sampleUV = uv + offset;
            
            if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 && 
                sampleUV.y >= 0.0 && sampleUV.y <= 1.0) {
              float distance = length(vec2(float(x), float(y)));
              float weight = exp(-distance * distance * 0.12);

              // Edge enhancement, simulate real lens bokeh characteristics
              if (distance > 2.5) {
                weight *= 1.2;
              }
              color += texture2D(tex, sampleUV) * weight;
              totalWeight += weight;
            }
          }
        }

        vec4 blurredColor = color / totalWeight;
        
        // Apple-style color processing
        // Increase contrast and saturation
        blurredColor.rgb = mix(blurredColor.rgb, 
          pow(blurredColor.rgb, vec3(0.92)), depthBlur * 0.25);
        
        // Add slight color temperature shift and vignetting
        blurredColor.rgb = mix(blurredColor.rgb, 
          blurredColor.rgb * vec3(1.02, 1.0, 0.98), depthBlur * 0.15);
        
        // Add slight vignetting effect
        float vignette = 1.0 - smoothstep(0.4, 0.8, distToCenter);
        blurredColor.rgb *= mix(0.92, 1.0, vignette);
        return blurredColor;
      }

      void main() {
        vec2 uv = vUv;
        float mask = roundedRectMask(uv, cornerRadius);
        
        if (mask < 0.01) {
          discard;
        }
        
        vec4 finalColor = appleSpacialBlur(map, uv, blurStrength);
        gl_FragColor = vec4(finalColor.rgb, finalColor.a * mask);
      }
    `,
    transparent: true
  })

  // Create window mesh
  windowMesh = new THREE.Mesh(windowGeometry, windowMaterial)
  scene.add(windowMesh)

  // Create blur effect around image
  createImageBlurEffect()

  // Add ambient light
  const ambientLight = new THREE.AmbientLight(0x404040, 0.8)
  scene.add(ambientLight)
}

// Create blur effect around image
const createImageBlurEffect = () => {
  // Create plane with same size as image
  const blurGeometry = new THREE.PlaneGeometry(windowWidth, windowHeight)

  // Create Gaussian blur material
  // In createImageBlurEffect function, adjust blur material corner radius parameters
  const blurMaterial = new THREE.ShaderMaterial({
    uniforms: {
      time: { value: 0 },
      uTexture: { value: null },
      resolution: { value: new THREE.Vector2(windowWidth, windowHeight) },
      blurStrength: { value: 24.0 }, // Greatly enhance blur strength
      cornerRadius: { value: 0.08 },
      focusPoint: { value: new THREE.Vector2(0.5, 0.5) }, // Focus position
      focusRadius: { value: 0.15 }, // Clear area radius
      maxBlurRadius: { value: 0.45 } // Maximum blur area radius
    },
    vertexShader: `
      varying vec2 vUv;
      void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,
    fragmentShader: `
      uniform float time;
      uniform sampler2D uTexture;
      uniform vec2 resolution;
      uniform float blurStrength;
      uniform float cornerRadius;
      uniform vec2 focusPoint;
      uniform float focusRadius;
      uniform float maxBlurRadius;
      varying vec2 vUv;
      
      float roundedRectMask(vec2 uv, float radius) {
        vec2 center = vec2(0.5, 0.5);
        vec2 pos = abs(uv - center);
        vec2 size = vec2(0.5, 0.5);
        
        vec2 corner = max(pos - size + radius, 0.0);
        float dist = length(corner) - radius;
        
        return 1.0 - smoothstep(-0.003, 0.003, dist);
      }
      
      vec4 visionProBlur(sampler2D tex, vec2 uv, float strength) {
        // Calculate distance to focus point
        float distToFocus = distance(uv, focusPoint);
        
        // Create Vision Pro style depth mapping
        float depthBlur = 0.0;
        
        if (distToFocus <= focusRadius) {
          // Focus area: completely clear
          depthBlur = 0.0;
        } else if (distToFocus <= maxBlurRadius) {
          // Transition area: progressive blur
          float normalizedDist = (distToFocus - focusRadius) / (maxBlurRadius - focusRadius);
          depthBlur = smoothstep(0.0, 1.0, normalizedDist);
        } else {
          // Background area: maximum blur
          depthBlur = 1.0;
        }
        
        // If in focus area, return original image
        if (depthBlur < 0.02) {
          return texture2D(tex, uv);
        }
        
        // Dynamic blur radius, simulate real lens effect
        float dynamicBlurSize = depthBlur * strength * 0.025;
        
        vec4 color = vec4(0.0);
        float totalWeight = 0.0;
        
        // Use larger sampling kernel (9x9) to simulate Vision Pro's high-quality blur
        int sampleRadius = int(mix(1.0, 4.0, depthBlur));
        
        for (int x = -4; x <= 4; x++) {
          for (int y = -4; y <= 4; y++) {
            if (abs(x) > sampleRadius || abs(y) > sampleRadius) continue;
            
            vec2 offset = vec2(float(x), float(y)) * dynamicBlurSize;
            vec2 sampleUV = uv + offset;
            
            if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 && 
                sampleUV.y >= 0.0 && sampleUV.y <= 1.0) {
              
              // Vision Pro style bokeh weight
              float distance = length(vec2(float(x), float(y)));
              float weight = exp(-distance * distance * 0.15);
              
              // Add hexagonal bokeh effect
              float angle = atan(float(y), float(x));
              float hexWeight = 1.0 + 0.2 * cos(angle * 6.0);
              weight *= hexWeight;
              
              // Edge enhancement, simulate real lens bokeh characteristics
              if (distance > 2.0) {
                weight *= 1.3;
              }
              
              color += texture2D(tex, sampleUV) * weight;
              totalWeight += weight;
            } // Add this missing closing brace
          }
        }
        
        vec4 blurredColor = color / totalWeight;
        blurredColor.rgb = mix(blurredColor.rgb, 
          pow(blurredColor.rgb, vec3(0.9)), depthBlur * 0.3);
        
        blurredColor.rgb = mix(blurredColor.rgb, 
          blurredColor.rgb * vec3(1.02, 1.0, 0.98), depthBlur * 0.2);
        
        return blurredColor;
      }
      
      void main() {
        vec2 uv = vUv;

        float mask = roundedRectMask(uv, cornerRadius);
        if (mask < 0.01) {
          discard;
        }
        
        // Apply Vision Pro style blur
        vec4 finalColor = visionProBlur(uTexture, uv, blurStrength);
        
        // Add slight vignetting effect to enhance spatial sense
        vec2 center = vec2(0.5, 0.5);
        float vignette = 1.0 - smoothstep(0.3, 0.8, distance(uv, center));
        finalColor.rgb *= mix(0.85, 1.0, vignette);
        
        // Apply mask
        gl_FragColor = vec4(finalColor.rgb, finalColor.a * mask);
      }
    `,
    transparent: true,
    depthWrite: false
  })

  // Pass original image texture to blur material
  if (windowMesh && windowMesh.material.map) {
    blurMaterial.uniforms.uTexture.value = windowMesh.material.map
  }

  blurMesh = new THREE.Mesh(blurGeometry, blurMaterial)
  blurMesh.position.z = 0.01 // Move slightly forward, overlay on image
  scene.add(blurMesh)
}

// Mouse move event handler
const onMouseMove = (event) => {
  const rect = containerRef.value.getBoundingClientRect()
  mouseX = ((event.clientX - rect.left) / rect.width) * 2 - 1
  mouseY = -((event.clientY - rect.top) / rect.height) * 2 + 1

  targetX = mouseX * 2
  targetY = mouseY * 1
}

// Animation loop
const animate = () => {
  requestAnimationFrame(animate)

  // Smooth camera movement
  camera.position.x += (targetX - camera.position.x) * 0.05
  camera.position.y += (targetY - camera.position.y) * 0.05
  camera.lookAt(0, 0, 0)

  // Window slight rotation animation
  if (windowMesh) {
    windowMesh.rotation.y = Math.sin(Date.now() * 0.001) * 0.1
    windowMesh.rotation.x = Math.cos(Date.now() * 0.0015) * 0.05
  }

  // Update blur effect animation
  if (blurMesh) {
    blurMesh.material.uniforms.time.value = Date.now() * 0.001
    blurMesh.rotation.y = Math.sin(Date.now() * 0.001) * 0.1
    blurMesh.rotation.x = Math.cos(Date.now() * 0.0015) * 0.05
  }

  renderer.render(scene, camera)
}

// Window resize handling
const onWindowResize = () => {
  if (!containerRef.value) return

  camera.aspect = containerRef.value.clientWidth / containerRef.value.clientHeight
  camera.updateProjectionMatrix()
  renderer.setSize(containerRef.value.clientWidth, containerRef.value.clientHeight)
}

onMounted(() => {
  initThreeJS()
  animate()

  containerRef.value.addEventListener('mousemove', onMouseMove)
  window.addEventListener('resize', onWindowResize)
})

onUnmounted(() => {
  if (containerRef.value) {
    containerRef.value.removeEventListener('mousemove', onMouseMove)
  }
  window.removeEventListener('resize', onWindowResize)

  if (renderer) {
    renderer.dispose()
  }
})
</script>

<template>
  <div ref="containerRef" class="spatial-window-demo">
    <!-- Three.js canvas will be inserted here -->
  </div>
</template>

<style scoped>
.spatial-window-demo {
  width: 100%;
  height: 100%;
  min-height: 400px;
  position: relative;
  background: transparent;
  border-radius: 20px;
  /* Add rounded corners */
  overflow: hidden;
  cursor: move;
}

.spatial-window-demo canvas {
  display: block;
  width: 100%;
  height: 100%;
  border-radius: 20px;
  /* Canvas also add rounded corners */
}
</style>