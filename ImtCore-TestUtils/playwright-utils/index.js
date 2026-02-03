// Main entry point for @imtcore/playwright-utils
const utils = require('./utils');
const globalSetup = require('./global-setup');

module.exports = {
  ...utils,
  createGlobalSetup: globalSetup
};
