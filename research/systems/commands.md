# commands (index)

Every 2.20 command (MSGID, COMMONTYPE, motion, NOTIFY, GSM, slash) with intent and owning system; source wins over docs.

- `commands/method.md` - sources, how commands were enumerated, counts per family, status rules, build switches assumed; read first to check completeness or regenerate.
- `commands/by-system.md` - system list and the system -> commands matrix with secondary owners; read to find everything a system touches.
- `commands/overloaded.md` - overloaded envelopes and sub-commands, straddlers, value collisions, dead/unreachable, switch-gated commands; read before redesigning the command layer.
- `commands/msgids.md` - all 139 MSGID_* on every link (client, Log, gate, monitor); read when tracing a top-level message.
- `commands/common-types.md` - 56 DEF_COMMONTYPE_* client commands (and their reuse as events/notifies); read when changing a UI action or ClientCommonHandler.
- `commands/motion.md` - 17 DEF_OBJECT* motion commands and replies; read for movement, attack or cast timing.
- `commands/notify-types.md` - 141 DEF_NOTIFY_* server->client reports; read when adding or debugging a notification.
- `commands/chat-commands.md` - chat prefix modes, 45 server slash commands with admin gates, 8 client-local commands; read for chat or GM tooling.
- `commands/gate-relay.md` - 18 GSM_* records relayed through the gate; read when a command must reach another game server.
- `commands/other-subtypes.md` - confirm/reject, Log result codes, enter-game modes; read only for login/Log replies.
- `commands/doc-discrepancies.md` - where existing research docs disagree with the source; read before trusting the message catalogue's link column.
