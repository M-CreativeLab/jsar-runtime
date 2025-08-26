console.log('Script2 start loading');
(function() {
    'use strict';
    
    if (typeof window !== 'undefined' && window.logExecution) {
        window.logExecution('Script2 start loading');
    }
    
    if (typeof window.script1Function !== 'function') {
        console.error('Script2 error: Script1 not loaded'); 
        if (window.logExecution) {
            window.logExecution('Script2 error: Script1 not loaded');   
        }
        return;
    }
    
    const script2Data = {
        name: 'Script2',
        version: '1.0.0',
        loadTime: new Date().toISOString(),
        dependsOn: 'Script1',
        script1Available: typeof window.script1Function === 'function'
    };
    
    window.script2Function = function() {
        if (window.logExecution) {
            window.logExecution('script2Function called');
        }
        
        const result1 = window.script1Function();
        return 'Script2 function executed, ' + result1;
    };
    
    window.script2Data = script2Data;
    
    if (window.scriptLoadedFlags) {
        window.scriptLoadedFlags.script2 = true;
    }
    
    if (window.logExecution) {
        window.logExecution('Script2 loaded');
    }
    
    console.log('Script2 loaded successfully, depends on Script1:', script2Data.script1Available);
})();