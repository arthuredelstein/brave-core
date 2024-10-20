import * as React from 'react'
// import '../../../../../../../ui/webui/resources/cr_elements/cr_shared_style.css'
// /Users/arthur/brave-browser/src/chrome/browser/resources/app_settings/app_management_shared_style.css
import './index.css'
import moreVertical from './assets/more-vertical.svg'
import copyIcon from './assets/copy.svg'
import launchIcon from './assets/launch.svg'

export type Alias = {
  email: string,
  note?: string,
  domains?: string[]
}

/*
  *
  Enter your search here
  Only search in this page
  Subtext
  Message goes here
  Autofill and passwords
  Level2
  Inbox Aliases
  Level3
  Level4
  Inbox Aliases
  Keep your personal email address private
  Create unique, random addresses that forward to your Brave account email and can be deleted at any time. Keep your actual email address from being disclosed or used by advertisers. Learn more
  aruiz@brave.com
  Brave account
  Your email aliases
  New alias
  Create up to 10 free email aliases to protect your real email address. You can create unlimited aliases with a Premium account. Get Premium
  horse.radish.record57@bravealias.com
  Alias for all my newsletters
  start.plane.division.laser42@bravealias.com
  Used on x.com
  racoon.pencil.test14@bravealias.com
  Marketplace email for Facebook</p>
}
  */

export const Introduction = ({email} : { email: string }) => (
  <div className='card' id='introduction'>
    <h2>Keep your personal email address private</h2>
    <div className='text'>Create unique, random addresses that forward to your Brave account email and can be deleted at any time. Keep your actual email address from being disclosed or used by advertisers. <a href="https://support.brave.com">Learn more</a></div>
    <div className='account-row row'>
      <div className='main-email-container row'>
        <div className='brave-icon'></div>
        <div className='main-email-text-container col'>
          <div className='main-email'>{email}</div>
          <div className='main-email-description'>Brave Account</div>
        </div>
      </div>
      <div>
        <img src={launchIcon}></img>
      </div>
    </div>
  </div>)


const copyEmailToClipboard = (email: string) => {
  navigator.clipboard.writeText(email)
  console.log(`copied to clipboard: ${email}`)
}

export const AliasItem = ({alias} : {alias: Alias}) => {
  return (
  <div className='alias-item row'>
    <div className='email-container'>
      <div className="alias-item-email clickable"
           onClick={() => copyEmailToClipboard(alias.email)}>
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
      <div className='clickable' onClick={() => copyEmailToClipboard(alias.email)}>
        <img src={copyIcon}></img>
      </div>
      <div className='clickable'>
        <img src={moreVertical}></img>
      </div>
    </div>
  </div>
)}

export const AliasList = ({aliases} : {aliases:Alias[]}) => {
  return (
    <div className='card alias-list col'>
      <div className='alias-list-intro row'>
        <div className='col'>
          <h2>Your email aliases</h2>
          <div className='fine-print'>Create up to 10 free email aliases to protect your real email address. You can create unlimited aliases with a Premium account. <a href="https://premium.brave.com">Get Premium</a></div>
        </div>
        <div className="button-container col">
          <button className='leoButton isFilled' id='add-alias'>New alias</button>
        </div>
      </div>
      {aliases.map(
        alias => <AliasItem alias={alias}></AliasItem>)}
    </div>)
}

export const ManagePage = ({email, aliases} : { email: string, aliases: Alias[]}) => {
  return (
    <div className='col'>
      <h1 className="flex page-title">Inbox Aliases</h1>
      <Introduction email={email}></Introduction>
      <AliasList aliases={aliases}></AliasList>
    </div >
  )
}