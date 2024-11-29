import { MappingService } from './types'
import { sendWithPromise} from 'chrome://resources/js/cr.js';

export class RemoteMappingService implements MappingService {
  createAlias (email: string): Promise<void> {
    console.log("createAlias", email)
    return Promise.resolve()
  }
  getAliases (): Promise<string[]> {
    console.log("getAliases")
    return Promise.resolve(["abc", "def"])
  }
  updateAlias (email: string, status: boolean): Promise<void> {
    console.log("updateAlias", email, status)
    return Promise.resolve()
  }
  deleteAlias (email: string): Promise<void> {
    console.log("deleteAlias", email)
    return Promise.resolve()
  }
  generateAlias (): Promise<string> {
    return sendWithPromise('inbox_aliases.generateNewAlias')
  }
}