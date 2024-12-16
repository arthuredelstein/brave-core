import '../../../../browser/resources/settings/email_aliases_page/email_aliases'
import { RemoteMappingService } from '../../../../browser/resources/settings/email_aliases_page/remote_mapping_service'

const root = document.getElementById('email-aliases-bubble-root')!
root.attachShadow({ mode: 'open' })
; (window as any).mountModal(root.shadowRoot, new RemoteMappingService())
