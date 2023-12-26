import * as BABYLON from 'babylonjs';

/**
 * This class is an abstract node for all nodes in the scene.
 */
export class TransmuteAbstractNode extends BABYLON.Node {

    /*
     * The Node guid.
     */
    public __vgoGuid: string;

    constructor(name: string, scene: BABYLON.Scene) {
        super(name || 'TransmuteAbstractNode', scene);
        this.__vgoGuid = BABYLON.Tools.RandomId();
    }
}
