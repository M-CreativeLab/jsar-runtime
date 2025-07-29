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

  test('should handle advanced operations', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const input = builder.input('input', {
      type: MLOperandType.FLOAT32,
      dimensions: [1, 10, 5, 5]
    });

    // Test pooling
    const avgPool = builder.averagePool2d(input, {
      windowDimensions: [2, 2],
      strides: [2, 2]
    });
    expect(avgPool.dimensions).toEqual([1, 10, 5, 5]);

    // Test global pooling
    const globalPool = builder.globalAveragePool2d(input);
    expect(globalPool.dimensions).toEqual([1, 10, 1, 1]);

    // Test softmax
    const softmax = builder.softmax(input);
    expect(softmax.dimensions).toEqual([1, 10, 5, 5]);

    // Test reduction
    const reduced = builder.reduceMean(input, { axes: [2, 3], keepDimensions: true });
    expect(reduced.dimensions).toEqual([1, 10, 1, 1]);
  });

  test('should handle tensor shape operations', async () => {
    const context = await ml.createContext();
    const builder = context.createGraphBuilder();
    
    const input = builder.input('input', {
      type: MLOperandType.FLOAT32,
      dimensions: [2, 3, 4]
    });

    // Test reshape
    const reshaped = builder.reshape(input, [6, 4]);
    expect(reshaped.dimensions).toEqual([6, 4]);

    // Test transpose
    const transposed = builder.transpose(input, { permutation: [2, 0, 1] });
    expect(transposed.dimensions).toEqual([4, 2, 3]);

    // Test element-wise operations
    const sqrt = builder.sqrt(input);
    expect(sqrt.dimensions).toEqual([2, 3, 4]);
  });
});