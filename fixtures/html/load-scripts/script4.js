console.log('Script4 start loading');
(function() {
    'use strict';
    
    if (typeof window !== 'undefined' && window.logExecution) {
        window.logExecution('Script4 start executing (defer)');
    }
    
    const domState = document.readyState;
    
    const script4Data = {
        name: 'Script4',
        version: '1.0.0',
        loadTime: new Date().toISOString(),
        loadType: 'defer',
        domState: domState,
        allScriptsAvailable: {
            script1: typeof window.script1Function === 'function',
            script2: typeof window.script2Function === 'function',
            script3: typeof window.script3Function === 'function'
        }
    };
    
    window.script4Function = function() {
        if (window.logExecution) {
            window.logExecution('script4Function called (defer)');
        }
        return 'Script4 defer function executed';
    };
    
    window.script4Data = script4Data;
    
    if (window.scriptLoadedFlags) {
        window.scriptLoadedFlags.script4 = true;
    }
    
    if (window.logExecution) {
        window.logExecution('Script4 loaded (defer)');
    }
    
    if (typeof window.script1Function === 'function' && typeof window.script2Function === 'function') {
        if (window.logExecution) {
            window.logExecution('Script4 confirm: all sync scripts are available');
        }
    } else {
        if (window.logExecution) {
            window.logExecution('Script4 warning: some sync scripts are not available');
        }
    }
    
    console.log('Script4 (defer) loaded successfully. DOM state:', domState);
    console.log('Available scripts:', script4Data.allScriptsAvailable);
    
    const resultElement = document.getElementById('result');
    if (resultElement) {
        if (window.logExecution) {
            window.logExecution('Script4 successfully accessed DOM element');
        }
    }
})();