# Issues: commands

Scope: dead, unused, stubbed and defective commands (MSGIDs, COMMONTYPEs, NOTIFYs, slash commands) in the 2.20 command catalogue that are not owned by one system. Split out of `research/systems/commands/overloaded.md`, `chat-commands.md` and `doc-discrepancies.md`. Overloaded envelopes and value collisions are design, documented in `overloaded.md`.

### COMMANDS-001 REQ_TRAINSKILL handler is commented out and no client sender exists

Merged into [SKILLS-003](skills.md#skills-003-req_trainskill-and-skilltrainfail-are-dead).

### COMMANDS-002 REQUEST_CREATENEWPARTY case commented; its reply notify has no sender

Merged into [PARTY-005](party.md#party-005-dead-legacy-per-server-party-path).

### COMMANDS-003 REQ_GETOCCUPYFLAG has a live handler and no client sender

Merged into [CRUSADE-001](crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).

### COMMANDS-004 REQUEST_HELP and its HELP/HELPFAILED replies are unreachable

Merged into [CRUSADE-004](crusade.md#crusade-004-help-call-is-unreachable-from-the-client).

### COMMANDS-005 BANGUILD and SUCCESSBANGUILDMAN are passed to SendNotifyMsg, which has no case

Merged into [GUILD-007](guild.md#guild-007-dead-or-unwired-guild-features).

### COMMANDS-006 MSGID_ITEMLOG has a packing case and gate handler but no sender

Merged into [ITEM-007](item.md#item-007-dead-taiwanlog-item-log-pair-the-shipped-pair-logs-only-good-items).

### COMMANDS-007 MSGID_REQUEST_SETACCOUNTINITSTATUS is only a shared case label

Merged into [SESS-004](session.md#sess-004-setaccountinitstatus-from-the-game-server-is-dead).

### COMMANDS-008 REQ_LISTCONTENTS and RESPONSE_JOINPARTY are defined only
- **Program**: client / game server
- **System**: commands
- **Kind**: dead code
- **Intent**: unknown (old commands).
- **Now**: `DEF_COMMONTYPE_REQ_LISTCONTENTS` (0x0A03, also an alias collision with LOGRES INPUTKEYCODE) and `RESPONSE_JOINPARTY` exist only in headers. [V]
- **Where**: S/C NetMessages.h:293-309
- **Evidence**: overloaded.md dead table.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/commands/overloaded.md:84, 66

### COMMANDS-009 CANNOTJOINMOREGUILDSMAN, LIMITEDLEVEL and CHARISMA notifies are never sent

Merged into [GUILD-008](guild.md#guild-008-no-guild-member-cap) and [CHARACTER-004](character.md#character-004-limitedlevel-and-charisma-notifies-have-no-server-sender).

### COMMANDS-010 SKILLTRAINFAIL is sent only inside a commented block

Merged into [SKILLS-003](skills.md#skills-003-req_trainskill-and-skilltrainfail-are-dead).

### COMMANDS-011 Six notifies are never sent and never handled
- **Program**: game server / client
- **System**: commands (notify)
- **Kind**: dead code
- **Intent**: unknown.
- **Now**: `DEF_NOTIFY_WAITFORGUILDOPERATION`, `EVENTMSGSTRING`, `NEWDYNAMICOBJECT`, `DELDYNAMICOBJECT`, `DEBUGMSG`, `DUTYSELECTED` are never sent and never handled. [V]
- **Where**: see notify-types.md
- **Evidence**: notify-types.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/commands/overloaded.md:87

### COMMANDS-012 MSGID_ADMINUSER and MSGID_MANAGER_SHUTDOWN handlers do nothing

Merged into [ADMIN-010](admin.md#admin-010-requestadminusermode-is-empty) and [TOPO-007](server-topology.md#topo-007-manager-shutdown-is-dead).

### COMMANDS-013 Occupy-flag save requests and MSGID_COLLECTEDMANA are sent only in comments

Merged into [CRUSADE-001](crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).

### COMMANDS-014 /dissmiss and /polymorph are stubs

Merged into [GUILD-007](guild.md#guild-007-dead-or-unwired-guild-features) and [ADMIN-011](admin.md#admin-011-polymorph-parses-and-does-nothing).

### COMMANDS-015 Korean slash aliases cannot match in the UTF-8 export
- **Program**: game server / client
- **System**: commands (chat)
- **Kind**: build / data
- **Intent**: Korean aliases such as `/귓속말`, `/접속확인`, `/공격`, `/강퇴` match what a cp949 client sends.
- **Now**: the source is a UTF-8 export of cp949 code, so each Korean literal is longer than the `memcmp` length beside it and cannot match; in the original cp949 build they matched. [V]
- **Where**: S Game.cpp:10745-11046 (rows flagged `alias (cp949)` in chat-commands.md)
- **Evidence**: chat-commands.md.
- **Blocks 2.20 setup**: unknown (English spellings exist for the commands that have one)
- **Source doc**: research/systems/commands/overloaded.md:93; chat-commands.md:7

### COMMANDS-016 About 30 MSGIDs are defined but unused
- **Program**: client / game server / gate
- **System**: commands (account, session)
- **Kind**: dead code
- **Intent**: mostly Main/World Log internals and official-only account messages.
- **Now**: 30 MSGIDs are defined only (status column of msgids.md), e.g. REGISTERDBSERVER, IPINFOCHANGE, ACCOUNTINFOCHANGE, OCCUPYFLAGDATA, METEORSTRIKE (old id). [V]
- **Where**: msgids.md status column
- **Evidence**: msgids.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/commands/overloaded.md:94

### COMMANDS-017 /closeconn Taiwan branch uses the undefined name DEF_TIAWAN

Merged into [GSO-011](game-server-other.md#gso-011-misspelt-build-switches-leave-blocks-dead).

### COMMANDS-018 /fi gives no reply when the player is not found
- **Program**: game server
- **System**: commands (chat)
- **Kind**: bug
- **Intent**: report not-found like the other name commands (shared PLAYERNOTONGAME reply).
- **Now**: `/fi` never reports not-found: its send is commented (S 34695) and a miss gets silence. [V]
- **Where**: S Game.cpp:34695
- **Evidence**: overloaded.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/commands/overloaded.md:33
