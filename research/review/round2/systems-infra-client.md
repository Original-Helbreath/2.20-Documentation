# Review: systems docs, group infra-client (round 2)

Docs: network.md, session.md, persistence.md, server-topology.md, admin.md, rendering.md, ui.md, audio.md,
client-state.md (all under `research\systems\`).
Method: read each doc in full; checked file:line of every function-table row with a script
(`.claude\scratch\review-sys\infra-client\chk.py`: function definition start within 3 lines of the cited
range - all rows passed except the two cite slips listed below); read the cited code for every headline
gap; cross-link script (`links.py`) resolved every `research/...`, `../...` and bare sibling link
(bare names such as `chat-gm.md`, `blitters.md`, `networking.md` resolve to
`game-server/functions/`, `client/rendering/`, `client/functions/` as the docs intend). Gate lines checked
with `git show 0b994cd:GateCore.cpp` (read-only). SRV = `sources/hgserver-librehelbreath/src`
(identical to the cited `srv220` export except the two casts the docs name). WLS/MLS facts were checked
against `research/servers/decomp/` and `official-*.md`, not against a binary.

Confidence: H = verified in source/decomp text, M = verified code, effect inferred, L = observation to confirm.

## Verdicts on the specifically requested claims

| Claim | Doc | Result |
|---|---|---|
| Gate build date 612 vs game server 429 | server-topology.md:224, 192 | VERIFIED. `sources/gateserver-librehelbreath/GateServer.cfg:5` `build-date = 612`; `GateCore.cpp` (0b994cd) 497-506 rejects when `m_iBuildDate != dwBuildDate`; SRV `Version.h:19-22` gives 429 under `DEF_CHINESE`, which `GlobalDef.h:16` defines. SRV reaction to REJECT is only "STOPPED!" and `m_bIsGateSockAvailable` is never set (`Game.cpp:14947-14958`), so `bAccept` (`399-416`) refuses everyone. Wording "shipped game server" means the source default; no `Version.h` branch yields 612. |
| Binary save vs text WLS | persistence.md:189 | CONCLUSION VERIFIED, REASONING INCOMPLETE (PE-1). `DEF_CHINESE` -> `DEF_DBGAMESERVER` (`GlobalDef.h:72-73`); save header carries a 4-byte char ID at offset 36 and the count flag moves to 40 (`Game.cpp:4492-4503`), `official-compat-220.md:93-94`. |
| Config the WLS never sends | server-topology.md:226-228 | VERIFIED against docs/decomp: `official-worldserver.md:132-134` lists 0x0FA40005..08 as never sent, and `decomp/WorldServer/*.c` contains no 0x0FA40005..08 case. Game side waits on `m_bIsWLServerAvailable` under `DEF_DEFENCEHACKING` (`Game.cpp:3699-3701`, accept check `411-413`). The registration ID mismatch is also verified: `NetMessages.h:262` = 0x0512A3F4; `decomp/WorldServer/ProcessMsgQueue_WLS.c:17` dispatches 0x0512A3E3. |
| Unauthenticated mute (BWM) | admin.md:218 | VERIFIED. `MsgProcess` client branch (`Game.cpp:14964`) has no init/login guard; `MSGID_BWM_COMMAND_SHUTUP` (`14986-14988`) calls `_BWM_Command_Shutup(pData)` (`40697-40720`), which reads the name at offset 16, sets `m_iTimeLeft_ShutUp = 20*3*10` (600 ticks x 3 s = 30 min) and never checks `m_bIsBWMonitor` (written only at `40688-40695`). Extra: it can mute a GM (no admin-level test) and skips the length check. Only precondition: the socket must pass `bAccept`. |
| "No session ticket" | session.md:35, 250 | TRUE BUT UNDERSTATED, see SE-1. |

## network.md

Claims checked: about 30 (XSocket send/receive lines and cipher `XSocket.cpp:414-446, 539-561`; client key
`CLI Game.cpp:1445`; queue ring `15275-15301`; `OnClientSocketEvent` 467-512 incl. stamp at 485 and
delete at 509; unknown-message default 15083-15086; QUENEFULL fatal 4974-4983; timeout 3850-3866;
keep-alive 4272-4277; payload offset 6 `CLI 1855-1866`; `ThreadProc` assignment `Wmain.cpp:61`;
buffer/ring constants `Client.h:22-24`, `Game.h:51`; MsgProcess source-first switch; envelope counts
vs `commands/by-system.md` and `method.md:38`; links). Two issues.

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| NE-1 | 194-195 (also session.md 101, ui.md 148, client-state.md 172, 208-211) | The "every 2 s, 7 strikes, 14 s" lag rule is stated as fact. The 2 s gate compares against `m_dwCheckChatTime`, which is set to `m_dwTime`, not the current time. `m_dwTime` is the stamp of the last received packet (set on receive `CLI Game.cpp:1297`, `14746`, and `ChangeGameMode` `15515`). While the server is silent (the only case where >= 6 commands stay outstanding) `dwTime - m_dwCheckChatTime` stays > 2000, so the block runs on every 1 s `OnTimer` tick and the "connection lost" mode triggers after about 7 s, not 14 s (the source comment says 14). With steady traffic the cadence is about 2 s. client-state.md:210 flags the line but leaves the effect [U]. | `CLI Game.cpp:4288-4299`, `1297`, `14746`, `15515`; timer period `Wmain.cpp:857` (1000 ms) | State: "check runs at most every 2 s of received traffic, every 1 s while the server is silent; kick after about 7 s of silence with >= 6 outstanding commands [V code, I effect]". Update the four docs together. | M |
| NE-2 | 62-63, 237-238 | "one copy of socket code compiled into every program" contradicts the same bullet ("prebuilt lib" in the server, "near-identical copies" in client and gate). The "unknown message saves" bullet is only true for initialised clients; uninitialised ones get NOSAVELOGOUT. | `Game.cpp:2775-2802`, `15083-15086` | Reword the intent as "same source lineage, three builds" and add "after init" to the save claim. | H |
| NE-3 | 62-74 (intent bullets) | Subsystem intents (2.2-2.6) are untagged conjecture while the header says [V]/[I]/[U]. | n/a | Tag each as [I]. | M |

Links, cross-doc couplings: all resolve; `server-topology.md` ownership of socket creation and `admin.md`
ownership of the frequency checks agree with the other docs.

## session.md

Claims checked: about 38 (all function-table rows by script; F1-F5 steps against `Game.cpp:1501-1609`,
`4855-5043`, `23344-23374`, `36354-36372`, `18644-18667`, `2571-2887`, `37973-37988`, `38098-38117`,
`2700-2720`, `2825-2850`, `5027-5040`; client countdown `CLI 33378-33392`, `35318-35345`, `43517-43533`;
decomp `HandleRequestPlayerData.c`, `HandleEnterGameConfirm.c`, `RequestEnterGame.c`;
`DEF_MAXONESERVERUSERS` 800 `Game.h:155`; duplicate-login branches; "no init guard" claim by grep over
`1611-2250`). No wrong line cites.

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| SE-1 | 34-37, 250-255 | "No session ticket" is correct (nothing unforgeable), but the section understates both sides. (a) The MLS ENTERGAME handler stores account + password + level in the WLS as state INIT (`SETACCOUNTINITSTATUS`, `decomp/MainServer/RequestEnterGame.c`); at ENTERGAMECONFIRM the WLS compares password and level for INIT/WAIT accounts and on mismatch orders a no-save disconnect (`decomp/WorldServer/HandleEnterGameConfirm.c`, SRV `RequestCheckAccountPasswordHandler` `Game.cpp:46052-46083`). That is a one-shot plaintext credential check after the client is already in the world, not a ticket. (b) With the account in state NULL (not logged in anywhere) there is no check at all: `HandleRequestPlayerData` ignores the password (`decomp/WorldServer/HandleRequestPlayerData.c`, password at +26 unused) and ENTERGAMECONFIRM for a NULL account sets PLAYING. So knowing an account name and one of its character names suffices. The text "unless the WLS state table blocks it" implies a block that exists only for INIT/WAIT/PLAYING accounts. (c) The ENTERGAME request carries a 120-byte command-line/billing token (`CLI Game.cpp:1799`, "!!! 과금처리용 스트링열", ENTERGAME offset 80) that the MLS reads and discards; worth one line so nobody mistakes it for a ticket. | files in column | Rewrite §9 bullet 1 with (a)-(c); mark the exploit [V decomp, I exploitability]. | H |
| SE-2 | 101, 221, 187 | Lag cadence (see NE-1). Logout countdown is called 10 s in §1 but `m_cLogOutCount = 11` ticks (§7 says 10 s, F3 says 11 ticks): 11 s. | `CLI Game.cpp:33386`, `35322-35330` | Say "11 one-second ticks". | M |
| SE-3 | 34-36 | "the game server is stateless about accounts" is not what the code does: it keeps `m_cAccountName/Password` and resolves duplicate logins by comparing passwords locally (F4) and by `RequestCheckAccountPasswordHandler`. | `Game.cpp:1556-1598`, `46052-46083` | Mark the sentence [I] and rephrase "authority for who may enter is the Log side". | M |
| SE-4 | 194-196, 289-292 | F4/§11 record the different count flags (first loop FALSE, second loop default TRUE for the password-mismatch drop of the NEW client) but not the consequence: a dropped new client goes through `DeleteClient` else-branch and sends `MSGID_REQUEST_NOSAVELOGOUT` carrying `bCountLogout` and the claimed account/character (`Game.cpp:1573, 1595`, `2828`). If the WLS clears account state on a count=TRUE NOSAVELOGOUT, a stranger who names a victim's character could reset the victim's PLAYING state. Not verified (WLS handler not decompiled). | `Game.cpp:1573, 1595, 2828`; `persistence.md:100-101` | Add as open question [U] and ask for the WLS `NOSAVELOGOUT` handler to be read. | L |
| SE-5 | 256-258 | The no-init-guard finding is right; also say that `RequestInitPlayerHandler` itself has a guard (`m_bIsInitComplete == TRUE` returns, `Game.cpp:1507`) but can be repeated before init completes, re-sending `REQUEST_PLAYERDATA` each time. | `Game.cpp:1507-1609` | Add one sentence. | H |
| SE-6 | 144-168 | Gap: the client has a gateway pre-step (`m_bIsCheckingGateway`, `MSGID_GETMINIMUMLOADGATEWAY`, `CLI Game.cpp:3041-3045`; only `DEF_USING_GATEWAY` builds, i.e. Taiwan, `GlobalDef.h:160-166`). The English build skips it, so the omission is defensible, but a one-line scope note is missing. | `CLI GlobalDef.h:1, 160-166` | Add scope note. | M |

## persistence.md

Claims checked: about 33 (codec ranges and `#ifdef` split `5549/6943/9477`; save header and flag layout
`4469-4522`; autosave `3927-3933` and `DEF_AUTOSAVETIME` `Game.h:60`; `m_dwAutoSaveTime` at `427`;
`LocalSavePlayerData` `38736-38794` incl. `"wt"`; `SaveOccupyFlagData` `return;` `47997`, timer `3756-3768`;
`m_bIsBankModified` setters 27745/27822/27966/32512 and encoder 6870, `Client.cpp:288`;
count-flag sites 1564/1586/23369; `ResponseSavePlayerDataReplyHandler` 36354-36372; admin-level clamp;
penalty-date check 6317-6325; only caller of `MSGID_REQUEST_SAVEPLAYERDATA` is 3931; dup-item calls).

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| PE-1 | 189-193 | Right conclusion, thin reason. "the WLS stores and returns the text file and has no binary parser" would not by itself break an opaque store. What breaks it: in DB mode the save message inserts a 4-byte char ID at offset 36 and moves the count flag and body to 40/41 (`Game.cpp:4492-4503`, size `41 + iSize`), the load reply layout differs, and the WLS opens files in text mode (`rt`). `official-compat-220.md:93-94` already says "DB mode incompatible (char ID at 36)". | `Game.cpp:4492-4503`; `decomp/WorldServer/HandleRequestPlayerData.c`; `official-compat-220.md:93-94` | Replace "has no binary parser" with the offset shift and text-mode I/O. | H |
| PE-2 | 161-162, 177-178 (and admin.md 94, 132) | Load-time item cleanup is missing. The decoders delete items of type `DEF_ITEMTYPE_NOTUSED` and any item whose id is in the DupItemID list, then credit the sum of their prices to `m_iRewardGold` (`+= iNotUsedItemPrice`). It also resets a 0-life Gem of Sacrifice to 1 and sets `m_iSpeedHackCheckExp = m_iExp`. Docs say duplicates are "logged". | `Game.cpp:6078-6101` (inventory), `6226-6241` (bank), `6410-6422`; FILE decoder `7300-7312`, `7856`, `8907`; reward gold sent in `1729` | Add to §7 and §6 F1: "NOTUSED and DupItemID items are deleted at load and refunded as reward gold". Admin 2.5 / 5 should say "deleted and refunded", not only logged. | H |
| PE-3 | 178 | Cite slip: `_bCheckDupItemID` calls are at `6097` and `6237`; `_bItemLog(DUPITEMID)` at 6099/6239. | `Game.cpp:6097-6099, 6237-6239` | Fix numbers. | H |
| PE-4 | 32-34, 51-62 | Intent text carries [V doc]/[V] tags ("Design intent: the game server owns the meaning ...", 2.2 "a later binary variant for builds backed by a database") although these are intents. The only evidence for the DB purpose is the `GlobalDef.h:268` change note ("faster WORLD server save by sending CharID"), which reads as a speed-up of the world-server store, not of a separate DB. | `GlobalDef.h:268`, `Game.cpp:4499` | Tag as [I]; quote the `GlobalDef.h:268` note. | M |
| PE-5 | 204-206 | "Account creation is broken in the MLS binary ... `Hack =`": doc-only, consistent with `official-compat-220.md:56`; not re-verified against the binary. | n/a | Tag [V doc], already so. No change. | H |

## server-topology.md

Claims checked: about 38 (all rows of the function table by script; `bInit` order 791-810, 819 comment,
`OnTimer` ready block 3693-3708, sub-socket cadence 3729-3738, drain 3791-3808, final countdown,
shutdown handlers 14852-14930, `bAccept` 399-416, ALIVE suppression 20714-20726, `_CheckGateSockConnection`
43099-43121, `_bCheckSubLogSocketIndex` 43123-43154, `_Manager_Shutdown` dead 40799, `DEF_DEFENCEHACKING`
sites, gate timeline 21 s x 9 steps = 189 s from `CheckAutoShutdownProcess` (count 10, `GateCore.cpp:1033-1106`),
30 s silence limit, `Wmain.cpp:254` timer).

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| ST-1 | 224-228 (shipped pieces list) | A fourth mismatch is missing and it fires before the build date does: the shipped `GateServer.cfg` has no `game-server-list`, and the gate admits peers only by exact match against that list (empty list rejects everyone). So the gate drops the game server's connection at `bAccept` before registration. | `sources/gateserver-librehelbreath/GateServer.cfg` (3 keys only); `GateCore.cpp` 0b994cd 143-146 (`game-server-list` -> mode 3), `231-283` (bAccept compare, reject at ~262) | Add bullet; note the sample `bin/config/gserver.cfg` points the gate at `211.51.62.243:22` while the gate cfg says `203.234.215.188:6502` (deployment values, so low weight). | H |
| ST-2 | 178-179, 195 | Retry-counter semantics are described as "if not re-registered within 20 ticks". The counter is reset to 0 on TCP connect (`OnGateSocketEvent` CONNECTIONESTABLISH, `Game.cpp:20556-20562`), not on registration CONFIRM, so a gate that accepts the socket and then rejects registration never triggers code 4. It is also initialised to 1 in `bInit` (`Game.cpp:812`), so a gate that is down at first start produces code 4 after about 60 s and a `bInit()` restart loop. §9 last bullet ("bConnect retries forever") should say that. | `Game.cpp:812, 20556-20562, 43099-43121` | Reword F4 and the last §9 bullet. | H |
| ST-3 | 73-74, 62-75 | Intent block mixes tags: 2.4 is tagged [V]; 2.2/2.3/2.5 [V doc]; 2.6-2.8 untagged. 2.9 "no remote admin protocol is needed" is speculation and contradicts `admin.md` (manager login, BWM monitor, gate monitor set, `MSGID_REQUEST_*_SHUTDOWN`). | `Game.cpp:14986-14997`, `admin.md:113-115` | Tag all intents [I]; rewrite 2.9 as "a single operator can run a world from keyboards; manager/monitor protocols exist but their tools are absent". | H |
| ST-4 | 229-231 | `DEF_DEFENCEHACKING` site list is incomplete: seven `#ifdef` sites, `15180` (Log CONFIRM check) is missing from the list of six. | `grep DEF_DEFENCEHACKING Game.cpp`: 411, 3699, 15180, 15236, 18655, 40760, 49005 | Add 15180. | H |
| ST-5 | 196 | "final countdown 20 s": counter starts at 20, decremented once per second, `WM_DESTROY` at <= 1, so about 19 s. | `Game.cpp:3715-3724, 3804` | Say "about 19 s". | M |
| ST-6 | 171-175 | Gate tail edge not mentioned: after step "1" the count reaches 0 and the `default` branch re-sends notice 1 every 21 s while any server still has `m_bIsShutDown`. Gap, low value. | `GateCore.cpp:1055-1066` | Optional note. | L |

## admin.md

Claims checked: about 40 (levels and passwords 10932/10937, 11011-11027, 10752-10756, 46323-46352, 46382-46385;
speed thresholds 1149-1180, 11304-11317 with arithmetic 1032/908/1480; gap checks 52615-52713 and callers
1047/1058/1082/1101; exp-hour block 4010-4060; clock check 48937-48976; BWM/manager 14986-14997,
40688-40745, 48978-48995; `m_bIsManager` count 2877-2879; GM immunity 12599; DEF_CLIENTTIMEOUT
`Game.h:59`; level table vs `commands/chat-commands.md` rows 26-70, all consistent).

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| AD-1 | 225-228 | Misleading: "run after the action and their return value is ignored" reads as if the gap checks do nothing. They all return FALSE regardless, but each calls `DeleteClient(i, TRUE, TRUE)` itself on a violation (gap < 250/500/1500 ms), so they do act; the weakness is only the client-supplied timestamp. The 250 ms move check also has two exemptions (`m_bIsMoveBlocked`, `m_bIsAttackModeChange`). | `Game.cpp:52615-52713` (deletes at ~52632, ~52662, ~52702) | Reword: "kick inside the function; return value unused; trusts client clock; two reset exemptions". | H |
| AD-2 | 94, 132 | Dup-item handling understated: `_bCheckDupItemID` hits are deleted from the record at load and refunded as reward gold, not just logged (see PE-2). | `Game.cpp:6097-6101, 6237-6241, 6419` | Same fix as PE-2. | H |
| AD-3 | 34-36, 52-61 | Design intent stated as [V] with code cites ("trust is per character and per session ... [V `5731, 10937`]"); the code only shows the mechanism. 2.1-2.6 intents are untagged. | n/a | Tag [I]. | M |
| AD-4 | 156-159 | Wording: exp window is a tumbling window (reset only when the hourly check passes, `Game.cpp:4057-4062`), consistent with the doc; but `mechanics.md:48` says "rolling hour" and "blocks the account". The penalty fields live in the character record (`m_iPenaltyBlock*` in `CClient`, saved with the character), so "account" is wrong. Contradiction with a reviewed doc. | `Game.cpp:4027-4062`, `6317-6325`; `research/game-server/mechanics.md:48` | Fix `mechanics.md` ("character", "fixed window that restarts after each passing check"). | M |
| AD-5 | 92, 230-231 | Clock check described as "client clock ahead of server by >= 10 s". The code compares elapsed time since the first CHECKCONNECTION on both clocks, i.e. the client clock running fast by >= `DEF_CLIENTTIMEOUT` (10000 ms) cumulatively; fine, but "ahead" suggests an absolute offset. | `Game.cpp:48946-48962` | Say "client elapsed time exceeds server elapsed time by >= 10 s". | M |
| AD-6 | 218-220 | Verified, one addition: the same dispatch has no admin-level check, so a GM can be muted, and the manager/BWM messages need only a socket that passed `bAccept` (server fully ready). | `Game.cpp:14986-14991, 40697-40720` | Add one sentence. | H |

## rendering.md

Claims checked: about 26 (function rows by script; `DXC_ddraw.cpp:34-38`, `119-124` (672x512 cache = 640+32 x 480+32),
`330-344` multiple-of-4 sysmem surfaces, `425-450` pixel-format test; `Game.h:69-71, 637-642`;
`MapData.cpp:25-41` animation defaults 60/70/42/78/88/150/70, `1925-1934` skip <= 3; FPS massage
`Game.cpp:35384-35395`; invisible rule `13769-13774`; detail key `28638-28642`; eviction `15618-15660`;
`Sprite.cpp:873` and `3224` quirks; `Effect5.pak` loop `3872-3876`; HUD ordering cites). [Vd] tags are
honest; this doc is a summary of linked docs.

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| RE-1 | 129 | "rows view-7..view+11" is not what the loop does and "view" is undefined. The loop starts 7 rows above the top row (`indexY = sDivY + sPivotY - 7`) and runs `iy` from `-sModY-224` to 779 in steps of 32, about 32 rows: roughly 9-10 rows below the 15-row screen. Columns -4..+4 beyond the 20-column screen are right. | `Game.cpp:2299-2303` | State rows as "7 above to about 10 below the screen" or define "view". | M |
| RE-2 | 128 | The 8 s sprite/sound sweep only runs while the window is active (`if (m_bIsProgramActive) ReleaseUnusedSprites()`), so eviction pauses when minimised. | `CLI Game.cpp:4270-4275` | Add. | H |
| RE-3 | 24 | "isometric-looking (actually top-down 32x32 tile)" is an interpretive claim; tag [I]. | n/a | Tag. | L |

## ui.md

Claims checked: about 30 (`m_cDialogBoxOrder` slots 40/39 and comment swap `945-952`; EnableDialogBox loop
bounds 0..38 `18584-18596`; DrawDialogBoxs observer return and 0..40 loop `18016-18030`; clamp `18570-18580`;
right-click exclusion list `17676-17680` incl. 34 and 24; throttle 5/6 `36852-36854`; polymorph
`36856-36865`; unwalkable stop `36925-36928`; run needs SP `36934-36939`; chat 700 ms `35059`;
`VK_HOME` `29100-29103`; observer panning `35713-35733`; hot keys F1-F12 under `DEF_LANGUAGE 4` and
`DEF_SHORTCUT`; `DEF_DOUBLECLICKTIME` 300 `Game.h:139`; dialog arrays `Game.h:566, 709, 865`;
231 `PlaySound('E',14,5)` sites; dialog ids 1..38).

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| UI-1 | 175-176 | "EnableDialogBox compacts the order array with a single pass, so gaps can remain" is wrong. Exactly one entry is nulled per call and the later entries each shift left because the slot before them has just become null; the array stays contiguous. `DisableDialogBox` uses the identical loop. | `Game.cpp:18584-18590`, `18717-18723` (worked example: `[A,B,C,D]` remove B gives `[A,C,D]`) | Delete the bullet or restate "single pass is sufficient for one removal". | H |
| UI-2 | 172-173 | Cite slip: `dwSelectClickTime` is overwritten at `35792`, not `35789` (35789 is `sClickY`). | `CLI Game.cpp:35782, 35788-35792` | Fix number. | H |
| UI-3 | 148 | Lag kick "2-s check ... comment 14 seconds": see NE-1 (effective 1 s cadence under silence). | `CLI Game.cpp:4288-4299` | Same fix. | M |
| UI-4 | 137-141 | Hot keys section says "English build" but also lists `Home`, `Ctrl+H` etc.; fine. `F1 help (35)` is true only without `DEF_SHORTCUT` (Taiwan/China use `UseShortCut(0)`), which §9 covers. No change; recorded as checked. | `Game.cpp:28784-29110`, `GlobalDef.h:160-190` | none | H |

## audio.md

Claims checked: about 24 (`PlaySound` formula and cut-off `22783-22817`; `Play` pan clamp `177-201`;
voice stealing `GetIdleBuffer` `204-270`; `YWSound::Create` `26-74`; sample counts 24/98/47 and
`DEF_MAXSOUNDEFFECTS` 110; `StartBGM` file choice, `DEF_XMAS` off `GlobalDef.h:29`; volume defaults
`956-957`; slider range `41921-41928`; explosion pan in pixels `5995-6010` vs tile pan
`MapData.cpp:1744-1747`; footstep frames and type list `2060-2068`; `bAddNewEffect` inactive return
`5910`; rain loop `23258-23285`; eviction 30 s `15651-15657`). The numeric rules in §7 are correct,
including "slider 0 = mute" and "at slider 100 a sound 10 tiles away is exactly at the cut-off".

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| AU-1 | 43, 83-93 | The rain loop ignores the effects slider: `m_pESound[38]->Play(TRUE)` uses the defaults `lPan = 0, iVol = 0` (full volume), unlike `PlaySound`. The doc lists it only as "rain loop as ambient bed". | `CLI Game.cpp:23265`; `SoundBuffer.h:24` | Add to §9. | H |
| AU-2 | 44, 123 | The music slider handler restarts the BGM with `Play(FALSE, 0, iVol)` (loop flag FALSE) after `bStop(TRUE)`, so after any slider drag the track plays once and then stops until a track change; `StartBGM` loops with `Play(TRUE,...)`. Dragging also restarts the track each frame. Not in the doc. | `CLI Game.cpp:41931-41937` vs `35493` | Add to §9 as [V code, I effect]. | M |
| AU-3 | 35-36, 91 | Sound eviction shares the sweep gated on `m_bIsProgramActive` (see RE-2). | `CLI Game.cpp:4270-4275` | Add. | H |
| AU-4 | 49-52 | `StartBGM` on the first game frame is conditional on `m_bMusicStat` (`34827`), not unconditional. | `CLI Game.cpp:34827` | Add "if music is on". | H |
| AU-5 | 113-115 | The `YWSound::Create` effect statement is flagged [I] correctly; additionally the primary buffer handle `lpDsb` is never released (leak, harmless). | `YWSound.cpp:56-66` | optional | L |

## client-state.md

Claims checked: about 28 (function rows by script; `OnTimer` 4266-4301; `MotionResponseHandler` reject/confirm
`35520-35612` (`m_cCommandCount = 0`, snap, redraw) and `++` at `2237`; `MotionEventHandler` id split 10000/30000
`46098-46186`; own-appearance matching by name `46239-46271` incl. the running exemption; `LogEventHandler`
14688-14704; `bSetOwner` cache-miss refresh 1215-1225; `MAPDATASIZEX/Y` 40x35, `m_tile[752][752]`
`MapData.h:26-27, 65`; bank 121 vs server 120 `Game.h:81` / `Client.h:26`; `OnTimer` 1000 ms
`Wmain.cpp:857`; `InitDataResponseHandler` flow; `ConnectionEstablishHandler` 3036-3038).

| id | doc line | issue | evidence | proposed fix | conf |
|---|---|---|---|---|---|
| CS-1 | 210-211 | The `m_dwCheckChatTime = m_dwTime` item is left [U effect]. Resolved: `m_dwTime` is the last-receive stamp, so the 2 s cadence degrades to 1 s while the server is silent (NE-1). Also contradicts §7 line 172-173 ("7 consecutive 2-s checks"). | `CLI Game.cpp:1297, 4289, 14746, 15515` | Update both places. | M |
| CS-2 | 5-6, 212-213 | "`CInt` ... probably a librehelbreath addition, not 2.20 [I]": `Cint.h`/`Cint.lib` are in `sources/client-2.20/`; the guess cannot be settled from this tree. Keep [I]; fine. | `sources/client-2.20/Cint.h` | none | L |
| CS-3 | 108 | Bank size: client array is 121 (`DEF_MAXBANKITEMS 121 // 120+1`), server 120; the doc says 121 and persistence.md says 120 without cross-reference. | `CLI Game.h:81`, `SRV Client.h:26` | Add "121 = 120 + 1 slot" note, link persistence.md §5. | H |
| CS-4 | 18-34 (intent) | "design favours bandwidth and responsiveness" is an intent tagged [V]; should be [I] (the mechanism is verified, the motive is not). | n/a | Tag [I]. | M |

## Cross-doc and consistency findings

| id | where | issue | proposed fix |
|---|---|---|---|
| X-1 | network 194 / session 101 / ui 148 / client-state 172 | same lag-cadence claim, one root cause (NE-1) | fix together |
| X-2 | persistence 161-162 / admin 94, 132 | dup/NOTUSED item deletion with refund missing (PE-2) | fix together |
| X-3 | `mechanics.md:48` vs admin.md F4 | "account"/"rolling hour" vs character record/tumbling window (AD-4) | fix mechanics.md |
| X-4 | server-topology 73-74 vs admin 113-115 | "no remote admin protocol" vs documented manager/BWM/monitor protocols (ST-3) | fix topology |
| X-5 | session 250-255 vs persistence 207-208 | persistence says "password in REQUEST_PLAYERDATA is decorative"; session says the WLS state table may block. Consistent only after SE-1(a): the password IS checked, but later (ENTERGAMECONFIRM, INIT/WAIT only). | align both wording |

No broken links. No wrong line citation beyond PE-3 and UI-2. Coverage: the script checked the
function-table rows (every row naming a function with a Game/MapData/XSocket/Sprite/SoundBuffer/Wmain
cite; start line within 3 of the cite); the other cites listed per doc were read by hand. Line numbers
quoted in this report for AD-1, AD-4, AU-2 and ST-1 are approximate (marked with ~ or ranges). Not
verified at all: WLS/MLS binary behaviour beyond the decomp pseudo-code and `official-*.md`, the
`WLS NOSAVELOGOUT` handler (SE-4), and the [Vd] items in rendering/ui/audio/client-state that defer to
`research/client/`.
