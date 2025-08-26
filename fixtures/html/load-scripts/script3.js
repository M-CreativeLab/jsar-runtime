console.log('Script3 start loading');
(function() {
    'use strict';
    
    if (typeof window !== 'undefined' && window.logExecution) {
        window.logExecution('Script3 start loading (async)');
    }
    
    const script3Data = {
        name: 'Script3',
        version: '1.0.0',
        loadTime: new Date().toISOString(),
        loadType: 'async',
        independent: true
    };
    
    window.script3Function = function() {
        if (window.logExecution) {
            window.logExecution('script3Function called (async)');
        }
        return 'Script3 async function executed';
    };
    
    window.script3Data = script3Data;
    
    if (window.scriptLoadedFlags) {
        window.scriptLoadedFlags.script3 = true;
    }
    
    if (window.logExecution) {
        window.logExecution('Script3 loaded (async)');
    }
    
    const otherScriptsStatus = {
        script1: typeof window.script1Function === 'function',
        script2: typeof window.script2Function === 'function'
    };
    
    console.log('Script3 (async) loaded successfully. Other scripts status:', otherScriptsStatus);
    
    if (document.readyState === 'complete') {
        if (window.logExecution) {
            window.logExecution('Script3 async loaded, page ready');
        }
    }
})();