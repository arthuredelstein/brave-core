import { Alias, MappingService } from './types'
import { sendWithPromise} from 'chrome://resources/js/cr.js';

export class RemoteMappingService implements MappingService {
  async createAlias (email: string): Promise<void> {
    console.log("createAlias", email)
    sendWithPromise('inbox_aliases.createAlias')
  }
  async getAliases (): Promise<Alias[]> {
    console.log("getAliases")
    return sendWithPromise('inbox_aliases.getAliases')
  }
  async updateAlias (email: string, note:string, status: boolean): Promise<void> {
    console.log("updateAlias", email, status)
    sendWithPromise('inbox_aliases.updateAlias', email, note)
  }
  async deleteAlias (email: string): Promise<void> {
    console.log("deleteAlias", email)
    sendWithPromise('inbox_aliases.deleteAlias')
  }
  generateAlias (): Promise<string> {
    return sendWithPromise('inbox_aliases.generateNewAlias')
  }
}