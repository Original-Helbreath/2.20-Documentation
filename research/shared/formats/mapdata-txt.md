# mapdata .txt (server map info)

Per-map text file `mapdata\<name>.txt` next to the `.amd` (format of the `.amd`: `amd-map.md`).
Read only by the game server: `CGame::__bReadMapInfo` `.claude/scratch/srv220/Game.cpp:25556`
(path built `:25600-25603`, called through `_bReadMapInfoFiles` `:9976` from `:20230`) [V].
Hand-written; nothing writes it [I]. The client does not use it. Tags: [V] read in code, [I] inferred.

## Syntax

Separators `"= \t\n"` (`:25559`). File starts `[MAP-INFO]`; `[END-MAP-INFO]` (`:27086-27093`) is optional: only 3 of the 59 sample files
(47 `.txt` + 12 `.TXT`) contain it, and without it the token loop ends at end of file and the post-check
(`:27095-27099`) still passes [V].
Comments `//...` work because their first token is no key. Keys matched by `memcmp` prefix
(`:26978-27086`) in a chain of independent `if`s, so a token can match several keys and the last
match wins: `npc-avoidrect` first sets the `npc` mode (`:26989`) and is then overridden by its
own test (`:27002`), which is correct by luck of order [V]. Hard-coded name rules also apply: maps named `fight*` become fight zones and
`icebound*` always snows (`:25586-25592`) [V].

## Keys

Usage counts are over the 2.20 sample `srv220/mapdata/*.txt` [measured].

| Key | Fields (in order) | Count in samples | Code |
|---|---|---|---|
| `teleport-loc` | SrcX SrcY DestMap DestX DestY Dir | 783 | `:25646-25706` |
| `waypoint` | Index X Y | 570 | `:25724-25756` |
| `npc` | Name MoveType Waypoint1..10 NamePrefix(char) | 16 | `:25769-25815` |
| `random-mob-generator` | Enabled(0/1) Level | 22 | `:25832-25844` |
| `maximum-object` | N | 41 | `:25860` |
| `npc-avoidrect` | Index Left Top Right Bottom (fills `m_rcMobGenAvoidRect`) | 2 | `:25876-25933` |
| `spot-mob-generator` | Index Type, then Type 1: Left Top Right Bottom MobType MaxMobs; Type 2: Waypoint1..10 MobType MaxMobs | 73 | `:25951-26086` [type split I from the two branches] |
| `map-location` | Name (max 10 chars) | 59 | `:26095` |
| `initial-point` | Index X Y | 71 | `:26110-26140` |
| `no-attack-area` | Index Left Top Right Bottom | 54 | `:26158-26210` |
| `fixed-dayornight-mode` | 0/1 | 39 | `:26218` |
| `fish-point` | Index X Y | 73 | `:26235-26268` |
| `max-fish` | N | 5 | `:26283` |
| `type` | map type | 10 | `:26295` |
| `level-limit` | N | 8 | `:26307` |
| `mineral-generator` | Enabled Level | 5 | `:26322-26333` |
| `mineral-point` | Index X Y | 183 | `:26349-26382` |
| `max-mineral` | N | 5 | `:26397` |
| `upper-level-limit` | N | 10 | `:26409` |
| `strategic-point` | Index Side Value X Y | 3 | `:26423-26480` |
| `energy-sphere-creation-point` | Index Type X Y | 0 | `:26497-26542` |
| `energy-sphere-goal-point` | Index Result AresdenX AresdenY ElvineX ElvineY | 0 | `:26559-26629` |
| `strike-point` | Index X Y HP EffectX/Y x5 RelatedMap | 8 | `:26646-26819` |
| `item-event` | Index ItemName Amount TotalNum Month Day Type Mob... `EOL` | 0 | `:26848-26949` |
| `energy-sphere-auto-creation` | flag, no value | 0 | `:27073` |
| `mobevent-amount` | N | 0 | `:26967` |

Field names come from the members assigned at each line (for example
`m_pTeleportLoc[...]->m_sSrcX`) [V].

## Gaps

- Value domains (map `type` codes, mob type numbers, spot generator types) are not traced.
- Retail client data has no `.txt` map info (client does not need it) [V listing].
