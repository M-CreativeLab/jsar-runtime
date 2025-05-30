export interface Capability {
  request(input: string): Promise<void>;
}