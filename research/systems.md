# systems (index)

Helbreath 2.20 grouped by design intent. Each doc: purpose, subsystems, functions, commands, data, flows,
rules, couplings, intent-vs-implementation gaps, open questions. Read the one system you need.

## World
- `systems/map.md` - maps, .amd loading, teleports, cross-server handoff, access rules, weather; read for anything about where a player is.
- `systems/tile.md` - tile flags, occupancy, item piles, field effects; read before touching map cells or collision.
- `systems/movement.md` - client prediction, server move checks, resync, speed checks; read for walk/run or desync issues.
- `systems/entity.md` - object ids, appear/disappear, spawning, NPC AI, summons, taming; read for monsters, NPCs or view updates.
- `systems/gathering.md` - fishing, mining, farming; read for resource gathering.

## Combat and character
- `systems/combat.md` - attacks, hit and damage, criticals, death and corpses; read for any fight logic.
- `systems/magic.md` - spells, casting, effect types, fields, summon control; read for spells.
- `systems/skills.md` - 24 skills, gain, the 700-point cap; read for skill progression.
- `systems/character.md` - stats, exp, level-up, majestic points, regen, hunger, 3-second upkeep; read for progression.
- `systems/pk-law.md` - citizenship, criminal tiers, guards, jails, bounty, hunter mode; read for PK and town alignment.

## Economy
- `systems/item.md` - item definitions, bag, weight, equip, use, floor, durability, drops, item log; read for any item work.
- `systems/crafting.md` - alchemy, manufacturing, upgrades; read for crafting.
- `systems/shop.md` - NPC shops, buy/sell/repair, prices, city funds; read for shops.
- `systems/trade.md` - give and player exchange; read for player-to-player transfers.
- `systems/warehouse.md` - bank storage; read for warehouse.

## Social and events
- `systems/chat.md` - chat modes, filter, whisper, cross-server relay, mute; read for chat.
- `systems/party.md` - party lifecycle on the gate, exp share; read for parties.
- `systems/guild.md` - guild lifecycle, membership tickets, guild files; read for guilds.
- `systems/crusade.md` - crusade schedule, structures, war points, commander tools; read for the war event.
- `systems/arena.md` - fight-zone booking and rules; read for arenas.
- `systems/quest.md` - quest offer, tracking, reward; read for quests.

## Infrastructure (spans all five programs)
- `systems/network.md` - sockets, envelopes, dispatch, gate relay, broadcast; read before touching any message.
- `systems/session.md` - login, character select, enter game, server change, timeouts; read for connection flow.
- `systems/persistence.md` - record encoding, load and save paths, save cadence; read before touching saved data.
- `systems/server-topology.md` - start-up, registration, config push, heartbeat, shutdown; read before running the servers.
- `systems/admin.md` - admin levels, GM commands, monitoring, logging; read for GM tools.

## Client presentation
- `systems/rendering.md` - DirectDraw, frame pipeline, sprites, blitters, effects (links client/rendering/ and client/resolution/); read for drawing.
- `systems/ui.md` - screen state machine, dialogs, HUD, cursor, input; read for UI.
- `systems/audio.md` - DirectSound, sound triggers, music; read for sound.
- `systems/client-state.md` - how the client mirrors server state, game loop; read for client-side logic.

## Commands
- `systems/commands.md` - every command, notify and slash command by intent and system; read to find what drives a system.
