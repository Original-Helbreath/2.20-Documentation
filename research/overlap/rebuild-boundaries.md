# rebuild boundaries suggested by the overlaps

Conventions: see `../overlap.md`. Observations about the current design, not a plan; the plan is
`../servers/recreation-plan.md`.

## 1. Transport library (strongest) [V]
All five programs already speak one framing and cipher through one XSocket class (shared-code.md).
The copies differ only in queueing/blocking behaviour and key choice. A single transport module
(frame, cipher, send queue, reconnect) covers every link in connections.md.

## 2. Protocol catalogue [V]
One message-ID space is copied into three headers and two binaries; 509 of 516 shared constants
agree and no ID has two values (shared-definitions.md). The natural contract is one generated
catalogue with per-link families: client-log, client-game, game-log, game-gate, game-game (GSM),
main-world (`../shared/messages/`). Per-program extras (log result codes, monitor IDs) are just
families only one side uses.

## 3. Persistence format and its single owner [V/I]
Character and guild files are written only by the WLS, encoded only by the GS, and partly read by
the WLS and MLS (shared-data.md). The codec plus the new-character template is one contract spread
over two programs; keeping it in one library used by both is what the current split implies.
Account storage is separate and MLS-only.

## 4. Content tables [V]
Item/NPC/magic/skill/potion/quest/build tables are authored once, stored by the WLS, decoded by the
GS, and echoed by name in client display cfgs. Two consumers (server rules, client display) of one
source: a content library with a server view and a client view.

## 5. Routing and account state [V]
Map -> server routing and account state live in both log servers (MLS coarse, WLS fine) and are fed
by GS registration. The gate gets the same map list and ignores it. One routing/session service is
the boundary the current flows draw.

## 6. Inter-server bus [V]
The gate is a pure relay plus the party registry. GSM records are opaque to it, so the bus contract
is the GSM container plus party ops (`../servers/game-gate-protocol.md` 4-5).

## 7. Client-only and server-only modules [V]
Rendering, UI, audio, client-state are client-only; every gameplay rule is GS-only. Same-named
classes (Item, Magic, Skill, BuildItem, Tile) are independent records, not shared code; a rebuild
that wants them shared needs a designed common record (shared-code.md).

## 8. Map files [V]
`.amd` is the only binary format parsed on both sides with two loaders; a shared reader is the
smallest shared asset library.

## Open questions
- Whether the MLS accepts a WLS on its public port [U, runtime test; official-mainserver.md 1].
- Whether client-side limits (bank 121, guild members 32, recipes 100) are display choices or
  constraints the server relies on [I].
- MLS behaviour when a WLS link drops [U].
