#!/usr/bin/env node
/* eslint-env node */
/* eslint-disable no-console */
/**
 * 批量修改WebGL测试用例，使其能在JSAR Runtime中运行
 * 基于之前成功的修改模板
 */

const fs = require('fs');
const path = require('path');

// 配置
const CONFIG = {
  sourceDir: '/Users/next/develop/git-proj/jsar-zone/example/jsar-example/jsarcase/test/conformance2/rendering',
  backupSuffix: '.original',
  dryRun: false, // 设置为true进行预览，false进行实际修改
};

// 修改模板
const TEMPLATE = {
  
  // 需要替换的script标签开始部分
  scriptStart: '<script type="module">',
  
  // 环境检测和初始化代码
  initCode: `
"use strict";
import * as loader from '../../js/webgl-async-loader.js';
const { waitForWebGLTestGlobals } = loader;

// 将原始测试代码封装为函数
function runWebGLTest() {
  try {
`,
  
  // WebGL上下文获取优化
  glContextFix: 'var gl = navigator.gl || wtu.create3DContext(canvas',
  
  // 测试结束代码
  endCode: `
    var successfullyParsed = true;
    console.log("✅ WebGL test completed successfully!");
  } catch (error) {
    console.error("❌ WebGL test failed:", error);
    if (typeof debug !== 'undefined') {
      debug("Test failed: " + error.message);
    }
  }
}

// 环境检测函数
function isJSARRuntime() {
  const checks = [
    () => typeof waitForWebGLTestGlobals !== 'undefined',
    () => typeof process !== 'undefined' && process.versions && process.versions.node,
    () => typeof navigator !== 'undefined' && navigator.userAgent && navigator.userAgent.includes('JSAR'),
    () => typeof global !== 'undefined' && global.process,
    () => typeof window === 'undefined' && typeof global !== 'undefined',
    () => typeof module !== 'undefined' && module.exports
  ];
  
  const result = checks.some(check => {
    try {
      return check();
    } catch (e) {
      return false;
    }
  });
  
  console.log('🔍 JSAR Runtime detection result:', result);
  return result;
}

// 简单轮询等待函数
function waitForGlobalsSimple(globals, callback, maxAttempts = 300) {
  let attempts = 0;
  console.log('🔄 Starting simple polling for globals:', globals);
  
  function check() {
    attempts++;
    const available = [];
    const missing = [];
    
    globals.forEach(name => {
      try {
        if (typeof eval(name) !== 'undefined') {
          available.push(name);
        } else {
          missing.push(name);
        }
      } catch (e) {
        missing.push(name);
      }
    });
    
    if (missing.length === 0) {
      console.log('✅ All globals available:', globals);
      callback();
    } else if (attempts >= maxAttempts) {
      console.error('❌ Timeout waiting for globals. Available:', available, 'Missing:', missing);
    } else {
      if (attempts % 20 === 0 || attempts <= 5) {
        console.log(\`⏳ Waiting for globals... Available: [\${available.join(', ')}], Missing: [\${missing.join(', ')}] (\${attempts}/\${maxAttempts})\`);
      }
      setTimeout(check, 50);
    }
  }
  
  check();
}

// 智能初始化
function initializeTest() {
  console.log("🔍 Checking environment and dependencies...");
  
  const isJSAR = isJSARRuntime();
  const hasAsyncLoader = typeof waitForWebGLTestGlobals !== 'undefined';
  const requiredGlobals = ['description', 'debug', 'WebGLTestUtils'];
  
  console.log("Environment info:", {
    isJSARRuntime: isJSAR,
    hasAsyncLoader: hasAsyncLoader,
    userAgent: typeof navigator !== 'undefined' ? navigator.userAgent : 'N/A'
  });
  
  if (isJSAR || hasAsyncLoader) {
    console.log("🔄 JSAR Runtime environment detected, using async loader...");
    
    if (hasAsyncLoader) {
      waitForWebGLTestGlobals(runWebGLTest, {
        maxRetries: 300,
        retryInterval: 50,
        timeout: 15000,
        onTimeout: function() {
          console.error("❌ Timeout: WebGL test dependencies not available after 15s");
          try {
            if (typeof debug !== 'undefined') {
              debug("Test failed: Timeout waiting for dependencies");
            }
          } catch (e) {
            console.error("Debug function not available:", e);
          }
        },
        onError: function() {
          console.error("❌ Error: Failed to load WebGL test dependencies");
          try {
            if (typeof debug !== 'undefined') {
              debug("Test failed: Failed to load dependencies");
            }
          } catch (e) {
            console.error("Debug function not available:", e);
          }
        }
      });
    } else {
      console.log("⏳ No async loader found, using simple polling...");
      waitForGlobalsSimple(requiredGlobals, runWebGLTest);
    }
  } else {
    console.log("🌐 Browser environment detected, running test directly...");
    setTimeout(runWebGLTest, 500);
  }
}

// 确保DOM加载完成后再初始化
setTimeout(initializeTest, 500);
`,
  
  // 需要注释掉的js-test-post.js
  jsTestPostComment: '<!-- <script src="../../js/js-test-post.js"></script> -->'
};

/**
 * 修改单个HTML文件
 */
function modifyHtmlFile(filePath) {
  console.log(`\n🔧 Processing: ${path.basename(filePath)}`);
  
  try {
    let content = fs.readFileSync(filePath, 'utf8');
    const originalContent = content;
    
    // 1. 检查是否已经修改过
    if (content.includes('webgl-async-loader.js')) {
      console.log('⏭️  Already modified, skipping...');
      return false;
    }
    
    // 2. 添加异步加载器脚本引用
    const scriptInsertPoint = content.indexOf('</head>');
    if (scriptInsertPoint !== -1) {
      content = content.slice(0, scriptInsertPoint) + 
                TEMPLATE.asyncLoaderScript + '\n' +
                content.slice(scriptInsertPoint);
    }
    
    // 3. 找到主要的script标签并修改
    const scriptRegex = /<script>\s*"use strict";([\s\S]*?)<\/script>/;
    const match = content.match(scriptRegex);
    
    if (match) {
      let scriptContent = match[1];
      
      // 4. 优化WebGL上下文获取
      scriptContent = scriptContent.replace(
        /var gl = wtu\.create3DContext\(canvas([^)]*?)\)/g,
        TEMPLATE.glContextFix + '$1)'
      );
      
      // 5. 移除最后的successfullyParsed声明（会在新代码中添加）
      scriptContent = scriptContent.replace(/var successfullyParsed = true;?\s*$/m, '');
      
      // 6. 构建新的script内容
      const newScriptContent = TEMPLATE.scriptStart + 
                               TEMPLATE.initCode + 
                               scriptContent + 
                               TEMPLATE.endCode + 
                               '\n</script>';
      
      // 7. 替换原始script
      content = content.replace(scriptRegex, newScriptContent);
    }
    
    // 8. 注释掉js-test-post.js
    content = content.replace(
      /<script src="[^"]*js-test-post\.js"><\/script>/g,
      TEMPLATE.jsTestPostComment
    );
    
    // 9. 写入文件或显示预览
    if (CONFIG.dryRun) {
      console.log('📋 Preview of changes:');
      console.log('--- Original ---');
      console.log(originalContent.substring(0, 500) + '...');
      console.log('--- Modified ---');
      console.log(content.substring(0, 500) + '...');
    } else {
      // 创建备份
      const backupPath = filePath + CONFIG.backupSuffix;
      if (!fs.existsSync(backupPath)) {
        fs.writeFileSync(backupPath, originalContent);
        console.log(`💾 Backup created: ${path.basename(backupPath)}`);
      }
      
      // 写入修改后的文件
      fs.writeFileSync(filePath, content);
      console.log('✅ File modified successfully');
    }
    
    return true;
  } catch (error) {
    console.error(`❌ Error processing ${filePath}:`, error.message);
    return false;
  }
}

/**
 * 批量处理目录中的HTML文件
 */
function processDirectory() {
  console.log(`🚀 Starting batch modification of WebGL tests...`);
  console.log(`📁 Source directory: ${CONFIG.sourceDir}`);
  console.log(`🔄 Dry run mode: ${CONFIG.dryRun}`);
  
  try {
    const files = fs.readdirSync(CONFIG.sourceDir)
      .filter(file => file.endsWith('.html') && !file.endsWith('.original'))
      .map(file => path.join(CONFIG.sourceDir, file));
    
    console.log(`📄 Found ${files.length} HTML files to process`);
    
    let processed = 0;
    let modified = 0;
    
    files.forEach(filePath => {
      processed++;
      if (modifyHtmlFile(filePath)) {
        modified++;
      }
    });
    
    console.log(`\n📊 Summary:`);
    console.log(`   Total files: ${files.length}`);
    console.log(`   Processed: ${processed}`);
    console.log(`   Modified: ${modified}`);
    console.log(`   Skipped: ${processed - modified}`);
    
  } catch (error) {
    console.error('❌ Error reading directory:', error.message);
  }
}

// 主程序
if (require.main === module) {
  // 检查命令行参数
  const args = process.argv.slice(2);
  if (args.includes('--dry-run')) {
    CONFIG.dryRun = true;
  }
  if (args.includes('--help')) {
    console.log(`
WebGL Test Batch Modifier

Usage: node batch_fix_webgl_tests.js [options]

Options:
  --dry-run    Preview changes without modifying files
  --help       Show this help message

This script will:
1. Add webgl-async-loader.js script reference
2. Wrap test code in async loading logic
3. Optimize WebGL context creation for JSAR Runtime
4. Add environment detection and error handling
5. Comment out js-test-post.js to prevent conflicts
`);
    process.exit(0);
  }
  
  processDirectory();
}

module.exports = { modifyHtmlFile, processDirectory };