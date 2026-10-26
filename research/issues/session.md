# Issues: session

Defects and divergences in login hand-off, player init, duplicate-login handling, server change, keep-alive and forced disconnects. Behaviour lives in [systems/session.md](../systems/session.md).

### SESS-001 No session ticket; credential checks are weak

- **Program**: game server / world log / main log
- **System**: session
- **Kind**: security
- **Intent**: the main log server's ENTERGAME confirm hands the client to a game server that should trust the login.
- **Now**: the confirm carries only address/port (24 bytes); the game server re-authenticates by asking the WLS for the file. The WLS checks only that the file's `account-name` matches; it does not read the password in `REQUEST_PLAYERDATA` (password at +26 unused). Existing checks: (a) the MLS ENTERGAME handler stores account + password + level as state INIT (`SETACCOUNTINITSTATUS`); at ENTERGAMECONFIRM the WLS compares password and level for INIT/WAIT accounts and on mismatch orders a no-save disconnect: a one-shot plaintext check after the client is already in the world. (b) With the account in state NULL there is no check at all: ENTERGAMECONFIRM for a NULL account sets PLAYING. (c) The ENTERGAME request carries a 120-byte command-line/billing token (ENTERGAME offset 80) that the MLS reads and discards; it is not a ticket [V decomp].
- **Where**: `decomp/WorldServer/HandleRequestPlayerData.c`; `decomp/MainServer/RequestEnterGame.c`; `decomp/WorldServer/HandleEnterGameConfirm.c`; SRV `RequestCheckAccountPasswordHandler` `Game.cpp:46052-46083`; `WLS 0x4033f0`; `CLI Game.cpp:1799`; `official-worldserver.md` 3.2
- **Evidence**: [V doc] [V decomp]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9

### SESS-002 RequestInitDataHandler has no init-complete guard

- **Program**: game server
- **System**: session
- **Kind**: bug
- **Intent**: INITDATA should be accepted only after the player data has loaded.
- **Now**: no `m_bIsInitComplete` test in `Game.cpp:1611-2250`. `RequestInitPlayerHandler` itself guards (`m_bIsInitComplete == TRUE` returns, `Game.cpp:1507`) but can be repeated before init completes, re-sending `REQUEST_PLAYERDATA` each time. A client that sends INITDATA before its player data loads, with an all-zero name, passes the name check against the zeroed `m_cCharName` [V] [I effect].
- **Where**: `SRV Game.cpp:1611-1641`, `1507`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9

### SESS-003 Duplicate-login race

- **Program**: game server / world log
- **System**: session
- **Kind**: bug
- **Intent**: refuse a second login for an account that is already playing.
- **Now**: the duplicate check happens before the Log reply; the Log server's PLAYING state is the cross-server guard, but the WLS overwrites PLAYING on a second ENTERGAMECONFIRM instead of refusing [V doc].
- **Where**: `WLS 0x4033f0`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9

### SESS-004 SETACCOUNTINITSTATUS from the game server is dead

- **Program**: game server
- **System**: session
- **Kind**: dead code
- **Intent**: the game server tells the Log server an account is initialising.
- **Now**: the case exists in `bSendMsgToLS` with a layout (level at 16) that does not match the WLS's MLS layout (password at 16); it is never called [V doc]. The only trace of the request in the game server is a case label shared with SETACCOUNTWAITSTATUS (S 4281-4282) (merged from COMMANDS-007). [V]
- **Where**: `game-log-protocol.md` 3; `official-compat-220.md` 4; S Game.cpp:4281-4282
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9; research/systems/commands/overloaded.md:82

### SESS-005 Server-change port depends on a build switch

- **Program**: game server
- **System**: session
- **Kind**: build
- **Intent**: the server-change notify should give the client an address it can reach.
- **Now**: without `DEF_DEFENCEHACKING` the notify carries the game server's internal Log port, where the official WLS drops non-allow-listed peers (clients) [V]. See also [setup-2.20.md](setup-2.20.md).
- **Where**: `SRV Game.cpp:18652-18664`; `official-compat-220.md` 3
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes when the build switch is off and the official WLS is used
- **Source doc**: systems/session.md section 9

### SESS-006 Redundant timeout branch in CheckClientResponseTime

- **Program**: game server
- **System**: session
- **Kind**: dead code
- **Intent**: a second, different timeout test.
- **Now**: the `else if` repeats the outer timeout test, so it is always true there; harmless [V].
- **Where**: `SRV Game.cpp:3852-3862`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9

### SESS-007 World user cap parsed, never enforced

- **Program**: game server
- **System**: session
- **Kind**: incomplete
- **Intent**: `world-server-max-user` caps users in the world.
- **Now**: it sets `m_iWorldMaxUser`, which is only displayed; the only cap is the per-server 800 redirect [V].
- **Where**: `SRV Game.cpp:25089`, `895`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9

### SESS-008 Force-disconnect relies on client co-operation

- **Program**: game server / client
- **System**: session
- **Kind**: security
- **Intent**: `ForceDisconnectAccount` ends a session.
- **Now**: it only notifies; a modified client ignoring `DEF_NOTIFY_FORCEDISCONN` keeps its session, because it keeps sending keep-alives and the 10 s timeout never fires [V code] [I effect]. `ForceDisconnectAccount` clamps `wCount` to 10 and sends `DEF_NOTIFY_FORCEDISCONN` to each client of the account; its `DeleteClient` call is commented out (S 38103, 38112) (merged from GSO-036). [V]
- **Where**: `ForceDisconnectAccount`; S Game.cpp:38098-38117
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/session.md section 9; research/game-server/functions/network.md:215
