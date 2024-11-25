/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { ManagePage, InboxAliasesManagementState } from '../../../../../../../browser/resources/settings/inbox_aliases_page/inbox_aliases'

export default {
  title: 'Inbox Aliases/Main',
}

const demoData: InboxAliasesManagementState = {
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
