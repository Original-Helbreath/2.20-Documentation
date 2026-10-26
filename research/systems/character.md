# System: character (stats, levels, exp, level-up, stat points, hunger, regen, majestic)

*Cohesive single doc: every subsystem here reads or writes the same CClient progression fields.
Formulas live in `research/game-server/mechanics.md` sections 1, 2 and 10 and are pointed to.
Citizenship, town alignment and hunter (civilian) mode are covered in `pk-law.md`.*

Sources: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`. Tags [V]/[I]/[U].

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow -
7. Rules - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose
The long-term progression loop: kill and work -> exp -> levels -> three stat points per level -> stronger
character, up to level 180, after which surplus exp becomes "majestic" (Gizon) item-upgrade points.
Around it sit the upkeep systems that make time matter: hunger slows regeneration, HP/MP/SP refill on
timers, and an automatic trickle of exp rewards simply being online. Intent visible in code:
- **Pre-committed stat growth** - the player sets a standing allocation (up to 3 points across six stats)
  that is applied automatically at every level-up (`S Game.cpp:27310`, `:27249-27286`) [V]; there is no
  "spend later" pool on the server.
- **Catch-up for the young** - levels <= 80 get bonus exp, low levels get gold on level-up, no hunger
  below level 20, extra SP regen at <= 60 (mech 1-2) [V].
- **Anti-bot / anti-speed-hack** - more than 300 000 exp in an hour sets a one-month block on the character (`S :4030`) [V].
- **Majestic endgame** - at max level, each table step of surplus exp gives one Gizon point, cap 37
  (`S :27218-27230`) [V]; spending them is the crafting/items cluster's job.
- **Data repair on load** - illegal stat totals are trimmed or topped up to the level budget
  (`S :34878`) so a corrupted or edited save cannot carry impossible stats [V ch].

## 2. Subsystems
| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Stats and derived values | Str/Vit/Dex/Int/Mag/Cha (cap 200) -> max HP/MP/SP, carry weight, defence | everything |
| 2.2 | Exp intake | GetExp: low-level bonus, farm-map cuts, party split; m_iExpStock buffer | combat, crafting, gathering |
| 2.3 | Exp settlement | CalcExpStock every 10 s; no-reward maps discard; level-up gold | 3 s timer |
| 2.4 | Level-up and stat allocation | standing LU_* allocation; bCheckLevelUp loop | 2.3 |
| 2.5 | Traveller level cap | neutral characters stop at 19 to push citizenship | 2.3 |
| 2.6 | Majestic (Gizon) points | max-level surplus exp -> upgrade points | 2.4 |
| 2.7 | Auto exp | Level exp per 6 min for idle-but-online players below a level step | 3 s timer |
| 2.8 | Regeneration | HP 15 s, MP 20 s, SP 10 s, slowed by hunger | 3 s timer |
| 2.9 | Hunger | -1 per minute (level >= 20), notifies below 30, failure chance at <= 10 | 3 s timer |
| 2.10 | Poison upkeep | 12 s damage tick, cure roll (shared with magic) | 3 s timer |
| 2.11 | Map level gates | force recall below a map's level limit or above its upper limit | 3 s timer |
| 2.12 | Speed-hack catcher | exp-per-hour block | 3 s timer |
| 2.13 | Load-time repair | ___RestorePlayerCharacteristics, ___RestorePlayerRating | login |
| 2.14 | Reputation and profile | /rep+ /rep-, /setpf /pf | chat |
| 2.15 | Client character UI | character sheet, level-up settings dialog, gauges | client |

## 3. Functions
Details in `research/game-server/functions/character.md` (ch).

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | iGetMaxHP | game server | S Game.cpp:42328 | Vit*3 + Level*2 + Str/2, MaxHPdown divisor [V ch] |
| 2.1 | CalcTotalItemEffect | game server | S Game.cpp:35250 | recompute derived stats from gear (items cluster owns gear) [V ch] |
| 2.1 | _iCalcMaxLoad / iCalcTotalWeight | game server | S Game.cpp:28572 / :34605 | carry limit and load [V ch] |
| 2.2 | GetExp | game server | S Game.cpp:51636 | exp intake into m_iExpStock [V ch] |
| 2.3 | CalcExpStock | game server | S Game.cpp:36374 | settle stock, level-up gold, notify EXP [V ch] |
| 2.3 | CheckClientResponseTime | game server | S Game.cpp:3827 | the 3 s per-client upkeep loop driving 2.3, 2.7-2.12 [V] |
| 2.4 | LevelUpSettingsHandler | game server | S Game.cpp:27310 | validate and store LU_* (each 0-3, sum <= 3) [V] |
| 2.4 | bCheckLevelUp | game server | S Game.cpp:27210 | level loop, stat add, cap 200, NOTIFY_LEVELUP, Gizon at max [V] |
| 2.4 | iGetLevelExp | game server / client | S Game.cpp:27183 / C :23051 | exp table formula [V ch] |
| 2.4 | DrawDialogBox_LevelUpSetting / DlgBoxClick_LevelUpSettings / _iCheckLUS | client | C Game.cpp:38095 / :20719 / :14712 | allocation UI; sends MSGID_LEVELUPSETTINGS [V map] |
| 2.4 | NotifyMsg_LevelUp | client | C Game.cpp:44368 | new level and stats, sound, "Level up!" bubble [V] |
| 2.5 | bCheckLimitedUser | game server | S Game.cpp:27531 | clamp neutral exp below level-20 entry [V ch] |
| 2.6 | bCheckLevelUp (max-level branch) | game server | S Game.cpp:27218-27230 | Gizon point, NOTIFY_GIZONITEMUPGRADELEFT [V] |
| 2.7 | CheckClientResponseTime (auto exp) | game server | S Game.cpp:3942-3956 | auto exp grant [V] |
| 2.8 | TimeHitPointsUp / TimeManaPointsUp / TimeStaminarPointsUp | game server | S Game.cpp:20297 / :20343 / :20375 | regen ticks [V ch] |
| 2.9 | CheckClientResponseTime (hunger) | game server | S Game.cpp:3878-3900 | hunger decrement, HUNGER notify, regen delay [V] |
| 2.9 | NotifyMsg_Hunger | client | C Game.cpp:43713 | hunger messages [V] |
| 2.10 | PoisonEffect | game server | S Game.cpp:35975 | poison tick [V ch] |
| 2.11 | CheckClientResponseTime (level gates) | game server | S Game.cpp:4061-4075 | TOBERECALLED + recall [V] |
| 2.12 | CheckClientResponseTime (speed hack) | game server | S Game.cpp:4027-4057 | month block, disconnect [V] |
| 2.13 | ___RestorePlayerCharacteristics / ___RestorePlayerRating | game server | S Game.cpp:34878 / :36430 | load-time repair [V ch] |
| 2.13 | InitPlayerData | game server | S Game.cpp:4855 | decode record, run repairs, compute next-level exp [V ch] |
| 2.14 | SetPlayerReputation / SetPlayerProfile / GetPlayerProfile | game server | S Game.cpp:36192 / :34811 / :34834 | /rep+ /rep- /setpf /pf (via ChatMsgHandler) [V ch] |
| 2.15 | NotifyMsg_Exp / NotifyMsg_HP / _MP / _SP | client | C Game.cpp:43490; dispatch C :31376-31393 | gauges and exp bar [V] |

## 4. Commands and messages
From `by-system.md` "character".
- Client -> game: `MSGID_LEVELUPSETTINGS` (S :15068), `MSGID_REQUEST_RESTART` (S :14974, shared with combat),
  `MSGID_REQUEST_CIVILRIGHT` (S :15023, pk-law). Slash: `/setpf <text>`, `/pf <name>`, `/rep+ <name>`, `/rep- <name>` [V].
- Game -> client: `PLAYERCHARACTERCONTENTS` on login; notifies `EXP`, `HP`, `MP`, `SP`, `LEVELUP`, `HUNGER`,
  `TRAVELERLIMITEDLEVEL`, `GIZONITEMUPGRADELEFT`, `PLAYERPROFILE`, `CANNOTRATING`, `RATINGPLAYER`,
  `TOBERECALLED` (level gate). `LIMITEDLEVEL` and `CHARISMA` have packing and client cases but no server
  caller (`overloaded.md`) [V].
- Game -> world log: `MSGID_REQUEST_SAVEPLAYERDATA` every 30 min outside fightzones (S :3931), save on
  logout in DeleteClient; `MSGID_REQUEST_PLAYERDATA` / `RESPONSE_PLAYERDATA` on login (infrastructure) [V].

## 5. Data
- CClient progression fields: `m_iLevel`, `m_iExp`, `m_iExpStock`, `m_iNextLevelExp`, `m_iAutoExpAmount`,
  `m_iStr/Vit/Dex/Int/Mag/Charisma`, `m_cLU_Str..m_cLU_Char`, `m_iHP/MP/SP`, `m_iHungerStatus`,
  `m_bIsPoisoned`, `m_iPoisonLevel`, `m_iGizonItemUpgradeLeft`, `m_iRating`, `m_iTimeLeft_Rating`,
  `m_cProfile`, `m_iSpeedHackCheckExp`, `m_iPenaltyBlockYear/Month/Day`, `m_iTimeLeft_FirmStaminar` [V at uses].
- Timer stamps: `m_dwHPTime`, `m_dwMPTime`, `m_dwSPTime`, `m_dwHungerTime`, `m_dwPoisonTime`,
  `m_dwExpStockTime`, `m_dwAutoExpTime`, `m_dwAutoSaveTime`, `m_dwSpeedHackCheckTime` [V S :3878-4057].
- Persisted (player record, S Game.cpp:6428 binary / :8917 text): LU_* (S :6491-6511), rating (S :6686),
  `penalty-block-date`, `gizon-item-upgade-left` (sic), `timeleft-firm-staminar`, plus stats, level, exp,
  hunger [V names].
- Game tables: `m_iLevelExpTable[]` (bInit), `m_iPlayerMaxLevel` (DEF_PLAYERMAXLEVEL 180 or world config),
  `m_iLevelExp20` (traveller clamp) [V ch].
- Constants (S Game.h): DEF_HPUPTIME 15 s, MPUPTIME 20 s, SPUPTIME 10 s, HUNGERTIME 60 s, POISONTIME 12 s,
  EXPSTOCKTIME 10 s, AUTOEXPTIME 6 min, AUTOSAVETIME 30 min (:60-73); TOTALLEVELUPPOINT 3 (:74),
  CHARPOINTLIMIT 200 (:87), LEVELLIMIT 20 (:174, comment "trial version level limit"), MAXGIZONPOINT 37
  (:206) [V].

## 6. Flow
**A. Exp to level-up**
1. A source calls GetExp (S :51636) - combat per hit, kill share, crafting, fishing, mining, quests -> adds to
   `m_iExpStock` after multipliers [V ch].
2. Every 10 s the upkeep loop calls CalcExpStock (S :3938 -> :36374): stock moved into exp unless on a
   no-reward map; bCheckLimitedUser clamps travellers; bCheckLevelUp runs; NOTIFY_EXP; level-up gold for
   levels 1-20 [V ch].
3. bCheckLevelUp (S :27210): at max level -> Gizon branch (S :27218-27230); hunter reset at level >= 100
   (S :27235); while exp >= next: level++, add LU_* to each stat still below 200 (S :27249-27276),
   clamp to 200, NOTIFY_LEVELUP, next-level exp from the table, CalcTotalItemEffect (S :27280-27295) [V].
4. Client NotifyMsg_LevelUp (C :44368) updates level and six stats, plays the gender sound, shows a bubble;
   the next save persists it [V].

**B. Setting the allocation** - client LevelUpSetting dialog (C :38095), `_iCheckLUS` flags unspent points or
stats over 200 (C :14712) -> `MSGID_LEVELUPSETTINGS` -> LevelUpSettingsHandler (S :27310): each value 0-3,
sum <= 3, otherwise the whole message is ignored silently [V].

**C. Upkeep tick (every client, once per pass; the pass runs when more than 3000 ms have elapsed, S :3682-3685, so the period is 3 s or slightly more; S :3827)** - timeout check -> shut-up and rating counters ->
hunger (S :3878-3895) -> regen with hunger delay (S :3897-3918) -> poison (S :3921) -> autosave (S :3927) ->
exp stock (S :3936) -> auto exp (S :3942) -> special ability (S :3961) -> locked-map and dead-penalty
timers (S :3986-3996, pk-law) -> force recall (S :3997-4011, pk-law) -> skill flood (S :4016) -> speed hack
(S :4027) -> level gates (S :4061-4075) -> hunter area recall (S :4094-4111, pk-law) -> crit refill
(S :4115) [V].

## 7. Rules and invariants
Full formulas (max HP/MP/SP, regen, exp table, GetExp, party share, auto exp, carry weight): `mechanics.md`
sections 1, 2, 10. Verified additions:
- **Hunger slows all three regens**, not only HP: the same `iPlusTime = (30 - hunger) s` is added to the HP,
  MP and SP intervals (S :3897-3918) [V]. At hunger 0 none of the three regens run (TimeHitPointsUp /
  TimeManaPointsUp / TimeStaminarPointsUp return when `m_iHungerStatus <= 0`, S :20297-20412) [V].
- Hunger falls only for level >= DEF_LEVELLIMIT (20) non-admins and only while alive (S :3880-3886); the
  HUNGER notify is sent only below 30 and while HP > 0 (S :3890-3892) [V].
- **Allocation is all-or-nothing per message** (S :27310 validation) and **applied per stat only if that stat
  is below 200**; points aimed at a capped stat are lost, and a stat at 199 receiving 3 is clamped to 200
  (S :27251-27286) [V].
- **One Gizon point per bCheckLevelUp call**: surplus is reset to the max-level table value, so any exp
  beyond one step in the same settlement is discarded (S :27220-27225) [V].
- Level loop guard: more than 100 iterations returns FALSE ("file manipulation by hacking", S :27246) [V].
- Map level gates exempt admins (S :4061, :4069) [V].
- Speed-hack block date is "same day next month" with December rolling to January (S :4033-4044) [V].

## 8. Couplings
| Direction | Other system (owner) | Coupling point |
|---|---|---|
| character <- | combat (`combat.md`) | GetExp per hit and on kill; ApplyCombatKilledPenalty / ApplyPKpenalty subtract exp (pk-law) |
| character <- | crafting, gathering, quests (items/social clusters) | GetExp callers BuildItemHandler, ReqCreatePortionHandler, ReqGetFishThisTimeHandler, _CheckMiningAction, bCropsItemDrop, _iTalkToNpcResult_Cityhall |
| character <- | party (social cluster) | GetExp party split reads `m_iPartyID`, party member HP and `m_dwLastActionTime` |
| character -> | items (items/trade cluster) | CalcTotalItemEffect on level-up; carry weight; level-up gold via bAddItem; Gizon points spent in item upgrade |
| character -> | pk-law (`pk-law.md`) | hunter reset in bCheckLevelUp; traveller cap pushes citizenship; locked-map timers in the same loop |
| character -> | map (world cluster) | `m_iLevelLimit` / `m_iUpperLevelLimit`, map type NOPENALTY_NOREWARD, farm maps in GetExp |
| character -> | persistence (infrastructure) | autosave MSGID_REQUEST_SAVEPLAYERDATA (S :3931); InitPlayerData decode; character block date (`m_iPenaltyBlock*`, stored with the character) |
| character -> | gate (infrastructure) | none direct; guild/party side effects on login go through other systems |
| character <- | skills (`skills.md`) | play-dead suppresses regen; stat values cap skills |
| character -> | client presentation | gauges, character sheet, level-up dialog |

## 9. Current behaviour vs intent
- [V] **`break` rather than `continue` in the upkeep loop**: after the skill-message flood kick or the speed-hack block deletes a client, the loop leaves and higher-numbered clients are not processed in that tick (S :3853-4132; see [COMBAT-004](../issues/combat.md#combat-004-checkclientresponsetime-breaks-out-of-the-client-loop-after-a-deletion)).
- [V] **No max-level stop inside the level loop**: bCheckLevelUp checks max level only on entry (S :27218); one large settlement can raise the level past m_iPlayerMaxLevel (S :27245-27298); the table extends to 299 so it does not crash (mech 2) ([CHARACTER-001](../issues/character.md#character-001-no-max-level-stop-inside-the-level-up-loop)).
- [V] **DEF_LEVELLIMIT** is named and commented as the trial-version level limit (S Game.h:173-174) but used only as the hunger threshold (S :3883) ([CHARACTER-002](../issues/character.md#character-002-def_levellimit-is-documented-as-a-trial-limit-but-used-as-the-hunger-threshold)).
- [V] Flags bStr..bChr in bCheckLevelUp are set and never read (S :27242-27276) ([CHARACTER-003](../issues/character.md#character-003-unused-per-stat-flags-in-bchecklevelup)).
- [V] **LIMITEDLEVEL / CHARISMA notifies** have no server sender (`overloaded.md`) ([CHARACTER-004](../issues/character.md#character-004-limitedlevel-and-charisma-notifies-have-no-server-sender)).
- [V] Comment in CalcExpStock history (v2.12 -> v2.15) shows majestic characters were briefly blocked from gaining exp, then allowed again (ch).

## 10. Open questions
- Where `m_iLevelExp20` and `m_iPlayerMaxLevel` overrides come from at runtime (world config message;
  infrastructure) [U].
- How the official world log server validates stats on save, if at all (no source; decomp only) [U].
- Whether the client gauges recompute max HP with the same formula including MaxHPdown (client side not
  traced) [U].

## 11. Doc discrepancies
- `mechanics.md` 1 says "When hunger is 30 or less the interval grows" under HP regen only; the code applies
  the same delay to MP and SP (S :3909-3918) [V].
- `mechanics.md` 10 "Hunger drops 1 per minute, but not below level 20" matches DEF_LEVELLIMIT 20 [V].
