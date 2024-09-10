const path = require('path');
const webpack = require('webpack');
const TerserPlugin = require('terser-webpack-plugin');
const CompressionPlugin = require("compression-webpack-plugin");
const { BundleAnalyzerPlugin } = require('webpack-bundle-analyzer');

const isDevelopment = process.env.NODE_ENV === 'dev';
const outputDir = path.resolve(__dirname, 'build/output');

function getBaseConfig(entry, filename) {
  return {
    target: 'node',
    entry,
    output: {
      filename: `${filename}.js`,
      path: outputDir,
    },
    mode: isDevelopment ? 'development' : 'production',
    devtool: isDevelopment ? 'inline-source-map' : false,
    resolve: {
      extensions: ['.ts', '.js'],
      fallback: {
        canvas: false,
      },
      alias: {
        '@transmute': path.resolve(__dirname, 'lib/bindings'),
      },
    },
    module: {
      rules: [
        {
          test: /\.ts$/,
          use: 'ts-loader',
          exclude: /node_modules/,
        },
      ]
    },
    plugins: [
      new webpack.ProgressPlugin(),
      new webpack.DefinePlugin({
        'process.env.JSAR_VERSION': JSON.stringify(require('./package.json').version),
      }),
      new webpack.optimize.LimitChunkCountPlugin({
        maxChunks: 1,
      }),
      new CompressionPlugin({
        test: /\.js$/,
        algorithm: 'gzip',
      }),
      new BundleAnalyzerPlugin({
        analyzerMode: 'static',
        openAnalyzer: false,
        reportFilename: `${filename}.report.html`,
      }),
    ],
    optimization: {
      minimizer: [
        new TerserPlugin(),
      ],
    },
  };
}

module.exports = [
  {
    ...getBaseConfig('./lib/bootstrap-babylon.ts', 'jsar-bootstrap-babylon'),
  },
  {
    ...getBaseConfig('./lib/main.ts', 'jsar-client-entry'),
  },
];
