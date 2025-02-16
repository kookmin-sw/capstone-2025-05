import globals from 'globals';
import pluginJs from '@eslint/js';
import pluginReactConfig from 'eslint-plugin-react/configs/recommended.js';
import airbnbConfig from 'eslint-config-airbnb';
import prettierConfig from 'eslint-config-prettier';
import prettierPlugin from 'eslint-plugin-prettier';

export default [
  {
    files: ['**/*.{js,mjs,cjs,jsx}'],
    languageOptions: {
      parserOptions: {
        ecmaFeatures: {
          jsx: true,
        },
      },
      globals: globals.browser,
    },
    extends: [
      'eslint:recommended',
      'plugin:react/recommended',
      'airbnb',
      'plugin:prettier/recommended',
    ],
    plugins: ['react', 'prettier'],
    rules: {
      'prettier/prettier': 'off',
      'react/react-in-jsx-scope': 'off',
      'react/prefer-stateless-function': 'off',
      'react/jsx-filename-extension': 'off',
      'react/jsx-one-expression-per-line': 'off',
      'no-nested-ternary': 'off',
      'react/prop-types': 'off',
      'import/no-extraneous-dependencies': 'off',
      'arrow-body-style': ['error', 'always'],
      'react/function-component-definition': [
        'off',
        {
          namedComponents: 'arrow-function',
          unnamedComponents: 'arrow-function',
        },
      ],
    },
  },
  pluginJs.configs.recommended,
  pluginReactConfig,
  prettierConfig,
];
