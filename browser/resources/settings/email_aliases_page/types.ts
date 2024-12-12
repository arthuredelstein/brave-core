export type Alias = {
  email: string,
  note?: string,
  domains?: string[]
}

export interface MappingService {
  createAlias(email: string, note: string): Promise<void>
  getAliases(): Promise<Alias[]>
  updateAlias(email: string, note: string, status: boolean): Promise<void>
  deleteAlias(email: string): Promise<void>
  generateAlias(): Promise<string>
  getAccountEmail (): Promise<string | undefined>
  requestAccount (accountEmail: string): Promise<void>
  getAccountState (): Promise<AccountState>
  onAccountReady (): Promise<boolean>
  cancelAccountRequest (): Promise<void>
  logout (): Promise<void>
}

export enum ViewMode {
  Startup,
  Main,
  Create,
  Edit,
  Delete,
  SignUp,
  AwaitingAuthorization
}

export enum AccountState {
  NoAccount,
  AwaitingAccount,
  AccountReady
}