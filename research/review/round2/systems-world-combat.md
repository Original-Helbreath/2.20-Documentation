# Round 2 review: systems world-combat group

Docs: map, tile, movement, entity, gathering, combat, magic, skills, character, pk-law (research/systems/).
S = sources/hgserver-librehelbreath/src/Game.cpp unless stated; C = sources/client-2.20/Game.cpp.
Line numbers are the working tree (match the doc numbering). Confidence: H = read in source, M = read, inference involved, L = unverified.

## combat.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| COMBAT-1 | sec 7 legality list | Missing the tile-attribute safe-zone / physical-protection check for PvP melee: iGetAttribute(...,0x6) on attacker and target tiles. This is the main safe-town mechanism. Magic uses mask 0x5. | S:12906-12907; magic S:31242, S:21046 | Add to the legality list and to the magic cross-reference | H |
| COMBAT-2 | crit section | Missing crit-counter refills: hit-taken refill (m_iSuperAttackCount > 14), AddChargeCritical chance refill, magic-hit refills. Hit-taken shares the counter with the 3 s timer. | S:13847-13857, S:13767-13775, S:31384, S:31936 | Add a refill-sources list | H |
| COMBAT-3 | _CheckAttackType | "Wrong weapon" crit rejection is overstated: only bows are matched to the weapon; otherwise only mastery of the claimed type's skill is checked. Crit bonus switch uses m_sUsingWeaponSkill. | S:38193-38257 | Reword to the actual checks | H |
| COMBAT-4 | safe mode / magic | Safe mode halves damage for melee only. Spells get a +40% mana surcharge, no halving. "Effect_Damage_Spot reuses the same filters" is too broad. | S:12612-12631; S:31224-31239; S:20972 | Split melee and magic rules | H |
| COMBAT-5 | open question 1 | Answerable: ATTACK_CONFIRM and plain CONFIRM both only decrement m_cCommandCount. | C:35515-35523 | Close the question | H |
| COMBAT-6 | open question 2 | Partly answerable: 7 attack messages inside 1480 ms (80*8*7-3000), and a client-clock gap under 500 ms. | S:11304-11317; S:52627 | Fill in | H |
| COMBAT-7 | ALLPLAYERPROTECT remark | Imprecise: DEF_ALLPLAYERPROTECT is commented out everywhere, even inside DEF_CONSULTATION. | GlobalDef.h:83 | Reword | H |
| COMBAT-8 | special ability | Source of the ability is item-granted via CalcTotalItemEffect and cancels on unequip; initial cooldown at login is 1200. Neither is mentioned. | S:35760-35812; Client.cpp:228 | Add | H |
| COMBAT-9 | sec 9 break list | Lists :4082 as a loop break, but that line is in a comment. Force recall does not delete the client; only the :4020/:4050 DeleteClient makes the later breaks matter. | S:4081-4090, S:4020, S:4050 | Remove :4082 and explain | H |
| COMBAT-10 | cite | :19261 should be :19262. | S:19262 | Fix | H |
| COMBAT-11 | "shipped DEF_CHINESE build" | Inference stated as fact. | GlobalDef.h:16 | Mark [I] | M |

Verified OK: ClientMotionHandler 976-1126 (ATTACK 1085, ATTACKMOVE 1070, dash, crit decrement 1090-1094); filters 12339-12606; lag protection :13147, :31212; bAnalyzeCriminalAction melee only on the killing blow :13745; iTemp=10 at 1117; crit refill 4115-4124; cooldown 3961-3983; ClientKilledHandler duplicate branches 19350-19414; killer-name commented out in client C:44355-44364; fightzone resurrection commented out 4081-4090; all function definition lines.

## pk-law.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| PKLAW-1 | force-recall residual cap | Wrong and moot. Both callers zero m_iTimeLeft_ForceRecall right before SetForceRecallTime, so the residual branch is unreachable. Where reached it sets 1 tick (~3 s), not a 1 min cap. | S:2090-2113, S:23695-23719, S:52754-52776 (52773-52774) | Replace with "dead code" | H |
| PKLAW-2 | sec G | Omits the immediate 1-tick recall when a sided non-admin enters an enemy farm or building; it sets m_bIsInBuilding, which the crusade pause rule at :4000 depends on. | S:2126-2144, S:23729-23750 | Add | H |
| PKLAW-3 | (gap) | Hunter spell restrictions missing: hunters cannot cast on other-side players; hunter to non-hunter blocked for SPDOWN_AREA, SUMMON, PROTECT, HOLDOBJECT, INVISIBILITY, BERSERK, POISON, HPUP_SPOT. Also absent from magic.md and mechanics.md. | S:21060-21085 | Add here and cross-link | H |
| PKLAW-4 | hunter level | Off-by-one: enable allows level <= 100, bCheckLevelUp forces off at >= 100, login check is > 100. | S:52793, S:27235, S:2085 | Document the three thresholds | H |
| PKLAW-5 | bisle trigger | "Forced disconnect -> bisle 10 min" fires only from the GM close-connection command (bForceCloseConn TRUE), not hack kicks (DeleteClient(i,TRUE,TRUE)). | S:36707 | Narrow the trigger | H |
| PKLAW-6 | GetRewardMoneyHandler | No city-hall location check server-side; intent is client-side only. | S:28438 | Note as client-only check | H |
| PKLAW-7 | Stone of Sacrifice | Not "no drops": the stone itself is dropped instead. | S:28346-28376 | Reword | H |
| PKLAW-8 | reward exp | Reward exp is added directly to m_iExp, bypassing GetExp. | S:28129, 28165, 28192, 28228 | Add | H |
| PKLAW-9 | hunter EK | "Hunters don't raise EK" is likely ineffective: location "arehunter" never equals a map name. | S:28083 area | Mark [I] | M |
| PKLAW-10 | cites | Admin-exemption cites 2096/2106/23699 are 2097/2107/23702. | S:2097, 2107, 23702 | Fix | H |

Verified OK: ApplyPKpenalty 27986-28055; PK_KillRewardHandler 28058; EnemyKillRewardHandler 28083; ApplyCombatKilledPenalty 28263-28335; tiers 19442-19475; DeleteClient respawn routing 2644-2718; stale m_bIsNeutral (52894 sets TRUE, never reset; Client.cpp:214); RequestCivilRightHandler 27547-27616; hunter mode 52782-52854.

## map.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| MAP-1 | "-1 = initial point" comment | Cite 23422-23423 is wrong. | S:23431-23432 | Fix | H |
| MAP-2 | poison clear | Cite 23525 wrong. | S:23533 | Fix | H |
| MAP-3 | RTH_NEXTSTEP start | Cite 23521 wrong. | S:23529 | Fix | H |
| MAP-4 | memcmp 10 | Cite 23347 wrong. | S:23349 | Fix | H |
| MAP-5 | "20x20 heat map" | Ambiguous: 20-tile cells in a 60x60 array (MAXSECTORS 60). | Map.h:47; S:1185 | Reword | H |
| MAP-6 | client enemy-building pre-check | Under `#if DEF_LANGUAGE == 3`, not stated. Server enforces by immediate recall (see PKLAW-2). | C:45570-45610; S:2126-2144 | Add the condition and server side | H |
| MAP-7 | "-1 works only by accident" | Conflicts with the explicit fallback comment. | S:11497 | Mark [I] | M |

Verified OK: _bRegisterMap 5488; bInit/amd decode 392-512; bSearchTeleportDest 515; RequestTeleportHandler 23250-23560; guards 23261-23264; enemy-town block 23268-23281; locked-map override 23324-23342; recall level 80 rule 23394-23405; dead block 5052-5201; pData[0]=0x01 matches no case; GetMapInitialPoint uninitialised locals (42358, 11484/11499).

## tile.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| TILE-1 | overrun condition | Imprecise for non-square maps: overruns when SizeY exceeds SizeX by more than 1. | Map.cpp (bInit area) | Reword | M |
| TILE-2 | ClearOwner | "Clears both when handle matches" is loose; two independent checks. | Map.cpp ClearOwner | Reword | M |
| TILE-3 | cite 1219-1250 | Starts at 1221. | S:1221 | Fix | H |

Verified OK: Map.cpp 180-383, 581-717, 773-796 (bRemoveCropsTotalSum bug real); Tile.h; DynamicObjectID.h; iAddDynamicObjectList 28579-28640 (rain factors); by-value pTopItem at Map.cpp:247; mine code commented S:1251-1307; occupy confirm 1358-1359; ClearDeadOwner and ClearSectorInfo have no callers; stale-handle repair 3401-3418.

## movement.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| MOVE-1 | hold-person cite | Doc 1215-1216, actual 1218-1219; quest check doc 1222, actual 1224. | S:1218-1224 | Fix | H |
| MOVE-2 | sec 9 SP wording | Misleading: cost byte is 1 whenever firm stamina is 0 in both SP>0 and SP<=0 branches; branches differ only in the < -10 kick. | S:1329-1353 | Reword | H |
| MOVE-3 | cGetNextMoveDir | Tries one side only (+1,+2 for turn 0; -1,-2 for turn 1), not both. | S cGetNextMoveDir | Reword | H |
| MOVE-4 | (gap) | Attack-rate checks absent (500 ms client gap, 7 in 1480 ms). | S:52627, S:11304-11317 | Add or link to combat | H |

Verified OK: 7-step windows (1032/908 ms) 1149-1180; 250 ms gap 52699; reject packet 1381-1483; range logic 3067-3070; client MotionResponseHandler C:35498-35697; lag counter C:4292-4301; _bCheckMoveable C:1402-1429; feign-death DeleteClient 16493-16498.

## entity.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| ENT-1 | summon lifetime | Ogre (type 29) summons expire after 90 s, not 5 min as the doc says for all. | S:10519-10521 | Add exception | H |
| ENT-2 | crops flow | Crops (type 64) return early from bCreateNewNpc at 10353 and skip the EVENT_LOG CONFIRM at 10360; __bSetAgricultureItem sends it itself. | S:10353, 10360, 53047 | Add | H |
| ENT-3 | /tgt | Target must be an other-side, non-neutral player on the same map; undocumented. | S:39945, 39956-39957 | Add | H |
| ENT-4 | taming | A caller exists (S:33737) though the effect is a stub; skills.md says "no caller". Reconcile. | S:33737; S:46085-46142 | Align both docs | H |

Verified OK: +10000 ids 2394/2494/3199/3494/3593; bCreateNewNpc flow (10083, 10168, 10343-10353); NpcProcess timing 10444-10461; summon flow 21999-22082; SetSummonMobAction 39893-39976; MobGenerator crusade `break` 29038 and overwrite 29030-29032.

## gathering.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| GATH-1 | delay event name | "DelayEventProcess" should be DelayEventProcessor; DelayEventProcess at 20414 is an empty stub. | S:33021, S:20414 | Fix | H |
| GATH-2 | "nine species" | Really 8 fish plus a rare-item case where only 13 of 150 rolls give an item; the rest leave iItemID=0 and sDifficulty uninitialised. Wasted pass is inferred. | S:37255-37357 | Reword; mark the waste [I] | M |
| GATH-3 | FishProcessor | "else lowers": equality does nothing (`break` at 36956). | S:36947-36986 | Fix | H |
| GATH-4 | harvest | At BuildCount 1 the drop is probabilistic but DeleteNpc is unconditional, so a crop can be destroyed for nothing; drop is zero when skill tier < crop tier; skill gain only when skill <= cropLevel+10. | S:12806-12810, S:12800 | Add | H |
| GATH-5 | minerals | MineralGenerator cadence (10 s) missing; minerals never expire (dwLastTime NULL in iCreateMineral). | S:3811-3816 | Add | H |

Verified OK: iCalculateUseSkillItemEffect 33700-33798; iCheckFish 36901-36945; ReqGetFishThisTimeHandler 37201-37252; _CheckMiningAction and difficulty/remain table 38796-39151; item-use skill delay 31057-31090 (last use registers no delay event); farming 12780-12887, 52945-53215; plant relocation claim.

## magic.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| MAGIC-1 | cast throttle | bCheckClientMagicFrequency (1500 ms) is defined but never called; the only throttle is the 100 ms m_dwRecentAttackTime. | S:52645; S:20866-20899 | State as dead path | H |
| MAGIC-2 | iTemp=10 comment | Says "caster's level"; source comment says magic ability level. | S:1116 | Fix | H |
| MAGIC-3 | hunter restrictions | Missing (see PKLAW-3). | S:21060-21085 | Add | H |
| MAGIC-4 | "PvP filters identical to melee" | Overstated (see COMBAT-1, COMBAT-4). Category-1 spells are blocked on safe tiles. | S:21044-21047 | Reword | H |

Verified OK: iClientMotion_Magic_Handler 20773; PlayerMagicHandler gates 20866-20899; cast chance 20911-20965; mana +40% 20972-20976; hunger/SP 10% failure 21000; MP check 21007; GUILDWARMODE; recall 21993; iGetWhetherMagicBonusEffect 37465-37494; 50 CFG rows; 27 magic types; RequestStudyMagicHandler 24898-24941.

## skills.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| SKILL-1 | SSN doubling | "Combat skills 4-10, 14, 21 gain SSN twice as fast" holds only in CalculateSSN_SkillIndex. Melee weapon hits use CalculateSSN_ItemIndex (no doubling). Effectively only magic and bare hands are doubled. Same claim in mechanics.md sec 3; Flow A conflates the paths. | S:28850-28863; S:14219-14223; S:22806; S:14260 | Correct here and in mechanics.md | H |
| SKILL-2 | skill message flood | m_iSkillMsgRecvCount is incremented only by crafting handlers, not UseSkillHandler. | S:38273, S:43537; check S:4016, S:4023 | Re-attribute to crafting | H |
| SKILL-3 | client cite | bInitSkillCfgList is at C:8521, doc says 8577 "region". | C:8521 | Fix | H |
| SKILL-4 | special ability | Item-source gap (see COMBAT-8). | S:35760-35812 | Add | H |
| SKILL-5 | taming | "No caller" inconsistent with entity.md (ENT-4). | S:33737 | Align | H |

Verified OK: SSN math, stat caps, skill list 28700-28986; UseSkillHandler 33802-33909; SetDownSkillIndexHandler 39397; pretend-corpse regen stop 20306 and combat-toggle refusal 20526.

## character.md

| id | doc loc | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| CHAR-1 | break list | Includes :4082 (inside a comment); see COMBAT-9. | S:4081-4090 | Fix | H |
| CHAR-2 | upkeep tick | Is >3000 ms, not exactly 3 s. | S:3682-3824 | Reword | H |
| CHAR-3 | regen at hunger 0 | Rule not stated here (mechanics has it). | S:20297-20412 | Add | H |

Verified OK: upkeep loop 3853-4139; hunger 3880-3894; regen delay 3897-3919; regen functions 20297-20412; bCheckLevelUp 27210-27306 (Gizon 27218-27230); LevelUpSettingsHandler 27310-27357; bCheckLimitedUser 27531; speed-hack block 4027-4057; constants.

## Not done

- Cross-link resolution: the linked systems docs exist in research/systems (checked by listing, not per link).
- commands/ and shared/messages consistency not checked in depth.
- party.md exp-split coupling with combat.md not compared.
- NPC AI internals cited to npc.md not re-read.
- The 25-claim sample per doc is mostly met but not counted exhaustively.
