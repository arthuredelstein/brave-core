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
      <a title='Go to Brave Accounts' href='https://account.brave.com' target='_blank'>
        <img src={launchIcon}></img>
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
  console.log(`copied to clipboard: ${event.target} ${email}`)
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
        <button title='Create a new alias email' id='add-alias' onClick={() => onViewChange(ViewMode.Create)}>New alias</button>
      </div>
    </div>
    {aliases.map(
      alias => <AliasItem alias={alias} onEdit={() => onViewChange(ViewMode.Edit)}></AliasItem>)}
  </div>
)

const EmailAliasOverlay = ({returnToMain, viewState} : {returnToMain: any, viewState: ViewMode}) => {
  const [startedNote, setStartedNote] = React.useState(viewState !== ViewMode.Create)
  return (<div className='overlay col'>
    <img className='close clickable' src={crossIcon} onClick={returnToMain}></img>
    <h2>{viewState == ViewMode.Create ? 'New email alias' : 'Edit email alias'}</h2>
    <div>
      <h3>Email alias</h3>
      <div className='generated-email-container row'>
        <div>coolnews.airplane.potato57@bravealias.com</div>
        {viewState == ViewMode.Create && <img title='Suggest another email alias' className='clickable' src={refreshIcon}></img>}
      </div>
      <div className='fine-print'>Emails will be forwarded to aruiz@brave.com.
        {viewState == ViewMode.Create && <span>Custom aliases are a premium feature. <a href='https://support.brave.com'>Learn more</a>`</span>}
      </div>
    </div>
    <div className='col'>
      <h3>Note</h3>
      <div className='note-input-container col'>
        {!startedNote && <div className='note-input note-input-overlay'>Enter a note for your new address</div>}
        <input className='note-input' type='text'
              onFocus={() => setStartedNote(true)}>
      </input>
      </div>
      {viewState == ViewMode.Edit && <div className='fine-print'>Used by bbcnews.com, nytimes.com</div>}
    </div>
    <div className='overlay-buttons row'>
      <button className='cancel-button' onClick={returnToMain}>Cancel</button>
      <button>{viewState == ViewMode.Create ? 'Create' : 'Save'}</button>
    </div>
  </div>)
}

export const ManagePage = ({email, aliases} : InboxAliasesManagementState) => {
  const [viewState, setViewState] = React.useState<ViewMode>(ViewMode.Main)
  const returnToMain = () => setViewState(ViewMode.Main)
  return (
  <div className='app col'>
    <div className='col'>
      <h1 className="flex page-title">Inbox Aliases</h1>
      <Introduction email={email}></Introduction>
      <AliasList aliases={aliases} onViewChange={setViewState}></AliasList>
    </div >
    {viewState == ViewMode.Main ? undefined :
      <div className='grey-out' onClick={returnToMain}>&nbsp;
    </div>}
    {(viewState == ViewMode.Create || viewState == ViewMode.Edit) &&
     <EmailAliasOverlay returnToMain={returnToMain} viewState={viewState}></EmailAliasOverlay>}
  </div>
)}