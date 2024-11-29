export type Alias = {
  email: string,
  note?: string,
  domains?: string[]
}

export interface MappingService {
  createAlias(email: string): Promise<void>
  getAliases(): Promise<string[]>
  updateAlias(email: string, status: boolean): Promise<void>
  deleteAlias(email: string): Promise<void>
  generateAlias(): Promise<string>
}