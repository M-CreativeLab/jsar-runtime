import { ApiProvider } from '../shared/api'

const envVariables = {
  JSAR_SETUP_THREEPIO_API_KEY: 'JSAR_SETUP_THREEPIO_API_KEY',
  JSAR_SETUP_THREEPIO_API_PROVIDER: 'JSAR_SETUP_THREEPIO_API_PROVIDER',
  JSAR_SETUP_THREEPIO_API_MODELID: 'JSAR_SETUP_THREEPIO_API_MODELID',
  JSAR_SETUP_THREEPIO_SYSTEM_PROMPT: 'JSAR_SETUP_THREEPIO_SYSPROMPT',
}



export const getApiKey = (): string => {
  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_KEY] as string;
}
export const getApiProvider = (): ApiProvider => {

  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_PROVIDER] as ApiProvider;
}

export const getApiModelId = (): string => {
  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_MODELID] as string;
}

console.log('getApiKey', getApiKey(), 'getApiProvider', getApiProvider(), 'getApiModelId', getApiModelId());