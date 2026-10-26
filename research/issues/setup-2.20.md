# Issues: setup-2.20

Everything that blocks (or complicates) a working v2.20 client + game server + gate + log servers setup, gathered in one place: configuration mismatches, build-switch requirements, the byte patches for the official log binaries (`research/servers/official-compat-220.md`, the verdict is "not usable as-is; usable with a few small binary patches plus a 2.20 game server rebuilt in FILE mode with two source changes"), and missing client data. Entries link to the defect entries in other issue files rather than duplicate them. The minimum working combination from `official-compat-220.md` section 1 is: MLS version words patched (and optionally the account-creation fix); WLS registration id patched or the source define changed; game server built without `DEF_DBGAMESERVER` and `DEF_DEFENCEHACKING` with the server-change port fixed; client log address patched; the game server's IP in the WLS `game-server-list`. Static analysis only; nothing was run.

### SETUP-001 Gate build date (612) does not match the game server (429)

- **Program**: gate / game server
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the gate admits only game servers of the matching build.
- **Now**: the shipped `GateServer.cfg` has `build-date = 612`; the game server sends `DEF_BUILDDATE` 429 (China build, `Version.h`) -> the gate answers REJECT and the game server never starts [V doc]. Set the gate cfg value to the game server's `DEF_BUILDDATE` (`servers/gate-server.md` section on configuration). Defect entry: [TOPO-001](../issues/server-topology.md#topo-001-shipped-pieces-do-not-fit-together-as-configured).
- **Where**: `GateServer.cfg`; `SRV Version.h`; `research/servers/gate-server.md`, `game-gate-protocol.md` 8
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: yes
- **Source doc**: servers/official-compat-220.md 1; systems/server-topology.md

### SETUP-002 Shipped gate cfg has no game-server-list (allow-list)

- **Program**: gate
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the allow-list names the game server IPs the gate accepts.
- **Now**: the shipped `GateServer.cfg` has only 3 keys and no `game-server-list`; the gate admits peers only by exact match against that list (an empty list rejects everyone), so every connection is dropped at `bAccept`, before the build date is compared. Add one `game-server-list = <ip>` line per game server IP, at most 10 ([GATE-008](../issues/gate.md#gate-008-allow-list-loader-has-no-bound)) [V].
- **Where**: `GateCore.cpp:143-146, 231-283` (0b994cd)
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: systems/server-topology.md section 9; gate/subsystems.md 3

### SETUP-003 Binary vs text saves: the game server must be built in FILE mode

- **Program**: game server / world log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the exported tree saves and loads characters through the WLS.
- **Now**: the exported configuration (`DEF_CHINESE`) enables `DEF_DBGAMESERVER`, which saves and loads binary block C with a char ID at offset 36; the only existing WLS stores and returns the character as a text file. Build without `DEF_DBGAMESERVER` (FILE mode, count flag at 36, text at 37). Defect entry: [PERSIST-001](../issues/persistence.md#persist-001-the-shipped-game-server-build-cannot-talk-to-the-shipped-wls); the fallback save text-mode bug is [PERSIST-002](../issues/persistence.md#persist-002-fallback-save-writes-binary-in-text-mode).
- **Where**: `SRV GlobalDef.h:16, 72-78`; `Game.cpp:4492-4503`; WLS `HandleSavePlayerData` `0x404e80`, `HandleRequestPlayerData` `0x403640`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: yes
- **Source doc**: servers/official-compat-220.md 1, 4

### SETUP-004 The config push the world log server never sends

- **Program**: game server / world log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the WLS pushes all game-server configuration after registration, including (under `DEF_DEFENCEHACKING`) `MSGID_WLSERVERFILECONTENTS`.
- **Now**: this WLS (linked 2002-11-29) predates the 2002-12-08 world-server security feature. Under `DEF_DEFENCEHACKING` the game server waits for `MSGID_WLSERVERFILECONTENTS` 0x0FA40007, which the WLS never sends, so it never starts. Build without `DEF_DEFENCEHACKING` (see [SETUP-009](#setup-009-server-change-notify-carries-the-wrong-port-game-server-source-change)). The WLS also never sends `MSGID_WORLDCONFIGCONTENTS` 0x0FA40005, `NPCITEMCONFIGCONTENTS` 0x0FA40006 and `TELEPORTLISTCONTENTS` 0x0FA40008; the game server keeps defaults for these (behaviour with no NPC-item config under `DEF_NPCITEM`, on in `DEF_CHINESE`, not checked [U]). Defect entry: [TOPO-001](../issues/server-topology.md#topo-001-shipped-pieces-do-not-fit-together-as-configured), [TOPO-002](../issues/server-topology.md#topo-002-defencehacking-is-a-world-server-security-switch-not-anti-cheat).
- **Where**: `SRV Game.cpp:3698-3701, 15180-15187, 15255-15267`; `official-compat-220.md` 4, 6
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes (for builds with `DEF_DEFENCEHACKING`)
- **Source doc**: servers/official-compat-220.md 1, 4, 6; systems/server-topology.md

### SETUP-005 Game server registration id differs from the WLS (byte patch)

- **Program**: world log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the WLS recognises the 2.20 game server's registration message.
- **Now**: the 2.20 game server sends `MSGID_REQUEST_REGISTERGAMESERVER` 0x0512A3F4 (`SRV Game.cpp:4156`); the WLS expects 0x0512A3E3 (`0x40221c`) and ignores the game server entirely. Fix: patch `WorldServer.exe` file offset 0x221D `E3` -> `F4`, or change the define in the 2.20 game-server source [V]. Layouts are identical.
- **Where**: `WorldServer.exe 0x221D`; `SRV Game.cpp:4156`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: servers/official-compat-220.md 1, 4, 7

### SETUP-006 MainServer login CONFIRM version words (byte patch)

- **Program**: main log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the MLS reports the version the 2.20 client requires.
- **Now**: the client requires upper 2, lower 20; the MLS sends 4 / 0x60, so the client shows the "version not match" screen. Patch `MainServer.exe` 0x3EF1 `04` -> `02` and 0x3F08 `60` -> `14` [V].
- **Where**: `MainServer.exe 0x3EF1, 0x3F08` (VA 0x403eee, 0x403f05)
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: servers/official-compat-220.md 2, 7

### SETUP-007 MainServer account creation writes `Hack =` (byte patch)

- **Program**: main log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: create an account that can log in.
- **Now**: new accounts are written with `Hack = <password>` instead of `account-password = `, so they cannot log in. Patch `MainServer.exe` 0xC0C6 `80 12 45 00` -> `F0 1A 45 00`, or create account files by another route. Defect entry: [PERSIST-006](../issues/persistence.md#persist-006-account-creation-is-broken-in-the-mls-binary).
- **Where**: `MainServer.exe 0xC0C6` (string 0x451af0); `0x40c0c5`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes unless accounts are made by another tool
- **Source doc**: servers/official-compat-220.md 2, 7; servers/official-mainserver.md

### SETUP-008 MainServer internal-port address allow-list string (optional patch)

- **Program**: main log / world log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: only known WLS addresses may use the internal port.
- **Now**: the MLS internal port accepts only five hard-coded address prefixes (`0x402894`). The WLS dials the public port `main-log-server-port` (2981), so the check is not reached in that setup; if the WLS is pointed at the internal port 3840, patch `MainServer.exe` 0x51224 `211.115.208.` to e.g. `127.0.0.1` + NUL padding (12 bytes). Whether the MLS accepts a WLS registration on a public socket is not verified [U, runtime test needed].
- **Where**: `MainServer.exe 0x51224`; `0x40289c`, `0x402770`
- **Evidence**: [V bytes] [U runtime]
- **Blocks 2.20 setup**: unknown
- **Source doc**: servers/official-compat-220.md 5, 7, 8

### SETUP-009 Server-change notify carries the wrong port (game server source change)

- **Program**: game server
- **System**: setup
- **Kind**: setup blocker
- **Intent**: after a server change the client reconnects to the WLS world port.
- **Now**: `DEF_DEFENCEHACKING` builds use the pushed `WLserver.cfg`, which this WLS never pushes (port 0); other builds send `internal-log-server-port`, the WLS internal port, where the WLS closes every peer not listed in `game-server-list`, so a client would be dropped. Required source change: send the WLS `world-server-port` (e.g. a new `gserver.cfg` key). Defect entry: [SESS-005](../issues/session.md#sess-005-server-change-port-depends-on-a-build-switch).
- **Where**: `SRV Game.cpp:5290, 5336, 18646-18667`; WLS `0x4019e0`
- **Evidence**: [V] [I fix]
- **Blocks 2.20 setup**: yes
- **Source doc**: servers/official-compat-220.md 3

### SETUP-010 WLS game-server-list and world name configuration

- **Program**: world log / main log
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the WLS knows which game servers and which world are valid.
- **Now**: the game server's IP must be in `game-server-list` of `WLserver.cfg`; the client forces the world name "WS1", and the MLS requires it to equal a registered and activated world, so set `world-server-name = WS1` [V].
- **Where**: `CLI Game.cpp:15516-15523`; MLS `0x403abb`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes (config)
- **Source doc**: servers/official-compat-220.md 1, 2

### SETUP-011 The client's log server address is hard-coded

- **Program**: client
- **System**: setup
- **Kind**: setup blocker
- **Intent**: `login.cfg` sets the log server address; the v2.15 change forced English/Japanese builds to a fixed IP because players edited the port.
- **Now**: with `DEF_LANGUAGE > 3` (English, the shipped setting) `login.cfg` is never read; the client connects to `61.100.0.103:2848` (`216.27.13.36:2848` with `DEF_FUCK_USA`). A recreated server needs a rebuilt or patched client: change the literal at `Game.cpp:15549` (and the port at 15552), or change the `#if DEF_LANGUAGE > 3` at 15543 so the file parser runs [V].
- **Where**: `CLI Game.cpp:15543-15556`; `GlobalDef.h:145`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: client/build-config.md "Log server address"; servers/client-log-protocol.md 1, 11

### SETUP-012 Curse.txt is required by the English client

- **Program**: client
- **System**: setup
- **Kind**: setup blocker
- **Intent**: load a chat language filter list.
- **Now**: with `DEF_LANGUAGE > 2` `bInit` fails with "CURSE.TXT file contains wrong infomation." when `_iConvertFileXor` cannot read `Curse.txt`, and the client exits; neither the 2.02 install nor the 2.23 patch ships it. Supply an empty or real (XOR-obfuscated) `Curse.txt`. Related defects: [CHAT-001](../issues/chat.md#chat-001-language-filter-is-client-side-only), [CHAT-003](../issues/chat.md#chat-003-loadcurse-passes-file-text-as-a-printf-format).
- **Where**: `CLI Game.cpp:1003-1010`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: client/assets.md "Contents and config files"

### SETUP-013 Missing sprite paks

- **Program**: client
- **System**: setup
- **Kind**: setup blocker
- **Intent**: the client loads about 208 sprite pak names.
- **Now**: the 2.02 install plus the 2.23 patch lacks 49 names the client asks for (LoginDialog, DialogText, post-2.02 monsters, effect6-8, several weapon/gear sheets, tiles 226-229 and 430-461, and `Effect5.pak` see [RENDER-003](../issues/rendering.md#render-003-energy-ball-sprite-pak-name-is-wrong)). `MakeSprite` returns silently on a missing file, leaving NULL slots; blits are skipped by the `this == NULL` guard, but about 165 `m_pSprite[..]->m_*` sites read members directly and crash. A 2.20-era (or later) sprite folder is needed for about 48 files. Also missing: sounds M71-M98, `music\Carol.wav` (DEF_XMAS only), and maps added after 2.02. Not run.
- **Where**: `Game.cpp:3543-3590` and the sites in `assets.md`
- **Evidence**: [V] for the lists, [I] for the crash outcomes
- **Blocks 2.20 setup**: yes
- **Source doc**: client/assets.md "Sprite files: present and missing", "Sounds", "Music", "Maps"

### SETUP-014 No client project file in the tree

- **Program**: client
- **System**: setup
- **Kind**: build
- **Intent**: build the client from the tree.
- **Now**: there is no project file (`.dsp` / `.vcxproj`) in the client tree, so the preprocessor defines a project would supply are unknown; `_DEBUG` is the only switch expected from the compiler, and everything else is in `GlobalDef.h`. A project must be written (libraries: DirectX 7, WS2_32, WSOCK32, WINMM, IMM32, `search.lib`, `Cint.lib`, `StrTok.lib`, `DesDll.lib`).
- **Where**: `sources\client-2.20`; `client/build-config.md` "Where switches live"
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes
- **Source doc**: client/build-config.md

### SETUP-015 Potion cfg keyword: portion vs potion

- **Program**: game server / world log
- **System**: setup
- **Kind**: data
- **Intent**: the WLS pushes a `Potion.cfg` the game server can parse.
- **Now**: no official 2.20 `Potion.cfg` exists; the 2.03 `portion.cfg` (14 recipes, cp949 Korean) uses the keyword `portion`, but the 2.20 parser tests `memcmp(token,"potion",5)`, so it parses nothing. The 3.51 `Potion.cfg` (19 recipes) matches. No crafting issue entry covers it.
- **Where**: `SRV Game.cpp:38713`; `research/shared/formats/config-files.md`
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes if the 2.03 file is used
- **Source doc**: systems/crafting.md (outside this group); servers/official-worldserver.md (Potion.cfg push)

### SETUP-016 Occupy flags are never saved with this WLS

- **Program**: game server / world log
- **System**: setup
- **Kind**: incomplete
- **Intent**: crusade occupy flags persist across restarts.
- **Now**: `SAVEARESDEN/ELVINEOCCUPYFLAGDATA` is never sent by the 2.20 game server, so the WLS handler (`0x408f80`) is never reached; a functional gap rather than a start blocker. Defect entry: [CRUSADE-001](../issues/crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).
- **Where**: `SRV Game.cpp:3762-3764, 4424-4468, 47997`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: servers/official-compat-220.md 4

### SETUP-017 The MLS rejects a WLS shutdown notice

- **Program**: main log / world log
- **System**: setup
- **Kind**: bug
- **Intent**: the WLS tells the MLS when the world stops.
- **Now**: the world shutdown message can never be accepted (code compared at the wrong offset, `0x40f5ff`); harmless for normal play [I]. See [TOPO-008](../issues/server-topology.md#topo-008-wls-shutdown-notice-never-accepted-by-the-mls).
- **Where**: MLS `0x40f5ff`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: servers/official-compat-220.md 5
