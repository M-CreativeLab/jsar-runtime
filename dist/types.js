/**
 * Extension system types inspired by Chrome Extension architecture
 */
export var ExtensionState;
(function (ExtensionState) {
    ExtensionState["LOADING"] = "loading";
    ExtensionState["LOADED"] = "loaded";
    ExtensionState["RUNNING"] = "running";
    ExtensionState["DISABLED"] = "disabled";
    ExtensionState["ERROR"] = "error";
    ExtensionState["UNLOADED"] = "unloaded";
})(ExtensionState || (ExtensionState = {}));
