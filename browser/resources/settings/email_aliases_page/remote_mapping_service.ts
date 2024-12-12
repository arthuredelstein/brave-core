import { AccountState, Alias, MappingService } from './types'
import { sendWithPromise} from 'chrome://resources/js/cr.js';

export class RemoteMappingService implements MappingService {
  private pending_cancellation_ = false

  async getAccountEmail (): Promise<string | undefined> {
    throw new Error('Method not implemented.');
  }
  async requestAccount (accountEmail: string): Promise<void> {
    await sendWithPromise('email_aliases.requestAccount', accountEmail)
  }
  async getAccountState (): Promise<AccountState> {
    throw new Error('Method not implemented.');
  }
  async getAliases (): Promise<Alias[]> {
    const result = await sendWithPromise('email_aliases.getAliases')
    return result
  }
  async createAlias (email: string, note: string): Promise<void> {
    console.log("createAlias called")
    await sendWithPromise('email_aliases.createAlias', email, note)
  }
  async updateAlias (email: string, note: string, status: boolean): Promise<void> {
    await sendWithPromise('email_aliases.updateAlias', email, note, status)
  }
  async deleteAlias (email: string): Promise<void> {
    await sendWithPromise('email_aliases.deleteAlias', email)
  }
  /*
  async generateAlias (): Promise<string> {
    return sendWithPromise('email_aliases.generateAlias')
  }*/
  async generateAlias (): Promise<string> {
    return "mock-" + Math.random().toString().slice(2,6) + "@bravealias.com"
  }
  async onAccountReady(): Promise<boolean> {
    while (!this.pending_cancellation_) {
      try {
        await sendWithPromise('email_aliases.getSession')
        return true
      } catch (e) {
        // ignore
      }
    }
    return false
  }
  async cancelAccountRequest(): Promise<void> {
    this.pending_cancellation_ = true
  }
}