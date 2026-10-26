# Round 2 resolution findings: applied log

Source of findings: `resolution.md` (same folder). Ground truth: `sources\client-2.20` (read directly; nothing compiled or run).
Edited: `research\client\resolution.md`, `research\client\resolution\*`, and the scripts in
`.claude\scratch\resolution\` (`res_enum.py`, `res_classify.py`, `res_gendocs.py`, new `ppeval.py`). The `values-*.md`
and `coincidental-lines.md` tables were regenerated from the scripts.

Important deviation: the review took the default build as `DEF_LANGUAGE 3`. GlobalDef.h:1 hard-codes
`#define DEF_LANGUAGE 4`, so the `#ifndef` fallback at :157-159 never applies; the as-shipped build is language 4
(matches `build-config.md`). Build activity is therefore measured against language 4.

| id | result | where | reason |
|---|---|---|---|
| W1 | applied | window-setup.md Window section; variables.md CURSOR_HOME; res_classify.py notes for Wmain.cpp:197,201-204 and Game.cpp:35275-35279 (-> values-platform.md); method.md | Verified: Wmain.cpp:161-211 and Game.cpp:35252-35304 are `#ifdef DEF_HTMLCOMMOM` (language 2), inside `if(G_bDlg1)`. "Five copies" is now 3 live + 2 build-conditional. |
| W2 | applied | window-setup.md Presenting | Verified DXC_ddraw.cpp:198-219: WIN_IME -> `Blt(NULL)`, HTML adds a third branch; 215-216 is the language 3/4 path (stated as shipped language 4, not 3). |
| W3 | applied | coincidental.md; res_classify.py note for Game.cpp:571-572; res_gendocs.py (dialog 5 row now 310x170 with the 320x320 note) | Verified Game.cpp:567-579: 320x320 only under `DEF_FEEDBACKCARD`. Dialog table right/bottom edge now 310,170. |
| W4 | applied | window-setup.md Hard limits | Verified hbxdevteam Game.cpp:2886-2889/3125-3128 (X+-10, Y+-8+sRange), 2931-2934 (X+-9, Y+-7), 9515-9518 (strict: X+-9, Y+-6); librehelbreath src/Game.cpp:2060, 2271-2272. I did not re-check the review's `23666` citation and did not cite it. |
| W5 | applied | window-setup.md Chain | DXC_dinput.cpp:33-60 verified. |
| W6 | rejected | method.md Scope (clarified instead) | The script scans 63 files (measured). The 68 count is lower-case `.cpp`/`.h`; the 6 excluded files (DDRAW.H, DINPUT.H, DSOUND.H, CNTREVNT.H, WEBCTRL.H/.CPP) have upper-case extensions and are not in the 68, so 57 is wrong. Scope text now says so. |
| C1 | applied | ppeval.py (new), res_enum.py (activity column), res_classify.py/res_gendocs.py (Build column), method.md "Build conditionals", change-plan.md phase 0 | Real `#if` evaluator in the enumeration scripts, not the review's approximation; 0 unparsed directives. Builds: languages 1-5, each with `_DEBUG`, each with one optional switch. Default = language 4 release. Measured over 1330 screen-dependent lines: default 793, other builds only 312, dead in all 127, commented 98. All 1688 rows: 1117 / 331 / 128 / 112. Differences from the review's 495/141/98: default is language 4 not 3; optional switches count as "other builds" not dead; the commented count (98) agrees. |
| C2 | applied | res_enum.py (family F14), res_classify.py, values-screen-game.md (Game.cpp:22826, 22827, 22834, 22839); method.md; change-plan.md | Verified Game.cpp:22819-22841 and the commented caller 35316. 4 SCREEN rows, noted as dead code (the function itself is compiled in the default build; the Build column says `default`, the note says dead). |
| C3 | applied | window-setup.md Surfaces | Verified Sprite.cpp:483-572 and no caller anywhere. |
| C4 | applied | change-plan.md option 3 | Verified librehelbreath src/Game.cpp:23, 71 live `[9][37]`; commented `[9][55]` tables at 120-252 (review said 118-252; the `/*` is on 120). |
| K1 | applied | res_classify.py (5 rows -> COINC); variables.md (SKY_START_Y, BOLT_START_Y deleted); coincidental.md (new row, bullet removed); method.md | Verified Game.cpp:6393-6394, 6923-6924, 7704-7714: offsets are from the effect's own world position. VIEW 157 -> 152, COINC 101 -> 106. |
| K2 | applied | res_classify.py note for Game.cpp:17998; coincidental.md | Verified Game.cpp:17897-17905, 17998: cell coordinates. |
| K3 | partly applied | res_classify.py note for Game.cpp:15852/15867 | Class kept VIEW (it is the tile-cache clip pair documented with the cache; low-confidence finding); added a note that 15867 equals DXC_ddraw.cpp:60. 15870 left VIEW. |
| K4 | applied | variables.md (QUERY_W/H), change-plan.md bug rows (235x100 vs 252x87, dialog 22 bottom 434, dialog 29 ends 487), res_classify.py note on the 30 clamp rows | Verified Game.cpp:4717-4719, 669-670, 679-682, 721-724. |
| V1 | applied | variables.md (SRV_CENTER_TX/TY = 10, 7 constants; VIEW_CENTER_X/Y derived from the screen; PLAYER_CELL, LOD use SRV_CENTER); res_classify.py variable names; change-plan.md | Verified Game.cpp:2542-2543, 45987-45988, hbxdevteam Game.cpp:1901. The 43 centre-tile rows now map to the constants. |
| V2 | rejected | variables.md | Low-confidence design preference (shrink the variable set); no source disagreement, and the listed names each replace a verified literal. Left for the user. |
| V3 | n/a | - | Review states numbers are correct; no change. |
| Q1 | applied | change-plan.md Riskiest areas | Verified Game.cpp:15854-15861: for sModY <= 5 tile rows end at y 464 (rows 464-479 stale), else 496. |
| Q2, Q3 | n/a | - | Review verified, no change requested. |
| Q4 | n/a | - | Verified by review, doc already states it. |
| Q5 | applied | change-plan.md bug row; res_gendocs.py intro of values-dialogs.md | Verified Game.cpp:18574-18581 (min clamps 18578-18579). |

Counts (measured by tallying this table): 15 applied (W1-W5, C1-C4, K1, K2, K4, V1, Q1, Q5), 1 partly applied (K3), 2 rejected (W6, V2), 4 no-change confirmations (V3, Q2, Q3, Q4).
Note: a small slip is recorded here for the process: two shell commands with empty heredocs were issued by mistake and stopped; they wrote nothing.
