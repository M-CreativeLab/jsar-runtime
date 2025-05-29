import { ApiProvider } from '../shared/api';

const envVariables = {
  JSAR_SETUP_THREEPIO_API_KEY: 'JSAR_SETUP_THREEPIO_API_KEY',
  JSAR_SETUP_THREEPIO_API_PROVIDER: 'JSAR_SETUP_THREEPIO_API_PROVIDER',
  JSAR_SETUP_THREEPIO_API_MODELID: 'JSAR_SETUP_THREEPIO_API_MODELID',
  JSAR_SETUP_THREEPIO_SYSTEM_PROMPT: 'JSAR_SETUP_THREEPIO_SYSPROMPT',
  JSAR_SETUP_THREEPIO_BASEURL: 'JSAR_SETUP_THREEPIO_BASEURL',
};

export const getApiKey = (): string => {
  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_KEY] as string;
}
export const getApiProvider = (): ApiProvider => {

  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_PROVIDER] as ApiProvider;
}

export const getApiModelId = (): string => {
  return process.env[envVariables.JSAR_SETUP_THREEPIO_API_MODELID] as string;
}

export const getBaseUrl = (): string => {
  return process.env[envVariables.JSAR_SETUP_THREEPIO_BASEURL] as string;
}
