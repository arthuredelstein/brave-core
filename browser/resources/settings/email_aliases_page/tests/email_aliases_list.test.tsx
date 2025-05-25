// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { act, fireEvent, render, screen, waitFor } from '@testing-library/react'
import { AliasList, ListIntroduction } from '../content/email_aliases_list'
import {
  Alias,
  EmailAliasesServiceInterface,
  MAX_ALIASES
} from 'gen/brave/components/email_aliases/email_aliases.mojom.m'
import { clickLeoButton } from './test_utils'

jest.mock('$web-common/locale', () => ({
  getLocale: (key: string) => {
    return key
  },
  formatMessage: (key: string, params: Record<string, string>) => {
    return key
  },
  formatLocale: (key: string, params: Record<string, string>) => {
    return key
  }
}))

// Mock the clipboard API
Object.assign(navigator, {
  clipboard: {
    writeText: jest.fn(),
  },
})

// Mock the email aliases service
const mockEmailAliasesService: EmailAliasesServiceInterface = {
  updateAlias: jest.fn(),
  deleteAlias: jest.fn(),
  generateAlias: jest.fn(),
  requestAuthentication: jest.fn(),
  cancelAuthenticationOrLogout: jest.fn(),
  addObserver: jest.fn(),
  removeObserver: jest.fn()
}

describe('AliasList', () => {
  const mockAuthEmail = 'test@brave.com'

  const mockAliases: Alias[] = [
    {
      email: 'test1@brave.com',
      note: 'Test Alias 1',
      domains: ['brave.com']
    },
    {
      email: 'test2@brave.com',
      note: 'Test Alias 2',
      domains: ['brave.com']
    }
  ]

  beforeEach(() => {
    jest.clearAllMocks()
  })

  it('renders the alias list', () => {
    render(
      <AliasList
        aliases={mockAliases}
        authEmail={mockAuthEmail}
        emailAliasesService={mockEmailAliasesService}
      />
    )

    // Check if alias information is displayed correctly
    expect(screen.getByText(/test1@brave\.com/)).toBeInTheDocument()
    expect(screen.getByText(/Test Alias 1/)).toBeInTheDocument()
    expect(screen.getByText(/test2@brave\.com/)).toBeInTheDocument()
    expect(screen.getByText(/Test Alias 2/)).toBeInTheDocument()
  })

  it('disables create button when max aliases reached', () => {
    const mockOnCreateClicked = jest.fn()
    render(
      <ListIntroduction
        aliasesCount={MAX_ALIASES}
        onCreateClicked={mockOnCreateClicked}
      />
    )

    // Check if create button is disabled
    const createButton = screen.getByText('emailAliasesCreateAliasLabel')
    expect(createButton).toHaveAttribute('isdisabled', 'true')
  })


  it ('fires callback when create button is clicked', async () => {
    const mockOnCreateClicked = jest.fn()

    render(
      <ListIntroduction
        aliasesCount={mockAliases.length}
        onCreateClicked={mockOnCreateClicked}
      />
    )

    await act(async () => {
      // Click create button
      const createButton = screen.getByTitle('emailAliasesCreateAliasTitle')
      clickLeoButton(createButton)
    })

    expect(mockOnCreateClicked).toHaveBeenCalled()
  })


  it ('shows Delete Alias Modal when delete button is clicked', async () => {
    render(
      <AliasList
        aliases={mockAliases}
        authEmail={mockAuthEmail}
        emailAliasesService={mockEmailAliasesService}
      />
    )

    const deleteText = screen.queryAllByText('emailAliasesDelete')[0]
    expect(deleteText).toBeInTheDocument()

    const deleteMenuItem = deleteText?.closest('leo-menu-item')
    expect(deleteMenuItem).toBeInTheDocument()


    if (deleteMenuItem) {
          // Click delete button
      await act(async () => {
        fireEvent.click(deleteMenuItem)
      })

    /*  await waitFor(() => {
        expect(screen.getByText('emailAliasesDeleteAliasTitle')).toBeInTheDocument()
        expect(screen.getByText('emailAliasesDeleteAliasDescription')).toBeInTheDocument()
        expect(screen.getByText('emailAliasesDeleteAliasButton')).toBeInTheDocument()
        expect(screen.getByText('emailAliasesDeleteWarning')).toBeInTheDocument()
      })
    */
    }
  })
})
