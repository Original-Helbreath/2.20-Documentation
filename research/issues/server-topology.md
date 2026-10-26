# Issues: server-topology

Defects and divergences in how the gate, game servers and Log servers find, admit and shut down each other. Behaviour lives in [systems/server-topology.md](../systems/server-topology.md). The display-only world user cap is recorded once, in [SESS-007](session.md#sess-007-world-user-cap-parsed-never-enforced).

### TOPO-001 Shipped pieces do not fit together as configured

- **Program**: gate / game server / world log
- **System**: server-topology
- **Kind**: setup blocker
- **Intent**: the shipped gate, game server and official WLS start together.
- **Now**: gate cfg `build-date = 612` vs game server `DEF_BUILDDATE` 429 -> gate REJECT and the game server never starts; the official WLS expects registration ID 0x0512A3E3 vs 0x0512A3F4 sent; under `DEF_DEFENCEHACKING` the game server waits for `MSGID_WLSERVERFILECONTENTS`, which the official WLS never sends. The shipped `GateServer.cfg` also has no `game-server-list` (3 keys only), and the gate admits peers only by exact match against that list, so the connection is dropped at `bAccept`, before the build date is compared. Sample `bin/config/gserver.cfg` points the gate at `211.51.62.243:22`, the gate cfg says `203.234.215.188:6502` (deployment values) [V doc].
- **Where**: `research/servers/official-compat-220.md` 1; `research/servers/game-gate-protocol.md` 8; `GateCore.cpp` 0b994cd `143-146`, `231-283`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: yes
- **Source doc**: systems/server-topology.md section 9

### TOPO-002 DEFENCEHACKING is a world-server security switch, not anti-cheat

- **Program**: game server
- **System**: server-topology
- **Kind**: build
- **Intent**: stop clients from being sent to the internal Log port (where only game servers belong) after a server change [I].
- **Now**: every site gates start-up on the pushed WLserver config or uses its world port (comment "2002-12-8 World server security"); the name suggests anti-cheat [V]. Detail: `bAccept` refuses every client until the WLserver config arrives from the log server (`m_bIsWLServerAvailable`, set in MsgProcess at S 15240), together with the other config pushes (Item, Npc, Magic, Skill, Potion, Quest, BuildItem); a log server that does not push the WLserver file leaves the game server accepting nobody (merged from GSO-013; see SETUP-004). [V]
- **Where**: `SRV Game.cpp:411, 3699, 15180, 15236, 18655, 40760, 49005`; S Game.cpp:399-464, 15240
- **Evidence**: [V]
- **Blocks 2.20 setup**: yes (the switch needs the WLS config push, see TOPO-001)
- **Source doc**: systems/server-topology.md section 9; research/game-server/build-config.md:23; research/game-server/functions/network.md:13

### TOPO-003 Rejected gate peers keep receiving traffic

- **Program**: gate / game server
- **System**: server-topology
- **Kind**: bug
- **Intent**: a rejected game server is cut off.
- **Now**: the gate sets `m_bIsGameServer` before the build/duplicate checks, and the 2.20 game server stops at "STOPPED!" but keeps the socket, so it still gets stock messages and totals [V doc] [I effect].
- **Where**: `research/gate/subsystems.md` 3
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9

### TOPO-004 Gate shutdown tail re-sends notice 1

- **Program**: gate
- **System**: server-topology
- **Kind**: bug
- **Intent**: the countdown ends after the last step.
- **Now**: after the last step the count reaches 0 and the `default` branch re-sends notice 1 every 21 s while any server still has `m_bIsShutDown` [V doc].
- **Where**: `GateCore.cpp:1055-1066`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9

### TOPO-005 Heartbeat doubles as Log-link health

- **Program**: game server / gate
- **System**: server-topology
- **Kind**: bug
- **Intent**: heartbeat tells the gate the game server is alive.
- **Now**: `MSGID_GAMESERVERALIVE` is withheld while the Log link is down, so the gate's 30 s silence alarm reports a Log-server problem as a game-server problem [V].
- **Where**: `SRV Game.cpp:20716-20717`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9

### TOPO-006 Comment vs code on shutdown drain speed

- **Program**: game server
- **System**: server-topology
- **Kind**: bug
- **Intent**: drain 50 players per tick, as the comment states.
- **Now**: the timer disconnects 15 per 2 s [V].
- **Where**: `SRV Game.cpp:819` (comment), `3792`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9

### TOPO-007 Manager shutdown is dead

- **Program**: game server
- **System**: server-topology
- **Kind**: dead code
- **Intent**: remote shutdown from the manager tool.
- **Now**: `_Manager_Shutdown` starts with `return;`, so it never works; only the gate path does [V]. The intent was to start the exit process and send the log server MSGID_GAMESERVERSHUTDOWNED; the body after `return ;` never runs (merged from GSO-037 and COMMANDS-012). [V]
- **Where**: `SRV Game.cpp:40799`; S Game.cpp:40797-40824
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9; research/game-server/functions/network.md:236; research/systems/commands/overloaded.md:89

### TOPO-008 WLS shutdown notice never accepted by the MLS

- **Program**: main log / world log
- **System**: server-topology
- **Kind**: bug
- **Intent**: the WLS tells the MLS it is shutting down.
- **Now**: the code is compared at the wrong offset, so the notice is never accepted [V doc].
- **Where**: `official-mainserver.md` 5
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9

### TOPO-009 Start order is implicit

- **Program**: game server
- **System**: server-topology
- **Kind**: incomplete
- **Intent**: a game server starting before its dependencies should say what is missing.
- **Now**: if a game server starts before the WLS or gate, `bConnect` retries forever; nothing reports which dependency is missing beyond the per-flag "(!!!) STOPPED!" lines on the Log CONFIRM [V doc].
- **Where**: `research/servers/wire-format.md` "Connection behaviour"; `SRV Game.cpp:15126-15191`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/server-topology.md section 9
