# Data structures and limits (v2.20 HGserver)

Read from Game.h, Client.h, Npc.h, Map.h, Item.h and the small class headers in `.claude/scratch/srv220/`.
All entries are [V] unless tagged. Wire layouts are in `research/shared/messages/` and file formats in
`research/shared/formats/`; this page covers only in-memory state.

## CGame (Game.h): the whole world in one object
Lists, each a fixed array of pointers where NULL means a free slot:

| Field | Size | Holds |
|---|---|---|
| m_pClientList | DEF_MAXCLIENTS 2000 | CClient |
| m_pNpcList | DEF_MAXNPCS 5000 | CNpc |
| m_pMapList | DEF_MAXMAPS 100 | CMap |
| m_pItemConfigList | DEF_MAXITEMTYPES 5000 | CItem templates from Item.cfg |
| m_pNpcConfigList | DEF_MAXNPCTYPES 100 | CNpc templates from Npc.cfg |
| m_pMagicConfigList / m_pSkillConfigList | 100 / 60 | Magic.cfg, Skill.cfg |
| m_pBuildItemList | DEF_MAXBUILDITEMS 300 | BuildItem.cfg recipes |
| m_pPortionConfigList | DEF_MAXPORTIONTYPES 500 | Potion.cfg recipes |
| m_pQuestConfigList | DEF_MAXQUESTTYPE 200 | Quest.cfg |
| m_pDupItemIDList | DEF_MAXDUPITEMID 100 | DupItemID.cfg |
| m_pDynamicObjectList / m_pDelayEventList | 60000 each | CDynamicObject / CDelayEvent |
| m_pFish / m_pMineral | 200 each | CFish / CMineral |
| m_pMsgQuene | DEF_MSGQUENESIZE 100000 | CMsg ring buffer |
| m_pSubLogSock | DEF_MAXSUBLOGSOCK 10 | sub-log XSockets |
| m_iClientShortCut | 2001 | dense list of live client indices |
| m_pGuildTeleportLoc | 1000 (DEF_MAXGUILDS) | crusade guild teleport/construct points |

Other CGame state: m_iLevelExpTable[300], m_iSkillSSNpoint[102], m_stCityStatus[3] (per side: crimes, wins,
funds), m_stPartyInfo (DEF_MAXPARTYNUM 5000 x DEF_MAXPARTYMEMBERS 8), m_stCrusadeWarSchedule[10],
m_stCrusadeStructures[300], crusade flags (m_bIsCrusadeMode, GUID, winner side), and the server
addresses and ports from GServer.cfg.

## Timers and constants (Game.h)

| Constant | Value |
|---|---|
| DEF_CLIENTTIMEOUT | 10 s |
| DEF_AUTOSAVETIME | 30 min |
| DEF_HPUPTIME / MPUPTIME / SPUPTIME | 15 / 20 / 10 s |
| DEF_HUNGERTIME | 60 s |
| DEF_POISONTIME | 12 s |
| DEF_SUMMONTIME | 5 min |
| DEF_NOTICETIME | 80 s |
| DEF_PLANTTIME | 5 min |
| DEF_EXPSTOCKTIME | 10 s |
| DEF_AUTOEXPTIME | 6 min |
| DEF_RAGPROTECTIONTIME | 7000 ms |
| DEF_NIGHTTIME | minute 40 of each hour |
| DEF_TOTALLEVELUPPOINT | 3 |
| DEF_MAXSKILLPOINTS | 700 |
| DEF_CHARPOINTLIMIT | 200 |
| DEF_GUILDSTARTRANK | 12 |
| DEF_MAXREWARDGOLD | 99999999 |
| DEF_MAXONESERVERUSERS | 800 |
| DEF_MAXCONSTRUCTNUM | 10 (Taiwan 7) |
| DEF_MAXFIGHTZONE | 10 |
| DEF_LEVELLIMIT | 20 (trial accounts; [CHARACTER-002](../issues/character.md#character-002-def_levellimit-is-documented-as-a-trial-limit-but-used-as-the-hunger-threshold)) |
| DEF_MINIMUMHITRATIO / MAXIMUMHITRATIO | 15 / 99 |
| DEF_PLAYERMAXLEVEL | 180 |
| DEF_GMGMANACONSUMEUNIT | 15 |
| DEF_MAXCONSTRUCTIONPOINT | 30000 |
| DEF_MAXWARCONTRIBUTION | 500000 |
| DEF_MAXGIZONPOINT | 37 |
| DEF_MAX_CRUSADESUMMONMOB (Game.h:222-224; 5 under DEF_TAIWAN) | 500, enforced per guild through m_iNumSummonNpc (Game.cpp:48864) |
| DEF_LIMITHUNTERLEVEL | 100 |

Codes:
- Sides: 0 traveller, 1 Aresden, 2 Elvine, 100 DEF_BOTHSIDE (Game.h:212, "both sides" in the teleport list, used at Game.cpp:23935, 23958, 24081). In Npc.cfg a town NPC has side 1/2 (Guard-Aresden 1) and monsters use side 10 (Slime, Orc); the code branches on `m_cSide < 10` for town-bound NPCs (Game.cpp:11764).
- Town PK zones: 0, DEF_NONPK 1, DEF_NEVERNONPK 2.
- Log codes: item log 1-30, crusade log 1-4, PK log 1-6.

## CClient (Client.h)
- Limits: DEF_MAXITEMS 50 inventory, DEF_MAXBANKITEMS 120 warehouse, DEF_MAXGUILDSMAN 128, DEF_MAXMAGICTYPE 100, DEF_MAXSKILLTYPE 60, DEF_MAXTELEPORTLIST 20, DEF_SPECABLTYTIMESEC 1200.
- Identity: char/account/password, IP, m_dwCharID (DB mode), admin level, map index and x/y/dir.
- Stats: Str, Vit, Dex, Int, Mag, Cha, level, exp, exp stock, HP/MP/SP, hunger, LU_* allocation, rating, PK count, EK count, reward gold.
- Items: m_pItemList / m_pItemInBankList, equip index per slot, item icon positions.
- Skills: m_cSkillMastery[60], m_iSkillSSN[60]; m_cMagicMastery[100].
- Item-derived bonuses (filled by CalcTotalItemEffect): m_iAdd* (AR, DR, HP, SP, MP, MR, PR, CD, Exp, Gold, abs per element, physical/magical damage), damage dice, hit ratio, defence ratio, absorption per slot.
- Status: magic effect status, poison, invisibility, berserk, ice, protection; special ability type, time and enabled flag.
- Social: guild name/GUID/rank, party id and status (0 none, 1 processing, 2 confirmed), whisper target, exchange state.
- Location flags: side, town, shop, tower, warehouse, building, hunter mode, m_iIsOnTown.
- Crusade: duty (1 soldier, 2 builder, 3 commander), construction points, war contribution, crusade GUID.
- Timers: last-message time, autosave, regen, lock map name/time (jail), force-recall time.

## CNpc (Npc.h)
- Move types 0-6 (Npc.h:20-26): 0 STOP, 1 SEQWAYPOINT, 2 RANDOMWAYPOINT, 3 FOLLOW, 4 RANDOMAREA, 5 RANDOM, 6 GUARD. Behaviours 0-4: stop, move, attack, flee, dead.
- Element attribute 1-4. Special ability 1-8.
- Combat: HitDice, ExpDice, DefenseRatio, HitRatio, AbsDamage, magic level, attack range, size.
- Ownership: summoned flag, follow target, guild GUID (war units), side.
- DEF_NPCITEM: m_vNpcItem list, m_iNpcitemType and m_iNpcitemMax. Crop fields for farming.

## CMap (Map.h)
- Limits: TELEPORTLOC 200, WAYPOINTCFG 200, MGAR (mob-gen avoid rects) 50, NMR (no-attack rects) 50, SPOTMOBGEN 100, FISHPOINT 200, MINERALPOINT 200, OCCUPYFLAG 20001, INITIALPOINT 20, ENERGYSPHERES 10, STRATEGICPOINTS 200, SECTORS 60 (sector = 20x20 tiles), STRIKEPOINTS 20, ITEMEVENTS 50.
- Map type: 0 normal, 1 NOPENALTY_NOREWARD. Weather: 0 none, 1-3 rain, 4-6 snow.
- Tiles: CTile grid with owner (player/NPC), dead owner, up to 12 items per tile, dynamic object, move/teleport/farm flags, occupy values.

## CItem (Item.h)
- Equip positions 0-13; item types -1..12; item effect types 0-30; touch effect types (ITET) 1-3, where ITET_ID stamps a unique id.
- m_dwAttribute bits:

  | Bits | Meaning |
  |---|---|
  | 0 | crafted |
  | 8-11 | sub effect value |
  | 12-15 | sub effect type |
  | 16-19 | main effect value |
  | 20-23 | main effect type |
  | 28-31 | upgrade level |

## Small classes
- CMagic: magic types 1-27; DEF_MAXMAGICEFFECTS 100.
- CDelayEvent: types 1-6: 1 DAMAGEOBJECT, 2 MAGICRELEASE, 3 USEITEM_SKILL, 4 METEORSTRIKE, 5 DOMETEORSTRIKEDAMAGE, 6 CALCMETEORSTRIKEEFFECT [V, DelayEvent.h:15-20].
- CQuest: types 1-12: 1 MONSTERHUNT, 2 MONSTERHUNT_TIMELIMIT, 3 ASSASSINATION, 4 DELIVERY, 5 ESCORT, 6 GUARD, 7 GOPLACE, 8 BUILDSTRUCTURE, 9 SUPPLYBUILDSTRUCTURE, 10 STRATEGICSTRIKE, 11 SENDTOBATTLE, 12 SETOCCUPYFLAG [V, Quest.h:19-30].
- CMsg: sources DEF_MSGFROM_CLIENT 1, LOGSERVER 2, GATESERVER 3.
- XSocket: events -121..-136; the unsent-block limit is per socket via the constructor (15 client sockets, DEF_CLIENTSOCKETBLOCKLIMIT; 300 server sockets, DEF_SERVERSOCKETBLOCKLIMIT); DEF_XSOCKBLOCKLIMIT 300 is only the array size [V, XSocket.cpp:39,367,392].
- Others: CFish, CMineral, CDynamicObject, COccupyFlag, CStrategicPoint, CTeleport/CTeleportLoc, CGuildsMan, CBuildItem, CPortion, CSkill, CTile.
