# Unverified items resolved (2026-10-25)

| # | Item | Result | Evidence |
|---|------|--------|----------|
| 1 | build-config.md: librehelbreath `src` vs `.claude/scratch/srv220` differ by two casts | verified | `diff -r --strip-trailing-cr`: only Game.cpp:48960 (`static_cast<int32_t>` around `dwTimeGapClient - dwTimeGapServer`) and Wmain.cpp:357 (`(LPTIMECALLBACK)` on `_TimerFunc`) differ; every other src file is identical; scratch has extra data files only (cfgs, logs, .dsp/.dsw, mapdata, GameData). Tag U -> V. |
| 2 | R2-21 rating cooldown "about 1 hour" | confirmed, trace corrected | Decrement at Game.cpp:3876 is in `CheckClientResponseTime` (3827), called from the `m_dwGameTime2 > 3000` block at 3682-3688, not the m_dwGameTime3 block at 3743. Period 3000 ms, so 20*60 ticks = 1200 x 3 s = 3600 s = 1 hour (minimum; the check is `> 3000` on a timer, so slightly more). Text in character.md updated. |
| 3 | Commit id 1697e41 in 16 headers | verified | `sources/hgserver-librehelbreath`: `1697e41` exists, "Imported HGServer 2.20 sources" (2015-02-13); HEAD is 57cbb27 (later CMake commits). `git show 1697e41:Game.cpp` and `:Wmain.cpp` equal the scratch copies (CRLF ignored). `hgserver-hbxdevteam` HEAD 24aa889 "2.24b" is a different line, no 2.20 commit. [U] replaced by [V] in game-server.md, mechanics.md and 14 functions/*.md. |

Old review notes (game-server-part3.md I3, game-server-applied.md) left as historical records.
