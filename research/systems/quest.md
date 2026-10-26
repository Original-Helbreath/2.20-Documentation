# System: quest

*Verdict (size review): small and cohesive (offer, accept, track, reward); single file.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`. Tags: [V] read in
source, [I] inferred, [U] unverified. Function bodies: `research/game-server/functions/crusade-quests.md` (quest rows).

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules - 8. Couplings -
9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Quests are civic duties handed out by the town's City Hall officer to citizens of that town: hunt N monsters of a type in
a given map, or travel to a place. Finishing one and returning to the officer pays an item, experience or nothing, and
always raises the character's **contribution** to the town [V S Game.cpp:41351-41475]. Contribution is the civic score
that unlocks harder quests (`m_iReqContribution`) and caps easy ones (`m_iContributionLimit`) [V S 41931, 41944]; the
evident intent is a level- and reputation-banded task board that ties PvE to citizenship [I]. During a crusade the board switches to
war tasks only (assign type 1), and one war quest type is a one-shot teleport to the front [V S 41939-41942, 41984-41991].
The type list (`Quest.h`) shows a much larger plan (assassination, delivery, escort, guard, build, supply, strike, flag)
that the 2.20 code never completes [V S Quest.h:19-30; 42099-42122].

## 2. Subsystems

| # | Subsystem | Intent |
|---|---|---|
| 2.1 | Quest catalogue | `Quest.cfg` pushed by the world log, parsed into `m_pQuestConfigList[200]` |
| 2.2 | Offer (NPC talk) | City Hall officer filters the catalogue for this player and offers one at random with a random reward slot |
| 2.3 | Accept / cancel | record the quest; one-shot war teleports; cancel at the City Hall menu |
| 2.4 | Tracking | location flag on map change, kill counter, place check on every step |
| 2.5 | Completion and reward | flag complete, then reward on the next talk to the giver |
| 2.6 | Contribution | civic score raised by rewards, also by energy-sphere goals; read by offers and other systems |

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `CGame::_bDecodeQuestConfigFileContents` | game server | S Game.cpp:41537-41910 | parse `MSGID_QUESTCONFIGURATIONCONTENTS` (S 15226) [V doc] |
| 2.2 | `CGame::NpcTalkHandler` | game server | S Game.cpp:39153-39239 | only City Hall (iWho 4) and guard (21) talk; sends `NPCTALK` with the offer or a dialogue code [V] |
| 2.2 | `CGame::_iTalkToNpcResult_Cityhall` | game server | S Game.cpp:41351-41475 | reward a completed quest, or search for a new one [V] |
| 2.2 | `CGame::__iSearchForQuest` | game server | S Game.cpp:41913-41974 | filter and random pick; reward slot 1d3 [V] |
| 2.2 | `CGame::_iTalkToNpcResult_Guard` | game server | S Game.cpp:41478-41534 | guard chatter (no quests) [V doc] |
| 2.2 | `_iTalkToNpcResult_GuildHall/GShop/BSmith/WHouse/WTower` | game server | S Game.cpp:42128-42151 | stubs, never called [V doc] |
| 2.2 | `CGame::DlgBoxClick_NpcTalk` | client | C Game.cpp:20482-20520 | accept button sends `QUESTACCEPTED` [V doc] |
| 2.3 | `CGame::QuestAcceptedHandler` | game server | S Game.cpp:41978-42004 | record quest or do the war teleport [V] |
| 2.3 | `CGame::CancelQuestHandler`, `_ClearQuestStatus` | game server | S Game.cpp:47163-47171, 42317-42326 | cancel; clear fields [V] |
| 2.3 | `CGame::DlgBoxClick_CityhallMenu` | client | C Game.cpp:22400-22574 | City Hall menu incl. cancel quest [V doc] |
| 2.4 | `CGame::_CheckQuestEnvironment` | game server | S Game.cpp:42041-42086 | validate quest, set `m_bQuestMatchFlag_Loc` [V doc] |
| 2.4 | `NpcKilledHandler` quest block | game server | S Game.cpp:14544-14556 | MONSTERHUNT counter for the killer [V] |
| 2.4 | `iClientMotion_Move_Handler` | game server | S Game.cpp:1224 | completion check each step [V] |
| 2.5 | `CGame::_bCheckIsQuestCompleted` | game server | S Game.cpp:42088-42126 | MONSTERHUNT / GOPLACE completion -> `QUESTCOMPLETED` [V] |
| 2.5 | `CGame::_SendQuestContents` | game server | S Game.cpp:42007-42039 | current quest to the client (accept, login, teleport) [V doc] |
| 2.5 | `NotifyMsg_QuestContents`, `NotifyMsg_QuestReward`, `DrawDialogBox_Quest` | client | C Game.cpp:25283, 44923, 40254-40385 | quest dialog 28 and reward text [V] |

## 4. Commands and messages

- Client -> game: `DEF_COMMONTYPE_TALKTONPC` (entity) -> `NpcTalkHandler`; `DEF_COMMONTYPE_QUESTACCEPTED`
  [V S 15451]; `DEF_COMMONTYPE_REQUEST_CANCELQUEST` [V S 15427].
- Game -> client: `DEF_NOTIFY_NPCTALK` (quest type, response mode, reward amount, reward name, contribution, target type,
  count, x, y, range, target name; or dialogue code 101-104 / 130+iWho) [V S 39208-39235]; `QUESTCONTENTS`,
  `QUESTCOMPLETED`, `QUESTABORTED`, `QUESTREWARD` (4, success flag, amount, item name, contribution) [V S 41391].
- World log -> game: `MSGID_QUESTCONFIGURATIONCONTENTS` (Quest.cfg verbatim) [V doc official-worldserver.md].

## 5. Data

`CQuest` (S Quest.h:32-60): `m_cSide`, `m_iType`, `m_iTargetType` (NPC type for hunts), `m_iMaxCount`, `m_iFrom` (giver
NPC kind, 4 = City Hall), `m_iMinLevel/m_iMaxLevel`, `m_iRequiredSkillNum/Level`, `m_iTimeLimit`, `m_iAssignType`
(-1 any time, 1 crusade only), `m_iRewardType[4]`/`m_iRewardAmount[4]` (slots 1..3), `m_iContribution`,
`m_iContributionLimit`, `m_iResponseMode` (0 ok, 1 accept/decline, 2 next), `m_cTargetName` (map), `m_sX/m_sY/m_iRange`,
`m_iQuestID` (version guard), `m_iReqContribution` [V]. Catalogue `m_pQuestConfigList[DEF_MAXQUESTTYPE=200]` (S Game.h:101, 721).

Per character (S Client.h:229-240), saved in the character file [V S 9170-9194]:
`m_iQuest` (`character-quest-number`), `m_iQuestID`, `m_iCurQuestCount`, `m_iQuestRewardType` / `m_iQuestRewardAmount`
(item ID; -1 fixed exp; -2 level-scaled exp; 0 none), `m_iContribution` (`character-contribution`, floored at 0 on save).
Not saved: `m_iAskedQuest`, `m_bQuestMatchFlag_Loc`, `m_bIsQuestCompleted` (recomputed) [V S 9170-9194] [I recompute].

Quest types (S Quest.h:19-30): 1 MONSTERHUNT, 2 MONSTERHUNT_TIMELIMIT, 3 ASSASSINATION, 4 DELIVERY, 5 ESCORT, 6 GUARD,
7 GOPLACE, 8 BUILDSTRUCTURE, 9 SUPPLYBUILDSTRUCTURE, 10 STRATEGICSTRIKE, 11 SENDTOBATTLE, 12 SETOCCUPYFLAG.

## 6. Flow

**F1 Offer.** Player talks to the City Hall officer -> `NpcTalkHandler` iWho 4 -> `_iTalkToNpcResult_Cityhall`: if a quest
is running and it came from the City Hall: completed -> reward (F4), else -1 "busy" [V S 41365-41454]; no quest: must be
in own town (`m_iIsOnTown == DEF_NONPK`), not a criminal (PK count 0) [V 41461-41464] -> `__iSearchForQuest`: giver 4,
same side, level in range, contribution >= required and <= limit, skill level if required, crusade gating; random pick,
reward slot 1d3 [V 41924-41961] -> offer stored as `m_iAskedQuest` + reward type/amount, `NPCTALK` sent [V 39204-39209]
-> client dialog (accept/decline).

**F2 Accept.** `QUESTACCEPTED` -> crusade quest of type 10: clear and teleport to target map/x/y, done [V S 41984-41991];
otherwise `m_iQuest = m_iAskedQuest`, ID copied, count 0, not complete; `_CheckQuestEnvironment` sets the location flag if
the player is already on the target map; `_SendQuestContents` [V 41996-42003].

**F3 Track.** Kill: `NpcKilledHandler` -> if MONSTERHUNT and location flag set and the dead NPC's type equals the target,
count++ and check [V S 14545-14555]. Step: every successful move calls `_bCheckIsQuestCompleted` [V S 1224]; GOPLACE
completes inside the x/y range square on the right map [V 42111-42119]. Map change and login re-run
`_CheckQuestEnvironment` (via `RequestTeleportHandler` / `RequestInitDataHandler`) [V doc].

**F4 Reward.** Talk to the officer with the quest complete -> item reward if the bag can take it (else "cannot carry", quest kept and no contribution added) [V S 41372-41408];
-1 -> `GetExp(amount)`; -2 -> `GetExp(1d(10 x level) x amount)`; other -> nothing; each of these branches adds
`m_iContribution += quest.m_iContribution`, sends `QUESTREWARD`, clears the quest [V 41410-41451].

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| giver | only City Hall (iWho 4) offers quests | S 39169-39185, 41466 [V doc + V] |
| eligibility | own town, PK count 0, side match, level band, contribution band, optional skill | S 41461-41464, 41927-41944 [V] |
| crusade | only assign type 1 in crusade; never assign type 1 outside | S 41940-41942 [V] |
| one at a time | a running quest blocks new offers | S 41365, 41454 [V] |
| reward slot | 1d3 at offer time | S 41958 [V] |
| completable types | MONSTERHUNT (count >= max on the target map), GOPLACE (inside range square) | S 42099-42122 [V] |
| kill credit | killer only (`sAttackerH`), no party credit | S 14545-14555 [V] |
| retired quests | indexes 35-40 or ID mismatch -> `QUESTABORTED`, cleared | doc S 42052-42058 [V doc] |
| contribution floor | 0 on save | S 9193 [V] |

## 8. Couplings

- **entity / NPC (other cluster)** - `TALKTONPC` dispatch and NPC kind numbers (`m_iFrom`, iWho); `NpcKilledHandler`
  hosts the hunt counter (coupling field `m_iQuest`, `m_sType`).
- **movement / map (other cluster)** - `iClientMotion_Move_Handler` calls the completion check; `RequestTeleportHandler`
  re-checks environment and performs the war teleport.
- **character / combat** - `GetExp` pays exp rewards (party split applies through `GetExp`) [V S 51636].
- **items** - item rewards (`_bInitItemAttr`, `_bCheckItemReceiveCondition`, `_bAddClientItemList`).
- **crusade** - `m_bIsCrusadeMode` switches the catalogue; type 10 war teleport.
- **pk-law / citizenship** - side, own-town state `m_iIsOnTown`, PK count gate; `m_iContribution` is also read by the City
  Hall menu and changed by the energy sphere (S 46940-46947).
- **persistence / config (infrastructure)** - Quest.cfg pushed by the world log; quest fields in the character file.

## 9. Current behaviour vs intent

Defects are in [issues/quest.md](../issues/quest.md); the neutral lines are:

1. Only MONSTERHUNT and GOPLACE can complete; types 2-6 and 8-12 have no completion logic, and `m_iTimeLimit` is parsed but never read ([QUEST-001](../issues/quest.md#quest-001-most-quest-types-are-unimplemented-the-time-limit-is-never-read)).
2. The war teleport branch tests type 10 (STRATEGICSTRIKE) while the type list names 11 SENDTOBATTLE for it ([QUEST-002](../issues/quest.md#quest-002-war-teleport-uses-quest-type-10-instead-of-11)).
3. `QuestAcceptedHandler` indexes `m_pQuestConfigList[m_iAskedQuest]` without a NULL or pending-offer check and never clears `m_iAskedQuest` ([QUEST-003](../issues/quest.md#quest-003-accept-trusts-the-client)).
4. The reward slot (1d3) is chosen at offer time and overwritten on every talk before accepting ([QUEST-004](../issues/quest.md#quest-004-reward-can-be-re-rolled-by-re-asking)).
5. The offer path names the reward "exp" only for type -10 (S 39199) while rewards use -1 and -2, and the reward text literal is Korean ([QUEST-005](../issues/quest.md#quest-005-reward-label-mismatch-for-experience-rewards)).
6. Five `_iTalkToNpcResult_*` stub givers for guild hall, shops, blacksmith, warehouse and wizard tower exist with dialogue codes 111-136 reserved, but are never called ([QUEST-006](../issues/quest.md#quest-006-stub-giver-functions-are-never-called)).
7. [V] No party credit for hunt quests: only the killing blow counts (S 14545).

## 10. Open questions

- No official 2.20 Quest.cfg exists. `Quest.cfg` is pushed by the world log, not read from disk by the game server
  (S 15228). The facts below are from the 2.03 copy (`sources/hgserver-librehelbreath/bin/config/Quest.cfg`, identical to
  `downloads/configs/sf_203/.../hbs2/Quest.cfg`; cp949 Korean comments, decoded with cp949; measured by script) unless
  another version is named.

**The live quest set (2.03 copy, 40 rows).** Of the 12 types in `Quest.h` only two appear: **type 1 MONSTERHUNT x28**
(rows 1-28) and **type 7 GOPLACE x12** (rows 29-40). Types 2-6 and 8-12 are not configured, which agrees with the code
completing only those two (9.1). 20 rows per side (side 1 Aresden, side 2 Elvine); all rows are giver 4 (City Hall), all
have assign type -1 (none is crusade-only; [QUEST-008](../issues/quest.md#quest-008-the-203-questcfg-has-no-crusade-only-quests)),
and no row has a time limit or skill requirement (-1).
- Hunts, by level band (target = Npc.cfg type; kill count in brackets): 11-20: Giant-Ant 16 [22], Amphis 22 [17], Slime 10
  [35, Elvine only], Orc 14 [11], on the home town map; 20-40: Orc [23], Zombie 18 [18], Skeleton 11 [14] on `huntzone1/2`;
  40-50: Hellbound 27 [12], Clay-Golem 23 [15], Stone-Golem 12 [15]; 50-70: Cyclops 13 [13], Troll 28 [15]; 70-80:
  Ogre 29 [25] (`huntzone3/4`); 80+: Liche 30 [10] on `dglv3`; 90+: Demon 31 [10] on `dglv4`. The sides use mirrored
  hunt-zone maps (side 1 `huntzone2/4`, side 2 `huntzone1/3`).
- Hunt rewards: slot 1 fixed exp (type -1, 100 rising to 12,000 by band), slots 2 and 3 item 90, which is `Gold` in the
  2.03 Item.cfg (100-15,000); contribution +1 to +12, contribution cap 10 to 300. Quest ID guard 417 (rows 1-24) and 208
  (rows 25-28).
- GOPLACE rows 29-40: level 50-300, required contribution 70-150, +1 contribution, cap 500, range 3, all three reward
  slots level-scaled exp (type -2, amount 1; the City Hall rows use 3/2/2). Rows 29-34 send each side to a point on the
  *enemy* town's map (side 1 to `elvine`, side 2 to `aresden`), ID 208/209. Rows 35-40 target interior maps
  (`bsmith_2`, `cityhall_2`, `gldhall_2` and the `_1` set for side 2).
- Rows 35-40 are exactly the indexes `_CheckQuestEnvironment` aborts (S 42052-42058, comment "v2.181 deleted quests").
  The offer filter has no such exclusion; see [QUEST-007](../issues/quest.md#quest-007-questcfg-rows-35-40-are-listed-but-aborted-on-environment-check).
  That the cleanup matches this file's rows is confirmed for the 2.03 copy.
- Other versions: the 3.51 Quest.cfg has 120 rows (112 type 1, 6 type 7, 2 type 10); the 3.82 file has 84 rows, all
  type 1 [V, measured].

## 11. Doc discrepancies

None against the code.
