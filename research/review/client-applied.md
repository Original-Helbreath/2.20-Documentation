# Client docs: review findings applied (2026-10-25)

Source review: `research\review\client.md`. Docs edited: `research\client\` only (the index `research\client.md` held none of the affected claims, so it is unchanged).
Re-check method: each cited code location was re-read in `sources\client-2.20\` before editing (Game.cpp 4270-4302, 18505-18540, 8500-8519, 45039-45090, 43459-43553, 43675-43711, 5617-5674, 23809-23839, 25050-25181, 14712, 19962, 23051, 24628, 48009, 1238, 24598-24622, 1431-2275 case counts, 29946-31462 case counts; Wmain.cpp 236-300, 392-414, 143-148, 857; DXC_ddraw.cpp 196-214; Sprite.cpp this==NULL guards, 774-850; Misc.cpp function list; Game.h 101-102; counts of Put* = 25, `m_pSprite[..]->m_` = 165, `_DrawThunderEffect` = 7 text hits). No disagreement with the review was found for any row listed as applied.
Limits (unverified): the bodies of the other roughly 100 purpose rows were not re-read line by line; their text is taken from the review's "Corrected purposes" section (the review states it read all 62 NotifyMsg_*, all CMisc, XSocket and Wmain). One refinement was made against the review: NotifyMsg_ForceDisconn now says it prints the notice when the program is active (it does not itself start a countdown) and closes the socket otherwise.

Tag convention: a function-table Purpose cell ending in `[V]` means re-checked against the body by this review; all other Purpose cells stay inferred. The column key of all 14 function docs says so.

## Issues table (30 rows)

| # | Status | Where applied |
|---|---|---|
| 1 DEF_GIZON | applied | build-config.md (macro row) |
| 2 missing sprite crash | applied | assets.md (missing-file note, 3 asset-group rows) |
| 3 keep-alive vs disconnect | applied | functions/networking.md overview; startup.md OnTimer line |
| 4 timer is timeSetEvent | applied | functions/startup.md Initialize line; `_StartTimer` rows |
| 5 WinMain order, mutex | applied | functions/startup.md Entry line |
| 6 dialog 39 | applied | data-structures.md (table cell and note) |
| 7 contents1000.txt source | applied | assets.md |
| 8 Put* count 25 | applied | rendering/blitters.md |
| 9 direct member reads crash | applied | rendering/blitters.md prologue and Make* note |
| 10 m_bOnCriticalSection stays TRUE | applied | rendering/blitters.md prologue item 2 |
| 11 thunder 6 call sites | applied | rendering/blitters.md |
| 12 PutShadowSpriteClip | applied | rendering/directdraw.md |
| 13 iFlip IME variant | applied | rendering/directdraw.md |
| 14 WM_ACTIVATEAPP checksum | applied | rendering/directdraw.md |
| 15 133 + 4 cases | applied | functions/networking.md |
| 16 26 MSGID labels | applied | functions/networking.md |
| 17 cipher ranges | applied | functions/networking.md |
| 18 NoticementHandler Dlg 5 | applied | functions/networking.md row |
| 19 bCheckValidName languages | applied | functions/support.md (5 rows, language per body) |
| 20 Purpose column | applied in part: 133 rows below; 4 rows rejected | see rejected list |
| 21 XSocket::bConnect inet_addr | applied | build-config.md; XSocket rows |
| 22 version check `#ifndef _DEBUG` | applied | build-config.md |
| 23 GoHomepage | applied | functions/startup.md |
| 24 _iConvertFileXor | applied | functions/support.md row |
| 25 GetDirPoint, bEncode/bDecode | applied | functions/support.md rows |
| 26 ClearInputString | applied | functions/input.md row |
| 27 bCheckExID | applied | functions/characters.md row |
| 28 NotifyMsg_BanGuildMan never called | applied | functions/networking.md row |
| 29 Effect5 40 copies | applied | rendering/porting-notes.md |
| 30 WM_USER+2000 parentheses | applied | functions/networking.md overview |

Issues-table rows: 30 applied (one of them, #20, only in part).

## Corrected purposes (rows retagged [V])

133 function-table rows had their Purpose replaced and tagged `[V]` (list of `file|function`: `.claude\scratch\srv220\apply\applied_names.txt`). Breakdown by file (measured by the script): networking.md 75, support.md 28, characters.md 12, startup.md 7, chat.md 5, magic-effects.md 2, input.md 1, items.md 1, ui-dialogs.md 1, world-map.md 1 = 133; 0 mapped names left unmatched.

Purposes the review lists that have no matching row in the docs: none. Review names not listed in its corrected section (for example NotifyMsg_SP) were left as they were.

## Rejected rows (4)

All four come from issue 20; the review names them as wrong but its "Corrected purposes" section gives no verified replacement, so nothing could be retagged [V] without guessing.

- `CGame::bAddNewEffect` (magic-effects.md): comment "Rudolph" kept.
- `CGame::DrawEffects` (magic-effects.md): comment "Fire Explosion" kept.
- `CGame::DrawNpcName` (characters.md): comment " Berserk" kept.
- `CGame::DrawVersion` (rendering.md): comment "Upper Version" kept.

## Counts

- Applied: 30 issues-table rows (#20 in part) + 133 purpose rows = 163.
- Rejected: 4 rows (no code disagreement; no verified replacement text).
