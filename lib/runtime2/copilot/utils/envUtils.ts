import { ApiProvider } from "../shared/api"


export const getApiKey = (): string => {
  return process.env.JSAR_SETUP_API_KEY as string;
}
export const getApiProvider = (): ApiProvider => {

  return process.env.JSAR_SETUP_API_PROVIDER as ApiProvider;
}

export const getApiModelId = (): string => {
  return process.env.JSAR_SETUP_API_MODELID as string;
}