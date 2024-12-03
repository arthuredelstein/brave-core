import * as React from 'react'
import { render } from 'react-dom'
import Button from '@brave/leo/react/button'
import { color, spacing } from '@brave/leo/tokens/css/variables'
import { Alias, MappingService, ViewMode } from './types'
import Icon from '@brave/leo/react/icon'
import { StyleSheetManager } from 'styled-components'
import Input from '@brave/leo/react/input'
import ButtonMenu from '@brave/leo/react/buttonMenu'
import Tooltip from '@brave/leo/react/tooltip'
import {
  AccountRow,
  AliasAnnotation,
  AliasControls,
  AliasItemRow,
  AliasListIntro,
  BraveIconCircle,
  BraveIconWrapper,
  ButtonRow,
  Card,
  CloseButton,
  Col,
  EmailContainer,
  GeneratedEmailContainer,
  GrayOverlay,
  MainEmail,
  MainEmailDescription,
  MainEmailTextContainer,
  ManageAccountLink,
  MenuButton,
  Modal,
  ModalSectionCol,
  Row,
  SignupRow,
} from './styles'

export const copyTitle = 'Click to copy alias email to clipboard';

export type InboxAliasesManagementState = {
  email: string,
  aliases: Alias[]
}

type ViewState = {
  mode: ViewMode,
  alias?: Alias
}

const BraveIcon = ({style}: {style?: React.CSSProperties | undefined}) => (
  <BraveIconCircle style={{...style, flexGrow: 0}}>
    <BraveIconWrapper>
      <Icon name='brave-icon-release-color' />
    </BraveIconWrapper>
  </BraveIconCircle>
)

const Introduction = () => (
  <Card id='introduction'>
    <h2>Keep your personal email address private</h2>
    <div>Create unique, random addresses that forward to your Brave account email and can be deleted at any time. Keep your actual email address from being disclosed or used by advertisers. <a href="https://support.brave.com" target='_blank'>Learn more</a></div>
  </Card>
)

const MainEmailDisplay = ({ email }: { email: string }) => (
  <Card id='main-email-display'>
    <AccountRow>
    <Row>
      <BraveIcon />
      <MainEmailTextContainer>
        <MainEmail>{email}</MainEmail>
        <MainEmailDescription>Brave Account</MainEmailDescription>
      </MainEmailTextContainer>
    </Row>
    <ManageAccountLink title='Manage Brave account' href='https://account.brave.com' target='_blank'>
      <Icon name="launch" />
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

const AliasMenuItem = ({ onClick, iconName, text }:
  { onClick: EventListener, iconName: string, text: string }) => (
  <leo-menu-item
    onClick={onClick}>
    <Row style={{ fontSize: '90%' }}>
      <Icon name={iconName} />
      <span style={{ marginInlineStart: '0.5em' }}>{text}</span>
    </Row>
  </leo-menu-item>
)

const CopyToast = ({ children }: React.PropsWithChildren) => {
  const [copied, setCopied] = React.useState<boolean>(false)
  return (<div onClick={() => {
    setCopied(true)
    setTimeout(() => setCopied(false), 1000)
  }}>
    <Tooltip text="✔ Copied to clipboard" mode="mini" visible={copied}>
      {children}
    </Tooltip>
  </div>
  )
}

const AliasItem = ({ alias, onEdit, onDelete }: { alias: Alias, onEdit: Function, onDelete: Function }) => {
  return (
    <AliasItemRow>
      <Col>
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
      </Col>
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
            onClick={() => onDelete(alias)} />
        </ButtonMenu>
      </AliasControls>
    </AliasItemRow>
  )
}

const AliasList = ({ aliases, onViewChange, onListChange, mappingService }: { mappingService: MappingService, aliases: Alias[], onViewChange: Function, onListChange: Function }) => (
  <Card style={{ borderTop: `1px solid ${color.legacy.divider1}` }}>
    <AliasListIntro>
      <Col>
        <h2>Your email aliases</h2>
        <div>
          Create up to 10 free email aliases to protect your real email address.
        </div>
      </Col>
      <Button style='flex-grow: 0;'
        title='Create a new alias email'
        id='add-alias'
        onClick={
          async () => {
            onViewChange({ mode: ViewMode.Create })
            const newEmailAlias = await mappingService.generateAlias()
            onViewChange({ mode: ViewMode.Create, alias: { email: newEmailAlias } })
          }
        }>
        New alias
      </Button>
    </AliasListIntro>
    {aliases.map(
      alias => <AliasItem alias={alias}
        onEdit={() => onViewChange({ mode: ViewMode.Edit, alias: alias })}
        onDelete={async (alias: Alias) => {
          await mappingService.deleteAlias(alias.email)
          onListChange()
        }}></AliasItem>)}
  </Card>
)


const RefreshButton = ( {mode, onViewChange, mappingService} : { mappingService: MappingService, mode: ViewMode, onViewChange: Function }) => (
  <Button title='Suggest another email alias'
    onClick={async () => {
      const newEmailAlias = await mappingService.generateAlias()
      onViewChange({ mode, alias: { email: newEmailAlias } })
    }}
    kind="plain" style='flex-grow: 0; padding: 0px'>
    <Icon name="refresh" />
  </Button>
)

const EmailAliasModal = (
  { returnToMain, viewState, email, onViewChange, onListChange, mappingService }:
    { returnToMain: any, viewState: ViewState, email: string, onViewChange: Function, onListChange: Function, mappingService: MappingService }
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
      <h3 style={{ margin: '0.25em' }}>Email alias</h3>
      <GeneratedEmailContainer>
        <div>{viewState?.alias?.email ?? 'blah'}</div>
        {mode == ViewMode.Create && <RefreshButton {...{mappingService, mode, onViewChange}} />}
      </GeneratedEmailContainer>
      <div>{`Emails will be forwarded to ${email}.`}</div>
    </ModalSectionCol>
    <ModalSectionCol>
      <h3 style={{ margin: '0.25em' }}>Note</h3>
      <Input id='note-input'
        type='text'
        placeholder={notePlaceholder}
        value={viewState.alias?.note ?? ''}
        ref={noteInputRef}
        style='margin: 0.25em 0em'>
      </Input>
      {mode == ViewMode.Edit && viewState.alias?.domains && <div>Used by {viewState.alias?.domains?.join(', ')}</div>}
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
            await mappingService.updateAlias(aliasEmail, noteInputRef?.current?.value ?? "", true)
            onListChange()
            onViewChange({ mode: ViewMode.Main })
          }
        }
        }>{mode == ViewMode.Create ? 'Create' : 'Save'}
      </Button>
    </ButtonRow>
  </Modal>)
}

const BeforeSendingEmailForm = () => (
  <Col>
    <h3>To get started, sign in or create a Brave account</h3>
    <div style={{ marginBottom: '1em' }}>Enter your email address to get a secure login link sent to your email. Clicking this link will either create or access a Brave Account and let you use the free Email Aliases service.</div>
    <Row>
      <Input style='flex-grow: 4; margin-inline-end: 1em;' type='text' placeholder='Email address'></Input>
      <Button style='flex-grow: 1' kind='filled'>Get login link</Button>
    </Row>
  </Col>
)


const AfterSendingEmailMessage = () => (
  <Col style={{flexGrow: 1}}>
    <h3>A login email is on the way</h3>
    <div style={{ marginBottom: '1em' }}>Click on the secure login link in the email to access your account.</div>
    <div style={{ marginBottom: '1em' }}>Don't see the email? Check your spam folder or try again.</div>
  </Col>
)

const MainEmailEntryForm = ({viewState} : {viewState:ViewState}) => (
  <Card id='main-email-entry-form'>
    <SignupRow>
      <BraveIcon style={{marginTop: '1em'}}/>
      {viewState.mode === ViewMode.SignUp ? (<BeforeSendingEmailForm/>) : (<AfterSendingEmailMessage/>)}
    </SignupRow>
  </Card>
)

export const ManagePage = ({ email, mappingService, initMode }:
  { email: string,
    mappingService: MappingService,
    initMode: ViewMode }) => {
  const [viewState, setViewState] = React.useState<ViewState>({ mode: initMode })
  const returnToMain = () => setViewState({ mode: ViewMode.Main })
  const mode = viewState.mode
  const [aliasesState, setAliasesState] = React.useState<Alias[]>([]);
  const onListChange = async () => {
    console.log("onListChange")
    const aliases = await mappingService.getAliases()
    setAliasesState(aliases)
  }
  React.useEffect(() => {
    onListChange();
  }, [] /* Only run at mount. */)
  return (
    <Col style={{ padding: spacing.l }}>
      <Introduction />
      {viewState.mode === ViewMode.SignUp || viewState.mode === ViewMode.AwaitingAuthorization ?
      (<MainEmailEntryForm viewState={viewState} />) :
      (<span><MainEmailDisplay email={email} />
      <AliasList aliases={aliasesState} onViewChange={setViewState}
        mappingService={mappingService}
        onListChange={onListChange}></AliasList></span>)}
      {(mode == ViewMode.Create || mode == ViewMode.Edit) &&
        (<span><GrayOverlay onClick={returnToMain}>&nbsp;</GrayOverlay>
        <EmailAliasModal returnToMain={returnToMain} viewState={viewState} email={email}
          onListChange={onListChange}
          mappingService={mappingService}
          onViewChange={setViewState}></EmailAliasModal></span>)}
    </Col>
  )
}

export const mount = (at: HTMLElement, mappingService: MappingService) => {
  render(
    <StyleSheetManager target={at}>
      <ManagePage initMode={ViewMode.Main}
                  email={'arthuredelstein@gmail.com'}
                  {...{mappingService}}/>
    </StyleSheetManager>,
    at
  )
}

  ; (window as any).mountInboxAliases = mount
