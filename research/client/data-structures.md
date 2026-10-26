# Client data structures (v2.20)

Key classes, fields and array limits of the 2.20 client. All entries were read from the headers in
`sources\client-2.20\` (commit c01544d) and are verified unless marked *inferred*. Functions that use
these structures are listed in `functions\`.

## Contents
- Array limits
- CGame (Game.h:143)
- Dialog box table
- CMapData and CTile
- Small records: CItem, CMagic, CSkill, CEffect, CMsg, CCharInfo
- CInt
- Other classes

## Array limits

| Define | Value | Where | Used for |
|---|---|---|---|
| DEF_MAXSPRITES | 20000 | Game.h:69 | `m_pSprite[]`, every UI, character, item and monster sheet (id map in `assets.md`) |
| DEF_MAXTILES | 500 | Game.h:70 | `m_pTileSpr[]` map tiles and objects |
| DEF_MAXEFFECTSPR | 100 | Game.h:71 | `m_pEffectSpr[]` |
| DEF_MAXSOUNDEFFECTS | 110 | Game.h:72 | `m_pCSound/m_pMSound/m_pESound[]` (comment: 100 originally) |
| DEF_MAXCHATMSGS | 500 | Game.h:73 | over-head chat `m_pChatMsgList[]` |
| DEF_MAXWHISPERMSG | 5 | Game.h:74 | whisper history |
| DEF_MAXCHATSCROLLMSGS | 80 | Game.h:75 | chat dialog scrollback |
| DEF_MAXEFFECTS | 300 | Game.h:76 | live effects `m_pEffectList[]` (comment: 600 originally) |
| DEF_MAXITEMS | 50 | Game.h:80 | inventory `m_pItemList[]` |
| DEF_MAXBANKITEMS | 121 | Game.h:81 | bank `m_pBankList[]` |
| DEF_MAXGUILDSMAN | 32 | Game.h:82 | guild member list |
| DEF_MAXMENUITEMS | 140 | Game.h:83 | shop list `m_pItemForSaleList[]` (raised from 120 in v2.15) |
| DEF_TEXTDLGMAXLINES | 300 | Game.h:84 | text dialog lines (3000 before v2.18) |
| DEF_MAXMAGICTYPE | 100 | Game.h:86 | `m_pMagicCfgList[]`, `m_cMagicMastery[]` |
| DEF_MAXSKILLTYPE | 60 | Game.h:87 | `m_pSkillCfgList[]`, `m_cSkillMastery[]` |
| DEF_MAXWHETHEROBJECTS | 600 | Game.h:89 | rain/snow particles |
| DEF_MAXBUILDITEMS | 100 | Game.h:90 | manufacture recipes |
| DEF_MAXGAMEMSGS | 300 | Game.h:92 | GameMsgList lines |
| DEF_MAXITEMNAMES | 1000 | Game.h:93 | ItemName.cfg entries |
| DEF_MAXGUILDNAMES | 100 | Game.h:96 | guild-name cache per character |
| DEF_MAXSELLLIST | 12 | Game.h:99 | sell list dialog |
| DEF_MAXPARTYMEMBERS | 8 | Game.h:140 | party |
| DEF_MAXCRUSADESTRUCTURES | 300 | Game.h:141 | crusade structure markers |
| DEF_MAXITEMEQUIPPOS | 15 | Item.h:14 | equip slots |
| DEF_MAXBADWORD | 500 | GameMonitor.h:21 | badword list |
| DEF_MAXRECTS | 30 | MouseInterface.h:15 | clickable rectangles per menu |
| DEF_MAXSOUNDBUFFERS | 2 | SoundBuffer.h:14 | concurrent copies of one sound |
| DEF_TOTALCHARACTERS | 80 | ActionID.h:1 | owner types in `m_stFrame` (100 originally) |
| DEF_TOTALACTION | 15 | ActionID.h:2 | actions per owner type |
| MAPDATASIZEX / Y | 40 / 35 | MapData.h:26-27 | cell window around the player |
| teleport list | 20 | Game.h:630 | `m_stTeleportList` |
| character slots | 4 | Game.h:662 | `m_pCharList[4]` |

Overflow behaviour: no bound checks were found on most server-driven indices (*inferred* from the
handlers; e.g. item index from a notify is used directly). A server sending ids past these limits
corrupts client memory. A concrete case is `CMapData::bGetIsLocateable` (MapData.cpp:865-880; [CLIENT-STATE-004](../issues/client-state.md#client-state-004-bgetislocateable-indexes-out-of-range)); `bIsTeleportLoc` does not index m_pData.

## CGame (Game.h:143)

One global object owns everything. Groups of fields (verified, Game.h:560-890):

- **Resources**: `m_pSprite[]`, `m_pTileSpr[]`, `m_pEffectSpr[]` (Game.h:640-642), sound buffers
  (633-635), `DXC_ddraw m_DDraw`, `DXC_dinput m_DInput`, `CMapData * m_pMapData`.
- **Lists**: chat (646-648), effects (649), items and bank (650-651), magic/skill config (652-653),
  text dialogs (654-656), build items (658-659), shop (661), characters (662), game messages (663),
  item names (664).
- **Player**: name (844), `m_sPlayerX/Y` (797), appearance `m_sPlayerAppr1-4` (800),
  `m_iPlayerApprColor` (745), `m_iHP/m_iMP/m_iSP` as `CInt` (746-748), equipment
  `m_sItemEquipmentStatus[15]` (796), `m_bIsItemEquipped/Disabled[50]` (705-706), item order (821),
  masteries (866-867), location (847-848), guild (849), map name (851).
- **Account** (login screens): `m_cAccountName[12]`, `m_cAccountPassword[12]`, age, country, SSN,
  e-mail, quiz/answer, new password (827-842). Fixed 10-char limits come from these sizes
  (11 + NUL; `DEF_ACCOUNTLEN` 16 only for Terra Japan, GlobalDef.h:195).
- **View**: `m_sViewPointX/Y` (808), dye tables `m_wR/G/B[16]`, `m_wWR/WG/WB[16]` (813-814).
- **Net**: `m_cLogServerAddr[16]` (859; see `build-config.md`), world and game server names
  (868, 885), `m_pGSock`, `m_pLSock`.
- **UI**: `m_stDialogBoxInfo[41]`, `m_cDialogBoxOrder[42]`, `m_bIsDialogEnabled[41]` (566, 865,
  709), shortcuts `m_sShortCut[5]` (774), chat input `m_cChatMsg[64]` (860), `m_cItemDrop[25][25]`.
- **Misc records**: sell list, guild-op list (100), event history (2x6 lines of 96), weather
  objects, party, crusade structures, guild names, teleport list (563-630).

## Dialog box table

`m_stDialogBoxInfo[41]` (Game.h:566): position, size, view mode, cursor/scroll state and four
32-byte strings per dialog; set up in the CGame constructor (Game.cpp:543-784). Ids:

| Id | Dialog | Id | Dialog | Id | Dialog |
|---|---|---|---|---|---|
| 1 | Character | 14 | Bank | 27 | Exchange |
| 2 | Inventory | 15 | Skill | 28 | Quest |
| 3 | Magic | 16 | Magic shop | 29 | Gauge panel |
| 4 | Item drop confirm | 17 | Query drop amount | 30 | Icon panel |
| 5 | 15-age / feedback | 18 | Text | 31 | Sell list |
| 6 | Warning message | 19 | System menu | 32 | Party |
| 7 | Guild menu | 20 | NPC action query | 33 | Crusade job |
| 8 | Guild operation | 21 | NPC talk | 34 | Item upgrade |
| 9 | Guide map | 22 | Map | 35 | Help |
| 10 | Chat | 23 | Sell or repair | 36 | Commander |
| 11 | Shop | 24 | Fishing | 37 | Constructor |
| 12 | Level-up setting | 25 | Shutdown / noticement | 38 | Soldier |
| 13 | City hall | 26 | Manufacture / skill dialog | 39 | (hidden scratch record, never opened) |

Id 39 has no case in `DrawDialogBoxs` and no `EnableDialogBox(39...)`; `m_stDialogBoxInfo[39].sV1, sV3-sV6` is used as a scratch record for the give/repair target (Game.cpp:18629, 32205, 34996, 36327, 42162; verified). Id 1 (Character) is drawn at Game.cpp:18028.

Index 0 and 40 are unused (*inferred*: no `EnableDialogBox(0` or `(40` in the source).

## CMapData and CTile

- `CMapData` (MapData.h:29): `CTile m_pData[40][35]` and a shift buffer `m_pTmpData` (63-64);
  `CTileSpr m_tile[752][752]` (65), the whole static map, so the largest supported map is 752x752
  cells; `m_stFrame[80][15]` animation table (frame count, frame time per owner type and action,
  71); object-id location cache `m_iObjectIDcacheLocX/Y[30000]` (75-76), which caps object ids.
- `CTile` (Tile.h:15): live owner (object id, type, appearance 1-4, colour, status, name, action,
  frame, direction), a dead owner with the same fields, item sprite/frame/colour, dynamic object
  (type, frame, four data bytes), effect type/frame, chat message index, and timestamps
  (Tile.h:24-71).

## Small records

| Class | Key fields | Where |
|---|---|---|
| CItem | name[21], type, equip pos, colour, speed, gender/level limit, sprite+frame, x/y in bag, effect values 1-6, spec values 1-3, life span cur/max, price, weight, count, attribute | Item.h:44-66 |
| CMagic | name[31], value1-3, visible | Magic.h:37-44 |
| CSkill | name[21], level, useable, use method | Skill.h:14-24 |
| CEffect | type, frame/max, dir, times, source/dest, three motion points, error term, iV1 | Effect.h:14-28 |
| CMsg | type, text pointer, x/y, time, object id | Msg.h:16-38 |
| CCharInfo | name, map, skin, sex, appearance 1-4, six stats, exp, colour, logout date | CharInfo.h:15-27 |

## CInt

`CInt` (Cint.h:15) wraps an int with a heap pointer, a key value and an `old` copy: a memory-scanner
guard for HP/MP/SP, equip flags and dialog flags. The `//was int` / `//was BOOL` comments at each
use (Game.h:705-748) suggest it was added after the original source, possibly by librehelbreath
(*inferred*; not checked against the retail binary). Only `Cint.h` and a prebuilt `Cint.lib` are in
the tree; there is no `Cint.cpp`, so the encoding is unknown.

## Other classes

`DXC_ddraw`, `CSprite`, `CMyDib`: `rendering\directdraw.md`, `rendering\blitters.md`.
`XSocket`: `functions\networking.md`. `YWSound`, `CSoundBuffer`, `DXC_dsound`: `functions\sound.md`.
`CBuildItem`, `CItemName`: `functions\items.md`. `CGameMonitor`, `CCurse`, `CMisc`, `CStrTok`,
`CWebCtrl`, `CMouseInterface`: `functions\support.md`, `functions\chat.md`, `functions\input.md`.
