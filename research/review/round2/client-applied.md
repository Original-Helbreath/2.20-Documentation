# Client docs: round-2 findings applied (2026-10-25)

Source: `research\review\round2\client.md`. Each finding was checked against `sources\client-2.20` before editing. Docs edited: `research\client.md`, `research\client\` (not `resolution*`). Result: 22 applied (R2-14 and R2-22 in part), 0 rejected. Pak: no finding raised.

| id | status | doc file:line | note |
|---|---|---|---|
| R2-01 | applied | functions/networking.md:28 and rows 41-57 (guild op cells) | op 8; shared cell now lists ops 6/2/1/3/4 plus DismissGuildApprove 5, GuildDisbanded 7. Source 5673 verified |
| R2-02 | applied | functions/characters.md:46 | memcpy overflow into cTxt[12]; 23078-23080 verified |
| R2-03 | applied | functions/networking.md:13 | 27 distinct / 28 labels / 34 total; 34 re-counted by grep |
| R2-04 | applied | functions/networking.md:13 | RequestFullObjectData exception, 8664 verified |
| R2-05 | applied | functions/networking.md rows HP, MP, Exp, Charisma, Hunger, CannotGiveItem, CannotJoinMoreGuildsMan, CannotRating | per-row text; CannotGive/JoinMore/Rating bodies re-read |
| R2-06 | applied | functions/networking.md NotifyMsg_GlobalAttackMode row | review text, M confidence, 43657-43669 not re-read by me |
| R2-07 | applied | functions/networking.md NotifyMsg_WhisperMode row | per review, not re-read by me |
| R2-08 | applied | functions/support.md:28 | per review, not re-read by me |
| R2-09 | applied | functions/magic-effects.md:29 | body re-read; 10 names, floor 1, safe mode, index guard |
| R2-10 | applied | magic-effects.md:18-19, characters.md:54, rendering.md:43 | DrawNpcName, DrawVersion, bAddNewEffect start re-read; DrawEffects text per review |
| R2-11 | applied | build-config.md:49 | 22654 verified; 22651 rect not re-read |
| R2-12 | applied | functions/chat.md:9 | 1002 and 35063 verified |
| R2-13 | applied | functions/game-modes.md:11 | #if/#elif/#else lines verified |
| R2-14 | applied (partial) | characters.md:16,39; startup.md:37,28; networking.md:23; sound.md:18; rendering.md Put* rows | the named examples and the 25 Put* rows only; other comment-fragment cells remain inferred by design |
| R2-15 | applied | research\client.md:13 | 38 case labels counted, array [41] verified |
| R2-16 | applied | data-structures.md (overflow paragraph); functions/world-map.md:32-33 | MAPDATASIZE 40/35 verified |
| R2-17 | applied | build-config.md:35, :58 | re-counted: `== 1` is 9 #if (11 with #elif), differs from review's 7; `> 2` 4; TERRA 44 (verified) |
| R2-18 | applied | functions/networking.md:11 | |
| R2-19 | applied | functions/startup.md:13 | OnTimer body re-read; 5 s block is the level-up/crusade check |
| R2-20 | applied | rendering/blitters.md:222 | 32 occurrences of `PutShadowSpriteClip(` in Game.cpp verified (grep -o; 24 lines because several lines hold two); sites 8975 and 14042 verified gated by detail level and sX < 50 |
| R2-21 | applied | functions/networking.md NotifyMsg_SP row | per review |
| R2-22 | applied (partial) | items.md:26-28, magic-effects.md:23,30 | all five rows; bodies re-read |

