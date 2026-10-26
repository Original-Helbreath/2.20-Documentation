# game-server (index)
v2.20 HGserver (export of commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]; reports itself as v2.19). Topology and other servers: `servers.md`.
Messages: `shared/messages/`. File formats: `shared/formats/`.

- `game-server/mechanics.md` - formulas and rules (stats, exp, skills, combat, magic, PK, drops, economy, crafting) with code lines; read before reasoning about any gameplay number.
- `game-server/build-config.md` - the #define switches with their main sites (not exhaustive, see its region-chain section), the project file list, dead code; read when a behaviour depends on the build or a function seems missing.
- `game-server/data-structures.md` - CGame/CClient/CNpc/CMap/CItem fields, limits and timer constants; read before touching server state or array sizes.
- `game-server/functions/startup.md` - bInit, GServer.cfg, map registration, world/WL config; read for start-up and config loading.
- `game-server/functions/network.md` - sockets, message queue, MsgProcess/ClientCommonHandler dispatch, Log/gate links, notify senders; read to find who handles a MSGID.
- `game-server/functions/character.md` - player init, levels, regen, skills, side/hunter mode; read for player state changes.
- `game-server/functions/combat-magic.md` - attacks, spells, resistances, death and PK handlers; read for damage code.
- `game-server/functions/items.md` - equip, shops, warehouse, exchange, upgrade, crafting, alchemy, fishing, mining, farming, drops; read for item flows.
- `game-server/functions/npc.md` - NPC creation, AI behaviours, mob generators; read for monster behaviour.
- `game-server/functions/map-movement.md` - movement, map data, teleport, weather, dynamic objects, CMap members; read for movement and map code.
- `game-server/functions/guild-party.md` - guilds and parties; read for social systems.
- `game-server/functions/crusade-quests.md` - crusade, occupy flags, fightzone, energy sphere, quests; read for war and quest code.
- `game-server/functions/chat-gm.md` - chat channels and GM commands; read for chat or admin features.
- `game-server/functions/persistence.md` - player record encode/decode and saving; read with `shared/formats/`.
- `game-server/functions/timers.md` - OnTimer schedule, per-client 3 s tick, delay events; read for anything periodic.
- `game-server/functions/ui-logging.md` - window, log files, item/PK/crusade logs; read for logging.
- `game-server/functions/support.md` - Wmain, XSocket framing, Misc, StrTok, Msg, small-class constructors, plus two CGame helpers (iDice, _bGetIsStringIsNumber) and the Wmain functions (WinMain, Initialize, OnDestroy, ThreadProc); read for low-level helpers.
