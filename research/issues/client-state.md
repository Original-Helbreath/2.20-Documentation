# Issues: client-state

Defects and divergences in the client's world state: object matching, movement prediction, the map cache and lag detection. Behaviour lives in [systems/client-state.md](../systems/client-state.md).

### CLIENT-STATE-001 Own-player events matched by name, not id

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: identify the player's own object reliably.
- **Now**: events are matched by name; a name collision, or a server that reuses names across objects, would let another object's appearance overwrite the player's [V code] [I risk].
- **Where**: `Game.cpp:46239`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-002 Prediction without a reconciliation queue

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: hide latency while keeping server authority; click-to-move is deliberate latency hiding [I].
- **Now**: on reject the client discards all outstanding predicted steps (`m_cCommandCount = 0`, snap) rather than replaying them: simple and robust, but causes rubber-banding under lag [V 35545] [I effect].
- **Where**: `Game.cpp:35545`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9; systems/ui.md section 9

### CLIENT-STATE-003 Cache-miss refresh drops the event

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: apply the event once the object record is known.
- **Now**: the object stays stale until the full record arrives; under packet loss objects can freeze for a round trip [V] [I effect].
- **Where**: `MapData.cpp:1221-1224`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-004 bGetIsLocateable indexes out of range

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: test a tile within the map.
- **Now**: `CMapData::bGetIsLocateable` accepts pivot+40 / pivot+35 and indexes out of range [Vd].
- **Where**: `MapData.cpp:865-880`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-005 Functions with no caller

- **Program**: client
- **System**: client-state
- **Kind**: dead code
- **Intent**: ban a guild member notification; billing socket-closed accounting.
- **Now**: `NotifyMsg_BanGuildMan` has no caller; `_CalcSocketClosed` is a billing leftover with no caller [Vd].
- **Where**: `Game.cpp:5617-5674`; `Game.cpp:34749`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-006 Lag kick ignores keep-alive replies

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: detect a dead link.
- **Now**: the lag kick counts unanswered motion commands, not keep-alive replies, so an idle player behind a dead link is never kicked by this rule [Vd networking.md] [I effect].
- **Where**: `research/client/functions/networking.md`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-007 OnTimer chat-check stamp compares the wrong clock

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: check chat every 2 s.
- **Now**: `OnTimer` stores `m_dwCheckChatTime = m_dwTime` while comparing against `dwTime`; `m_dwTime` is the last-receive stamp (set at `1297`, `14746`, `15515`), so the 2 s cadence holds with steady traffic and degrades to every 1 s `OnTimer` tick while the server is silent [V code] [I effect].
- **Where**: `Game.cpp:4288-4289`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/client-state.md section 9

### CLIENT-STATE-008 bCheckExID overflows a 12-byte buffer

- **Program**: client
- **System**: client-state
- **Kind**: bug
- **Intent**: compare the stored sender with a name.
- **Now**: `memcpy` of `strlen(m_pExID->m_pMsg)` bytes into `char cTxt[12]` overflows if the stored message is over 12 bytes [Vd].
- **Where**: `Game.cpp:23074-23083`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: client/functions/characters.md

### CLIENT-STATE-009 Dead and faulty helper functions in CMisc

- **Program**: client
- **System**: client-state
- **Kind**: dead code
- **Intent**: direction stepping, field cipher and file checksum helpers.
- **Now**: `CMisc::GetDirPoint` steps the pointers (`*pY--`, `*pX++`), not the values, and has no caller; `CMisc::bEncode` and `bDecode` have no caller (the field cipher is not used by the client); `CMisc::_iGetFileCheckSum` sums 3 sampled bytes of a name-obfuscated file and is unused [V].
- **Where**: `Misc.cpp:117-129, 254-275, 434-476`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: client/functions/support.md
