# servers (index)

Reconstruction of the Helbreath v2.20 server roles and protocols (Log server, Gate server,
game server, client), for recreating the missing Log server and checking the Gate server.
Source abbreviations and the verified/inferred convention are defined in `servers/topology.md`
(section "Sources and conventions"); every other doc uses them.

- `servers/topology.md` - which programs exist (Client, HGserver, Gate, Log = MLS + WLS), who
  connects to whom on which port, and the verdict on both user hypotheses; read first.
- `servers/wire-format.md` - XSocket framing (3-byte header, key, length), the byte-wise cipher,
  MsgID/MsgType layout, string conventions; read before implementing any socket code.
- `servers/client-log-protocol.md` - every client <-> Log message: login, character list and
  slot layout, create/delete character, password change, enter game, server change, error
  codes; read when building the client-facing side of the Log server.
- `servers/game-log-protocol.md` - every HGserver <-> Log message: registration, config-file
  push, player data load/save (binary and text layouts), guilds, logout, account checks; read
  when building the game-server-facing side of the Log server.
- `servers/game-gate-protocol.md` - every HGserver <-> Gate message, with layouts taken from both
  the official gate source and the game server: registration, heartbeat, user counts, stock
  (broadcast) messages and every GSM record, party operations, shutdown; read when touching
  inter-server traffic.
- `servers/gate-server.md` - what the official v2.19 Gate server does (config, IP allow-list,
  build-date check, monitors, party manager, timers, admin commands) and how to run it with the
  2.20 game server; read before deciding to rebuild or reuse the gate.
- `servers/world-log-server.md` - the 2.20 split of the Log server into Main Log Server and World
  Log Server, what is known and unknown about the MLS <-> WLS link; read when deciding how many
  Log processes to build.
- `servers/official-mainserver.md` - static map of the official MainServer.exe (MLS v3.51): ports,
  config, dispatcher, client messages, account file format, the full Main <-> World protocol; read
  when reusing or re-implementing the MLS or the MLS <-> WLS link.
- `servers/official-worldserver.md` - static map of the official WorldServer.exe (WLS V2.15): game
  server and client handling, config push, character and guild file formats, INIT/WAIT/PLAYING/NULL
  state machine; read when reusing or re-implementing the WLS.
- `servers/official-compat-220.md` - item-by-item compatibility of both binaries with the 2.20 game
  server and client, the verdict and the byte patch list; read before deciding to run the official
  log servers.
- `servers/decomp/README.md` - reconstructed pseudo-C of the core MLS and WLS handlers (not original
  source); read when writing code from the official behaviour.
- `servers/version-diff.md` - 2.03 vs 2.20 (and gate 2.19 vs 2.20) protocol differences; read
  when targeting a version other than 2.20.
- `servers/persistence.md` - what the Log server must store, the verified character text-file
  format, the proposed account and guild files, folder layout, plaintext passwords; read
  before designing storage.
- `servers/ui.md` - the Win32 look of the official HGserver and Gate windows (class, title,
  size, painted log list, hotkeys, log files) as the template for the Log server window; read
  when building the Log server UI.
- `servers/recreation-plan.md` - minimal Log and Gate behaviour for one game server plus one
  client, build order, effort estimate; read when planning the implementation.
