# issues (index)

Scope: the documentation in `research/systems/`, `research/client/`, `research/game-server/` and the other topic folders describes intent and current behaviour. This register holds the defects: bugs, dead code, half-finished features, client-only checks, security gaps and setup blockers, one entry per defect in `issues/<system>.md`. Entries use the id `<SYS>-<nnn>`; docs link to them by anchor. An entry that duplicated another was merged into the owning system's entry and left as a stub ("Merged into ...") so old anchors still resolve. Counts below are live entries (stubs excluded). Read the one file you need.

Start here: `issues/setup-2.20.md` - everything that blocks a working 2.20 client + game server + gate + log servers setup (17 entries).

## World
- `issues/map.md` - maps, .amd loading, teleports, cross-server routing, access rules (9 entries); read for where a player is or why a teleport misbehaves.
- `issues/tile.md` - tile grid, flags, occupancy, item stacks, dynamic objects (5 entries); read for touching map cells or collision.
- `issues/movement.md` - walk/run, collision, position sync, speed-hack checks (5 entries); read for walk, run or desync faults.
- `issues/entity.md` - object ids, view updates, spawning, NPC AI, summons, taming (11 entries); read for monsters, NPCs or view updates.
- `issues/gathering.md` - fishing, mining, farming (6 entries); read for resource gathering faults.

## Combat and character
- `issues/combat.md` - attacks, hit and damage, criticals, death and corpses (6 entries); read for fight logic.
- `issues/magic.md` - spells, mana, casting, effects (3 entries); read for spell faults.
- `issues/skills.md` - skill list, gain, the 700-point cap (5 entries); read for skill progression.
- `issues/character.md` - stats, exp, level-up, hunger, regeneration, majestic points (7 entries); read for progression.
- `issues/pk-law.md` - PK and criminal status, town alignment, jails, bounty, hunter mode (7 entries); read for PK and town alignment.

## Economy
- `issues/item.md` - item definitions, inventory, floor, use, durability, drops, item log (5 entries); read for any item work.
- `issues/crafting.md` - alchemy, manufacturing, upgrades (8 entries); read for crafting.
- `issues/shop.md` - NPC shops: buy, sell, repair, prices, city funds (10 entries); read for shops.
- `issues/trade.md` - give and two-way exchange (including warehouse deposit distance) (9 entries); read for player-to-player transfers.
- `issues/warehouse.md` - bank/warehouse withdraw and the persistence flag (1 entries); read for warehouse.

## Social and events
- `issues/chat.md` - chat modes, curse filter, whisper, relay, mute (11 entries); read for chat.
- `issues/party.md` - party join, local mirror, exp share, the legacy path (6 entries); read for parties.
- `issues/guild.md` - guild create, admission, departure, disband, guild notices (9 entries); read for guilds.
- `issues/crusade.md` - crusade loop, results, structures, command tools, retired occupy flags (9 entries); read for the war event.
- `issues/arena.md` - fight-zone booking, tickets, slot clock (9 entries); read for arenas.
- `issues/quest.md` - quest offer, accept, tracking, reward, Quest.cfg (8 entries); read for quests.

## Infrastructure
- `issues/network.md` - framing, cipher, socket events, dispatchers, no-op obfuscation, dead threading (8 entries); read for any wire-level fault.
- `issues/session.md` - login hand-off, player init, duplicate login, force disconnect (8 entries); read for connection flow.
- `issues/persistence.md` - save/load, fallback save, bank, account storage (5 entries); read for saved data.
- `issues/server-topology.md` - start-up, registration, DEFENCEHACKING, shutdown (9 entries); read for running the servers.
- `issues/admin.md` - GM tooling, monitor and manager links, anti-hack checks, shut-up (10 entries); read for GM tools.
- `issues/gate.md` - the Gate server: sockets, registration, party registry, shutdown, config (26 entries); read for the gate.

## Client
- `issues/rendering.md` - DirectDraw layer, blitters, frame pipeline (13 entries); read for drawing faults.
- `issues/ui.md` - dialog boxes, input, shortcuts (4 entries); read for UI faults.
- `issues/audio.md` - DirectSound, pan and volume, music (7 entries); read for sound.
- `issues/client-state.md` - object matching, prediction, map cache, lag detection (9 entries); read for client-side logic.
- `issues/client-resolution.md` - 640x480 assumptions in HUD, dialogs and input (7 entries); read for changing the client resolution.

## Cross-cutting
- `issues/commands.md` - dead, unused or stubbed MSGIDs, notifies and slash commands not owned by one system (5 entries); read for a command that does nothing.
- `issues/game-server-other.md` - game-server function-map defects with no home system: drops, charisma, build switches, logging helpers, CMisc (22 entries); read for a game-server fault not found in a system file.

## Setup
- `issues/setup-2.20.md` - setup blockers and byte patches for a working 2.20 setup (17 entries); read before building or running anything.

Total live entries: 299 (stubs: 49).
