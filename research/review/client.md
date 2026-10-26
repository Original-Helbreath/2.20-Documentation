# Review: client docs (v2.20) against sources\client-2.20

Reviewer: read-only verification. Method: scripts parsed all 593 table rows and re-derived location, callers/callees,
Msgs, Dlg and Files from the source (scratch: .claude\scratch\srv220\rev\). Line numbers are 1-based, split on LF.
PAKLib layout fetched from https://github.com/shadowevil/PAKLib (2026-10-25): the blitters.md mapping matches it.

## Mechanical results (verified by script)

- Where/Lines: 593 of 593 rows start on a line containing the function name, end on a col-0 `}`, and Lines = end-start+1. No source function is missing (the only unlisted col-0 definition is a commented-out DrawDialogBox_Bank at Game.cpp:37103-37231, inside a block comment; two `catch` blocks are not functions).
- Callers/Callees: 593 of 593 reproduce exactly as "name-matched, comments stripped, truncated to 10 + N". Merges: bInit (CGame, DXC_ddraw, DXC_dinput), cGetNextMoveDir (CGame, CMisc), GetRegKey, GoHomepage, WebCtrlProc (free and CWebCtrl). Not flagged per row.
- Msgs: 593 of 593 reproduce. Dlg: 593 of 593 reproduce (literal EnableDialogBox/DisableDialogBox ids). Files: 591 of 593 (WinMain misses search.dll/Riched20.dll strings, ThreadProc misses cjconnector.dll).
- Caveat: Msgs/Dlg/Files ignore `#if`, so inactive-language branches are listed as if live (see table).

## Issues

| # | Doc file | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|---|
| 1 | build-config.md:49 (also task key finding) | DEF_GIZON | Claims DEF_GIZON "off in English so dialog 34 never opens". | Game.cpp:18507-18538: DEF_GIZON only picks the opening branch. Without it (English) dialog 34 opens in cMode 6 if the inventory holds sprite 6 frame 128/129 items, else it shows DRAW_DIALOGBOX_ITEMUPGRADE30 and returns. DEF_GIZON is also a macro used only at Game.cpp:18508. Whether any path calls EnableDialogBox(34) in English was not traced. | wrong |
| 2 | assets.md:34-36, 73-77 | Missing sprite | Says drawing a missing sheet "calls a method on NULL and crashes" (7 of 1283 sites check NULL; counts are right, conclusion wrong). | Every CSprite blit starts `if (this == NULL) return;` (e.g. Sprite.cpp:104, 710, 880); non-virtual call, UB but works on VC6. See answer below. Contradicts blitters.md:109. | wrong |
| 3 | functions/networking.md:14 | Keep-alive | "7 missed replies disconnects (`_CalcSocketClosed`)". | Game.cpp:4272-4276 sends CHECKCONNECTION every 8 s when mode != ONLOADING and socket available. Disconnect is separate: every 2 s, if m_cCommandCount >= 6 then ++m_iNetLagCount, >=7 -> ONCONNECTIONLOST (4288-4301); counter counts unanswered motion commands, not keep-alive replies. `_CalcSocketClosed` (34749) has no caller found and handles the billing socket closing -> ONQUIT (comment: billing socket). | wrong |
| 4 | functions/startup.md:10 | Initialize | "1000 ms SetTimer drives OnTimer". | Wmain.cpp:857/407-414: `timeSetEvent` periodic multimedia timer, `_TimerFunc` posts WM_USER_TIMERSIGNAL, WndProc:101 calls OnTimer. frame-pipeline.md:36 states it correctly; the two docs disagree. | wrong |
| 5 | functions/startup.md:9 | WinMain | Order implied wrong; mutex unconditional. | Wmain.cpp:275-294: class/window created and `Initialize` (bInit) run BEFORE the mutex; mutex only `#ifndef _DEBUG`. A failed search.dll check returns before any window. | imprecise |
| 6 | data-structures.md:100 | Dialog 39 | "39 Bank auto-store". | No case 39 in DrawDialogBoxs (18028-18141) and no EnableDialogBox(39). `m_stDialogBoxInfo[39].sV1,sV3-sV6` is a scratch record for give/repair target (Game.cpp:18629, 32205, 34996, 36327, 42162). Id 1 (Character) is also missing a case line in the 2-38 listing but exists at 18028. | wrong |
| 7 | assets.md:120 | contents1000.txt | "written when the log server sends a new notice". | Request is bSendCommand(MSGID_REQUEST_NOTICEMENT) over m_pGSock (Game.cpp:46070, 2155-2152 path); reply handled in GameRecvMsgHandler (2951) -> NoticementHandler writes the file at 24600. Game server, not log server. | wrong |
| 8 | rendering/blitters.md:106 | Put* count | "All 31 `CSprite::Put*`". | 25 Put* methods (+ `_GetSpriteRect`, `_SetAlphaDegree`, `_bCheckCollison` also guarded = 28). Constructors, `_pMakeSpriteSurface`, `iRestore`, `_iOpen/_iCloseSprite` are unguarded. | imprecise |
| 9 | rendering/blitters.md:109-111, 98-99 | NULL sprite | "silent no-op rather than a crash" is only true for the blit calls. | 165 sites read members of m_pSprite[x] directly (`->m_rcBound`, `->m_stBrush`, `->m_sPivotX`), e.g. Game.cpp:2755, 4533, 8199, 8995, 9235; `_GetSpriteRect` on NULL returns without writing, then `->m_rcBound.left` (4533) faults. Also 4 tile/effect sites (2654). | imprecise |
| 10 | rendering/blitters.md:112 | m_bOnCriticalSection | Described as a guard that is cleanly released. | Set TRUE at entry (Sprite.cpp:883, 108, 713) but every early `return` (clipped out, `_iOpenSprite` failure, szx==0 at 960, `_pMakeSpriteSurface` 74/80/88) leaves it TRUE, so ReleaseUnusedSprites (15618-15660) skips that sheet until a later full draw. | missing |
| 11 | rendering/blitters.md (thunder row, "7 uses") | _DrawThunderEffect | 7 uses. | 6 call sites (Game.cpp:7704-7726) plus the definition. All other use counts in the routine tables match grep. | imprecise |
| 12 | rendering/directdraw.md:173-175 | PutShadowSpriteClip | "clips only on the left edge". | Sprite.cpp:796-831 clips against m_rcClipArea like other routines, then adds a per-pixel left test (848). | imprecise |
| 13 | rendering/directdraw.md:147-149 | iFlip IME variant | "instead of flipping, Blt ... while the edit window exists". | DXC_ddraw.cpp:198-202: under DEF_USING_WIN_IME the Blt to primary is unconditional (no flip chain use); only FlipToGDISurface depends on G_hEditWnd and no SURFACELOST. HTML variant (205-213) is conditional as stated. | imprecise |
| 14 | rendering/directdraw.md:217-218 | WM_ACTIVATEAPP | "re-runs the file checksum". | Wmain.cpp:146: bCheckImportantFile only checks that CONTENTS\badword.txt exists (Game.cpp:8500-8518; the size check on TREES1.PAK is commented out). Error text says "checksum" but nothing is summed. `_iGetFileCheckSum` (3-byte sample sum, name chars +1) has no caller. | imprecise |
| 15 | functions/networking.md:11 | NotifyMsgHandler | "about 139 DEF_NOTIFY_* cases". | 133 `case DEF_NOTIFY_*` + 4 `case DEF_COMMONTYPE_*` in 29946-31462. | imprecise |
| 16 | functions/networking.md:13 | bSendCommand | "about 32 MSGIDs". | 26 distinct `case MSGID_*` (27 labels; REQUEST_CHANGEPASSWORD appears twice under #if); 32 total case labels incl. non-MSGID. | imprecise |
| 17 | functions/networking.md:10 | Cipher ranges | "465-497, 592-610". | iSendMsg is 465-501, pGetRcvDataPointer 589-611. The cipher itself is verified: send `b += (i ^ key); b ^= (key ^ (size - i))` (488-489), receive reverses (606-607), key byte 0 disables it. | imprecise |
| 18 | functions/networking.md NoticementHandler row | Dlg "5, 18, 35" | Dlg 5 listed. | `EnableDialogBox(5...)` at 24618 is inside `#if DEF_LANGUAGE == 3` (Korean only). English opens 18 and (level<42) 35. Same #if blindness affects other rows. | imprecise |
| 19 | functions/support.md (all bCheckValidName rows) | 5 bodies | Does not say which language each is. | 278 Korea(3), 308 Taiwan(1), 353 China(2), 386 English(4, active), 418 Japan(5). English body rejects non-ASCII and `, = space \n \t . \ / : * ? < > | " `` (386-398); stricter set only with DEF_FUCK_USA. | missing |
| 20 | functions/*.md (Purpose column) | ~250 rows | Purpose is the nearest source comment or "(name only)" (163 rows), often unrelated. | e.g. bAddNewEffect "Rudolph" (comment), DrawNpcName `" Berserk"`, DrawVersion "Upper Version", DrawEffects "Fire Explosion", cGetNextMoveDir "v1.42", bSendCommand "Quiz cMsg[256]". See corrected purposes. | imprecise |
| 21 | build-config.md:87-88 | XSocket::bConnect | marked *inferred* (inet_addr). | XSocket.cpp:191 `inet_addr` verified; bBlockConnect (137) uses gethostbyname, so host names work only on the blocking path. | imprecise |
| 22 | build-config.md:90-93 | Version check | Not conditional. | Game.cpp:15095 wraps the check in `#ifndef _DEBUG`. | imprecise |
| 23 | functions/startup.md:16 | GoHomepage | marked *inferred*. | Wmain.cpp:396-398: `#if DEF_LANGUAGE < 3` (Taiwan, China) calls GoHomepage from OnDestroy; verified, English never does. | imprecise |
| 24 | functions/support.md | CMisc::_iConvertFileXor | Purpose omitted. | Misc.cpp:478-543: file = 10-byte header (decimal body size, each byte XOR 20) + body XOR key (35); returns FALSE if size mismatches. Opened `"rt"` (text mode: CRLF folding and Ctrl-Z end-of-file can corrupt XORed bytes); on mismatch the destination FILE is never closed. Missing Curse.txt/badword.txt -> bInit FALSE -> Initialize PostQuitMessage (Wmain.cpp:851-853), key findings 1004-1008 verified. | missing |
| 25 | functions/support.md | CMisc::GetDirPoint | No bug noted. | `*pY--` / `*pX++` step the pointer, not the value (Misc.cpp:120-127); function has no caller. Likewise bEncode/bDecode (254-275) have no caller. | missing |
| 26 | functions/input.md | ClearInputString | No bug noted. | `ZeroMemory(m_pInputBuffer, sizeof(m_pInputBuffer))` (19456-19463) zeroes only pointer-size bytes. | missing |
| 27 | functions/characters.md | bCheckExID | No bug noted. | memcmp against uninitialised `cTxt[12]` (23074-23083). | missing |
| 28 | functions/networking.md | NotifyMsg_BanGuildMan | Callers "-" not explained. | Only definition at 5617; never called (dead code duplicate of DismissGuildApprove logic). | missing |
| 29 | rendering/porting-notes.md:104 | Effect5 bug | Fine, but incomplete. | Game.cpp:3874 builds all 40 slots from entry 0 of that pak, so even with the right name they are 40 copies of one sprite. | missing |
| 30 | functions/networking.md header | WM_USER+2000 | n/a | Game.h:101-102 define without parentheses (`WM_USER + 2000`), precedence hazard in expressions. Minor. | missing |

Claims checked and verified: hard-coded log server 61.100.0.103:2848 at Game.cpp:15549 under `#if DEF_LANGUAGE > 3` (216.27.13.36 with DEF_FUCK_USA at 15546, DEF_TESTSERVER 203.234.215.200 at 15530, Terra 211.43.213.43:2849); Curse.txt startup exit (Game.cpp:1003-1008, plus `#endif` 1011); DEF_LANGUAGE table and per-language switches (GlobalDef.h:164-206); login.cfg parser (15557-15615); XSocket 3-byte header and queueing (479-497); all blitter use counts; 565/555 masks (0xE79C/0x739C, night -3/-3/+2, G double depth in 565 at Sprite.cpp:3225); table builds (DXC_ddraw.cpp:135-169, Wmain.cpp:859-879, floor 1); 672x512 cache; FPS bookkeeping (35386-35395); PAKLib mapping.

## Answer to open question 1: missing sprite (m_pSprite[i] == NULL)

Drawing is skipped, not a crash, for every CSprite method: all 25 Put* plus `_GetSpriteRect`, `_SetAlphaDegree`, `_bCheckCollison` begin with `if (this == NULL) return;` (Sprite.cpp:104 PutSpriteFast, 880 PutTransSprite, etc.; CSprite has only a virtual destructor so the call is a direct non-virtual call that VC6 allows). It is undefined behaviour, so a modern compiler may delete the check. Slots become NULL when a pak is missing (MakeSprite returns at 3549; single loads check INVALID_HANDLE_VALUE) or short (3554). The client CAN still crash on a NULL slot wherever code reads a member directly: ~165 `m_pSprite[..]->m_*` sites (hit-test rects of bodies 8995/9235, item pack bounds 4533, SprFont2/FONT2 glyph widths 8199/8220, item pivot 4558). Missing body, item-pack and font sheets are therefore the dangerous ones; missing effect/monster sheets mostly just vanish. Also a partial pak with a bad BMP crashes in CMyDib (no CreateFile/ReadFile check, Mydib.cpp:17-21).

## Corrected purposes (verified from code)

NotifyMsg_* (all in Game.cpp):
- NotifyMsg_BanGuildMan: clear guild rank, recompute m_bAresden/m_bCitizen/m_bHunter from m_cLocation, open guild-operation dialog 8 with op 5 (never called).
- NotifyMsg_AdminInfo: print five ints as one event-list line.
- NotifyMsg_DismissGuildApprove / GuildDisbanded: reset guild rank and nation flags from m_cLocation; list entry in dialog 8 (GuildDisbanded also replaces '_' with ' ').
- NotifyMsg_DismissGuildReject / QueryDismissGuildPermission / QueryJoinGuildPermission / JoinGuildApprove / JoinGuildReject: queue a guild-operation entry (modes 6/2/1/3/4) and open dialog 8 (JoinGuildApprove also stores guild name and rank).
- NotifyMsg_DownSkillIndexSet: store m_iDownSkillIndex, unlock dialog 15.
- NotifyMsg_FishChance: set fishing dialog 24 chance value.
- NotifyMsg_EventFishMode: open fishing dialog 24 with price/sprite of the catch.
- NotifyMsg_SetExchangeItem / OpenExchageWindow: fill the exchange dialog 27 slots (own/partner item data); Open also opens the dialog.
- NotifyMsg_WhetherChange: set weather via SetWhetherStatus. TimeChange: set G_cSpriteAlphaDegree (day/night), play sound, force background redraw.
- NotifyMsg_RepairItemPrice / SellItemPrice: open dialog 23 (mode 2 repair / 1 sell) with the quoted price.
- NotifyMsg_QuestContents: copy the quest record into m_stQuest. QuestReward: show reward, clear quest, update contribution.
- NotifyMsg_PlayerProfile / NoticeMsg: print text to the event list.
- NotifyMsg_CannotGiveItem / CannotRepairItem / CannotSellItem / CannotRating / CannotJoinMoreGuildsMan: event-list refusal text (Repair/Sell re-enable the item).
- NotifyMsg_DropItemFin_CountChanged / EraseItem, GiveItemFin_CountChanged / EraseItem: report thrown/given items; Erase variants unequip and remove the item.
- NotifyMsg_DismissGuildsMan / NewGuildsMan: announce roster change and clear the guild-name cache.
- NotifyMsg_Charisma / Exp / HP / MP / Hunger: update the stat, print up/down delta (HP/MP ignore changes under 10), hunger warnings by threshold.
- NotifyMsg_EnemyKillReward / PKcaptured / PKpenalty: kill/PK rewards and penalties, update exp and war contribution.
- NotifyMsg_EventFishMode, ForceDisconn: ForceDisconn sets m_bForceDisconn, starts the logout countdown or closes the socket and goes to main menu/quit.
- NotifyMsg_GlobalAttackMode: announce attack-mode permission by area. ItemColorChange, ItemLifeSpanEnd, ItemReleased, ItemRepaired, ItemObtained, ItemPurchased, ItemToBank, ItemDepleted_EraseItem, SetItemCount: inventory/bank bookkeeping with event text.
- NotifyMsg_Killed: zero HP, cancel command/skill, show death messages. LevelUp: store level and stats, message.
- NotifyMsg_MagicEffectOn / Off: event text per magic type/effect. MagicStudyFail / Success: message; success sets m_cMagicMastery.
- NotifyMsg_PlayerShutUp, RatingPlayer, PlayerStatus, TotalUsers, WhisperMode (adds to the 5-entry whisper list), ShowMap (opens map dialog 22), Skill / SkillTrainSuccess / SkillUsingEnd (skill level text), ServerChange (drops both sockets and reconnects to the supplied game server address via a new log-socket).

Support and helpers:
- CMisc::cGetNextMoveDir: direction code 1-8 (1 north, clockwise) from source toward destination, 0 if equal. CMisc::cCalcDirection: same using slope thresholds 1/3 and 3 (one branch is unreachable). GetPoint: Bresenham stepping iCount steps carrying the error term. ColorTransfer: COLORREF to per-channel 565/555 values. bCheckValidString: no spaces. bCheckIMEString: no high-bit bytes. ReplaceString: char substitution. bIsValidSSN/bIsValidEmail: length/digit and '@'+'.' checks (not used in English paths). iGetTextLengthLoc: smallest prefix length whose GDI width exceeds a limit. _iGetFileCheckSum: sum of 3 sampled bytes of a name-obfuscated (+1 per char) file, unused.
- CStrTok::pGet: next token by separator set; CGameMonitor::iReadBadWordFileList / bCheckBadWord: load and match the badword list.
- CWebContainer/CWebCtrl: embedded IE ActiveX host (add/remove/setLocation/Navigate); only used with DEF_HTMLCOMMOM.
- CGame helpers: PutString_SprFont(2/3)/SprNum: sprite-font text (PutString2 black 3-pass outline, PutAlignedString centred DrawText); _InitOnCreateNewCharacter: randomise gender/skin/hair, stats 10; GetPlayerTurn: simulate the walk path to count turns; DrawChatMsgs: draw chat bubbles in range; iGetLevelExp: recursive exp-to-level formula; _iGetFOE: friend/foe code from status bits and nation; SetCameraShakingEffect: shake degree from distance; iGetManaCost: spell mana minus equipped mana-save items; _iCheckLUS: 0 ok, 1 unspent level-up points, 2 stat over 200; SetTopMsg/DrawTopMsg: timed blinking top banner; _PutGuildOperationList/_ShiftGuildOperationList: 100-entry guild-operation queue push/shift; FindGuildName: guild-name cache lookup with LRU replacement; EraseItem: remove item and clear shortcuts; CalcViewPoint: ease camera towards target.
- Wmain: _StartTimer/_StopTimer/_TimerFunc: 1 s multimedia timer posting WM_USER_TIMERSIGNAL; EventLoop: PeekMessage pump with idle UpdateScreen; Initialize: WSAStartup, bInit, blend/add tables; ThreadProc: Korean billing connect thread (DEF_LANGUAGE==3 only).
- XSocket: bInitBufferSize allocates send/receive buffers (+8); iOnSocketEvent dispatches FD_CONNECT/READ/WRITE/CLOSE; bConnect async inet_addr connect, bBlockConnect blocking gethostbyname; iSendMsgBlockingMode: loop send(); _iRegisterUnsentData: ring-queue unsent block.
- bCheckImportantFile: checks only that contents\badword.txt exists.

## Entries checked

593 function rows checked mechanically (location, lines, callers, callees, Msgs, Dlg, Files); about 170 bodies read for purpose/claim verification (all 62 NotifyMsg_*, all CMisc, XSocket send/receive, Wmain startup, sprite blitters, DXC_ddraw init/flip); 5 non-table docs read in full (data-structures, build-config, assets, directdraw, blitters, frame-pipeline, porting-notes). Issues: wrong 6, imprecise 16, missing 8 (30 rows). Not verified: purposes of the draw-dialog and click-handler rows beyond name patterns, the ui-dialogs/world-map/chat narrative overviews, per-asset presence in retail installs.
