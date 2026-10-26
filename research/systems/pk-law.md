# System: pk-law (PK, criminal status, town alignment, penalties, rewards, jails, hunter mode)

*Cohesive single doc: alignment, legality, crime, punishment and reward all hang off the same two fields
(`m_cSide`, `m_iPKCount`). Formula detail lives in `research/game-server/mechanics.md` sections 6 and 12.*

Sources: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`. Tags [V]/[I]/[U].

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow -
7. Rules - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose
Helbreath is a two-nation war game (Aresden vs Elvine) with a neutral Traveller start. The law system
turns open PvP into a structured conflict:
- **Killing the enemy nation is honourable** - enemy kills (EK) pay exp, gold and a counter used for hero
  items (`S Game.cpp:28083`) [V].
- **Killing your own nation's innocents is a crime** - PK count, exp loss, rating loss, the town's crime
  counter and, inside your own town, 3 minutes of jail (`S :27986`) [V].
- **Criminals are fair game** - anyone (except other criminals) may kill them for bounty gold
  (`S :28058`), and their deaths cost far more (tiers 3/6/12) [V].
- **Travellers are protected but capped** - they cannot hurt innocents, are protected in neutral towns,
  lose 1/3 exp and no items on death, but cannot pass level 19 (`character.md`) [V], pushing every
  player into a nation.
- **Civilians (hunter mode, v2.19-2.20)** - low-level citizens can opt out of PvP entirely outside crusade,
  but are barred from the war zones and lose the status at level 100 (`S :52782`, `:4094-4111`) [V].
- **Enemy territory is time-limited** - force recall after a weekday-dependent stay (`S :52716`) [V].

## 2. Subsystems
| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Citizenship | traveller (side 0) chooses Aresden or Elvine at a city hall, level >= 5 | MSGID_REQUEST_CIVILRIGHT |
| 2.2 | Side and town derivation | side, own-town (NONPK), shop/tower/warehouse flags, hunter flag from location strings | iSetSide on login and teleport |
| 2.3 | Relationship codes | 0-7 relation used by safe mode, colouring and status bits | combat, client |
| 2.4 | Criminal status | `m_iPKCount` 0 innocent, 1-3 criminal, 4-11 murderer, 12+ slaughterer | everything |
| 2.5 | Town guards | hitting a non-hostile player in a town spawns a guard that hunts the attacker | combat/magic |
| 2.6 | Kill classification | who killed whom -> penalty, bounty or enemy-kill reward | ClientKilledHandler |
| 2.7 | PK penalty and jail | PK count +1, exp loss, rating -10, city crimes +1, own-town jail | 2.6 |
| 2.8 | Death penalty by tier | exp loss and item drops scale with the victim's criminal tier; PK count -1 per death | 2.6 |
| 2.9 | Bounty (reward gold) | criminal kills bank gold, redeemed at the city hall within carry weight | 2.6 + cityhall |
| 2.10 | Enemy-kill reward | exp, gold, EK counter, crusade contribution | 2.6 |
| 2.11 | Locked maps (jails, crusade town lock, forced-disconnect lock) | a timed map lock that teleport respects | RequestTeleportHandler |
| 2.12 | Respawn routing | where the dead reappear: default / resurr1/2 / farm / enemy jail | restart and logout-while-dead |
| 2.13 | Force recall (war location) | timed stay in enemy town, fightzone, jail; immediate for travellers in towns during crusade | 3 s timer |
| 2.14 | Hunter (civilian) mode | opt-out of PvP for level <= 100 citizens, area bans | COMMON REQUEST_HUNTMODE |
| 2.15 | Reputation | /rep+ /rep- rating, -10 per PK (rating mechanics in `character.md`) | chat |
| 2.16 | PK logging | _bPKLog to the log server / files for every penalty and reward | all of the above |

## 3. Functions
Details in `research/game-server/functions/combat-magic.md` (cm) and `character.md` (ch).

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | RequestCivilRightHandler | game server | S Game.cpp:27547 | traveller, level >= 5 -> m_cLocation = map location name; side set; leaves party; auto hunter mode [V] |
| 2.1 | DlgBoxClick_CityhallMenu / CivilRightAdmissionHandler | client | C Game.cpp:22400 / (networking.md) | city hall menu, response handling [V map] |
| 2.2 | iSetSide | game server | S Game.cpp:52857 | side, `m_iIsOnTown`, `m_bIsNeutral`, `m_bIsHunter`, location flags [V] |
| 2.3 | iGetPlayerRelationship | game server | S Game.cpp:37498 | relation 0-7 [V cm] |
| 2.3 | iGetPlayerABSStatus / iGetPlayerRelationship_SendEvent | game server | S Game.cpp:37686 / :37837 | status bits for other clients (side, criminal, hunter) [V cm] |
| 2.3 | _iGetFOE | client | C Game.cpp:24628 | friend/foe colour from status bits [V map] |
| 2.5 | bAnalyzeCriminalAction | game server | S Game.cpp:35838 | guard spawn in aresden/elvine/default, not in crusade [V cm] |
| 2.5 | _bGetIsPlayerHostile | game server | S Game.cpp:35898 | legal-target test [V] |
| 2.6 | ClientKilledHandler | game server | S Game.cpp:19253 | classification matrix (S :19334-19414), victim tier (S :19438-19474) [V] |
| 2.7 | ApplyPKpenalty | game server | S Game.cpp:27986 | PK penalty and own-town jail [V] |
| 2.8 | ApplyCombatKilledPenalty | game server | S Game.cpp:28263 | PK count -1, exp loss, drops (none in crusade) [V] |
| 2.8 | _PenaltyItemDrop | game server | S Game.cpp:28338 | random inventory drops, Stone of Sacrifice [V cm] |
| 2.9 | PK_KillRewardHandler | game server | S Game.cpp:28058 | bounty += victim exp level x 3 [V cm] |
| 2.9 | GetRewardMoneyHandler | game server | S Game.cpp:28438 | pay bounty as gold up to half of free carry weight [V] |
| 2.10 | EnemyKillRewardHandler | game server | S Game.cpp:28083 | EK reward [V cm] |
| 2.10 | GetHeroMantleHandler | game server | S Game.cpp:40457 | EK counter -> hero items (crusade/items clusters) [V def] |
| 2.11 | RequestTeleportHandler (lock override) | game server | S Game.cpp:23250, lock checks :23324-23494, notify :23550 | destination replaced by the locked map while the lock runs; NOTIFY_LOCKEDMAP [V] |
| 2.11 | CheckClientResponseTime (lock timers) | game server | S Game.cpp:3986-3996 | lock and dead-penalty countdown -3 per 3 s tick (seconds) [V] |
| 2.12 | RequestRestartHandler | game server | S Game.cpp:46246 | respawn routing [V ch] |
| 2.12 | DeleteClient (dead / forced) | game server | S Game.cpp:2645-2720 | same routing on logout; GM close-connection (`bForceCloseConn`) -> bisle lock 10 min [V] |
| 2.13 | SetForceRecallTime | game server | S Game.cpp:52716 | stay time by weekday or GM override [V] |
| 2.13 | RequestInitDataHandler (war location) | game server | S Game.cpp:2091-2172 | arms force recall on login by map [V] |
| 2.13 | CheckClientResponseTime (recall) | game server | S Game.cpp:3997-4011 | countdown, TOBERECALLED, recall [V] |
| 2.14 | RequestHuntmode / SetNoHunterMode / RecallHunterPlayer | game server | S Game.cpp:52782 / :52825 / :49251 | toggle, forced off, crusade-end recall [V ch] |
| 2.14 | CheckClientResponseTime (hunter area) | game server | S Game.cpp:4094-4111 | hunters recalled from middleland, 2ndmiddle, dungeons d1, huntzone3/4 outside crusade [V] |
| 2.16 | _bPKLog | game server | S Game.cpp:43050 | PK log entries (logging/infrastructure) [V def] |
| client | NotifyMsg_PKpenalty / PKcaptured / EnemyKillReward | client | C Game.cpp:44810 / :44769 / :43387 | stat/exp/PK-count refresh and messages [V] |

## 4. Commands and messages
From `by-system.md` "pk-law".
- Client -> game: `MSGID_REQUEST_CIVILRIGHT` (S :15023) -> `MSGID_RESPONSE_CIVILRIGHT` (with location name);
  `MSGID_COMMAND_COMMON` `REQUEST_HUNTMODE` (S :15381), `REQ_GETREWARDMONEY` (S :15515),
  `TOGGLESAFEATTACKMODE` (S :15584, shared with combat), `REQ_GETHEROMANTLE` (crusade) [V].
- Game -> client notifies: `PKPENALTY`, `PKCAPTURED`, `ENEMYKILLREWARD`, `REWARDGOLD`, `TOBERECALLED`,
  `FORCERECALLTIME`, `RESPONSE_HUNTMODE`, `LOCKEDMAP`, `ENEMYKILLS` [V by-system].
- GM: `/setforcerecalltime <min>` relayed to all servers as `GSM_REQUEST_SETFORCERECALLTIME` through the
  gate (gate hop; S :10913) [V]; `/setattackmode` (DEF_TAIWAN only).
- Log: `MSGID_GAMEITEMLOG` / PK log via _bPKLog (logging cluster).

## 5. Data
- CClient: `m_cSide` (0 traveller / 1 Aresden / 2 Elvine), `m_cLocation` ("aresden", "elvine", "arehunter",
  "elvhunter", "NONE"), `m_bIsNeutral`, `m_bIsHunter`, `m_iIsOnTown` (DEF_PK / DEF_NONPK / DEF_NEVERNONPK),
  `m_iPKCount`, `m_iRewardGold`, `m_iEnemyKillCount`, `m_iRating`, `m_cLockedMapName`, `m_iLockedMapTime`,
  `m_iDeadPenaltyTime`, `m_bIsWarLocation`, `m_iTimeLeft_ForceRecall`, `m_dwWarBeginTime`,
  `m_bIsSafeAttackMode` [V at uses].
- Persisted: `character-loc-map`/location, `character-reward-gold`, `locked-map-name`, `locked-map-time`,
  `dead-penalty-time`, PK and EK counts (S Game.cpp:8917 field labels; lock fields also :6588-6592) [V].
- Game: `m_stCityStatus[side].iCrimes` (per-town crime counter, S :28020), `m_sForceRecallTime` (GM override),
  `m_iFightzoneNoForceRecall` [V].
- Map: `m_cLocationName` ("aresden", "elvine", "arejail", "elvjail", "middleland"...), map names
  "cityhall*", "resurr1/2", "arefarm/elvfarm", "default", "bisle" (map cluster) [V at uses].
- Build switches (S GlobalDef.h): DEF_BEGINNERTOWNPROTECT on (:25); DEF_PKPENALTY, DEF_ITEMDROP,
  DEF_SAMESIDETOWNPROTECT only under DEF_CONSULTATION (Korea) and DEF_USA (:26-29, :45-49, :82-87) -
  all off in the DEF_CHINESE export; DEF_ALLPLAYERPROTECT commented out (:83) [V].

## 6. Flow
**A. Becoming a citizen** - city hall menu -> `MSGID_REQUEST_CIVILRIGHT` -> RequestCivilRightHandler
(S :27547): reject if already sided or level < 5; `m_cLocation` = current map's location name; side from
"aresden"/"elvine" prefix; `RESPONSE_CIVILRIGHT` with the name; appearance broadcast; leave party;
third-year event check; RequestHuntmode - so new citizens standing in a cityhall map start as civilians
(S :27556-27614) [V].

**B. PK in town (same nation, innocent victim)**
1. Melee or spell hits an innocent same-side player; legality passes because DEF_SAMESIDETOWNPROTECT is off
   (combat.md section 7). Magic calls bAnalyzeCriminalAction on cast (S :22331); melee only on the killing
   blow (S :13745) -> a guard spawns and targets the attacker (S :35838) [V].
2. The victim dies -> ClientKilledHandler (S :19253): same side, victim PK 0 -> ApplyPKpenalty (S :19370) [V].
3. ApplyPKpenalty (S :27986): no penalty if the attacker is in safe mode and innocent (S :27994); PK +1,
   exp loss, NOTIFY_PKPENALTY, appearance broadcast (red name), city crimes +1, rating -10; if the PK
   happened on a map whose location is the attacker's own town -> locked map arejail/elvjail 180 s and
   teleport there (S :28027-28056) [V].
4. The victim gets no exp loss for being PKed by a countryman (commented code, S :19428-19435) [V].
5. Client NotifyMsg_PKpenalty (C :44810) refreshes exp, stats and PK count [V].

**C. Enemy kill** - other-side killer -> EnemyKillRewardHandler (S :19379/:19412 -> :28083): reward only if
the killer is innocent; exp, gold 1dL, EK+1 if the victim level qualifies and died outside its own town;
victim gets tier-2 ApplyCombatKilledPenalty (S :19438) [V].

**D. Criminal dies** - victim PK 1-3 / 4-11 / 12+ -> ApplyCombatKilledPenalty tier 3 / 6 / 12
(S :19444-19452): PK -1, exp loss 1d(5 x tier x level), up to tier items dropped (one fewer under 80, min
1); the killer gets PK_KillRewardHandler bounty if innocent (S :28058) [V].

**E. Bounty redemption** - city hall -> `REQ_GETREWARDMONEY` (S :15515) -> GetRewardMoneyHandler
(S :28438): gold paid up to half of the remaining carry weight; the rest stays banked (REWARDGOLD) [V].

**F. Respawn routing (restart or logout while dead)** - RequestRestartHandler (S :46246) and DeleteClient
(S :2645-2705): traveller -> `default`; citizen killed on the enemy town map -> that town's jail for 180 s
(Aresden in "elvine" -> elvjail); else level > 80 -> resurr1/resurr2, otherwise own farm; in crusade a
second death within 10 min locks the player in the home town for 5 min (S :2659-2668, :2681-2690) [V].

**G. Force recall** - entering the enemy town (S :23695-23705 on teleport, S :2094-2113 on login) arms
`m_iTimeLeft_ForceRecall` via SetForceRecallTime (Mon-Thu 3 min, Fri 15, Sat 45, Sun 60, or the GM value;
20 ticks = 1 min; both callers zero `m_iTimeLeft_ForceRecall` just before the call, S :2090, :23695, so the first-entry branch always runs) -> countdown in CheckClientResponseTime (paused in crusade unless in a building,
S :3999-4001) -> `NOTIFY_TOBERECALLED` and teleport home (S :4002-4011) [V]. Travellers entering
aresden/elvine during crusade get 1 tick (S :2115-2123); a sided non-admin entering an enemy farm or building map (map side from iGetMapLocationSide) gets 1 tick as well and `m_bIsInBuilding` is set, which the crusade pause rule at S :3999-4001 reads (S :2126-2144, :23729-23750); fightzones recall at the next even hour minus 2
min (S :2150-2160); jails also set a war location (S :2162-2172) [V].

**H. Hunter mode** - `REQUEST_HUNTMODE` -> RequestHuntmode (S :52782): PK 0, init complete, map name
"cityhall*", level <= 100 (enable test S :52793) -> m_cLocation "arehunter"/"elvhunter", RESPONSE_HUNTMODE, appearance broadcast;
three thresholds: enabling allows level <= 100, bCheckLevelUp forces it off at level >= 100 (S :27235), the login check forces it off above 100 (S :2085); outside crusade hunters
cannot attack or be attacked by players (S :12342, :12565) and are recalled from war maps (S :4094-4111) [V] ([PK-LAW-007](../issues/pk-law.md#pk-law-007-hunter-mode-level-thresholds-disagree)).

## 7. Rules and invariants
Penalty formulas, tier table, reward formulas: `mechanics.md` section 6. Verified additions:
- **Criminal tiers**: 0 innocent; 1-3, 4-11, 12+ map to penalty tier 3, 6, 12 (S :19438-19474) [V].
- **Innocent killed by a monster**: tier 1, with item drops (v2.19 comment, S :19462) [V]; because the
  "one fewer below 80" decrement floors at 1, every non-neutral death drops at least one item unless a
  Stone of Sacrifice is carried (S :28307-28331); with the stone, no other item drops and the stone itself is dropped instead, losing 1 life span (S :28346-28376) [V].
- **PK count heals only by dying**: -1 per death, in or out of crusade (S :28275, :28286) [V].
- **Rewards require innocence**: PK_KillReward and EnemyKillReward pay nothing to a killer with PK > 0 [V cm].
- **No law on NOPENALTY_NOREWARD maps** (S :19319); self-kill ignored (S :19334) [V].
- **Jail time counts only while online** - the lock timer is decremented in the per-client loop
  (S :3986-3992) and the lock is persisted (S :6588-6592) [V].
- **GM close-connection** (`/closeconn`, `DeleteClient(i,TRUE,TRUE,TRUE,TRUE)`, S :36707) puts the character in `bisle` with a 10-minute lock (S :2706-2718). Other forced deletions (`DeleteClient(i,TRUE,TRUE)`) do not set `bForceCloseConn` and take no lock [V].
- **Town guards** exist only in aresden, elvine and default; elsewhere no guard spawns; never in crusade
  [V cm].
- **Force recall** never applies to admins (S :2097, :2107, :23702) [V].
- **Hunter spell limits** (not in crusade; S :21060-21085) [V]: a hunter cannot cast on a player of the other side, and the other side's hunters cannot be targeted; a hunter casting on a non-hunter is blocked for SPDOWN_AREA, SUMMON, PROTECT, HOLDOBJECT, INVISIBILITY, BERSERK, POISON and HPUP_SPOT (cross-link `magic.md`).
- **Reward exp** (enemy kill) is added directly to `m_iExp`, bypassing GetExp, so no party split or farm cut applies (S :28129, :28165, :28192, :28228) [V].
- **Bounty collection** (GetRewardMoneyHandler, S :28438) has no city-hall location check on the server; the city-hall requirement is a client-side menu condition ([PK-LAW-006](../issues/pk-law.md#pk-law-006-bounty-collection-has-no-city-hall-check-on-the-server)).

## 8. Couplings
| Direction | Other system (owner) | Coupling point |
|---|---|---|
| pk-law <- | combat / magic (`combat.md`, `magic.md`) | ClientKilledHandler callers; bAnalyzeCriminalAction; legality filters read `m_iPKCount`, `m_bIsHunter`, `m_bIsNeutral`, `m_iIsOnTown` |
| pk-law -> | character (`character.md`) | exp loss on penalties; traveller cap; hunter reset in bCheckLevelUp; rating |
| pk-law -> | items (items/trade cluster) | _PenaltyItemDrop -> DropItemHandler; Stone of Sacrifice; reward gold as an item; hero mantle |
| pk-law -> | map / teleport (world cluster) | RequestTeleportHandler locked-map override; jail, resurr, farm maps; `m_cLocationName`; iSetSide on every teleport |
| pk-law -> | world/entity | guard NPC creation (bCreateNewNpc, bSetNpcAttackMode) |
| pk-law -> | party / guild (social cluster) | citizenship drops party; guild creation forbidden to neutrals/hunters/PK (mech 8) |
| pk-law -> | crusade (social/world) | no death penalty in crusade, EK war contribution, crusade town lock, hunter recall at crusade end (LocalEndCrusadeMode -> RecallHunterPlayer) |
| pk-law -> | gate (infrastructure) | GSM_REQUEST_SETFORCERECALLTIME relay for `/setforcerecalltime` |
| pk-law -> | logging (infrastructure) | _bPKLog |
| pk-law -> | client presentation | name colour from iGetPlayerABSStatus bits, _iGetFOE, PK messages |

## 9. Current behaviour vs intent
- [V] **Traveller flag persists after citizenship**: iSetSide sets `m_bIsNeutral = TRUE` for travellers and never resets it, and RequestCivilRightHandler changes `m_cSide` without touching it, so until the next login a fresh citizen keeps traveller protections ([PK-LAW-001](../issues/pk-law.md#pk-law-001-traveller-flag-is-never-cleared-after-citizenship)).
- [V] **Same-side town protection is off in the export** (S GlobalDef.h:48, :85); same-nation citizens can fight in their own town, deterred only by guards and the jail ([COMBAT-001](../issues/combat.md#combat-001-same-side-town-protection-is-compiled-out-in-the-selected-build)).
- [V] **Jail routing**: ApplyPKpenalty jails by map *location name* (S :28030, :28045) while respawn routing tests the *map name* "elvine"/"aresden" (S :2673, :2695, :46278, :46300) ([PK-LAW-002](../issues/pk-law.md#pk-law-002-jail-routing-tests-location-name-respawn-routing-tests-map-name)).
- [V] **Force-recall residual branch**: SetForceRecallTime has a "residual time" branch (S :52754-52776) that never runs because both callers zero `m_iTimeLeft_ForceRecall` first; the v2.19 2002-11-28 change (3 min Mon-Thu, S :52737-52740; Korea keeps 1 min under DEF_CONSULTATION) is the only live value ([PK-LAW-003](../issues/pk-law.md#pk-law-003-force-recall-residual-time-branch-is-dead-code)).
- [V] A comment says "bleeding teleport 5 minutes" above a 3-minute jail (S :28032, :28047).
- [V] **Duplicate victim branches** for citizen vs guildsman in ClientKilledHandler (S :19350-19414) ([COMBAT-003](../issues/combat.md#combat-003-duplicated-victim-branches-in-clientkilledhandler)).
- [V] **Traveller-on-innocent PK branch**: a neutral attacker cannot damage an innocent (S :12603, :31216), yet ClientKilledHandler still applies ApplyPKpenalty for that case (S :19341, :19357, :19390) ([PK-LAW-004](../issues/pk-law.md#pk-law-004-traveller-on-innocent-pk-branch-is-mostly-unreachable)).
- [V] Citizenship can be granted on any map: the handler uses the current map's location name with no city-hall check ([PK-LAW-005](../issues/pk-law.md#pk-law-005-citizenship-can-be-granted-on-any-map)).

## 10. Open questions
- Which maps have a location name equal to a town while having a different map name (needs the map
  configs; world cluster) [U].
- Whether guards spawned by bAnalyzeCriminalAction despawn or persist (NPC cluster) [U].
- The exact client presentation of PK tiers (colour per count) was not traced beyond _iGetFOE [U].

## 11. Doc discrepancies
- `mechanics.md` 6 says the killer jail is "180 time units ... so about 3 min [I]"; the counter is in seconds
  (-3 per 3 s tick, S :3986), so exactly 180 s of online time [V].
- `combat-magic.md` ApplyPKpenalty: "with DEF_PKPENALTY it is always" - correct, and DEF_PKPENALTY exists
  only under DEF_CONSULTATION (Korea) (S GlobalDef.h:82-87) [V].
