/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

 import * as React from 'react'
 //import styled from 'styled-components'
// import * as knobs from '@storybook/addon-knobs'
 //import MainPanel from '../components/main-panel'
 import { ManagePage, InboxAliasesManagementState } from '../index'
 
 export default {
   title: 'Inbox Aliases/Main',
 }
 
 /*
 function actionLogger (name: string, ...args: any[]) {
   return (...args: any[]) => console.log(name, ...args)
 }
 
 const style = {
   card: styled.div`
     width: 284px;
     background: #1C1E26B2;
     backdrop-filter: blur(27.5px);
     border-radius: 16px;
     padding: 24px;
   `
 }
*/


const demoData : InboxAliasesManagementState = {
  email: 'aruiz@brave.com',
  aliases: [
    {
      email: 'horse.radish.record57@bravealias.com',
      note: 'Alias for all my newsletters',
    },
    {
      email: 'start.plane.division.laser42@bravealias.com',
      domains: ['x.com'],
      note: 'Twitter account'
    },
    {
      email: 'racoon.pencil.test14@bravealias.com',
      note: 'Marketplace email for Facebook'
    }
  ]
}

 export const Manage = () => {
  return (
    <ManagePage email={demoData.email} aliases={demoData.aliases}></ManagePage>
  )
}
