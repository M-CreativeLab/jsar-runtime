console.log('Script1 start loading');
(function() {
    'use strict';
    
    if (typeof window !== 'undefined' && window.logExecution) {
        window.logExecution('Script1 start loading');
    }
    
    const script1Data = {
        name: 'Script1',
        version: '1.0.0',
        loadTime: new Date().toISOString()
    };
    
    window.script1Function = function() {
        if (window.logExecution) {
            window.logExecution('script1Function called');
        }
        return 'Script1 function executed';
    };
    
    window.script1Data = script1Data;
    
    if (window.scriptLoadedFlags) {
        window.scriptLoadedFlags.script1 = true;
    }
    
    if (window.logExecution) {
        window.logExecution('Script1 loaded');
    }
    
    console.log('Script1 loaded successfully');
})();