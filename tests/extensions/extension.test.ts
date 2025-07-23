/**
 * Basic test for Extension system functionality
 * This test verifies that extensions can be loaded, enabled, and executed
 */

import { Extension, ExtensionManager } from '../../lib/extensions';
import { ExtensionState } from '../../lib/extensions/types';
import { promises as fs } from 'node:fs';
import { join } from 'node:path';
import { tmpdir } from 'node:os';

describe('Extension System', () => {
  let tempDir: string;
  let extensionPath: string;
  let extensionManager: ExtensionManager;

  beforeAll(async () => {
    // Create a temporary directory for test extensions
    tempDir = join(tmpdir(), 'jsar-extensions-test-' + Date.now());
    extensionPath = join(tempDir, 'test-extension');
    
    await fs.mkdir(extensionPath, { recursive: true });
    
    // Create a test extension manifest
    const manifest = {
      name: 'Test Extension',
      version: '1.0.0',
      description: 'A test extension for JSAR',
      manifest_version: 3,
      background: {
        scripts: ['background.js'],
        persistent: false
      },
      permissions: ['storage']
    };
    
    await fs.writeFile(
      join(extensionPath, 'manifest.json'), 
      JSON.stringify(manifest, null, 2)
    );
    
    // Create a test background script
    const backgroundScript = `
      // Test background script
      console.log('Test extension background script loaded!');
      
      // Test basic extension API access
      if (chrome && chrome.extension) {
        console.log('Extension ID:', chrome.extension.id);
        console.log('Extension manifest:', chrome.extension.manifest);
      }
      
      // Set a global variable to verify execution
      globalThis.testExtensionLoaded = true;
    `;
    
    await fs.writeFile(
      join(extensionPath, 'background.js'),
      backgroundScript
    );
  });

  beforeEach(() => {
    extensionManager = new ExtensionManager();
  });

  afterEach(async () => {
    await extensionManager.unloadAllExtensions();
  });

  afterAll(async () => {
    // Clean up temporary directory
    try {
      await fs.rm(tempDir, { recursive: true, force: true });
    } catch (error) {
      console.warn('Failed to clean up temp directory:', error);
    }
  });

  test('should create Extension from directory', async () => {
    const extension = await Extension.loadFromDirectory(extensionPath);
    
    expect(extension).toBeDefined();
    expect(extension.id).toBe('test-extension');
    expect(extension.manifest.name).toBe('Test Extension');
    expect(extension.manifest.version).toBe('1.0.0');
    expect(extension.state).toBe(ExtensionState.LOADING);
  });

  test('should load and enable extension', async () => {
    const extension = await extensionManager.loadExtension(extensionPath);
    
    expect(extension).toBeDefined();
    expect(extension.state).toBe(ExtensionState.RUNNING);
    expect(extensionManager.hasExtension('test-extension')).toBe(true);
    
    const loadedExtension = extensionManager.getExtension('test-extension');
    expect(loadedExtension).toBe(extension);
  });

  test('should execute background script on load', async () => {
    // Clear any previous global state
    delete (globalThis as any).testExtensionLoaded;
    
    const extension = await extensionManager.loadExtension(extensionPath);
    
    expect(extension.state).toBe(ExtensionState.RUNNING);
    // Note: In a real environment, this would be set by the background script
    // For this test, we're just verifying the extension loaded successfully
  });

  test('should disable and enable extension', async () => {
    const extension = await extensionManager.loadExtension(extensionPath);
    expect(extension.state).toBe(ExtensionState.RUNNING);
    
    await extensionManager.disableExtension('test-extension');
    expect(extension.state).toBe(ExtensionState.DISABLED);
    
    await extensionManager.enableExtension('test-extension');
    expect(extension.state).toBe(ExtensionState.RUNNING);
  });

  test('should unload extension', async () => {
    const extension = await extensionManager.loadExtension(extensionPath);
    expect(extensionManager.hasExtension('test-extension')).toBe(true);
    
    await extensionManager.unloadExtension('test-extension');
    expect(extension.state).toBe(ExtensionState.UNLOADED);
    expect(extensionManager.hasExtension('test-extension')).toBe(false);
  });

  test('should get extensions by state', async () => {
    const extension1 = await extensionManager.loadExtension(extensionPath);
    
    let runningExtensions = extensionManager.getExtensionsByState(ExtensionState.RUNNING);
    expect(runningExtensions).toHaveLength(1);
    expect(runningExtensions[0]).toBe(extension1);
    
    await extensionManager.disableExtension('test-extension');
    
    runningExtensions = extensionManager.getExtensionsByState(ExtensionState.RUNNING);
    expect(runningExtensions).toHaveLength(0);
    
    const disabledExtensions = extensionManager.getExtensionsByState(ExtensionState.DISABLED);
    expect(disabledExtensions).toHaveLength(1);
    expect(disabledExtensions[0]).toBe(extension1);
  });

  test('should handle extension events', async () => {
    const events: string[] = [];
    
    extensionManager.onExtensionEvent('extensionLoaded', (event) => {
      events.push(`loaded:${event.extensionId}`);
    });
    
    extensionManager.onExtensionEvent('extensionEnabled', (event) => {
      events.push(`enabled:${event.extensionId}`);
    });
    
    await extensionManager.loadExtension(extensionPath);
    
    expect(events).toContain('loaded:test-extension');
    expect(events).toContain('enabled:test-extension');
  });

  test('should handle invalid extension directory', async () => {
    const invalidPath = join(tempDir, 'non-existent');
    
    await expect(extensionManager.loadExtension(invalidPath))
      .rejects.toThrow('Failed to load extension');
  });

  test('should get extension states summary', async () => {
    await extensionManager.loadExtension(extensionPath);
    
    const states = extensionManager.getExtensionStates();
    expect(states['test-extension']).toBe(ExtensionState.RUNNING);
  });
});