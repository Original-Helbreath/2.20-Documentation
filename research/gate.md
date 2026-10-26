# gate (index)

Function map and subsystem overview of the official Helbreath Gate server v2.19 (used with the v2.20 game server),
read from `sources/gateserver-librehelbreath` at commit `0b994cd`. Builds on `servers/gate-server.md` (behaviour and
how to run it) and `servers/game-gate-protocol.md` (wire layouts); does not repeat them.

- `gate/functions.md` - all 78 functions with file:line, purpose, callers, callees and the MSGIDs each handles or sends;
  read when you need to know what exactly one function does, who calls it, or when porting/rewriting a piece of the gate.
- `gate/subsystems.md` - threads, sockets and event loop, admission and registration, relay, parties, shutdown and
  monitor control, failure/cleanup matrix, UI, config and files; read when you need the whole-system picture or the
  behaviour under failure.
- `gate/data-structures.md` - classes, fields, constants and limits, wire layouts as the gate parses them, MsgID table;
  read when sizing, laying out buffers or checking a limit.

Tags: [V] verified from the cited line, [I] inferred, [U] unverified. Corrections to the older gate docs
(`servers/gate-server.md`, `servers/game-gate-protocol.md`): `gate/subsystems.md` section 10.
