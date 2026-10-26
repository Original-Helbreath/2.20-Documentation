# Issues: gate

Defects and divergences in the official Gate server 2.19 (as used with the v2.20 game server), read from `GATE` = `sources/gateserver-librehelbreath` at commit `0b994cd`: socket handling, registration, parties, shutdown and config parsing. Behaviour lives in [gate.md](../gate.md) and `research/gate/`. Wire-layer defects of the gate are in [network.md](network.md) ([NET-004](network.md#net-004-zero-length-frames-re-run-the-previous-message-in-the-gate), [NET-005](network.md#net-005-gate-full-ring-before-first-write-is-never-detected), [NET-008](network.md#net-008-gate-peer-table-slot-0-unused-and-slot-399-collides-with-the-timer-message)); rejected peers staying game servers is [TOPO-003](server-topology.md#topo-003-rejected-gate-peers-keep-receiving-traffic); the config/build-date mismatch is [TOPO-001](server-topology.md#topo-001-shipped-pieces-do-not-fit-together-as-configured). All entries: program = gate, blocks 2.20 setup = no unless stated.

### GATE-001 Listen failure is not detected

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: refuse to run when the listener cannot bind.
- **Now**: `bListen`'s return value is not examined; if the bind fails the process stays up, logs "Listening" and serves nobody [V].
- **Where**: `GATE Wmain.cpp:234-240`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no (but a port clash looks like a working gate)
- **Source doc**: gate/subsystems.md 2.1

### GATE-002 Handlers never check the body length

- **Program**: gate
- **System**: gate
- **Kind**: security
- **Intent**: validate received messages.
- **Now**: every handler reads fixed offsets from the received body and never checks the body length (`dwMsgSize` is used only for forwarding) [V].
- **Where**: `GATE GateCore.cpp:285-449`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md header notes

### GATE-003 Two senders never drop a stuck peer

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: a peer that cannot receive is dropped.
- **Now**: `SendMsgToAllGameServers` and the party replies in `PartyOperation` / `PartyManager` ignore every send result [V].
- **Where**: `GateCore.cpp:1015-1031`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 2.4

### GATE-004 Duplicate-name scan counts the registering peer's own slot

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: refuse a second server with an existing name.
- **Now**: the scan has no `i != iClientH` test, so a second REGISTER on the same connection fails as "already exists" (-2) even if the first was accepted, and an all-zero 10-byte name matches any unregistered peer [V].
- **Where**: `GateCore.cpp:509-520`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 3

### GATE-005 NULL dereference after a failed registration reply

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: log the rejection after replying.
- **Now**: both reject paths call `ResponseRegisterGameServer` first and then dereference the slot, which that function may have set to NULL on a fatal send result: a crash, with a narrow trigger (needs a full queue or dead socket) [V code] [I trigger].
- **Where**: `GateCore.cpp:499-505`, `512-518`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 3; gate/functions.md 1.3

### GATE-006 Registration parser: signed map count and unreachable tail

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: parse a map list of any length; every line reachable.
- **Now**: `iTotalMaps = (int)*cp` is a signed char: 128 or more maps gives a negative count, no skip, and the build date is read at the wrong place; lines 534-536 are unreachable [V code] [I effect].
- **Where**: `GateCore.cpp:479-481`, `534-536`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md 1.3

### GATE-007 A rejected server's name blocks a later correct server

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: a rejected registration leaves no trace.
- **Now**: rejected peers keep the name (504, 517), so a mismatched server named X makes a later correct server X fail with "already exists" until the first disconnects [V code] [I practical effect].
- **Where**: `GateCore.cpp:504, 517`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md 1.3

### GATE-008 Allow-list loader has no bound

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: at most 10 allowed servers, 15-character addresses.
- **Now**: entries are stored with `strcpy` and `iIPindex++` with no bound: an 11th entry writes past the 160-byte array, and a token over 15 chars overflows into the next entry. The fields that follow are those by declaration order, so the cfg key order matters [V] [I layout].
- **Where**: `GateCore.cpp:125-126`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 9; gate/functions.md 1.1

### GATE-009 MONITORALIVE MsgType is an unchecked array index

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: toggle one of 10 monitor status cells.
- **Now**: MsgType (u16) indexes `m_cMonitorStatus[10]` unchecked; out-of-range values write 1 or 2 outside the object [V].
- **Where**: `GateCore.cpp:409-423`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md 1.2

### GATE-010 Shutdown-one handler: unbounded strcpy and delete-in-loop

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: shut down the named server safely.
- **Now**: the name is copied with unbounded `strcpy` into a 22-byte buffer (323); the loop deletes entries and continues (no `return`/`break` after the delete, 344-355), and reuses `cTemp` (the name) for the error text via `memcpy(cTemp, m_cName, 20)` [V].
- **Where**: `GateCore.cpp:323, 344-355`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md 1.2

### GATE-011 SendMsgToMonitor aborts at the first failing monitor

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: every monitor gets every message.
- **Now**: a fatal send result deletes that monitor and returns, so the remaining monitors do not get this message [V]. The same early return exists in `CheckGlobalCommand` and `CheckAutoShutdownProcess`.
- **Where**: `GateCore.cpp:710-785`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 4, 7

### GATE-012 Shutdown countdown is not re-armed by a network request

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: a network-started timed shutdown behaves like F1+F4 (count 10, fresh clock).
- **Now**: `MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN` does not reset `m_iAutoShutdownCount` or set `m_dwAutoShutdownTime`. Once the count hit 0 a later network-started countdown never reaches the SHUTDOWNED step; it only repeats notice 1. On a monitor-started first shutdown `m_dwAutoShutdownTime` was never assigned, so the first step happens when an arbitrary value compares greater than 20000. Only F1+F4 resets the count [V code] [I that these are bugs].
- **Where**: `GateCore.cpp:365-386`, `630-631`, `1033-1106`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 6; gate/functions.md 1.6 (the 21 s notice repeat is also [TOPO-004](server-topology.md#topo-004-gate-shutdown-tail-re-sends-notice-1))

### GATE-013 Party id from the wire is an unchecked array index

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: only valid party ids are used.
- **Now**: `iPartyID` is a u16 (0..65535) used as an array index into 5000-entry tables (`bAddMember` reads `m_iMemberNumList[iPartyID]`; `bDeleteParty` does not check the range) [V]. PARTY-004 (merged here) also lists this with GATE-014 and GATE-017 as the gate party-registry weak points.
- **Where**: `PartyManager.cpp:119, 130`; `GateCore.cpp:1147-1272`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 5; gate/functions.md 1.7, 2; research/systems/party.md:155

### GATE-014 bAddMember decrements the wrong party

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: moving a member removes him from his old party only.
- **Now**: the found slot is cleared, but the code then decrements and, at <= 1 member, deletes the target party `iPartyID`, not the party that held the name. The old party's count stays one too high; the destination party loses one count and, if that leaves 1 or 0, is deleted (broadcast code 2) before the name is added, so rejoining one's own party can delete it. It also accepts unknown party ids and records no master [V].
- **Where**: `PartyManager.cpp:122-132`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 5; gate/functions.md 2

### GATE-015 iCreateNewParty leaks the id when no member slot is free

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: a failed create leaves no party.
- **Now**: the id count is incremented before a member slot is found; if the slot search fails the count stays at 1 [V].
- **Where**: `PartyManager.cpp:44-70`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/functions.md 2

### GATE-016 GameServerDown leaves small parties and is skipped on most deletes

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: clean up a lost server's party members and parties.
- **Now**: `GameServerDown` does not delete a party that falls to 0 or 1 members and notifies no one; it runs only from `OnClientSocketEvent`, so every other peer deletion (fatal-send paths, wrong-MsgType deletes) leaves its members in the registry [V]. A one-member party can also be left by `bAddMember` onto an unknown id.
- **Where**: `PartyManager.cpp:250-266`; `GateCore.cpp:221`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 5, 7; gate/functions.md 2

### GATE-017 Member server index is never updated on a crossing

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: track which game server currently hosts a party member.
- **Now**: `m_iIndex` is the socket slot of the server that created the entry or added the member and is not updated when the member crosses; if the original server dies the member is removed even though he plays elsewhere. Slot numbers are also reused by new connections [V code] [I effect].
- **Where**: `PartyManager.h`, `PartyManager.cpp`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 5; gate/functions.md 2

### GATE-018 No authentication beyond the IP allow-list and a fixed passphrase

- **Program**: gate
- **System**: gate
- **Kind**: security
- **Intent**: only operators may order shutdowns or reboots.
- **Now**: any listed IP may send any message in the dispatch table; authentication of the network requests is a fixed MsgType `0x5A8E` plus (for the two shutdown requests only) the literal `1dkld$#@01` at offset 6. A wrong MsgType deletes the sender; a wrong passphrase is ignored; REBOOT / EXEC_1 / EXEC_2 check the MsgType only [V].
- **Where**: `GateCore.cpp:308-407`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 3, 6

### GATE-019 The logging thread reads shared state without locking

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: a consistent user-count log.
- **Now**: the logging thread reads `m_pClientList[1..499]` and the `CClient` fields without locking while the main thread mutates them [V].
- **Where**: `Wmain.cpp:44-73`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 1

### GATE-020 Help text advertises F1+F4 as an immediate shutdown

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: the on-screen help describes the key pair.
- **Now**: it reads "[F1 + F4] Shut down all game servers" although that pair only starts the timed countdown [V].
- **Where**: `Wmain.cpp:296`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 8

### GATE-021 Connection-loss lines never reach a file

- **Program**: gate
- **System**: gate
- **Kind**: incomplete
- **Intent**: a persistent record of connection loss.
- **Now**: log lines go to the window only; the only persistent logs are `ItemEvents.log` and the three files in section 9, so a connection-loss line is lost on restart [V].
- **Where**: gate/subsystems.md section 8
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 8

### GATE-022 F6 falls through into F12

- **Program**: gate
- **System**: gate
- **Kind**: bug
- **Intent**: F6 sends notice 1 only.
- **Now**: the F6 case falls through F7 into F12 and clears the F12 flag [V].
- **Where**: `GateCore.cpp:648-666`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/subsystems.md 10

### GATE-023 Unused constants, globals and functions

- **Program**: gate
- **System**: gate
- **Kind**: dead code
- **Intent**: n/a.
- **Now**: `DEF_MAXGAMESERVERS` (20) and `G_iQuitProgramCount` are unused [V by grep]; `XSocket::bConnect` and `iGetSocket` are never called by the gate; `m_pAddr`, `m_iPortNum`, `m_bIsAvailable` are unused; the registered address and port are parsed but unused; `bInitBufferSize` is called a second time after the constructor (reallocates both 30008-byte buffers) [V].
- **Where**: `GateCore.h:27`; `XSocket.cpp:122-169, 542-545`; `GateCore.cpp:244`
- **Evidence**: [V by grep]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/data-structures.md; gate/functions.md

### GATE-024 Header message ids collide

- **Program**: gate
- **System**: gate
- **Kind**: data
- **Intent**: unique message ids.
- **Now**: three pairs of IDs in the gate's `NetMessages.h` collide (`MSGID_REQUEST_IPTIME` / `MSGID_REQUEST_SELLITEMLIST` 0x2900AD30; `MSGID_EVENT_COMMON` / `MSGID_MAGICCONFIGURATIONCONTENTS` 0x0FA314DB; `MSGID_COMMAND_COMMON` / `MSGID_SKILLCONFIGURATIONCONTENTS` 0x0FA314DC; see `../shared/messages/README.md`); none is among the IDs the gate dispatches [V by reading the header].
- **Where**: `GATE NetMessages.h`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: gate/data-structures.md

### GATE-025 gethostname fallback for the listen address never runs

- **Program**: gate
- **System**: gate
- **Kind**: dead code
- **Intent**: fill in the gate's address from `gethostname()` when the cfg gives none.
- **Now**: the fallback tests an array against NULL, so it never runs; `gate-server-address` is required in practice [V code] [I effect].
- **Where**: `GATE GateCore.cpp:157-175`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no (set `gate-server-address`)
- **Source doc**: servers/gate-server.md section 2

### GATE-026 GSM_REQUEST_SETFORCERECALLTIME falls through into default

- **Program**: game server
- **System**: gate
- **Kind**: bug
- **Intent**: process a stock-message record and continue with the next record.
- **Now**: the case has no `break`: it falls into `default`, which sets `bFlag = TRUE` and ends the record loop, so any record after it in the same stock message is dropped; send it last in a batch. Whether the sender ever batches after it is unverified [V code] [U].
- **Where**: `SRV Game.cpp:49744-49754`, loop `:49350`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: servers/game-gate-protocol.md (GSM table)
