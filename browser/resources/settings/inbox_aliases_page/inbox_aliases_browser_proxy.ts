import {Alias} from './types'
//import '//resources/js/cr.js'

/*
import {
  InboxAliasesServiceRemote
} from 'gen/brave/components/inbox_aliases/common/inbox_aliases.mojom.m.js'
*/
/*
try {
  await import('chrome://resources/cr_elements/icons.html.js');
  await import('chrome://resources/js/cr.js');
} catch (e) {
  console.log(e)
}
*/


const ENDPOINT = 'http://localhost:8090';

export const generateNewAlias = async (): Promise<string> => {
  //const remote = new InboxAliasesServiceRemote();
  //const x = await remote.generateNewAlias();//sendWithPromise('email_aliases.generateNewAlias')
  //return x + result.email
  return "asd;flkjasdf"
}

const createAlias = async (alias: string): Promise<void> => {
  const response = await fetch(`${ENDPOINT}/manage`, {
    method: "POST",
    body: JSON.stringify({email: alias})
  })
  if (response.status !== 201) {
    throw new Error(`Create alias failed: ${response.status} ${response.statusText}`)
  }
}

const readAliases = async (): Promise<{ alias_email: string }[]> => {
  const response = await fetch(`${ENDPOINT}/manage`)
  const result = await response.json()
  if (response.status !== 200) {
    throw new Error(`Read aliases failed: ${response.status} ${response.statusText}`)
  }
  console.log("readAliases: ", result)
  return result
}


const deleteAlias = async(alias: Alias) => {
  const response = await fetch(encodeURI(`${ENDPOINT}/manage/${alias.email}`), {
    method: "DELETE",
  })
  if (response.status !== 204) {
    throw new Error(`Delete alias failed: ${response.status} ${response.statusText}`)
  }
}

export const createAliasWithNotes = async (alias: Alias): Promise<void> => {
  localStorage.setItem(alias.email, JSON.stringify(alias))
  await createAlias(alias.email)
}

export const readAliasesWithNotes = async () : Promise<Alias[]> => {
  const results = await readAliases();
  let aliases : Alias[] = []
  for (const alias_email of [...results.map(x => x.alias_email)].sort()) {
    const data =localStorage.getItem(alias_email)
    if (data) {
      const alias: Alias = JSON.parse(data)
      aliases.push(alias)
    }
  }
  return aliases
}

export const deleteAliasWithNotes = async(alias: Alias) => {
  await deleteAlias(alias)
  localStorage.removeItem(alias.email)
}

export const updateAliasList = async (onDataReady: Function) => {
  const aliases = await readAliasesWithNotes();
  onDataReady(aliases)
}