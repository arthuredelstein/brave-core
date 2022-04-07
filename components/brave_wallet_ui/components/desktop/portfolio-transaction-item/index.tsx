import * as React from 'react'
import * as EthereumBlockies from 'ethereum-blockies'
import { useDispatch, useSelector } from 'react-redux'

import { getLocale } from '../../../../common/locale'
import {
  BraveWallet,
  WalletAccountType,
  DefaultCurrencies,
  WalletState,
  PageState,
  WalletRoutes
} from '../../../constants/types'

// Utils
import { toProperCase } from '../../../utils/string-utils'
import { formatDateAsRelative } from '../../../utils/datetime-utils'
import { mojoTimeDeltaToJSDate } from '../../../../common/mojomUtils'
import Amount from '../../../utils/amount'
import { copyToClipboard } from '../../../utils/copy-to-clipboard'
import { getNetworkFromTXDataUnion } from '../../../utils/network-utils'

// Hooks
import { useExplorer, useTransactionParser } from '../../../common/hooks'
import { SwapExchangeProxy } from '../../../common/hooks/address-labels'

// Styled Components
import {
  AddressOrAsset,
  ArrowIcon,
  BalanceColumn,
  CoinsButton,
  CoinsIcon,
  DetailColumn,
  DetailRow,
  DetailTextDark,
  DetailTextDarkBold,
  DetailTextLight,
  RejectedTransactionSpacer,
  FromCircle,
  MoreButton,
  MoreIcon,
  StatusRow,
  StyledWrapper,
  ToCircle,
  TransactionDetailRow,
  TransactionFeeTooltipBody,
  TransactionFeeTooltipTitle
} from './style'
import { StatusBubble } from '../../shared/style'
import TransactionFeesTooltip from '../transaction-fees-tooltip'
import TransactionPopup, { TransactionPopupItem } from '../transaction-popup'
import TransactionTimestampTooltip from '../transaction-timestamp-tooltip'
import { WalletActions } from '../../../common/actions'
import { useHistory } from 'react-router'

export interface Props {
  selectedNetwork: BraveWallet.NetworkInfo
  transaction: BraveWallet.TransactionInfo
  account: WalletAccountType | undefined
  accounts: WalletAccountType[]
  visibleTokens: BraveWallet.BlockchainToken[]
  transactionSpotPrices: BraveWallet.AssetPrice[]
  displayAccountName: boolean
  defaultCurrencies: DefaultCurrencies
}

const PortfolioTransactionItem = (props: Props) => {
  const {
    transaction,
    account,
    visibleTokens,
    displayAccountName
  } = props

  // routing
  const history = useHistory()

  // redux
  const dispatch = useDispatch()
  const {
    defaultNetworks,
    selectedNetwork,
    transactionSpotPrices,
    accounts,
    defaultCurrencies
  } = useSelector(({ wallet }: { wallet: WalletState }) => wallet)
  const {
    selectedTimeline
  } = useSelector(({ page }: { page: PageState }) => page)

  // state
  const [showTransactionPopup, setShowTransactionPopup] = React.useState<boolean>(false)

  // memos
  const transactionsNetwork = React.useMemo(() => {
    return getNetworkFromTXDataUnion(transaction.txDataUnion, defaultNetworks, selectedNetwork)
  }, [defaultNetworks, transaction, selectedNetwork])

  const parseTransaction = useTransactionParser(transactionsNetwork, accounts, transactionSpotPrices, visibleTokens)
  const transactionDetails = React.useMemo(
    () => parseTransaction(transaction),
    [transaction]
  )

  const fromOrb = React.useMemo(() => {
    return EthereumBlockies.create({ seed: transactionDetails.sender.toLowerCase(), size: 8, scale: 16 }).toDataURL()
  }, [transactionDetails.sender])

  const toOrb = React.useMemo(() => {
    return EthereumBlockies.create({ seed: transactionDetails.recipient.toLowerCase(), size: 8, scale: 16 }).toDataURL()
  }, [transactionDetails.recipient])

  const onShowTransactionPopup = () => {
    setShowTransactionPopup(true)
  }

  const onHideTransactionPopup = () => {
    if (showTransactionPopup) {
      setShowTransactionPopup(false)
    }
  }

  const onClickViewOnBlockExplorer = useExplorer(transactionsNetwork)

  const onClickCopyTransactionHash = (transactionHash: string) => {
    copyToClipboard(transactionHash)
  }

  const onClickRetryTransaction = () => {
    onRetryTransaction(transaction)
  }

  const onClickSpeedupTransaction = () => {
    onSpeedupTransaction(transaction)
  }

  const onClickCancelTransaction = () => {
    onCancelTransaction(transaction)
  }

  const findWalletAccount = React.useCallback((address: string) => {
    return accounts.find((account) => account.address.toLowerCase() === address.toLowerCase())
  }, [accounts])

  const onAddressClick = (address?: string) => () => {
    if (!address) {
      return
    }

    const account = findWalletAccount(address)

    if (account !== undefined) {
      onSelectAccount(account)
      return
    }

    onClickViewOnBlockExplorer('address', address)
  }

  const findToken = React.useCallback((symbol: string) => {
    return visibleTokens.find((token) => token.symbol.toLowerCase() === symbol.toLowerCase())
  }, [visibleTokens])

  const onAssetClick = (symbol?: string) => () => {
    if (!symbol) {
      return
    }

    const asset = findToken(symbol)
    if (asset) {
      onSelectAsset(asset)
    }
  }

  const onCancelTransaction = (transaction: BraveWallet.TransactionInfo) => {
    dispatch(WalletActions.cancelTransaction(transaction))
  }

  const onSpeedupTransaction = (transaction: BraveWallet.TransactionInfo) => {
    dispatch(WalletActions.speedupTransaction(transaction))
  }

  const onRetryTransaction = (transaction: BraveWallet.TransactionInfo) => {
    dispatch(WalletActions.retryTransaction(transaction))
  }

  const onSelectAsset = React.useCallback((asset: BraveWallet.BlockchainToken) => {
    if (asset.contractAddress === '') {
      history.push(`${WalletRoutes.Portfolio}/${asset.symbol}`)
      return
    }
    history.push(`${WalletRoutes.Portfolio}/${asset.contractAddress}`)
  }, [selectedTimeline])

  const onSelectAccount = (account: WalletAccountType) => {
    history.push(`${WalletRoutes.Accounts}/${account.address}`)
  }

  const transactionIntentLocale = React.useMemo(() => {
    switch (true) {
      case transaction.txType === BraveWallet.TransactionType.ERC20Approve: {
        const text = getLocale('braveWalletApprovalTransactionIntent')
        return (
          <>
            {displayAccountName ? text : toProperCase(text)}{' '}
            <AddressOrAsset onClick={onAssetClick(transactionDetails.symbol)}>
              {transactionDetails.symbol}
            </AddressOrAsset>
          </>
        )
      }

      // Detect sending to 0x Exchange Proxy
      case transaction.txDataUnion.ethTxData1559?.baseData.to.toLowerCase() === SwapExchangeProxy: {
        const text = getLocale('braveWalletSwap')
        return displayAccountName ? text.toLowerCase() : text
      }

      case transaction.txType === BraveWallet.TransactionType.ETHSend:
      case transaction.txType === BraveWallet.TransactionType.ERC20Transfer:
      case transaction.txType === BraveWallet.TransactionType.ERC721TransferFrom:
      case transaction.txType === BraveWallet.TransactionType.ERC721SafeTransferFrom:
      default: {
        const text = getLocale('braveWalletTransactionSent')
        return (
          <>
            {displayAccountName ? text : toProperCase(text)}{' '}
            <AddressOrAsset
              // Disabled for ERC721 tokens until we have NFT meta data
              disabled={transaction.txType === BraveWallet.TransactionType.ERC721TransferFrom || transaction.txType === BraveWallet.TransactionType.ERC721SafeTransferFrom}
              onClick={onAssetClick(transactionDetails.symbol)}
            >
              {transactionDetails.symbol}
              {transaction.txType === BraveWallet.TransactionType.ERC721TransferFrom || transaction.txType === BraveWallet.TransactionType.ERC721SafeTransferFrom
                ? ' ' + transactionDetails.erc721TokenId : ''}
            </AddressOrAsset>
          </>
        )
      }
    }
  }, [transaction])

  const transactionIntentDescription = React.useMemo(() => {
    switch (true) {
      case transaction.txType === BraveWallet.TransactionType.ERC20Approve: {
        const text = getLocale('braveWalletApprovalTransactionIntent')
        return (
          <DetailRow>
            <DetailTextDark>
              {toProperCase(text)} {transactionDetails.value}{' '}
              <AddressOrAsset onClick={onAssetClick(transactionDetails.symbol)}>
                {transactionDetails.symbol}
              </AddressOrAsset> -{' '}
              <AddressOrAsset onClick={onAddressClick(transactionDetails.approvalTarget)}>
                {transactionDetails.approvalTargetLabel}
              </AddressOrAsset>
            </DetailTextDark>
          </DetailRow>
        )
      }

      // FIXME: Add as new BraveWallet.TransactionType on the service side.
      case transaction.txDataUnion.ethTxData1559?.baseData.to.toLowerCase() === SwapExchangeProxy: {
        return (
          <DetailRow>
            <DetailTextDark>
              {transactionDetails.value}{' '}
              <AddressOrAsset onClick={onAssetClick(transactionDetails.symbol)}>
                {transactionDetails.symbol}
              </AddressOrAsset>
            </DetailTextDark>
            <ArrowIcon />
            <DetailTextDark>
              <AddressOrAsset onClick={onAddressClick(transactionDetails.recipient)}>
                {transactionDetails.recipientLabel}
              </AddressOrAsset>
            </DetailTextDark>
          </DetailRow>
        )
      }

      case transaction.txType === BraveWallet.TransactionType.ETHSend:
      case transaction.txType === BraveWallet.TransactionType.ERC20Transfer:
      case transaction.txType === BraveWallet.TransactionType.ERC721TransferFrom:
      case transaction.txType === BraveWallet.TransactionType.ERC721SafeTransferFrom:
      default: {
        return (
          <DetailRow>
            <DetailTextDark>
              <AddressOrAsset onClick={onAddressClick(transactionDetails.sender)}>
                {transactionDetails.senderLabel}
              </AddressOrAsset>
            </DetailTextDark>
            <ArrowIcon />
            <DetailTextDark>
              <AddressOrAsset onClick={onAddressClick(transactionDetails.recipient)}>
                {transactionDetails.recipientLabel}
              </AddressOrAsset>
            </DetailTextDark>
          </DetailRow>
        )
      }
    }
  }, [transactionDetails])

  // render
  return (
    <StyledWrapper onClick={onHideTransactionPopup}>
      <TransactionDetailRow>
        <FromCircle orb={fromOrb} />
        <ToCircle orb={toOrb} />
        <DetailColumn>
          <DetailRow>
            { // Display account name only if rendered under Portfolio view
              displayAccountName &&
              <DetailTextLight>
                {account?.name}
              </DetailTextLight>
            }
            <DetailTextDark>
              {transactionIntentLocale}
            </DetailTextDark>
            <DetailTextLight>-</DetailTextLight>

            <TransactionTimestampTooltip
              text={
                <TransactionFeeTooltipBody>
                  {mojoTimeDeltaToJSDate(transactionDetails.createdTime).toUTCString()}
                </TransactionFeeTooltipBody>
              }
            >
              <DetailTextDarkBold>
                {formatDateAsRelative(mojoTimeDeltaToJSDate(transactionDetails.createdTime))}
              </DetailTextDarkBold>
            </TransactionTimestampTooltip>
          </DetailRow>
          {transactionIntentDescription}
        </DetailColumn>
      </TransactionDetailRow>
      <StatusRow>
        <StatusBubble status={transactionDetails.status} />
        <DetailTextDarkBold>
          {transactionDetails.status === BraveWallet.TransactionStatus.Unapproved && getLocale('braveWalletTransactionStatusUnapproved')}
          {transactionDetails.status === BraveWallet.TransactionStatus.Approved && getLocale('braveWalletTransactionStatusApproved')}
          {transactionDetails.status === BraveWallet.TransactionStatus.Rejected && getLocale('braveWalletTransactionStatusRejected')}
          {transactionDetails.status === BraveWallet.TransactionStatus.Submitted && getLocale('braveWalletTransactionStatusSubmitted')}
          {transactionDetails.status === BraveWallet.TransactionStatus.Confirmed && getLocale('braveWalletTransactionStatusConfirmed')}
          {transactionDetails.status === BraveWallet.TransactionStatus.Error && getLocale('braveWalletTransactionStatusError')}
        </DetailTextDarkBold>
      </StatusRow>
      <DetailRow>
        <BalanceColumn>
          <DetailTextDark>
            {/* We need to return a Transaction Time Stamp to calculate Fiat value here */}
            {transactionDetails.fiatValue
              .formatAsFiat(defaultCurrencies.fiat)}
          </DetailTextDark>
          <DetailTextLight>{transactionDetails.formattedNativeCurrencyTotal}</DetailTextLight>
        </BalanceColumn>
        {/* Will remove this conditional for solana once https://github.com/brave/brave-browser/issues/22040 is implemented. */}
        {transaction.txType !== BraveWallet.TransactionType.SolanaSystemTransfer &&
          <TransactionFeesTooltip
            text={
              <>
                <TransactionFeeTooltipTitle>{getLocale('braveWalletAllowSpendTransactionFee')}</TransactionFeeTooltipTitle>
                <TransactionFeeTooltipBody>
                  {
                    new Amount(transactionDetails.gasFee)
                      .divideByDecimals(transactionsNetwork.decimals)
                      .formatAsAsset(6, transactionsNetwork.symbol)
                  }
                </TransactionFeeTooltipBody>
                <TransactionFeeTooltipBody>
                  {
                    new Amount(transactionDetails.gasFeeFiat)
                      .formatAsFiat(defaultCurrencies.fiat)
                  }
                </TransactionFeeTooltipBody>
              </>
            }
          >
            <CoinsButton>
              <CoinsIcon />
            </CoinsButton>
          </TransactionFeesTooltip>
        }

        {(transactionDetails.status !== BraveWallet.TransactionStatus.Rejected && transactionDetails.status !== BraveWallet.TransactionStatus.Unapproved) ? (
          <MoreButton onClick={onShowTransactionPopup}>
            <MoreIcon />
          </MoreButton>
        ) : (
          <RejectedTransactionSpacer />
        )}

        {showTransactionPopup &&
          <TransactionPopup>
            {[BraveWallet.TransactionStatus.Approved, BraveWallet.TransactionStatus.Submitted, BraveWallet.TransactionStatus.Confirmed].includes(transactionDetails.status) &&
              <TransactionPopupItem
                onClick={onClickViewOnBlockExplorer('tx', transaction.txHash)}
                text={getLocale('braveWalletTransactionExplorer')}
              />
            }

            {[BraveWallet.TransactionStatus.Approved, BraveWallet.TransactionStatus.Submitted, BraveWallet.TransactionStatus.Confirmed].includes(transactionDetails.status) &&
              <TransactionPopupItem
                onClick={() => onClickCopyTransactionHash(transaction.txHash)}
                text={getLocale('braveWalletTransactionCopyHash')}
              />
            }

            {[BraveWallet.TransactionStatus.Submitted, BraveWallet.TransactionStatus.Approved].includes(transactionDetails.status) &&
              transaction.txType !== BraveWallet.TransactionType.SolanaSystemTransfer &&
              <TransactionPopupItem
                onClick={onClickSpeedupTransaction}
                text={getLocale('braveWalletTransactionSpeedup')}
              />
            }

            {[BraveWallet.TransactionStatus.Submitted, BraveWallet.TransactionStatus.Approved].includes(transactionDetails.status) &&
              transaction.txType !== BraveWallet.TransactionType.SolanaSystemTransfer &&
              <TransactionPopupItem
                onClick={onClickCancelTransaction}
                text={getLocale('braveWalletTransactionCancel')}
              />
            }

            {[BraveWallet.TransactionStatus.Error].includes(transactionDetails.status) &&
              transaction.txType !== BraveWallet.TransactionType.SolanaSystemTransfer &&
              <TransactionPopupItem
                onClick={onClickRetryTransaction}
                text={getLocale('braveWalletTransactionRetry')}
              />
            }
          </TransactionPopup>
        }
      </DetailRow>
    </StyledWrapper>
  )
}

export default PortfolioTransactionItem
