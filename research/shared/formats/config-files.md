# Server config files: inventory of copies and chosen source

Which server config/data files the 2.20 game server (`sources/hgserver-librehelbreath/src/Game.cpp`,
G.cpp below) consumes, where copies exist, and which copy to use. Format details live in
`server-content-cfg.md` and `server-program-cfg.md`. Claims: verified = measured by script or read in code.

## Key findings

- The copies in `sources/hgserver-librehelbreath/bin/config/`, `.claude/scratch/srv220/` and
  `downloads/sf_203/.../hbs2/` are byte-identical (md5 equal for all 15 files). They are the **2.03**
  release's files, not 2.20. The 2.20 source zips ship no cfg at all (`220HGServersrc.zip` holds only a README).
- Item/Npc/Magic/Skill/Quest/Potion/BuildItem/DupItemID/Teleport/NpcItem/WLServer/World are **not read
  from disk by the game server**: the Log server pushes their text and the game server parses it
  (dispatch G.cpp:15201-15267). Only GServer.cfg, Crusade.cfg, WarSchedule.cfg, notice.txt, CrusadeGUID.txt
  and mapdata\*.txt/.amd are opened by the game server itself.
- 2.03 `portion.cfg` uses keyword `portion`; the 2.20 parser tests `memcmp(token,"potion",5)` (G.cpp:38713),
  so it would NOT parse. Use 3.51 `Potion.cfg`.
- 2.03 `Npc.cfg` has 24 fields per record, matching the 2.20 parser (max case 24, G.cpp:19677). 3.51 has 25 and
  3.82 has 29, so they do not fit 2.20 as-is, but they hold many more NPCs (30 vs 105 vs 165).
- Encoding: 2.03 Item/Item2/Item3/Npc/Magic/Skill/Quest/portion/BuildItem names are **cp949 (Korean)**; 3.51
  and 3.82 names are ASCII English except 3.51 Item.cfg/Item3.cfg and 3.82 Item.cfg/Quest.cfg/Item3.cfg
  (contain non-ASCII bytes that fail utf-8 and cp949 cleanly; probably cp1252/other, unverified).
  Key lookups use numeric IDs, so names differ freely (Item 2.03 vs 3.51: 112 common IDs, 1 identical name).
- Name-dependent code (breaks if a name is translated): G.cpp:9531 `memcmp(item name, "<Korean Giant-Sword>")`
  in the item loader; G.cpp:5979/6145/7105/7682 `memcmp(item name,"Gold",4)`; map-name tests ("fight",
  "icebound" G.cpp:25586-25591, "aresden/elvine/elvfarm/arefarm/middled1n" G.cpp:29583-29587/29771-29774,
  "fightzone" G.cpp:50808, "resurr"). Item names must keep the English "Gold" (3.51/3.82 do; 2.03 Korean? check
  Gold row), and map names must stay as in mapdata. Chat commands accept Korean and English (G.cpp:10762-10828).
- Potion names: 14 common IDs, same numeric rest (14/14) between 2.03 and 3.51, names differ (Korean vs English).

## Table

Version labels: 2.03 = `sf_203` zip (hbs2); 3.51 = `351configsandsqldump.rar` (Jan 2005) and `351SqlFiles.zip`
(eXtasis-GoldenBoy pack, a modified private pack); 3.82 = `helbreath-v3.82.zip` (Equilibrium-based modded
pack, not vanilla). Extracted to `downloads/configs/<archive>/`. "lib" = librehelbreath bin/config.

| file | read by (G.cpp line) | copies found (version, size) | chosen copy | differences | notes |
|---|---|---|---|---|---|
| GServer.cfg | bReadProgramConfigFile 5053 (with address) / 5202 (without); called 791 | lib/srv220/2.03 `gserver.cfg` 551; 3.51sql `GServer.cfg` 335, `GServer-example.cfg` 630; 3.82 GameServers/*/gserver.cfg ~700-950 | lib `gserver.cfg` (2.03; same keywords game-server-name/address/port, log-server-address, internal-log-server-port, game-server-map, gate-server-*) | later copies add keys read by HBX code only | file name case differs (gserver vs GServer); Windows is case-insensitive |
| Crusade.cfg | bReadCrusadeStructureConfigFile 5354; called 20233 | lib/srv220/2.03 974 (16 structures); 3.51sql 1377 (21); 3.82 GameConfigs 2342 | lib (2.03) | only `crusade-structure = n map type x y` lines; later copies add structures | parses identically |
| WarSchedule.cfg | bReadCrusadeScheduleConfigFile 50691; called 20239 | 3.51sql 49 bytes only (`game-server-name = middleland`, `schedule = 6 22 30`); 3.82 `Schedule.cfg` 168 (HBX name, different reader) | 3.51sql WarSchedule.cfg | none to compare | keywords game-server-name/schedule match parser |
| CrusadeGUID.txt | bReadCrusadeGUIDFile 50460; called 20237 | srv220/lib `GameData\CrusadeGUID.Txt` 40; 3.51sql 42; 3.82 62 | lib | runtime state (GUID, winner-side), regenerated | keywords CrusadeGUID / winner-side (50510-50511) |
| notice.txt | bReadNotifyMsgListFile 36258; called 834 | none. 3.82 `noticement.txt` 409 is free text, not 2.20 format | none; build from the parser | parser wants key `notify_msg` (36307) | MISSING; file name also differs in 3.82 |
| Item.cfg, Item2.cfg, Item3.cfg | _bDecodeItemConfigFileContents 9480 (pushed, 15207) | 2.03 27886 / 29585 / 9283 (cp949); 3.51 14530 / 14887 / 16982; 3.82 20532 / 19338 / 23669 | 2.03 (oldest, 26 fields = parser max case 26) | item ID sets differ across versions: Item 2.03 177 recs, 3.51 178, 3.82 237; only 112 IDs shared 2.03/3.51 and values mostly differ | which ID set 2.20 expects is unverified; 3.51 also fits the parser |
| Npc.cfg | _bDecodeNpcConfigFileContents 19677 (15213) | 2.03 5557 (30 recs, 24 fields); 3.51 20265 (105, 25 fields); 3.82 23475 (165, 29) | 2.03 | 3.51/3.82 add fields; 2.20 needs 24 | 2.20 comment G.cpp:29891 says more monsters than npc.cfg; 2.03 is likely short of 2.20 NPCs; key `world-server-max-level` also accepted (3.82 uses it) |
| Magic.cfg | _bDecodeMagicConfigFileContents 24173 (15218) | 2.03 7277 (50); 3.51 9969 (63); 3.82 10888 (90) | 2.03 | 21 fields in all; 3.51 same values for 49/50 shared IDs; 3.82 25/50 | |
| Skill.cfg | _bDecodeSkillConfigFileContents 24487 (15223) | 2.03 1245; 3.51 1325; 3.82 1325 (24 recs, 9 fields each) | 2.03 | 23/24 numeric rows equal 3.51 | names Korean vs English |
| Quest.cfg | _bDecodeQuestConfigFileContents 41537 (15228) | 2.03 6842 (40); 3.51 11394 (120); 3.82 12888 (84) | 2.03 | 27 fields in all; ~34 shared IDs have same name but different numbers | |
| Potion.cfg | _bDecodePortionConfigFileContents 38608 (15233) | 2.03 `portion.cfg` 1461 (14, Korean); 3.51 `Potion.cfg` 1942 (19); 3.82 5883 (43 + crafting lines) | 3.51 Potion.cfg | 2.03 keyword `portion` fails 2.20 test `potion` (38713) | file name translated/misspelled in 2.03 |
| BuildItem.cfg | _bDecodeBuildItemConfigFileContents 43156 (15201) | 2.03 9833 (75, Korean); 3.51 8435 (97); 3.82 9423 (81) | 2.03 | keyed by item name, 23 fields all; no shared names (language) | cross-version match must go via item IDs inside |
| DupItemID.cfg | _bDecodeDupItemIDFileContents 45794 (15247) | 2.03 593 (13 recs); 3.51 615; 3.82 Core 40, Tools 724 | 2.03 | whitespace-normalised hash of 2.03 and 3.51 equal | random per-install ids |
| Teleport.cfg | _bDecodeTeleportListConfigFileContents 24655 (15267) | 3.51 `Teleport.cfg` 806 (6 recs, 14 tokens); 3.82 `TeleportList.cfg` 945 (8, 13) | 3.51 Teleport.cfg | 2.20 reads 13 fields, 3.51 has 1 extra trailing token | not in 2.03; name differs in 3.82 |
| NpcItem.cfg | _bDecodeNpcItemConfigFileContents 25144 (15262) | 3.51 `NPCItem.cfg` 3470 (21 NPCs, `[ENDITEM]` terminated); 3.82 doc copy `Sources/Documentation/npcitem.cfg.txt` 10115 | 3.51 | not in 2.03 | |
| World.cfg | _bDecodeWorldConfigFileContents 25070 (15257) | 3.51 `World.cfg` 109, `_World.cfg` 94 | 3.51 World.cfg | keys world-server-max-user / max-level / forcerecall-time | |
| WLserver.cfg | _bDecodeWLServerConfigFileContents 25019 (15240) | lib/srv220/2.03 251; 3.82 304 | 2.03 (pushed from Log server side) | 3.82 adds keys | key world-server-port |
| Lserver.cfg, GateServer.cfg | read by Log / gate programs, not the game server | Lserver 220 (2.03); GateServer 82 (2.03) and 99 (`forum_gate220`, adds `build-date = 612`); 3.82 GateServer 121, HMLServer.cfg 149 | gate220 `GateServer.cfg` for gate | | see server-program-cfg.md |
| mapdata\<map>.txt | map loader G.cpp:25604-25615 | lib/srv220 `mapdata`; 3.51sql `MapData` (many) | lib | | see mapdata-txt.md |
| mapdata\<map>.amd | Map.cpp:427 | lib bin/mapdata | lib | | binary, see amd-map.md |
| contents*.txt, ItemName.cfg, LOGIN.CFG | client files, not read by the server | 3.82 client `CONTENTS\` (58 contents*.txt etc.); 3.51 client `LOGIN.CFG` | n/a | | see client-files.md |
| ApocalypseGUID, HeldenianGUID, AdminList, BannedList, Settings, AdminSettings | HBX/3.82 only (hbxdevteam Game.cpp 671-700, 45898-45900), not in 2.20 | 3.82, 3.51 (Apocalypse.cfg, Heldenian.cfg, HResult.cfg) | n/a | | out of 2.20 scope |

## Gaps and caveats

- No official 2.20 copy exists anywhere found; 2.03 is the oldest and fits every parser except Potion.
- Web search (English and Spanish queries) found only the already-held 3.82 GitHub repos
  (github.com/relivehelbreath/helbreath-v3.82, github.com/Helbreath/hgserver-hbxdevteam); nothing downloaded.
- Spanish-language packs were not found; no Spanish name variants observed in the held copies.
- Item.cfg gold row/name check against 9531/5979 was not done per record (unverified).
