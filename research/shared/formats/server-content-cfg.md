# Server content configs (Item, Npc, Magic, Skill, Quest, Potion, BuildItem, ...)

Game-content tables that the **Log server reads from disk and pushes to the game server** at
registration, one message per file; the game server never opens them itself in 2.20.
Tags: [V] read in code at the cited line, [I] inferred. `srv220` = `.claude/scratch/srv220/`.

## Contents
- Common syntax
- Delivery: which message carries which file
- Item.cfg / Item2.cfg / Item3.cfg
- Npc.cfg
- Magic.cfg
- Skill.cfg
- Quest.cfg
- Potion.cfg (portion.cfg)
- BuildItem.cfg
- DupItemID.cfg
- NpcItem, TeleportList, World config, WLserver contents, Noticement
- Gaps

## Common syntax

- Plain text, tokenised with separators `"= \t\n"` (`srv220/Game.cpp:9483` and every decoder
  below) [V]. So `key = a b c` and `key a b c` parse the same; `=` is just a separator.
- A record starts with a keyword token and is followed by a fixed number of positional fields;
  the decoder is a two-level state machine (`cReadModeA` = record type, `cReadModeB` = field
  index) [V]. Missing or non-numeric fields abort the whole file with "CRITICAL ERROR ... Wrong
  Data format" (example `srv220/Game.cpp:19905`) [V].
- Lines starting `//` or `;` are comments only because their first token matches no keyword; a
  comment containing a keyword would be parsed [I from the token loop].
- Section markers such as `[CONFIG]`, `[ITEMS]`, `[ENDITEMLIST]`, `[END]` are tokens; only those
  named below have meaning.
- If a decoder finishes with `cReadModeA/B != 0` the file is rejected (example
  `srv220/Game.cpp:9842`) [V].

## Delivery: which message carries which file

Dispatcher in the game server: `srv220/Game.cpp:15195-15268` (`case MSGID_*CONTENTS`, payload at
`pData + DEF_INDEX2_MSGTYPE + 2`) [V]. Official WLS file order and IDs: `research/servers/official-worldserver.md:109-113` [V there].

| File (on the Log server) | Message | Game-server decoder |
|---|---|---|
| `Item.cfg`, `Item2.cfg`, `Item3.cfg` | `MSGID_ITEMCONFIGURATIONCONTENTS` (each file one message) | `_bDecodeItemConfigFileContents` `srv220/Game.cpp:9480` |
| `Npc.cfg` | `MSGID_NPCCONFIGURATIONCONTENTS` | `_bDecodeNpcConfigFileContents` `:19677` |
| `Magic.cfg` | `MSGID_MAGICCONFIGURATIONCONTENTS` | `_bDecodeMagicConfigFileContents` `:24173` |
| `Skill.cfg` | `MSGID_SKILLCONFIGURATIONCONTENTS` | `_bDecodeSkillConfigFileContents` `:24487` |
| `Quest.cfg` | `MSGID_QUESTCONFIGURATIONCONTENTS` | `_bDecodeQuestConfigFileContents` `:41537` |
| `Potion.cfg` (sample name `portion.cfg`) | `MSGID_PORTIONCONFIGURATIONCONTENTS` | `_bDecodePortionConfigFileContents` `:38608` |
| `BuildItem.cfg` | `MSGID_BUILDITEMCONFIGURATIONCONTENTS` | `_bDecodeBuildItemConfigFileContents` `:43156` |
| `DupItemID.cfg` | `MSGID_DUPITEMIDFILECONTENTS` | `_bDecodeDupItemIDFileContents` `:45794` |
| `Noticement.txt` | `MSGID_NOTICEMENTFILECONTENTS` | `_bDecodeNoticementFileContents` `:46031` |
| `WLserver.cfg` (subset) | `MSGID_WLSERVERFILECONTENTS` | `_bDecodeWLServerConfigFileContents` `:25019` |
| world config (file name unknown) | `MSGID_WORLDCONFIGCONTENTS` | `_bDecodeWorldConfigFileContents` `:25070` |
| NPC drop table (file name unknown) | `MSGID_NPCITEMCONFIGCONTENTS` | `_bDecodeNpcItemConfigFileContents` `:25144` |
| teleport list (file name unknown) | `MSGID_TELEPORTLISTCONTENTS` | `_bDecodeTeleportListConfigFileContents` `:24655` |
| `GameData\AresdenOccupyFlag.txt`, `ElvineOccupyFlag.txt` | `MSGID_ARESDENOCCUPYFLAGSAVEFILECONTENTS` / `MSGID_ELVINEOCCUPYFLAGSAVEFILECONTENTS` | `_bDecodeOccupyFlagSaveFileContents` `:40340` (see `server-program-cfg.md`) |

Message hex values and handler lines: `../messages/game-log.md`.

## Item.cfg / Item2.cfg / Item3.cfg

Record keyword `Item` (memcmp of 4 bytes, `srv220/Game.cpp:9822`); list end `[ENDITEMLIST]`
(`:9827`) [V]. Fields in order [V `srv220/Game.cpp:9508-9810`]:

| # | Field | Member | Notes |
|---|---|---|---|
| 1 | Num | index into `m_pItemConfigList` | duplicate index rejected |
| 2 | Name | `m_cName` | name beginning with the cp949 Korean string "자이안트-소드" (Giant Sword) is special-cased, compared by prefix (`:9531`) |
| 3 | Type | `m_cItemType` | |
| 4 | EPos | `m_cEquipPos` | |
| 5 | Effect | `m_sItemEffectType` | |
| 6-11 | ETValue 1-6 | `m_sItemEffectValue1..6` | |
| 12 | MaxLife | `m_wMaxLifeSpan` | WORD |
| 13 | SEffect | `m_sSpecialEffect` | |
| 14 | Spr | `m_sSprite` | |
| 15 | SFrame | `m_sSpriteFrame` | |
| 16 | Price | `m_wPrice = abs(v)`, `m_bIsForSale = v >= 0` | negative = not sold (`:9688-9693`) |
| 17 | Wgt | `m_wWeight` | |
| 18 | ApprV | `m_cApprValue` | |
| 19 | Speed | `m_cSpeed` | |
| 20 | L.Lmt | `m_sLevelLimit` | |
| 21 | G.Lmt | `m_cGenderLimit` | |
| 22 | HR (SE value 1) | `m_sSpecialEffectValue1` | |
| 23 | SE value 2 | `m_sSpecialEffectValue2` | |
| 24 | R.Skill | `m_sRelatedSkill` | |
| 25 | Category | `m_cCategory` | |
| 26 | Color | `m_cItemColor` | |

Sample header comment in `srv220/Item.cfg` line 3 matches this order [V].

## Npc.cfg

Keyword `Npc` (`srv220/Game.cpp:20008`); also a scalar `world-server-max-level = N`
(`:20014`, sets `m_iPlayerMaxLevel`) [V]. Fields [V `:19707-19988`]: Name (`m_cNpcName`), Type,
HitDice, DefenseRatio, HitRatio, MinBravery, ExpDice, AttackDiceThrow, AttackDiceRange, Size,
Side, ActionLimit, ActionTime (ms), ResistMagic, MagicLevel, DayOfWeekLimit, ChatMsgPresence
(`:19893-19897` is an empty `if` body after it; no token is skipped), TargetSearchRange, RegenTime,
Attribute, AbsDamage, MaxMana, MagicHitRatio, AttackRange. **24 positional fields** (`cReadModeB`
runs 1..24, then resets to 0, `:19977-19991`); a record in the sample `Npc.cfg` is the keyword plus
24 values (25 tokens including `Npc`). No index field (slots are filled in order) [I from the
absence of an index read].

## Magic.cfg

Keyword `magic` (`srv220/Game.cpp:24460`) [V]. Fields [V `:24203-24448`]: Num (index), Name,
Type, DelayTime, LastTime, Value1..Value12, IntLimit, GoldCost, Category, Attribute. 21 fields.

## Skill.cfg

Keyword `skill` (`srv220/Game.cpp:24629`) [V]. Fields [V `:24517-24617`]: Num, Name, Type,
Value1..Value6. 9 fields.

## Quest.cfg

Keyword `quest` (`srv220/Game.cpp:41890`) [V]. Fields [V `:41567-41877`]: Num, Side, Type,
TargetType, MaxCount, From, MinLevel, MaxLevel, RequiredSkillNum, RequiredSkillLevel, TimeLimit,
AssignType, RewardType1, RewardAmount1, RewardType2, RewardAmount2, RewardType3, RewardAmount3,
Contribution, ContributionLimit, ResponseMode, TargetName (string), X, Y, Range, QuestID,
ReqContribution. 27 fields.

## Potion.cfg (portion.cfg)

Keyword test is `memcmp(token, "potion", 5)`, i.e. the prefix `potio` [V
`srv220/Game.cpp:38713`]. The sample `srv220/portion.cfg` writes `portion = ...` (prefix
`porti`), so its records would not be recognised [I, not run]. Fields [V `:38637-38701`]: Num, Name, 12 shorts `m_sArray[0..11]` (sample shows
six item-ID / count pairs, -1 0 = empty) , SkillLimit, Difficulty. The official WLS pushes a file
named `Potion.cfg` (`research/servers/official-worldserver.md:110`).

## BuildItem.cfg

Keyword `BuildItem` (`srv220/Game.cpp:43501`) [V]. Fields [V `:43180-43463`]: Name, SkillLimit,
then six triples (MaterialItemID, MaterialItemCount, MaterialItemValue), AverageValue, MaxSkill,
Attribute. 23 fields. The client has its own different layout of the same idea
(`client-files.md`, BItemcfg).

## DupItemID.cfg

Keyword `DupItemID` (`srv220/Game.cpp:45909`) [V]. Fields [V `:45824-45893`]: Num,
TouchEffectType, TouchEffectValue1..3, Price (WORD). Sample `srv220/DupItemID.cfg`: type 2 with
three ID numbers, used to recognise duplicated items by their unique ID [I from field names].

## NpcItem, TeleportList, World config, WLserver contents, Noticement

- **NpcItem** [V `srv220/Game.cpp:25144-25353`]: keyword `NpcItem`, then NPC name (must match a
  loaded Npc), NpcitemType, then repeated `ItemName FirstProbability SecondProbability` (item
  names must exist in the item table) until `[ENDITEM]`.
- **TeleportList** [V `:24684-24870`]: keyword `teleport`, fields Num, NpcName, SourceMap,
  TargetMap, X, Y, Cost, MinLvl, MaxLvl, Side (`aresden`/`elvine`/`both`), Hunter (0/1),
  Neutral (0/1), Criminal (0/1); list end `[END]`.
- **World config** [V `:25070-25120`]: scalars `world-server-max-user`, `world-server-max-level`,
  `world-server-forcerecall-time`.
- **WLserver contents** [V `:25019-25049`]: only `world-server-port` is read (sets
  `m_iWorldLogServerPort`).
- **Noticement**: free text shown to players [I; decoder at `:46031` not field-mapped here].

## Gaps

- File names behind `MSGID_WORLDCONFIGCONTENTS`, `MSGID_NPCITEMCONFIGCONTENTS` and
  `MSGID_TELEPORTLISTCONTENTS` are unknown: the official WLS V2.15 file list does not include
  them (`official-worldserver.md:109-113`), so a 2.20 game server fed by it never receives them [I].
- Sample data for NpcItem, TeleportList and world config is not in `srv220/`.
- The Potion keyword mismatch above is not tested at runtime.
