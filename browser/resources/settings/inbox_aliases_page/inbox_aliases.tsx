import * as React from 'react'
import { render } from 'react-dom'

import {Alias} from './types'
import * as Data from './data'
import Icon from '@brave/leo/react/icon'
import styled, { StyleSheetManager } from 'styled-components'
import { color, /*font, radius,*/ spacing } from '@brave/leo/tokens/css/variables'
import Button from '@brave/leo/react/button'
import Input from '@brave/leo/react/input'
import ButtonMenu from '@brave/leo/react/buttonMenu'
import Tooltip  from '@brave/leo/react/tooltip'

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

const BraveIconCircle = styled.div`
  align-items: center;
  background-position: center;
  background-repeat: no-repeat;
  background-size: 2.5em;
  border-radius: 50%;
  border: #E3E3E8 1px solid;
  display: flex;
  height: 4.5em;
  justify-content: center;
  margin-inline-end: 1.5em;
  width: 4.5em;
`

const BraveIconWrapper = styled.div`
  transform scale(2);
  display: inline-block;
`

const Card = styled.div`
  background-color: ${color.container.background};
  border: none;
  overflow: hidden;
  padding: ${spacing.l} ;
`

const BraveIcon = () => (
  <BraveIconCircle>
    <BraveIconWrapper>
      <Icon name='brave-icon-release-color' />
    </BraveIconWrapper>
  </BraveIconCircle>
)

const Row = styled.div`
  display: flex;
  flex-direction: row;
  align-items: center;
`

const Col = styled.div`
  display: flex;
  flex-direction: column;
`

const AccountRow = styled(Row)`
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 20px 0px 20px;
`

const MainEmailTextContainer = styled(Col)`
  justify-content: center;
  cursor: default;
  user-select: none;
`

const MainEmail = styled.div`
  font-size: 130%;
  font-weight: 600;
  padding-bottom: 6px;
`

const MainEmailDescription = styled.div`
  font-size: 115%;
`

const ManageAccountLink = styled.a`
  display: flex;
  flex-direction: row;
  align-items: center;
  font-size: 120%;
  align-items: center;
  text-decoration: none;
`

const MenuButton = styled(Button)`
  --leo-button-padding: 0;
  flex-grow: 0;
`

const copyTitle = 'Click to copy alias email to clipboard';

const AliasItemRow = styled(Row)`
  margin: 0px;
  font-size: 125%;
  padding: 18px 0px 18px 25px;
  border-top: ${color.legacy.divider1} 1px solid;
  justify-content: space-between;
`

const AliasAnnotation = styled.div`
  font-size: 80%;
  font-weight: 400;
  padding-top: 0.25em;
  color: rgb(80, 80, 80);
`

const AliasControls = styled(Row)`
  height: 1.5em;
  user-select: none;
`

const AliasListIntro = styled(Row)`
  margin-bottom: 20px;
  justify-content: space-between;
`

const Introduction = ({email} : { email: string }) => (
  <Card id='introduction'>
    <h2>Keep your personal email address private</h2>
    <div className='text'>Create unique, random addresses that forward to your Brave account email and can be deleted at any time. Keep your actual email address from being disclosed or used by advertisers. <a href="https://support.brave.com"  target='_blank'>Learn more</a></div>
    <AccountRow>
      <Row>
        <BraveIcon/>
        <MainEmailTextContainer>
          <MainEmail>{email}</MainEmail>
          <MainEmailDescription>Brave Account</MainEmailDescription>
        </MainEmailTextContainer>
      </Row>
      <ManageAccountLink title='Manage Brave account' href='https://account.brave.com' target='_blank'>
        <Icon name="launch"  />
        <span style={{ margin: '0.5em' }}>Manage Brave account</span>
      </ManageAccountLink>
    </AccountRow>
  </Card>
)

const copyEmailToClipboard = (
  email: string
) => {
  navigator.clipboard.writeText(email)
}


const Modal = styled(Col)`
  border-radius: var(--cr-card-border-radius);
  background-color: white;
  z-index: 2;
  border: none;
  opacity: 100%;
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%,-50%);
  width: 45em;
  padding: 1em 2em;
  justify-content: flex-start;
`

const CloseButton = styled.span`
  cursor: pointer;
  position: absolute;
  top: 1.75em;
  right: 1.75em;
`

const EmailContainer = styled.div`
  cursor: pointer;
`

const GeneratedEmailContainer = styled(Row)`
  font-size: 135%;
  background-color: #f4f4f4;
  border-radius: 0.5em;
  padding: 0em 0em 0em 0.75em;
  margin: 0.25em 0em;
  justify-content: space-between;
  height: 2.6em;
`

const ButtonRow = styled(Row)`
  justify-content: end;
  margin: 1em 0em;
`

const ModalSectionCol = styled(Col)`
  justify-content: flex-start;
  margin: 1em 0em;
`

const GrayOverlay = styled.div`
  background-color: rgb(50,50,50,0.5);
  position: fixed;
  z-index: 1;
  left: 0em;
  right: 0em;
  top: 0em;
  bottom: 0em;
`

const AliasMenuItem = ({onClick, iconName, text} :
                       {onClick: EventListener, iconName: string, text: string}) => (
  <leo-menu-item
  onClick={onClick}>
  <Row style={{fontSize: '90%'}}>
    <Icon name={iconName} />
    <span style={{marginInlineStart: '0.5em'}}>{text}</span>
  </Row>
</leo-menu-item>
)

const CopyToast = ({ children }: React.PropsWithChildren) => {
  const [copied, setCopied] = React.useState<boolean>(false)
  return (<div onClick={() => {
    setCopied(true)
    setTimeout(() => setCopied(false), 1000)
  }}>
    <Tooltip text="Copied ✔" mode="mini" visible={copied}>
      {children}
    </Tooltip>
  </div>
  )
}

const AliasItem = ({alias, onEdit, onDelete} : {alias: Alias, onEdit: Function, onDelete: Function}) => {
  return (
    <AliasItemRow>
      <div className='email-container'>
        <CopyToast>
          <EmailContainer title='Click to copy address'
            onClick={(event: React.MouseEvent<HTMLElement>) => copyEmailToClipboard(alias.email)}>
            {alias.email}
          </EmailContainer>
        </CopyToast>
        {((alias.note || alias.domains) &&
          <AliasAnnotation>
            {(alias.note && <span>{alias.note}</span>)}
            {alias.domains && alias.note && <span>. </span>}
            {(alias.domains && <span>Used by {alias.domains?.join(", ")}</span>)}
          </AliasAnnotation>
        )}
      </div>
      <AliasControls>
        <CopyToast>
          <Button size='medium'
            style='width: 1.5em;'
            kind='plain'
            title={copyTitle}
            onClick={() => {
              copyEmailToClipboard(alias.email)
            }}>
            <Icon name="copy" />
          </Button>
        </CopyToast>
        <ButtonMenu>
          <MenuButton slot='anchor-content' kind='plain-faint' size="medium" style='width: 1.5em;'>
            <Icon name="more-vertical" />
          </MenuButton>
          <AliasMenuItem
            iconName="edit-pencil"
            text="Edit"
            onClick={() => onEdit()} />
          <AliasMenuItem
            iconName="trash"
            text="Delete"
            onClick={() => async () => {
              await Data.deleteAliasWithNotes(alias)
              onDelete()
            }} />
        </ButtonMenu>
      </AliasControls>
    </AliasItemRow>
  )
}

const AliasList = ({aliases, onViewChange, onListChange} : {aliases:Alias[], onViewChange:Function, onListChange:Function}) => (
  <Card className='card alias-list col' style={{ borderTop: `1px solid ${color.legacy.divider1}`}}>
    <AliasListIntro>
      <Col>
        <h2>Your email aliases</h2>
        <div className='fine-print'>
          Create up to 10 free email aliases to protect your real email address.
        </div>
      </Col>
    <Button style='flex-grow: 0;'
            title='Create a new alias email'
            id='add-alias'
            onClick={
              async () => {
                onViewChange({mode: ViewMode.Create})
                const newEmailAlias = await Data.generateNewAlias()
                onViewChange({ mode: ViewMode.Create, alias: { email: newEmailAlias }})
              }
            }>
      New alias
    </Button>
    </AliasListIntro>
    {aliases.map(
      alias => <AliasItem alias={alias}
                          onEdit={() => onViewChange({mode: ViewMode.Edit, alias: alias})}
                          onDelete={onListChange}></AliasItem>)}
  </Card>
)


const EmailAliasModal = (
  {returnToMain, viewState, email, onViewChange, onListChange} :
  {returnToMain: any, viewState: ViewState, email: string, onViewChange: Function, onListChange: Function}
) => {
  const mode = viewState.mode
  const noteInputRef = React.useRef<HTMLInputElement>(null)
  const notePlaceholder = mode === ViewMode.Create ?
    'Enter a note for your new address (optional)' :
    'Enter a note for your address (optional)'
  return (<Modal>
    <CloseButton onClick={returnToMain}><Icon name='close' /></CloseButton>
    <h2>{mode == ViewMode.Create ? 'New email alias' : 'Edit email alias'}</h2>
    <ModalSectionCol style={{}}>
      <h3 style={{margin: '0.25em'}}>Email alias</h3>
      <GeneratedEmailContainer>
        <div>{viewState?.alias?.email ?? 'blah'}</div>
        {mode == ViewMode.Create &&
        <Button title='Suggest another email alias'
          onClick= {async () => {
            const newEmailAlias = await Data.generateNewAlias()
            onViewChange({ mode: viewState.mode, alias: { email: newEmailAlias }})
          }}
          kind="plain" style='flex-grow: 0; padding: 0px'>
          <Icon name="refresh" />
        </Button>}
      </GeneratedEmailContainer>
      <div className='fine-print'>{`Emails will be forwarded to ${email}.`}</div>
    </ModalSectionCol>
    <ModalSectionCol>
      <h3 style={{margin: '0.25em'}}>Note</h3>
      <Input id='note-input'
              type='text'
              placeholder={notePlaceholder}
              value={viewState.alias?.note ?? ''}
              ref={noteInputRef}
              style='margin: 0.25em 0em'>
      </Input>
      {mode == ViewMode.Edit && viewState.alias?.domains && <div className='fine-print'>Used by {viewState.alias?.domains?.join(', ')}</div>}
    </ModalSectionCol>
    <ButtonRow>
      <Button onClick={returnToMain} kind='plain' style='flex-grow: 0;'>
        Cancel
      </Button>
      <Button
        style='flex-grow: 0; margin-inline-start: 1em;'
        kind='filled'
        onClick={async () => {
        const aliasEmail = viewState?.alias?.email
        if (aliasEmail) {
          await Data.createAliasWithNotes(
            {email: aliasEmail,
             note: noteInputRef?.current?.value})
          onListChange()
          onViewChange({mode: ViewMode.Main})
        }
      }
      }>{mode == ViewMode.Create ? 'Create' : 'Save'}
      </Button>
    </ButtonRow>
  </Modal>)
}

export const ManagePage = ({email, aliases} : InboxAliasesManagementState) => {
  const [viewState, setViewState] = React.useState<ViewState>({ mode: ViewMode.Main})
  const returnToMain = () => setViewState({ mode: ViewMode.Main})
  const [aliasesState, setAliasesState] = React.useState<Alias[]>(aliases);
  const mode = viewState.mode
  return (
  <div className='app col' style={{ padding: spacing.l }}>
      <Introduction email={email}></Introduction>
      <AliasList aliases={aliasesState} onViewChange={setViewState}
                 onListChange={() => Data.updateAliasList(setAliasesState)}></AliasList>
    {mode == ViewMode.Main ? undefined :
      <GrayOverlay onClick={returnToMain}>&nbsp;</GrayOverlay>}
    {(mode == ViewMode.Create || mode == ViewMode.Edit) &&
     <EmailAliasModal returnToMain={returnToMain} viewState={viewState} email={email}
                      onListChange={() => Data.updateAliasList(setAliasesState)}
                      onViewChange={setViewState}></EmailAliasModal>}
  </div>
)}

export const mount = (at: HTMLElement) => {
  render(
    <StyleSheetManager target={at}>
      <ManagePage email={'arthuredelstein@gmail.com'} aliases={[]} />
    </StyleSheetManager>,
    at
  )
}

;(window as any).mountInboxAliases = mount
