# Issues: network

Defects and divergences in the wire layer: framing, the cipher, socket events, the dispatchers and the gate's ring buffers. Behaviour descriptions live in [systems/network.md](../systems/network.md); this file only records what someone might act on.

### NET-001 Cipher is obfuscation, not security

- **Program**: client / game server / gate
- **System**: network
- **Kind**: security
- **Intent**: defeat casual packet editing [I].
- **Now**: the key travels in clear in byte 0 and the transform is invertible from the frame alone. Only client output is encrypted; server replies are plain [V doc].
- **Where**: `research/servers/wire-format.md`
- **Evidence**: wire-format doc analysis [V doc].
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9

### NET-002 Field-level obfuscation (CMisc::bDecode) is a no-op

- **Program**: game server / world log
- **System**: network
- **Kind**: dead code
- **Intent**: obfuscate name, account and password inside the init-player record.
- **Now**: `CMisc::bDecode` is called on name, account and password in `RequestInitPlayerHandler` but returns without transforming; the world log server copy also returns early [V doc]. `CMisc::bEncode` is a no-op too: both bEncode and bDecode return before doing anything (S Misc.cpp:232-261); callers are RequestInitDataHandler and RequestInitPlayerHandler (merged from GSO-002; see NET-001 for the cipher). [V]
- **Where**: `SRV Game.cpp:1527, 1539, 1548`; `MLS 0x411add`; `research/game-server/functions/character.md` "bug 13.3"; SRV Misc.cpp:232-261
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9; research/game-server/mechanics.md:232; research/game-server/functions/support.md:186, :192; research/game-server/functions/character.md:13

### NET-003 Envelope ID collision with config pushes

- **Program**: game server / main log / world log
- **System**: network
- **Kind**: bug
- **Intent**: distinct message ids per direction and purpose.
- **Now**: the envelope id collides with config-push ids (network.md section 4); it works only because the dispatcher switches on the source socket first. A merged single-port Log server must keep game-server and client traffic apart by socket, not MsgID [V].
- **Where**: network.md section 4
- **Evidence**: [V]
- **Blocks 2.20 setup**: no (matters only for a merged single-port Log server)
- **Source doc**: systems/network.md section 9

### NET-004 Zero-length frames re-run the previous message in the gate

- **Program**: gate
- **System**: network
- **Kind**: bug
- **Intent**: a zero-length frame should be ignored or rejected.
- **Now**: it re-runs the previous message [V doc].
- **Where**: `research/gate/subsystems.md` 2.3
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9

### NET-005 Gate full-ring before first write is never detected

- **Program**: gate
- **System**: network
- **Kind**: bug
- **Intent**: a full send ring is reported as a socket event so the peer can be dropped.
- **Now**: `iSendMsg` returns -3/-4, not a `DEF_XSOCKEVENT_*` code, so the full ring goes unnoticed [V doc].
- **Where**: `research/gate/functions.md` 5
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9

### NET-006 Game server multithreading is dead

- **Program**: game server
- **System**: network
- **Kind**: dead code
- **Intent**: a worker thread running the timer.
- **Now**: the `ThreadProc` test `if (G_pGame = NULL)` is an assignment and no thread is created; the design is single-threaded in practice [V doc]. Located at S Wmain.cpp:58-66 (line 61) (merged from GSO-040). [V]
- **Where**: `research/game-server/functions/timers.md` OnTimer; SRV Wmain.cpp:58-66
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9; research/game-server/functions/timers.md:16; research/game-server/functions/support.md:400

### NET-007 Unknown message saves the character after init

- **Program**: game server
- **System**: network
- **Kind**: security
- **Intent**: drop a client that sends garbage.
- **Now**: the default branch deletes the client with `bSave = TRUE`, so an initialised hostile client still gets its state persisted; an uninitialised one gets NOSAVELOGOUT [V].
- **Where**: `SRV Game.cpp:15085`; `Game.cpp:2775-2802, 15083-15086`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 9

### NET-008 Gate peer table: slot 0 unused and slot 399 collides with the timer message

- **Program**: gate
- **System**: network
- **Kind**: bug
- **Intent**: up to 500 usable peer slots.
- **Now**: at most 499 peers (slot 0 unused) and slot 399 collides with the timer message [V doc].
- **Where**: `research/gate/subsystems.md` 2.2
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/network.md section 7
