import * as React from 'react'

// import '../../../../../../../ui/webui/resources/cr_elements/cr_shared_style.css'
// /Users/arthur/brave-browser/src/chrome/browser/resources/app_settings/app_management_shared_style.css
import './index.css'
import moreVerticalIcon from './assets/more-vertical.svg'
import copyIcon from './assets/copy.svg'
import launchIcon from './assets/launch.svg'
import crossIcon from './assets/cross.svg'
import refreshIcon from './assets/refresh.svg'
import editIcon from './assets/edit.svg'
import trashIcon from './assets/trash.svg'

const ENDPOINT = 'http://localhost:8090';

type Alias = {
  email: string,
  note?: string,
  domains?: string[]
}

export type InboxAliasesManagementState = {
  email: string,
  aliases: Alias[]
}

enum ViewMode {
  Main,
  Create,
  Edit,
  Delete
}

type ViewState = {
  mode: ViewMode,
  alias?: Alias
}

const Introduction = ({email} : { email: string }) => (
  <div className='card' id='introduction'>
    <h2>Keep your personal email address private</h2>
    <div className='text'>Create unique, random addresses that forward to your Brave account email and can be deleted at any time. Keep your actual email address from being disclosed or used by advertisers. <a href="https://support.brave.com"  target='_blank'>Learn more</a></div>
    <div className='account-row row'>
      <div className='main-email-container row'>
        <div className='brave-icon'></div>
        <div className='main-email-text-container col'>
          <div className='main-email'>{email}</div>
          <div className='main-email-description'>Brave Account</div>
        </div>
      </div>
      <a className='manage-account-link row' title='Manage Brave account' href='https://account.brave.com' target='_blank'>
        <img className='manage-account-icon' src={launchIcon}></img> <span>Manage Brave account</span>
      </a>
    </div>
  </div>
)

/*
const CopiedNotification = () => (
  <div>Copied!</div>
)
*/

const copyEmailToClipboard = (
  event: React.MouseEvent<HTMLElement>,
  email: string
) => {
  navigator.clipboard.writeText(email)
  //const copiedNotification = <CopiedNotification></CopiedNotification>
  //if (event.target instanceof Element) {
  //  event.target.after(useRef(copiedNotification).current)
  //}
}

const autoFocus = (element: HTMLElement|null) => element?.focus();

const PopupMenu = ({ onEdit }: { onEdit: Function }) => {
  const [visible, setVisible] = React.useState<boolean>(false)
  return (
    <div tabIndex={-1} ref={autoFocus}
    onBlur={() => setVisible(false)}>
      <div title='More options' className='clickable' onClick={() => setVisible(!visible)}>
        <img src={moreVerticalIcon}></img>
      </div>
      {
        visible &&
        (<div className='option-menu col' >
          <div title='Edit this email alias' className='row clickable option-menu-item'
            onClick={() => {
              setVisible(false)
              onEdit()
            }}>
            <img src={editIcon}></img>
            <div>Edit</div>
          </div>
          <div title='Delete this email alias' className='row clickable option-menu-item'>
            <img src={trashIcon}></img>
            <div>Delete</div>
          </div>
        </div>)
      }
    </div>
  )
}

const copyTitle = 'Click to copy alias email to clipboard';

const AliasItem = ({alias, onEdit} : {alias: Alias, onEdit: Function}) => {
  return (
    <div className='alias-item row'>
      <div className='email-container'>
        <div title={copyTitle} className="alias-item-email clickable"
            onClick={(event: React.MouseEvent<HTMLElement>) => copyEmailToClipboard(event, alias.email)}>
          {alias.email}
        </div>
        {((alias.note || alias.domains) &&
          <div className="alias-annotation">
            {(alias.note && <span>{alias.note}</span>)}
            {alias.domains && alias.note && <span>. </span>}
            {(alias.domains && <span>Used by {alias.domains?.join(", ")}</span>)}
          </div>
        )}
      </div>
      <div className='alias-controls row'>
        <div title={copyTitle}
             className='clickable' onClick={(event: React.MouseEvent<HTMLElement>) => copyEmailToClipboard(event, alias.email)}>
          <img src={copyIcon}></img>
        </div>
        <PopupMenu onEdit={onEdit}></PopupMenu>
      </div>
    </div>
  )
}

const generateNewAlias = async () => {
  const response = await fetch(`${ENDPOINT}/generate`)
  const result = await response.json()
  return result.email
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
  return result
}

/*
const deleteAlias = async(alias: Alias) => {
  const response = await fetch(`${ENDPOINT}/manage/${alias.email}`, {
    method: "DELETE"
  })
  if (response.status !== 204) {
    throw new Error(`Delete alias failed: ${response.status} ${response.statusText}`)
  }
}
*/

const createAliasWithNotes = async (alias: Alias): Promise<void> => {
  localStorage.setItem(alias.email, JSON.stringify(alias))
  await createAlias(alias.email)
}

const readAliasesWithNotes = async () : Promise<Alias[]> => {
  const results = await readAliases();
  let aliases : Alias[] = []
  for (const item of results) {
    const data =localStorage.getItem(item.alias_email)
    if (data) {
      const alias: Alias = JSON.parse(data)
      aliases.push(alias)
    }
  }
  return aliases
}

const updateAliasList = async (onDataReady: Function) => {
  const aliases = await readAliasesWithNotes();
  onDataReady(aliases)
}

const AliasList = ({aliases, onViewChange} : {aliases:Alias[], onViewChange:Function}) => (
  <div className='card alias-list col'>
    <div className='alias-list-intro row'>
      <div className='col'>
        <h2>Your email aliases</h2>
        <div className='fine-print'>
          Create up to 10 free email aliases to protect your real email
          address. You can create unlimited aliases with a Premium
          account. <a href="https://premium.brave.com" target="_blank">Get Premium</a>
        </div>
      </div>
      <div className="button-container col">
        <button title='Create a new alias email' id='add-alias'
                onClick={
                  async () => {
                    onViewChange({mode: ViewMode.Create})
                    const newEmailAlias = await generateNewAlias()
                    onViewChange({ mode: ViewMode.Create, alias: { email: newEmailAlias }})
                  }
                }>
          New alias
        </button>
      </div>
    </div>
    {aliases.map(
      alias => <AliasItem alias={alias} onEdit={() => onViewChange({mode: ViewMode.Edit, alias: alias})}></AliasItem>)}
  </div>
)

const EmailAliasModal = (
  {returnToMain, viewState, email, onViewChange, onListChange} :
  {returnToMain: any, viewState: ViewState, email: string, onViewChange: Function, onListChange: Function}
) => {
  const mode = viewState.mode
  const [startedNote, setStartedNote] = React.useState(mode !== ViewMode.Create)
  const noteInputRef = React.useRef<HTMLInputElement>(null)
  return (<div className='modal col'>
    <img className='close clickable' src={crossIcon} onClick={returnToMain}></img>
    <h2>{mode == ViewMode.Create ? 'New email alias' : 'Edit email alias'}</h2>
    <div>
      <h3>Email alias</h3>
      <div className='generated-email-container row'>
        <div>{viewState?.alias?.email}</div>
        {mode == ViewMode.Create && <img title='Suggest another email alias' className='clickable' src={refreshIcon}
          onClick= {async () => {
            const newEmailAlias = await generateNewAlias()
            onViewChange({ mode: viewState.mode, alias: { email: newEmailAlias }})
          }}></img>}
      </div>
      <div className='fine-print'>{`Emails will be forwarded to ${email}.`}
        {mode == ViewMode.Create && <span> Custom aliases are a premium feature. <a href='https://support.brave.com'>Learn more</a>`</span>}
      </div>
    </div>
    <div className='col'>
      <h3>Note</h3>
      <div className='note-input-container col'>
        {!startedNote && <div className='note-input note-input-modal'>Enter a note for your new address</div>}
        <input className='note-input' id='note-input' type='text' defaultValue={viewState.alias?.note}
               ref={noteInputRef}
               onFocus={() => setStartedNote(true)}>
      </input>
      </div>
      {mode == ViewMode.Edit && viewState.alias?.domains && <div className='fine-print'>Used by {viewState.alias?.domains?.join(', ')}</div>}
    </div>
    <div className='modal-buttons row'>
      <button className='cancel-button' onClick={returnToMain}>Cancel</button>
      <button onClick={async () => {
        const aliasEmail = viewState?.alias?.email
        if (aliasEmail) {
          await createAliasWithNotes(
            {email: aliasEmail,
             note: noteInputRef?.current?.value})
          onListChange()
          onViewChange({mode: ViewMode.Main})
        }
      }
      }>{mode == ViewMode.Create ? 'Create' : 'Save'}</button>
    </div>
  </div>)
}

export const ManagePage = ({email, aliases} : InboxAliasesManagementState) => {
  const [viewState, setViewState] = React.useState<ViewState>({ mode: ViewMode.Main})
  const returnToMain = () => setViewState({ mode: ViewMode.Main})
  const [aliasesState, setAliasesState] = React.useState<Alias[]>(aliases);
  const mode = viewState.mode
  return (
  <div className='app col'>
    <div className='col'>
      <h1 className="flex page-title">Inbox Aliases</h1>
      <Introduction email={email}></Introduction>
      <AliasList aliases={aliasesState} onViewChange={setViewState}></AliasList>
    </div >
    {mode == ViewMode.Main ? undefined :
      <div className='grey-out' onClick={returnToMain}>&nbsp;
    </div>}
    {(mode == ViewMode.Create || mode == ViewMode.Edit) &&
     <EmailAliasModal returnToMain={returnToMain} viewState={viewState} email={email}
                      onListChange={() => updateAliasList(setAliasesState)}
                      onViewChange={setViewState}></EmailAliasModal>}
  </div>
)}