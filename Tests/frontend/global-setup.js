// Используем утилиты из общего пакета ImtCore
// TODO: После переноса в ImtCore заменить путь на: '@imtcore/playwright-utils'
const { createGlobalSetup } = require('../../ImtCore-TestUtils/playwright-utils');

// Создаем global setup с настройками по умолчанию
module.exports = createGlobalSetup({
  baseURL: 'http://localhost:7778',
  username: 'su',
  password: '1',
  viewport: { width: 1400, height: 800 },
  storageStatePath: './storageState.json'
});
