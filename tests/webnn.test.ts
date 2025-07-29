/**
 * Basic test for WebNN API functionality
 */
import { ml, MLOperandType, MLDeviceType } from '../lib/webnn';

describe('WebNN API', () => {
  test('should create ML context', async () => {
    const context = await ml.createContext({
      deviceType: MLDeviceType.CPU
    });
    
    expect(context).toBeDefined();
    expect(context.deviceType).toBe(MLDeviceType.CPU);
  });

  test('should create graph builder', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    expect(builder).toBeDefined();
  });

  test('should create basic operands', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const inputA = builder.input('a', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });
    
    const inputB = builder.input('b', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });

    expect(inputA.type).toBe(MLOperandType.FLOAT32);
    expect(inputA.dimensions).toEqual([2, 3]);
    expect(inputB.type).toBe(MLOperandType.FLOAT32);
    expect(inputB.dimensions).toEqual([2, 3]);
  });

  test('should perform basic operations', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const inputA = builder.input('a', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });
    
    const inputB = builder.input('b', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });

    // Test add operation
    const addResult = builder.add(inputA, inputB);
    expect(addResult.type).toBe(MLOperandType.FLOAT32);
    expect(addResult.dimensions).toEqual([2, 3]);

    // Test ReLU operation
    const reluResult = builder.relu(addResult);
    expect(reluResult.type).toBe(MLOperandType.FLOAT32);
    expect(reluResult.dimensions).toEqual([2, 3]);
  });

  test('should perform matrix multiplication', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const matrixA = builder.input('a', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });
    
    const matrixB = builder.input('b', {
      type: MLOperandType.FLOAT32,
      dimensions: [3, 4]
    });

    const result = builder.matmul(matrixA, matrixB);
    expect(result.type).toBe(MLOperandType.FLOAT32);
    expect(result.dimensions).toEqual([2, 4]);
  });

  test('should handle matrix multiplication dimension errors', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const matrixA = builder.input('a', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3]
    });
    
    const matrixB = builder.input('b', {
      type: MLOperandType.FLOAT32,
      dimensions: [4, 5] // Incompatible dimensions
    });

    expect(() => builder.matmul(matrixA, matrixB)).toThrow('MatMul dimension mismatch');
  });

  test('should build and execute graph', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const input = builder.input('input', {
      type: MLOperandType.FLOAT32,
      dimensions: [1, 4]
    });
    
    const output = builder.relu(input);
    
    const graph = await builder.build({ output });
    expect(graph).toBeDefined();

    // Test computation (with mock data)
    const inputs = {
      input: new Float32Array([1, -2, 3, -4])
    };
    const outputs = {
      output: new Float32Array(4)
    };

    const result = await graph.compute(inputs, outputs);
    expect(result).toBeDefined();
    expect(result.outputs).toBeDefined();
  });
});