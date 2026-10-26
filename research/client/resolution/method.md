# Enumeration method and completeness

How the candidate occurrences were found, so the catalogue can be re-checked or re-run.

## Scope

- Scanned: every `*.cpp` / `*.h` in `sources\client-2.20` (63 files: 68 lower-case `.cpp`/`.h` minus the 5
  `lan_*.h`), read as cp949.
- Excluded: DirectX SDK headers (`DDRAW.H`, `DINPUT.H`, `DSOUND.H`), `CNTREVNT.H`, `WEBCTRL.*` (third-party
  web control; these 6 files have upper-case extensions and are not in the 68), and the `lan_*.h` string tables (verified: every non-blank, non-comment line is a string `#define`, and a grep for
  640/639/480/479/320/240/427 returns 0 in all five files).
- Scripts (re-runnable): `.claude\scratch\resolution\res_enum.py` (writes `hits.tsv`; uses `ppeval.py`, a
  small `#if` evaluator, to give every row a build activity),
  `res_classify.py` (ordered rules, writes `classified.tsv`, prints any unclassified row),
  `res_gendocs.py` (writes the `values-*.md` and `coincidental-lines.md` tables).
- Comments are matched too (a commented line is flagged "(commented)"); `//` text is stripped before
  matching live code.

## Pattern families (line counts, measured; a line may hit several families)

| Family | Pattern (Python regex, summarised) | Lines |
|---|---|---|
| F1 screen size | `640 639 641 480 479 481` as whole numbers | 121 |
| F2 half size | `320 319 321 240 239 241` | 210 |
| F3 play area | `427 426 428 413 414 425 429 431 213 478` | 73 |
| F4 view pixels | `288 224 352 672 512`, `640 + n`, `480 + n` | 20 |
| F5 view tiles | `/32)+10|7|8`, `-10)*32`, `-7)*32`, `32*21`, `32*16`, `4+5 5+5 6+5 14+5 12+5`, `sPivotX-4`, `sPivotY-7`, `MAPDATASIZE` | 75 |
| F6 buffers | `Pitch`, `*480`, `480*` | 130 |
| F7 HUD coords | in HUD functions (icon/gauge panel, chat, OnGame, CommandProcessor, event list, dialog dispatch): call with literal x,y, `msX/msY` compared to a 2-3 digit literal, `AddRect(`, `PutAlignedString(n,n,n`, `StartInputString(n`, `DrawNewDialogBox(id,n,n` - excluding lines that are only `sX+`/`sY+` relative | 113 |
| F8 menu coords | same coordinate regex inside `UpdateScreen_On*` (except OnGame), `_Draw_OnLogin`, `_Draw_UpdateScreen_OnCreateNewAccount`, `_bDraw_OnCreateNewCharacter`, `DrawVersion`, `UpdateScreen_OnLoading_Progress` | 929 |
| F9 dialog positions | `m_stDialogBoxInfo[..].sX|sY = ...digit` | 122 |
| F10 window | `cy > 280`, `cy -= 40`, `SetCursorPos`, `m_DInput.m_sX|m_sY = n` | 18 |
| F11 off-screen | `rand() % 940|800`, `- 460`, `- 800` | 9 |
| F12 cull | `DrawChatMsgs(<literal>` | 1 |
| F13 catch-all | any other Game.cpp function: call with two 2-3 digit literal args, or `msX/msY` vs 3-digit literal, not `sX/sY`-relative | 67 |
| F14 black rect | `40 - iSize*2`, `30 - iSize*2`, `464 - iSize*16`, `624 - iSize*16` (`_DrawBlackRect`, Game.cpp:22826-22839; missed by F1 because the 640x480 appear as 40x30 cells of 16 px; dead code, only caller commented at 35316) | 4 |

Total distinct lines: **1688** (1684 + the 4 F14 lines). All 1688 are classified (`res_classify.py` prints zero unclassified rows).

## Classes (lines, measured)

| Class | Lines | Meaning |
|---|---|---|
| SCREEN | 82 | true screen-dependent (mode, window, mouse clamp, full-screen rects, line clip, `_DrawBlackRect` 4 dead rows) |
| BUFFER | 5 | surface allocation / clear sized by the screen |
| VIEW | 152 | world view: tiles visible, centre, listener, picking, overscan bounds, server window |
| HUD | 72 | bottom icon/gauge panel, chat bar, lower event list |
| TOPLEFT | 9 | anchored to (0,0): listed, no change |
| DIALOG | 128 | dialog default anchors, runtime placement, clamps |
| MENU | 891 | absolute coordinates on 640x480 menu canvases |
| COINC | 106 | same number, unrelated (dialog-relative offsets, frame times, map coordinates, ids, sizes, effect-relative start offsets -460/-800) |
| FP | 114 | regex matched but the line holds no screen literal (colours, text ids, cursor-relative placement) |
| PITCH_OK | 129 | uses the pitch variable correctly; no literal width |

Screen-dependent total (SCREEN + BUFFER + VIEW + HUD + DIALOG + MENU) = **1330 lines** (1331 minus the 5
effect-relative rows reclassified COINC, plus the 4 `_DrawBlackRect` rows).

## Build conditionals (which lines are compiled)

The enumeration is textual; `res_enum.py` now also runs a real `#if` evaluator (`ppeval.py`: `#define`,
`#undef`, `#if`/`#ifdef`/`#ifndef`/`#elif`/`#else`/`#endif`, `defined()`, `==`, `>`, `&&`, block comments;
`#include` is ignored and `GlobalDef.h` is processed first as the seed of every file; 0 unparsed `#if` lines).
It is not the reviewer's approximation (`review-res\ppcheck.py`, unknown macros read as undefined). Every
row carries an activity code, shown as the **Build** column of the `values-*.md` and `coincidental-lines.md`
tables.

- **Default build** = the as-shipped one: `DEF_LANGUAGE 4` (English), hard-coded at GlobalDef.h:1, so the
  language-3 `#ifndef` fallback at GlobalDef.h:157-159 never applies; no `_DEBUG`, no optional switch.
  (The round-2 review assumed language 3; `build-config.md` has language 4.)
- **Other builds** evaluated: languages 1, 2, 3, 5; each language with `_DEBUG`; each language with one of the
  optional switches `DEF_TESTSERVER`, `DEF_FUCK_USA`, `DEF_JAPAN_FOR_TERRA`, `DEF_XMAS`, `DEF_COLOR` (all
  commented out in GlobalDef.h:28-32). "Only lang N" means compiled in that language build; "only dbg" in a
  `_DEBUG` build; "only opt" only with an optional switch.
- Activity: **default** (compiled in the default build), **other builds only**, **dead in all builds** (no
  evaluated build compiles it), **commented** (`//` or inside `/* */`).

Measured, over the 1330 screen-dependent lines (counts are lines; the compiler's view, not run):

| Class | Lines | Default build | Other builds only | Dead in all builds | Commented |
|---|---|---|---|---|---|
| SCREEN | 82 | 41 | 11 | 0 | 30 |
| BUFFER | 5 | 5 | 0 | 0 | 0 |
| VIEW | 152 | 151 | 1 | 0 | 0 |
| HUD | 72 | 65 | 7 | 0 | 0 |
| DIALOG | 128 | 120 | 6 | 0 | 2 |
| MENU | 891 | 411 | 287 | 127 | 66 |
| **Screen-dependent** | **1330** | **793** | **312** | **127** | **98** |

The 312 other-build lines are language 2 (230), language 3 (32), language 1 (19), language 5 (18), optional
switch (11) and `_DEBUG` (2). So **about 793 lines are live in the shipped build; 537 are not** (312 in some
other build, 127 in none, 98 commented). The 127 dead MENU lines include `UpdateScreen_OnCreateNewAccount`
Game.cpp:27147-27698 (the `#else` of `#if DEF_LANGUAGE == 2` inside `#ifdef DEF_MAKE_ACCOUNT`, which only
language 2 defines). The language-1-only select-character block is Game.cpp:21538-21804; the debug-only
chat caret `StartInputString(10, 414, ...)` at 28673 is under `_DEBUG`. Outside the screen-dependent set the
remaining 358 rows (COINC, FP, TOPLEFT, PITCH_OK) split default 324, other builds 19, dead 1, commented 14
(see the Build column of `coincidental-lines.md`). All 1688 rows: default 1117, other builds 331, dead 128,
commented 112.

Class totals above stay valid (a class says what the line is, not whether it is compiled); the work estimate
for a default-build port drops to the 793 default lines, but the other-build lines still need the same
variables if those builds are kept. The windowed cursor code (Wmain.cpp:197-204, Game.cpp:35273-35279) is
language 2 only (`DEF_HTMLCOMMOM`) and runs only while the internet dialog is open.

## Known limits of the method

- Counts are **lines**, not literals; a menu line often carries 2-4 numbers.
- A value computed through another name is found only if that name matched (e.g. `DEF_BTNSZX` button
  sizes are not screen-dependent and were not hunted).
- Single-digit and 1-digit-pair coordinates (`PutString(10, 10, ...)`) are found only inside the
  HUD/menu function sets; the F13 catch-all requires 2-3 digit literals. Inspection of the F13 output found
  no further absolute UI coordinates in dialog code: dialog bodies are `sX/sY`-relative (verified by the
  catch-all, which returned only colours, text ids and the chat-input caret).
- Literal screen-dependence in **other repositories** (server view window) is noted in
  `window-setup.md` but not catalogued row by row.
- Nothing was compiled or run; classifications are from reading code (verified by source reading,
  unverified at run time).
