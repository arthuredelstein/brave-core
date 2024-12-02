/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { ManagePage, InboxAliasesManagementState } from '../../../../../../../browser/resources/settings/inbox_aliases_page/inbox_aliases'
import { Alias, ViewMode, MappingService } from '../../../../../../../browser/resources/settings/inbox_aliases_page/types'

export default {
  title: 'Email Aliases',
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

class MockMappingService implements MappingService {
  aliases_ : Map<string, Alias>
  constructor() {
    this.aliases_ = new Map<string, Alias>();
    for (const alias of demoData.aliases) {
      this.aliases_.set(alias.email, alias)
    }
  }
  async createAlias (email: string, note: string): Promise<void> {
    const alias = { email, note }
    this.aliases_.set(email, alias)
  }
  async getAliases (): Promise<Alias[]> {
    return [...this.aliases_.values()]
  }
  async updateAlias (email: string, note: string, status: boolean): Promise<void> {
    const alias = { email, note }
    this.aliases_.set(email, alias)
  }
  async deleteAlias (email: string): Promise<void> {
    console.log("attempting to delete!!!")
    this.aliases_.delete(email)
  }
  async generateAlias (): Promise<string> {
    let generated: string = ''
    do {
      generated = "mock-" + Math.random().toString().slice(2,6) + "@bravealias.com"
    } while (this.aliases_.has(generated))
    return generated
  }
}

export const Manage = () => {
  const [mockMappingService] = React.useState(new MockMappingService());
  return (
    <ManagePage initMode={ViewMode.Main}
                mappingService={mockMappingService}
                email={demoData.email}></ManagePage>
  )
}

export const SignUp = () => {
  const [mockMappingService] = React.useState(new MockMappingService());
  return (
    <ManagePage initMode={ViewMode.SignUp}
                mappingService={mockMappingService}
                email={demoData.email}></ManagePage>
  )
}

export const AwaitAuth = () => {
  const [mockMappingService] = React.useState(new MockMappingService());
  return (
    <ManagePage initMode={ViewMode.AwaitingAuthorization}
                mappingService={mockMappingService}
                email={demoData.email}></ManagePage>
  )
}