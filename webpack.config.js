const path = require('path');
const webpack = require("webpack");
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const OptimizeCSSAssetsPlugin = require("optimize-css-assets-webpack-plugin");

module.exports = {
    mode: 'development',
    devtool: 'source-map',
    entry: './js/index.js',
    output: {
        filename: 'bundle.js',
        path: path.resolve(__dirname, 'dist'),
    },
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    {
                        loader: 'css-loader',
                        options: {
                            sourceMap: true
                        }
                    }
                ]
            },
            {
                test: /\.scss$/i,
                use: [
                    // fallback to style-loader in development
                    {
                        // Creates `style` nodes from JS strings
                        loader: 'style-loader',
                    },
                    MiniCssExtractPlugin.loader,
                    {
                        loader: 'css-loader',
                        options: {
                            sourceMap: true
                        }
                    },
                    {
                        loader: 'sass-loader',
                        options: {
                            sourceMap: true,
                        },
                    },
                ],
            },
            {
                test: /fa-.*\.(woff2|woff|eot|svg|ttf)$/,
                use: [{
                    loader: 'url-loader',
                    options: {
                        name: '[name].[ext]',
                        outputPath: 'fonts/'
                    }
                }]
            }
        ],
    },
    plugins: [
        new webpack.ProvidePlugin({
            $: "jquery",
            jQuery: "jquery"
        }),
        new webpack.ProvidePlugin({
            CodeMirror: "codemirror"
        }),
        new webpack.SourceMapDevToolPlugin({
            filename: '[name].js.map',
            exclude: ['vendor.js']
        }),
        new MiniCssExtractPlugin({
            // Options similar to the same options in webpackOptions.output
            // both options are optional
            filename: '[name].css',
            chunkFilename: '[id].css'
        }),
        new OptimizeCSSAssetsPlugin({
            cssProcessorOptions: {
                map: {
                    inline: true
                }
            }
        }),
    ]
};