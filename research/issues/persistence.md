# Issues: persistence

Defects and divergences in character save/load, the bank, the occupy flags and account storage. Behaviour lives in [systems/persistence.md](../systems/persistence.md). The WLS not checking the password on load is recorded once, in [SESS-001](session.md#sess-001-no-session-ticket-credential-checks-are-weak).

### PERSIST-001 The shipped game server build cannot talk to the shipped WLS

- **Program**: game server / world log
- **System**: persistence
- **Kind**: setup blocker
- **Intent**: the game server saves and loads through the world log server.
- **Now**: `DEF_CHINESE` turns on `DEF_DBGAMESERVER`, so saves and loads use binary block C. The save message inserts a 4-byte char ID at offset 36 and moves the count flag and body to 40/41 (size `41 + iSize`), the load reply layout differs, and the only existing WLS (V2.15) opens files in text mode (`rt`). The DB back end block C was written for (`MSGID_REQUEST_REGISTERDBSERVER` exists only as an MLS admin message) is not in any source or binary [V doc] [U].
- **Where**: `research/game-server/build-config.md`; `Game.cpp:4492-4503`; `decomp/WorldServer/HandleRequestPlayerData.c`; `research/servers/official-compat-220.md` 1, 4, 93-94 "DB mode incompatible (char ID at 36)"
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: yes
- **Source doc**: systems/persistence.md section 9

### PERSIST-002 Fallback save writes binary in text mode

- **Program**: game server
- **System**: persistence
- **Kind**: bug
- **Intent**: a local fallback copy of the character record.
- **Now**: in the DB build `LocalSavePlayerData` writes block C with `fopen(..., "wt")` into a `.txt` file, so CR/LF translation can corrupt the record, and nothing reads it back automatically [V code] [I corruption]. The file is `Character_<month>_<day>_<hour>_<min>\AscII<first byte of name>\<name>.txt`, written by `LocalSavePlayerData`, called from S 2800 (merged from GSO-039). [V]
- **Where**: `SRV Game.cpp:38781-38787`; S Game.cpp:38736-38794
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/persistence.md section 9; research/game-server/functions/persistence.md:60

### PERSIST-003 Occupy-flag persistence is dead

Merged into [CRUSADE-001](crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).

### PERSIST-004 Bank-modified flag never clears

- **Program**: game server
- **System**: persistence
- **Kind**: bug
- **Intent**: rewrite the bank only when it changed since the last save.
- **Now**: set TRUE on any bank change and never reset after a save, so after the first change every save in the session rewrites the bank [V by grep].
- **Where**: `SRV Game.cpp:6870` plus the four setters
- **Evidence**: [V by grep]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/persistence.md section 9

### PERSIST-005 Item state between autosaves is volatile (duplication window)

- **Program**: game server
- **System**: persistence
- **Kind**: security
- **Intent**: items survive a crash and cannot be duplicated.
- **Now**: with no save on trade or drop, a game-server crash rolls both parties of a trade back to their last save: a duplication window that the Log-side DupItemID list and item logs try to catch after the fact [I from the save triggers].
- **Where**: save triggers in persistence.md
- **Evidence**: [I]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/persistence.md section 9

### PERSIST-006 Account creation is broken in the MLS binary

- **Program**: main log
- **System**: persistence
- **Kind**: bug
- **Intent**: create a usable account file.
- **Now**: it writes `Hack = <password>` instead of `account-password = `, so new accounts cannot log in; accounts were evidently made by an outside tool [V doc] [I].
- **Where**: `official-mainserver.md` 3.2
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: yes (accounts must be created another way)
- **Source doc**: systems/persistence.md section 9
