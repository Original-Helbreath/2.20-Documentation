# Round 2 review: Helbreath 2.20 client resolution catalogue

Scope: `research/client/resolution.md` and `research/client/resolution/*` against `sources/client-2.20`
(ground truth) and `sources/hgserver-hbxdevteam`, `sources/hgserver-librehelbreath/src`.
Method: read the cited source lines; independent gap scans (`.claude/scratch/review-res/gap.py`,
`nothit2.py`); sample of ~70 classified rows (`sample.py`); dialog-table recheck against the constructor
(`dlgcheck.py`, 39 of 39 dialogs compared, 38 match, 1 build-dependent); preprocessor-aware activity check of
every classified row (`ppcheck.py`, approximate: unknown macros treated as undefined).
Status words: verified = read in source; unverified = not run (nothing was compiled or run).

## Summary of checks

| Check | Result |
|---|---|
| 1 window-setup.md | about 45 claims verified. 6 issues (1 medium, 5 low). |
| 2 completeness | Enumeration re-run: counts reproduce (1684 lines, 13 family counts, class totals, 1331). 12 other pattern scans (expressions, 53/48/21/16, 400/600/800, loops, SetRect/AddRect/Put* literal pairs, window APIs) found 1 real miss (dead code). 1 methodology gap (preprocessor). |
| 3 classification sample | ~70 rows checked. 1 wrong-class group (5 rows), 1 wrong rationale, 1 build-dependent value. Other classes right. |
| 4 variables.md | All numeric formulas reproduce the 640x480 values (checked ~35). 1 design contradiction, 2 variables that should not exist, set is larger than minimal. |
| 5 claims | All 7 headline claims hold. Qualifications listed (server ranges, tile cache stale rows, query clamp width). |

## Findings

Confidence: H = read directly and unambiguous, M = read plus inference, L = judgement call.

| id | doc file:line | issue | evidence file:line | proposed fix | conf |
|---|---|---|---|---|---|
| W1 | window-setup.md:47-49; values-platform.md rows Wmain.cpp:197,203,204 and Game.cpp:35275-35279; variables.md:266 (CURSOR_HOME) | The "windowed cursor sprite" and "SetCursorPos(510,400)" code is presented as general window behaviour. It exists only in the China/HTML build (`#ifdef DEF_HTMLCOMMOM`, language 2) and runs only while the internet dialog `G_bDlg1` is open. Not part of the normal fullscreen or windowed game loop. | Wmain.cpp:161-211 (`#ifdef` 161, `#endif` 211, `if(G_bDlg1)` 166); Game.cpp:35252-35304 (`#ifdef` 35252, `#endif` 35304), `if(G_bDlg1)` 35262; GlobalDef.h:171-179 (only language 2 defines it) | Say "HTML-dialog build only" in window-setup.md and mark these 6 rows (and CURSOR_HOME) "conditional, language 2 only". Reduce the "5 copies of centring" statement to 3 live copies (DXC_ddraw x2, Wmain.cpp:326-328) plus 2 build-conditional. | H |
| W2 | window-setup.md:72-79 | Present path described for one build only. With `DEF_USING_WIN_IME` (languages 1, 2, 5) fullscreen presents with `Blt(NULL, back)` to the whole primary instead of BltFast+Flip; HTML adds a third branch. Doc names "IME/HTML builds" but not that the 1:1 `m_rcFlipping` logic does not apply there. | DXC_ddraw.cpp:196-220; GlobalDef.h:165,172,190 | State that 215-216 is the language 3/4 path (default `DEF_LANGUAGE 3`, GlobalDef.h:157-159). | H |
| W3 | coincidental.md:153; values-dialogs.md dialog 5 row; coincidental-lines.md Game.cpp:571-572 | Dialog 5 is 320x320 only under `#ifdef DEF_FEEDBACKCARD` (language 2). Otherwise 310x170. The doc gives 320x320 as fact; rows 569-570 are likewise build-dependent. | Game.cpp:567-579 (`#ifdef` 567, else branch 575-578 = 310x170) | Annotate "320x320 in China build, 310x170 elsewhere". Dialog table row 5 uses 320x320, so the right/bottom edges (320,320) are wrong for the default build (310,170). Other 38 dialog rows match source exactly. | H |
| W4 | window-setup.md:131-133 | Server event range is described as "about +-10 x +-7" citing 2886, 2933-2934, 9515-9518. Actual ranges differ per site: 2886-2889 and 3125-3128 are X +-10, Y +-8 (plus `sRange`); 2931-2934 are X +-9, Y +-7; 9515-9518 are strict `>`/`<` so X +-9, Y +-6. The initial block (21x16 at -10,-7) is correct. | hgserver-hbxdevteam/Game.cpp:2886-2889, 2931-2934, 9515-9518, 2145-2146, 1901 | Replace with "X +-9..10, Y +-6..8 depending on message". Add that hgserver-librehelbreath has the same init block (`src/Game.cpp:2060, 2271-2272, 23666`) and same -10/-7. | H |
| W5 | window-setup.md:37-38 | `DXC_dinput::bInit` is lines 33-60, not 34-56. Trivial. | DXC_dinput.cpp:33-60 | Fix range. | H |
| W6 | method.md:7 | "63 files" is the count before the exclusions; after excluding DDRAW.H, DINPUT.H, DSOUND.H, CNTREVNT.H, WEBCTRL.H/.CPP the script scans 57 (68 files - 5 lan_*.h - 6 excluded). | `ls` of sources/client-2.20: 68 .cpp/.h; res_enum.py:9-12 | Say "57 scanned (63 after dropping lan_*.h)". | M |
| C1 | method.md:35,52; change-plan.md:97 (1331 rows) | The enumeration ignores the preprocessor. Approximate evaluation of every classified row against the five `DEF_LANGUAGE` builds: of the 1331 screen-dependent lines, 495 are inactive in the default language-3 build (460 MENU, 29 SCREEN, 4 DIALOG, 1 HUD, 1 VIEW); 138 MENU and 3 other rows are inactive in every build (e.g. the "for KOREAN" `UpdateScreen_OnCreateNewAccount` 27149-27698 sits in the `#else` of `#if DEF_LANGUAGE == 2` and inside `#ifdef DEF_MAKE_ACCOUNT`, which only language 2 defines; the debug-only `StartInputString(10,414)` at 28673 is under `_DEBUG`). 98 more of the 1331 are commented-out lines (66 MENU, 30 SCREEN, 2 DIALOG). | Game.cpp:26455, 26634, 27147-27149, 27699-27701; 21538-21804 (Taiwan-only select-character block, so rows 21563-21839 are language 1 only); 28668-28677; GlobalDef.h:157-197; ppcheck.py output | Add a "build conditionals" section to method.md, add an "active in builds" column (or at least a note) to the values-*.md tables, and give the phase-0 row count as live lines per build. Class totals stay, the work estimate drops. | M (evaluator approximate; the OnCreateNewAccount, OnSelectCharacter, HTML cases verified by hand) |
| C2 | not in catalogue | `CGame::_DrawBlackRect` hard-codes 640x480: `40 - iSize*2`, `30 - iSize*2` (640/16, 480/16), `464 - iSize*16`, `624 - iSize*16`. Matched by none of the 13 families. Only caller is commented out (35316), so dead code. | Game.cpp:22819-22841, 35316 | Add 4 rows as SCREEN (dead). | H |
| C3 | not in catalogue | `CSprite::PutSpriteFastFrontBuffer` blits straight to `m_lpFrontB4` (no literal size, no callers). Not screen-dependent as written, but it bypasses the back buffer and would be wrong after any scaling change. | Sprite.cpp:483-572; grep shows no caller | One line in window-setup "Surfaces": unused front-buffer blitter. | H |
| C4 | not in catalogue | hgserver-librehelbreath carries a commented-out wider move-strip table (`_tmp_iMoveLocX/Y[9][55]` versus live `[9][37]`), prior art for a larger server window. | hgserver-librehelbreath/src/Game.cpp:23, 71 (live), 118-252 (commented `/* ... */`) | Mention in change-plan.md option 3. | H |
| K1 | values-world-view.md rows Game.cpp:6394, 6924, 7704, 7708, 7712; variables.md:201-202 (`SKY_START_Y`, `BOLT_START_Y`); coincidental.md:181 | Misclassified as VIEW. The offsets are relative to the effect's own world position, not to the view: `m_mX = sX+300; m_mY = sY-460` and `m_mX = dX*32+300; m_mY = dY*32-460` are spell animation start offsets from the cast/target cell; the thunder bolt starts `-800` above the strike point (`dY*32 - m_sViewPointY - 800`). They do not change when the screen size changes. The formulas -(VIEW_H+33) and -(SCREEN_H+320) are numerology (460 = 427+33, 800 = 480+320). coincidental.md:181 says the opposite and is wrong. | Game.cpp:6392-6394, 6922-6924, 7703-7714 | Reclassify the 5 rows COINC, delete `SKY_START_Y` and `BOLT_START_Y`, remove the bullet in coincidental.md:181. VIEW 157 -> 152, COINC 101 -> 106. | H |
| K2 | coincidental.md:157 | Wrong rationale: `shX > 301 && shX < 330 && shY > 239 ...` is not "pixels of the 128-px guide-map art" (values exceed 128). `shX/shY` are map-cell coordinates (zoom branch: `m_sPlayerX-64 + (msX-sX)`; unzoomed branch scales by `m_sMapSizeX/128`). Class (COINC) is right. | Game.cpp:17897-17905, 17957-17969, 17998 | Reword to "map cells". | H |
| K3 | values-world-view.md row Game.cpp:15867 (VIEW) | Same statement as DXC_ddraw.cpp:60 (clip rect 640x480) but classed VIEW; also 15870 (BltFast source rect). Harmless (both are screen-dependent) but the platform table says "match 60" is SCREEN. | Game.cpp:15852, 15867, 15870; DXC_ddraw.cpp:60 | Class SCREEN/BUFFER for consistency. | L |
| K4 | change-plan.md:70-71 and variables.md:246 (`QUERY_MAX_X = SCREEN_MAX_X - 235`) | The 235x100 clamp does not equal dialog 20's size (252x87, Game.cpp:669-670), so at the right edge the box can extend to x=656, 16 px past the screen. Noted as "covers HUD" only. Also dialog 22 bottom is 434 (overlaps HUD by 7, doc only lists dialog 10) and dialog 29 (gauge) ends at y=487, 7 px below the screen. | Game.cpp:4717-4719, 669-670, 679-682, 721-724 | Add as bug rows; define `QUERY_W/H` from dialog size instead of 235/100. | H |
| V1 | variables.md:191-192 vs 215 | Contradiction. `VIEW_CENTER_TX/TY` are defined as `SCREEN_W/(2*TILE)` and `ceil(VIEW_H/(2*TILE))`, then line 215 says they "must equal the server's -10/-7". If SCREEN_W changes, the formula changes the tile offset and desyncs `PLAYER_CELL`. Pixel centre (`VIEW_CENTER_X/Y`, easing target) and tile offset (server window cell) are separate quantities. | Game.cpp:2542-2543 (pixel centre), 45987-45988 and 46001-46005 (tile offsets), hgserver-hbxdevteam/Game.cpp:1901 | Make `SRV_CENTER_TX/TY` = 10, 7 constants (server-coupled) and derive `VIEW_CENTER_X/Y` from the screen. Keep the 43 centre-tile rows mapped to the constants, not the formula. | M |
| V2 | variables.md:179-220 | Not minimal. About 60 names, many are one-use expressions: `TILE_LOOP_MAX_X/Y`, `CELL_VIS_MAX_X/Y`, `CHAT_CULL` (the +160/+120 margins are arbitrary), `SNOWLINE_Y` (DEF_XMAS only, Game.cpp:23146), `SKILLMSG_X/Y` (the -69/+4 is a guess; the literal is 280-29, 280-52 at Game.cpp:45532, not obviously centre-anchored), `HUD_Y0` duplicates `VIEW_H`, `VIEW_CENTER_X` duplicates `LISTEN_X`, `MAPDATA_W/H` label differs from the classified note ("VIEW_TILES + 2*margin" vs SRV_WIN+19). | variables.md; classified.tsv rows for MapData.h:26-27 | Keep the 4 base values, buffer sizes, `VIEW_W/H`, `HUD_H`, `SRV_*`, listener and overscan; express the rest inline as `VIEW_W + k`. | L |
| V3 | variables.md:204 (`WEATHER_BOX`) and numbers elsewhere | Checked and correct: 100..1039 / -360..439 / 0..799 (Game.cpp:23213-23249), overscan 128/224/352 (2300-2302), visibility 656/475 (2308), cache loop 688/475 (15854-15857), pick 431 (2390, 2513), LOD +8 (MapData.cpp:1731-1734), listener rows 33+21=54, centre rows 43, HUD 72 = 51+8+7+3+2+1, SRV_WIN 9/10, player cell 19/17 (45987-45988), map data 40x35 (MapData.h:26-27), query clamp 404/379, guide map 640-128-20 and 427-128-20 (17881-17882). | as listed | none | H |
| Q1 | change-plan.md:107-110; window-setup.md:143 | Tile-cache claim holds with detail: with the loop `iy < 427+48` and draw y = `iy-16+sModY`, tile rows end at y=464 for sModY<=5 and y=496 for sModY>5 (15 or 16 rows). The blit reads `480+sModY` rows, so for sModY<=5 rows 464-479+ are stale; only the HUD (from y=427) hides them. Columns: 22 (ix < 688), the 22nd tile ends at x=688 and is clipped by the 672 clip rect. | Game.cpp:15852-15871 | Add the sModY<=5 case explicitly. | H |
| Q2 | window-setup.md:88-91 | Verified: PutShadowSprite bounds x<640,y<427 at Sprite.cpp:742, 760 (only two such literals in Sprite.cpp, 320 `m_rcClipArea` references); PutPixel 0-639/0-479 at DXC_ddraw.cpp:585; DrawLine/DrawLine2 at Game.cpp:23347, 23381, 23455, 23489; DrawShadowBox unclipped (528-579), and all 46 call sites are literal (0,0,639,479) or (0,413,639,429). | as listed | none | H |
| Q3 | change-plan.md:117-119 | Verified: back buffer and cache allocated at DXC_ddraw.cpp:119/123 and 315/319; `ChangeDisplayMode` releases back buffer and front buffer (254-255) but not `m_lpPDBGS`, so the old cache leaks. | DXC_ddraw.cpp:244-328 | none | H |
| Q4 | change-plan.md:134; window-setup.md:110-111 | Observer `msX == 640` / `msY == 480` bug verified: clamp 639/479 (DXC_dinput.cpp:87-88), dead branches 35718, 35720, 35722, 35726, 35730; live: 35716, 35724, 35728 (up-left, left, up). Not run. | Game.cpp:35716-35730 | none | H |
| Q5 | change-plan.md:135; values-dialogs.md:5 | Dialog drag unclamped verified (Game.cpp:35849-35850); release only snaps dialog 9 (35880-35886); re-open clamps at 18574-18580 (the doc cites only the max clamps; 18578-18579 also push a dialog back by 20 when its right/bottom edge is above 10). | Game.cpp:18574-18580 | Mention the min clamps. | H |

## Sample result (check 3)

Seeded sample (`sample.py`, seed 220, stratified by class) plus manual reads of all FP non-EnableDialogBox
rows, all COINC non-dialog-relative rows and all 8 VIEW "other" groups. Rows whose class I would change:
K1 (5 rows). Rows with a wrong or incomplete explanation: K2, W3. Everything else (MENU, HUD, DIALOG,
TOPLEFT, PITCH_OK, FP colours/text ids/cursor-relative placement, COINC dialog-relative offsets, frame times,
teleport tiles, tile sprite ids, buffer sizes) matched the source. No coincidental line was found classed as
screen-dependent other than K1; no screen-dependent line was found classed coincidental.

## Completeness scans that found nothing new

Lines not in `hits.tsv` matching: any `ms[XY]` compared to a 2-3 digit literal (0); calls of
PutString*/PutAlignedString/Put*Sprite*/DrawShadowBox/DrawNewDialogBox/DrawLine/PutPixel with a literal pair
(0); AddRect/StartInputString/SetRect (all relative or object-bound); window APIs `ClipCursor`,
`AdjustWindowRect`, `GetClientRect`, `MoveWindow`, `WM_SIZE`, `ChangeDisplaySettings` (none in the client);
arithmetic forms of 640/480/320/240 (0); literal arrays sized by the screen (none); other-file hits for
160/120/400/600/800/579/560 (only frame times, buffer lengths, sprite ids, `DEF_MAXWHETHEROBJECTS 600`).
`lan_*.h` contain none of 640/639/480/479/320/240/427 (verified, 0 matches).

## Process note

Two empty helper files (`scan2.py`, `nothit.py`) in `.claude/scratch/review-res/` were created through a shell
heredoc before I noticed the constraint; `nothit.py` is broken and superseded by `nothit2.py`. No other file
outside this report and that scratch folder was written.
