# Review: systems docs, group economy-social (round 2)

Docs: item, crafting, shop, trade, warehouse, chat, party, guild, crusade, arena, quest (research\systems\).
Ground truth: sources\hgserver-librehelbreath\src (S), client-2.20 (C), gateserver-librehelbreath at 0b994cd (G, read with `git show`).
Method: every cited headline gap re-read in source; 25+ [V] file:line citations sampled per doc (item ~35, crafting ~30,
shop ~30, trade ~28, warehouse ~25, chat ~30, party ~28, guild ~28, crusade ~30, arena ~25, quest ~25). All headline
intent-vs-implementation gaps were verified individually; all but the entries below held. Confidence: H/M/L.
Cross-links: every `*.md` link resolves (overloaded.md, notify-types.md, by-system.md etc. exist under systems\commands\; the
`overloaded.md`:27 and :66 line pointers land on the right rows). Consistency with mechanics.md / shared messages: no
contradiction found except the notes tagged "cross" below.

## item.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| IT-1 | item.md:9-10 | Active build-switch list is incomplete. `DEF_CHINESE` also turns on `DEF_CHINESEGUILDLIMIT`, `DEF_NPCITEM` and `DEF_CHINESECANDYEVENT`; `DEF_NPCITEM` is why only the 47193-47258 block of `bGetItemNameWhenDeleteNpc` compiles, and the doc never links the two. | S GlobalDef.h:65-69, 73-75; S Game.cpp:47190-47196 | Add the three switches to the header line and note `DEF_NPCITEM` selects the compiled block. | H |
| IT-2 | item.md:206-207 (9.1) | Cited line is 12866; the `||` condition is at S 12864 (12862-12870 block). Claim itself (`(iTemp != -1) \|\| (m_pItemList[iTemp] != NULL)`) verified. | S Game.cpp:12864 | Change to 12864. | H |
| IT-3 | item.md:114-116 (5) | Type list omits 2 APPLY, 4 INSTALL, 10 USE_SKILL_ENABLEDIALOGBOX; `UseItemHandler` therefore rejects them (only 3,6,7,8,9,11 pass). | S Item.h:34,36,41; S Game.cpp:30645-30652 | Add the three types, mark "not usable via UseItemHandler". | H |
| IT-4 | item.md:217 (9.5) | "13th-oldest item" is confusing: the 13th drop pushes out the oldest of 12 (index 11), no log. | S Map.cpp:324-326 | Reword: "the 13th item dropped on a tile deletes the oldest one". | M |

Verified, no issue: weight formula 28576, gold /20 47180, 50 slots, stack types 5/6 15911-15936, Str*100 equip gate 16003, neutral wear
13389/14242, lottery empty branches 30678-30688, bank over-weight dead `dTemp < 0` 6133, dup price 45944, log filter 42711-42713/42880, own-town check
commented out 16834-16835, split stacks recreated from definition 15624/17124/41055, retired type refund 6088-6094/6419, rare prefix 45955-45986.

## crafting.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| CR-1 | crafting.md:98-100 (6), 118 (7) | Normal-weapon cap path is silent: at +7 a successful roll sets `iValue = 7`, writes nothing and does NOT consume the stone (the `ItemDepleteHandler` call is inside the `else`). Doc only says "normal weapons +7". Same shape for crafted at +10. | S Game.cpp:52062-52090 | Add: "at the cap a winning roll is a no-op that keeps the stone". | H |
| CR-2 | crafting.md:115-117 (7) | Section says crafted-bonus is /20 (base 8-20) or /40 (base <=7). Verified. cross: mechanics.md "Economy / Upgrade success" says "which item class triggers the bonus is unverified" and gives /10,/20,/40; crafting.md is the more exact one (the /10 branch is dead). | S Game.cpp:52463-52471; mechanics.md section 8 upgrade line | Fix mechanics.md to point at crafting.md 9.4 (outside this group). | M |
| CR-3 | crafting.md:109 (7) | Skill 12 +5 at skill 100 is skipped on `DEF_TESTSERVER` (`iSkillLevel = 100` there). Not relevant to shipped build; noted for completeness. | S Game.cpp:38432-38439 | Optional footnote. | L |

Verified, no issue: BuildItem slot/consume mismatch 43621-43637 vs 43761-43774, `> DEF_MAXITEMS` 43594, use after `bAddItem` 43752, dead `iProb > 20` 52465,
no break in recipe loop 38401-38414, roll-before-validation 43580-43583, upgrade odds 52449-52461, Gizon cost 51869, 37 cap Game.h:206, stones 656/657 at 52045.

## shop.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| SH-1 | shop.md:93-94, 113 | NEW GAP: the crusade-winner discount test is `m_iCrusadeWinnerSide == m_cSide`. Winner is 0 at boot, after a draw, and during every running crusade (reset at `LocalStartCrusadeMode`), and travellers have `m_cSide == 0`. So (a) neutrals get the 10 % discount (and `cPrice = -10` shown) whenever there is no winner, (b) the real winner's discount vanishes for the duration of the next crusade. | S Game.cpp:862, 1942-1943, 4953, 16903, 23544, 47918 | Add as 9.10: "winner 0 matches neutrals". | H |
| SH-2 | shop.md:11, 100 (1, 7) | "Neutrals (travellers) sell at half value / Neutrals get half": code halves the already-halved price again (comment "half of half"), so neutrals receive a quarter of list price for category 11-50 items. | S Game.cpp:33947-33955 | Say "half of the normal sell price (a quarter of list)". | H |
| SH-3 | shop.md:9-13 (1) | Treasury intent ("money paid in is meant to fill the town treasury") is design intent tagged `[V, sections below]`; only a v1.4 comment supports it. | S Game.cpp:17046 comment, 17047 | Tag `[I]`. | M |
| SH-4 | shop.md:65-66 (5), 125 (9.1) | `iFunds` is indexed by `m_cSide` (0..2); neutrals credit index 0 which the boot reset (379-383, 730-734) never zeroes (only [1],[2]). Harmless, but "zeroed at boot" is only half true. | S Game.cpp:379-383, 730-734, 17047, Game.h:768-773 | Footnote. | M |

Verified, no issue: gaps 9.2-9.9 (WORD wTempPrice 17030-17047, `(short)d3` 34154 vs `(int)d3` 33984, `short sPrice` 34431/34509, repair-confirm without NPC check,
Cha<10 rejected 6301, shop zone 52910, sell list 12 pairs 46623, gold dropped on floor 34398). m_wPrice is DWORD (Item.h:114) so 9.3-9.5 are reachable with config prices > 65535.

## trade.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| TR-1 | trade.md:119-121 (9.2) | Correct but incomplete: a negative amount is rejected only because `iAmount` is converted to unsigned in `m_dwCount < iAmount`; amount 0 passes. Worth stating so the proposed fix (`iAmount > 0`) is not skipped. | S Game.cpp:40834 | Add one sentence. | M |
| TR-2 | trade.md:62 (5) | Object-id test: `wObjectID` that is >= 10000 but names a NULL NPC slot, or a player slot that is NULL, is NOT cross-checked (the mismatch test only runs when the slot is non-NULL), so an id for an empty slot falls back to the tile owner. Doc says any mismatch becomes a floor drop. | S Game.cpp:17145-17161 | Qualify: "mismatch with a live slot". | M |

Verified, no issue: stack split reduces source first then self-give returns 17134/17182-17186, bounds-order bug 17106-17107, dead re-entry guard 40835 vs 40845,
empty BUG POINT null check 41140-41145, SetExchangeItem never clears confirm 40919-40963, partner state/fight-zone/combat-stance refusal 40872-40873,
weight/space pre-checks 41011-41040, Kennedy -300 exp 17598-17623, `bAddItem` always TRUE (final `return FALSE` at 41321 is unreachable).

## warehouse.md

No substantive issue. 25 claims verified: zone gate 27633/27873, Howard prefix match (note: `memcmp(name,"Howard",6)`, so any NPC whose name starts with Howard
qualifies), full-bank/outside-zone falls to floor 17321/17583, `m_bIsBankModified` set-only (Client.cpp:288; TRUE at 27745, 27822, 27966, 32512; written at 6870),
dupe-closing `return TRUE` 27973-27975, item 650 forced log 27701-27707, compaction 27711-27718, client 119 cap C 42205.

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| WH-1 | warehouse.md:105-106 (10) | Open question "how the world log uses the byte" is the only unknown; nothing else open. No action unless decomp is consulted. | n/a | none | L |

## chat.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| CH-1 | chat.md:108-113 (F2), 126-141 (7), 170-172 (9.4), 15-19 (1) | HEADLINE MISREAD: failing the SP/level/HP gate does not drop the line. Every prefix case sets `cSendMode = NULL` (= mode 0) on failure, and mode 0 is the normal spoken broadcast to everyone within +-9/+-6. So an `@`, `$`, `~`, `!` or `^` line sent without enough SP/level (or dead, for `!`,`~`,`^`) is spoken publicly in the area with the prefix blanked. F2 says "local mode 1 or nothing"; 9.4 says "only the local echo is refused"; both wrong. Privacy leak: a guild or party line becomes area chat. | S Game.cpp:10593-10606, 10611-10626, 10632-10650, 10701-10717, 10727-10741 (`else cSendMode = NULL`), 11090-11115 (case NULL) | Rewrite F2/9.4; add a new gap "gate failure downgrades to public local chat". Update chat-commands.md accordingly. | H |
| CH-2 | chat.md:95, 191 (5, 10) | "Mute persisted: not verified" is resolvable: `m_iTimeLeft_ShutUp` is saved in the binary record and restored at login (and in the text format). | S Game.cpp:5866 (decode), 6731 (compose), 8084, 9149 | State "persisted [V]"; close open question. | H |
| CH-3 | chat.md:142, 181 (7, 9.9) | BWM mute is `20*3*10 = 600` ticks. With the file's own unit (1 tick = 3 s, 20 = 1 min, 36131 comment) that is 30 minutes, while the comment on the same line says 10 minutes. Doc only says "units disagree"; add the number. | S Game.cpp:40710, 36131 | Add "30 min effective vs 10 min intended". | H |
| CH-4 | chat.md:13-19 (1) | Design-intent sentences ("shouting is a resource decision and new characters cannot spam", "real enforcement is the external BWM [I]") are presented under a `[V ...]` tag. | S Game.cpp:10589-10741 | Tag intent clauses `[I]`. | M |
| CH-5 | chat.md:60 (3) | `NoticeHandler` does nothing when `m_iTotalNoticeMsg <= 1`, i.e. a single notice is never broadcast. Doc says "random notice line every 80 s". | S Game.cpp:36329-36330, Game.h:68 (80000) | Add the guard. | H |

Verified, no issue: gates 10547-10552 (113 bytes), ranges 11097-11101, SP costs (shout needs 5 but `^` charges 3 at 10703-10706), `^` relay before gate 10654-10713,
GSM_DISCONNECT first-match break 49359-49364, WHISFERMSG GM logging 49369-49394, remote ChatMsgHandlerGSM `break` on uninitialised client 11266 and type 2/10 unfiltered 11274-11279,
no failure reply for remote lookup 34797, client throttle/IME/curse C 35056-35076, Curse `sprintf(ptr, buf)` Curse.cpp:47 and strip set Curse.cpp:153, XOR 35 C 1003-1011.

## party.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| PA-1 | party.md:106-109 (F5), 120 (7) | Missing branches of `GetExp`: when the gain is < 10 or the mirror has one member the attacker alone gets full `iExp` (no unit/10 bonus, nobody else paid). If every counted member is dead (`iPartyTotalMember == 0`) the bonus switch has no case and `dV2` is read uninitialised. Both consistent with mechanics.md:45 but the latter is a new low-risk gap. | S Game.cpp:51767-51771 (else), 51698-51727 (switch) | Add to F5 and 9. | M |
| PA-2 | party.md:44-49, 79 | Gate line drift at 0b994cd: `PartyOperation` is GateCore.cpp:1110-1273 (doc 1119-1272); `PartyManager` functions span 35-316 (`GameServerDown` 250, `SetServerChangeStatus` 268, `CheckMemberActivity` 279-316; doc 252-290). | G GateCore.cpp:1110, PartyManager.cpp:35,250,268,279 | Update the numbers. | H |
| PA-3 | party.md:79-80, 115-116 | Gate registry is one flat `m_stMemberNameList[5000]` shared by all parties: total capacity is 4,999 members, not 8 x 4,999. Also add rejects silently when full; `bAddMember` first removes the same name from another party but decrements the NEW party's count (doc 9.5 mentions this). | G PartyManager.h:20,37-43; PartyManager.cpp:114-140 | Add one line to section 5. | H |
| PA-4 | party.md:138-148 (9.3) | Doc omits that `/joinparty` requires the named target to be a CONFIRM member of a party (otherwise silently returns), and that a party-full refusal (gate reply 4 with success 0) goes only to the requesting game server, not broadcast. | S Game.cpp:51376-51379; G GateCore.cpp:1209-1221 | Add as clarification. | M |

Verified, no issue: exp bonus table and rounding 51698-51727, 5-minute test on the receiver 51678, unit/10 51760-51761, bounds bug 51021, ghost-member path via `case 1` guard 51195,
friendly-fire sites 12606/31219/31770/32228, citizenship leave 27608, save/restore 9255/8571/4993-5012, op 7 stamp 2831-2849, caps Game.h:203, 20 s timeout PartyManager.cpp:279-312.
cross: combat.md:169, character.md:151 and mechanics.md:45 all agree with party.md on the split.

## guild.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| GU-1 | guild.md:166-167 (9.5) | WRONG: the REJECT branch of `ResponseCreateNewGuildHandler` resets the name to "NONE", rank -1, GUID -1; the name is NOT left in place. The only window with a stored name is before the Log answers (or if no answer comes, see GU-2). | S Game.cpp:16593-16601 | Rewrite 9.5. | H |
| GU-2 | guild.md:103-104 (F1), 44 (3) | MISSING: `ResponseCreateNewGuildHandler` only acts for a client that still has level >= 100 (>= 20 without Chinese limit) AND Charisma >= 20; otherwise no branch runs and no reply is sent, although the Log server has already created the guild file (orphan file, client waits). The 1,000,000 gold is deducted as `dwGoldCount - 1000000` (double -> DWORD) with no re-check, so a player who spent the gold meanwhile wraps. mechanics.md:191 already notes the re-check. | S Game.cpp:16563-16570, 16584-16586 | Add both to F1 and 9. | H |
| GU-3 | guild.md:181-182 (10) | `DEF_GUILDWARMODE` also gates exchange (trade.md 7), `S 933`, `23756`, `47124`; the list only names two. Minor. | S Game.cpp:933, 23756, 40839, 47124 | Extend the list. | L |
| GU-4 | guild.md:136 (7) | "guild cleared when ... its GUID differs [V S 4937-4946]": the game server only tests `cGuildStatus == 0`; the GUID comparison is on the world log (decomp). Tag the GUID half `[V doc]`. | S Game.cpp:4937 | Retag. | M |

Verified, no issue: 9.1 (no authority check in `JoinGuildApproveHandler` 19039-19066, `DismissGuildApproveHandler` 19113), Kennedy leave sends no `DELGUILDSMAN` 17598-17623,
`/ban` self allowed 36755-36779, disband local only 16772-16784/20446-20459, `GuildNotifyHandler` stub 20497-20514, `CalculateGuildEffect` early return 20256, ranks 0/12 only,
`DEF_MAXGUILDSMAN` unused (Client.h:27), GUID formula 16710-16711, create conditions 16669-16679, join conditions 17415-17418.

## crusade.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| CU-1 | crusade.md:121-122 (F1), 187 (8) | Coupling missing on both sides: `LocalStartCrusadeMode` zeroes `m_iCrusadeWinnerSide`, which switches off the shop discount for the whole war and (via side 0) hands it to neutrals (shop SH-1). `m_iWinnerSide` is the separate field that keeps the loser head start. | S Game.cpp:47918, 47968, 49053 | Add to couplings; state both fields. | H |
| CU-2 | crusade.md:139-142 (F4) | `RequestSummonWarUnitHandler` accepts `cNum <= 10` then overwrites `cNum = 1`; multi-unit summons are impossible. Doc reads as if counts work. | S Game.cpp:48717, 48727 | Add one line to F4/9. | H |
| CU-3 | crusade.md:156-157 (F6), 168 | Cap order: the 300,000 clamp happens before the level bonus is added, so a payout can exceed 300,000 x 1.2. Draw has no level bonus. Doc implies a hard cap. | S Game.cpp:47846, 47855-47868 | State the order. | M |
| CU-4 | crusade.md:189-190 (8) | cross: mechanics.md:53 also lists a per-hit /3 in crusade (iCalculateAttackEffect); crusade.md mentions only the /4 kill cut. Not a contradiction, but link both. | mechanics.md:53, 230 | Add "see mechanics.md 13.1 and per-hit /3". | L |

Verified, no issue: day-guard defect 50282-50285, no role check in 48453-48935 (grep for duty/rank/crusade-mode returns nothing), `SelectCrusadeDutyHandler` 49042-49066 (head start via `m_iWinnerSide`),
payout /6, x1.2, /5, log /10 at 47849-47893, `__bSetOccupyFlag` early return 39508, `SaveOccupyFlagData` early return 47996, GMG pool > 15 surplus lost 48202-48212,
mana-stone selection overflow 48040-48045, "CGT-Aresen" 47670, victory logic 49944-49962, PK reward x5 / level/2 28128-28135, friendly NPC kill zeroes contribution 14640, engineer 500-700 12744-12752.

## arena.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| AR-1 | arena.md:17-20 (1) | Purpose asserts "Deaths in fight zones have softer consequences (special drop/penalty branches)" with cites 3928, 2151-2163, which show only no periodic autosave and the recall timer. No death-consequence evidence; section 10 itself says unknown. | S Game.cpp:3927-3933, 2151-2162 | Drop or tag `[U]`. | M |
| AR-2 | arena.md:81-86 (F3) | Ticket is consumed only on a valid teleport; if the holder is already in the target map nothing is consumed (and expired tickets are never consumed, 9.5). Doc F3 says "deplete ticket, teleport ... unless already there" ambiguously. | S Game.cpp:30869-30876 | Clarify order. | L |
| AR-3 | arena.md:128-130 (9.6) | Correct, but note the sign flip only matters if tickets are still exhausted; `m_iFightzoneNumber != 0` keeps `-4` refusal, so a negated booking still blocks rebooking until expiry/relog. | S Game.cpp:27466, 40156-40160 | Optional note. | L |

Verified, no issue: price 1500 and checks order 27454-27466, parity `(day+side+N)%2` 27448, zone range 1..10 27438, slot end calc 27476-27499, 50 tickets, expiry stamp tests 30864-30866 (`|` typo),
recall timer 2151-2162, no autosave 3927-3933, stale booking notify uses `i` 6333, 2-hour clear on uptime 3786/37440-37460, dead-time set 19264-19270, resurrection commented out 4078-4089.

## quest.md

| id | doc file:line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| QU-1 | quest.md:98-101 (F4) | "every branch adds contribution": the item-reward branch that cannot carry the item adds none and keeps the quest (doc says quest kept, but the "every branch" wording is wrong for that case). | S Game.cpp:41396-41405 | Reword. | M |
| QU-2 | quest.md:14-19 (1) | "ties PvE to citizenship", "level- and reputation-banded task board" are intent statements under `[V]`. | S Game.cpp:41927-41944 | Tag `[I]`. | L |

Verified, no issue: eligibility 41461-41464/41927-41944, crusade assign-type gating 41940-41942, reward slot 1d3 41958, accept path no NULL check 41984 and war teleport type 10 41986-41991,
reward-name only for -10 (39199 area) so -1/-2 show blank, only MONSTERHUNT/GOPLACE complete 42099-42122, `m_iTimeLimit` only parsed 41696, kill credit killer only 14545-14555, retired quests 35-40 42052-42058,
stub `_iTalkToNpcResult_*` never called, dialogue code 130+iWho 39226.

## Cross-doc summary
- No broken links. Couplings agree on both sides for party/combat exp, guild/crusade commander, chat/party mode 4, trade/warehouse (Howard), trade/guild (88/89, Kennedy).
- New cross-doc issues: shop SH-1 / crusade CU-1 (winner = 0 matches neutrals and lapses during war); chat CH-1 affects party.md (`$`) and guild.md (`@`, `^`) because failed gates become public speech, and commands\chat-commands.md should be updated too.
- Gaps not covered by any doc in the group: none important beyond those listed.
