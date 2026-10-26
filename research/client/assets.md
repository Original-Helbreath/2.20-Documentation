# Client assets (v2.20)

Every file the 2.20 client loads, where its sprites land in the id space, and whether it exists in
the retail 2.02 install (`downloads\retail_2001\extracted\`) or the 2.23 patch
(`downloads\patch_v223\x\`). Read this before assembling a data folder for the client or
porting the loader. The .pak format itself is PAKLib's (https://github.com/shadowevil/PAKLib); how
the client reads it is in `rendering\blitters.md`.

Method: the list was extracted from every non-comment `CreateFile("sprites\\...")`,
`new class CSprite(..., "name", ...)` and `MakeSprite/MakeTileSpr/MakeEffectSpr("name", start, count)`
in Game.cpp, then each name was checked case-insensitively against the two folders (script
`.claude\scratch\client-map\assets_check.py`, output `assets.json`). Existence is verified; whether an
existing file has enough entries for the requested count was not checked.

## Contents
- How sprites are loaded and indexed
- Sprite id map
- Sprite files: present and missing
- Sounds
- Music
- Maps
- Contents and config files
- Installed but never referenced

## How sprites are loaded and indexed

- Three arrays: `m_pSprite[20000]` (UI, items, characters, monsters), `m_pTileSpr[500]` (map tiles
  and objects), `m_pEffectSpr[100]` (Game.h:640-642).
- `MakeSprite(name, start, count, alpha)` (Game.cpp:3543) opens `sprites\<name>.pak`, **returns
  silently if the file is missing**, reads the entry count at offset 20 and creates
  `min(count, entries)` `CSprite`s at `start..start+count-1`. `MakeTileSpr` and `MakeEffectSpr`
  are the same for the other two arrays (3560-3590). Other entries are created one by one after a
  `CreateFile` that is checked for `INVALID_HANDLE_VALUE`.
- So a missing file or a short file leaves NULL slots. Only 7 of about 1283
  `m_pSprite[..]->Put*` call sites check for NULL (grep counts, verified), but every `CSprite::Put*`
  (and `_GetSpriteRect`, `_SetAlphaDegree`, `_bCheckCollison`) starts with `if (this == NULL) return;`
  (Sprite.cpp:104, 710, 880; verified), so a blit on a missing sheet is skipped (undefined behaviour
  that VC6 tolerates). The client still crashes where code reads a member of a NULL slot directly:
  about 165 `m_pSprite[..]->m_*` sites (e.g. Game.cpp:2755, 4533, 8199, 8995, 9235; verified by
  count). Missing body, item-pack and font sheets are the dangerous ones; missing effect and
  monster sheets mostly just vanish. Not run.
- Sheets are opened lazily on first draw and freed after 60 s (`rendering\frame-pipeline.md`).
  Load order and timing: `functions\config-loading.md` (`UpdateScreen_OnLoading`).

## Sprite id map

Verified from the `Make*` calls (Game.cpp:3600-4230) and SpriteID.h.

| Range | Contents | Formula |
|---|---|---|
| 0-73 | UI: cursor, fonts, dialogs, maps, guide map (SpriteID.h `DEF_SPRID_INTERFACE_*`) | fixed ids |
| 100+ / 200+ / 300+ / 400+ | items ground / equipment (M, W) / pack / dynamic (`DEF_SPRID_ITEM*_PIVOTPOINT`) | pivot + entry |
| 500-1219 | player bodies Bm, Wm, Ym, Bw, Ww, Yw | 500 + type*120, 96 frames each |
| 1220-4579 | monsters and NPCs, owner types 10-69 | 1220 + (type-10)*56, 40 frames (8 or 16 for NPCs) |
| 4580 / 4820 / 5060 / 5300 / 5540 / 5780 | male underwear, hair, body armour, arm armour, pants, boots | base + 15*n, 12 frames |
| 6020-9099 | male weapons | 6020 + 64*w, 56 frames |
| 9100 / 9230 / 9300 | male shields, mantles, helms | base + 15*n |
| 14580-19399 | female equivalents | male base + 10000 |
| tiles 0-461 | maptiles1 0, structures1 51/55, Sinside1 70, Trees1 100, TreeShadows 150, objects1-4 200-220, tile223-229, maptiles2-6 300-352, maptiles353-361, tile363-461 | `MakeTileSpr` start |
| effects 0-87 | effect 0, effect2 10, effect3 13, effect4 19, effect5 24-30, CruEffect1 31, effect6 40, effect7 45, effect8 57, effect9 66-86 | `MakeEffectSpr` start |

## Sprite files: present and missing

208 distinct names (plus one `%s` format string, MakeSprite's own path).

**Present in 2.02:** 155 names, including all 6 body sheets, item sheets, interface, interface2,
New-Dialog, sprfonts, Newmaps, all common monsters and equipment, maptiles1-6, objects1-4,
structures1, Sinside1, Trees1, TreeShadows, effect-effect5, CruEffect1. Eleven of them are also
replaced by the 2.23 patch: item-equipM, item-equipW, item-ground, item-pack, maptiles1, Mlboots,
Mshoes, objects1, objects2, Wlboots, Wshoes.

**Only in the 2.23 patch:** effect9, GameDialog, monster, Telescope.

**In neither (49):**

| Group | Names | Effect when missing |
|---|---|---|
| UI | LoginDialog, DialogText, FeedBack (FeedBack only with DEF_FEEDBACKCARD) | LoginDialog and DialogText are loaded by the English build (Game.cpp:3637, 3669): the login screen and text dialogs have no art; blits are skipped by the `this == NULL` guard unless a member is read directly (*inferred*, not run) |
| Monsters added after 2.02 | Beholder, Bunny, CanPlant, Cat, Crop, DarkElf, Devlin, DireBoar, Ettin, Frost, Gagoyle, GiantFrog, IceGolem, McGaffin, MTGiant, Perry, Rudolph, Wyvern | blits are skipped (`this == NULL` guard) but direct member reads of the NULL slot crash (e.g. hit-test rects, Game.cpp:8995, 9235); exact outcome per owner type *inferred*, not run |
| Effects | effect6, effect7, effect8 | effect ids 40-65 empty |
| Weapons and gear | Mhammer, MBHammer, Mhoe, MSW2, MSWx, Msanta, NMHelm1-4, and W equivalents | blits are skipped, but body/item-pack member reads (Game.cpp:4533, 8995, 9235) crash on a NULL slot; exact outcome *inferred*, not run |
| Tiles | tile226-229, tile430-443, tile444-444, tile445-461 | maps using these tiles draw nothing or crash (*inferred*) |
| Bug | `Effect5.pak` (name passed with the extension, Game.cpp:3874) | opens `sprites\Effect5.pak.pak` ([RENDER-003](../issues/rendering.md#render-003-energy-ball-sprite-pak-name-is-wrong)) |

Conclusion ([SETUP-013](../issues/setup-2.20.md#setup-013-missing-sprite-paks)): the 2.02 install plus the 2.23 patch is not a complete data set for this client.
A 2.20-era sprite folder (or later, e.g. a 3.x install) is needed for about 48 files; the 2.23
patch's own set (with `Tile462-475`, MH*/WH* armour etc.) targets a newer client and adds names
2.20 never loads.

## Sounds

`sounds\C%d.wav` 1-24, `sounds\M%d.wav` 1-98, `sounds\E%d.wav` 1-47, created only when sound is
enabled (Game.cpp:4229-4243). The 2.02 install has all 24 C and 47 E files, and M1-M70 only:
**M71-M98 are missing** (the v2.18 monster sounds, comment at 4234). `CSoundBuffer` on a missing file
fails quietly (*inferred*; see `functions\sound.md`).

## Music

`music\aresden.wav`, `elvine`, `dungeon`, `middleland`, `MainTm` all present in 2.02;
`music\Carol.wav` (DEF_XMAS only) is missing. Track choice: `StartBGM` (Game.cpp:35458).

## Maps

`mapdata\<name>.amd` built at Game.cpp:45978-45980 from the map name the server sends. The 2.02
install has 59 .amd files (60 entries in the folder); the 2.23 patch adds none. Maps added after
2.02 are therefore missing; which names the 2.20 server uses is in `..\servers\` (format:
`..\shared\formats\`).

## Contents and config files

All under `contents\` unless noted; the path is built as `contents\\<name>` (double backslash,
harmless on Windows).

| File | Read by | 2.02 | 2.23 |
|---|---|---|---|
| login.cfg (root) | `bReadLoginConfigFile`, ignored by the English build (`build-config.md`) | yes | no |
| BItemcfg.txt | build items (Game.cpp:23998) | yes | no |
| ItemName.cfg | `bReadItemNameConfigFile` (24720) | yes | yes |
| magiccfg.txt | magic config (8342) | yes (MAGICCFG.TXT) | no |
| Skillcfg.txt | skill config | yes (SKILLCFG.TXT) | no |
| GameMsgList.txt | `_LoadGameMsgTextContents` (19009) | yes | no |
| badword.txt | XOR 35 to badword.tmp, read, deleted (995-1001) | yes | no |
| Curse.txt | same, DEF_LANGUAGE > 2 (1004-1010) | no | no |
| contents%d.txt | NPC/shop/text dialogs (7949, 18891, 18949) | 47 numbered files | 13, incl. new 217-219, 901, 907 |
| contents1000.txt | notice text; **written** by the client when the game server (`m_pGSock`, `MSGID_REQUEST_NOTICEMENT`, Game.cpp:46070; reply handled in `GameRecvMsgHandler` -> `NoticementHandler`) sends a new notice (24600) | yes | no |

**Curse.txt is required by the English build** (DEF_LANGUAGE 4 > 2): if `_iConvertFileXor` fails,
bInit shows "CURSE.TXT file contains wrong infomation." and returns FALSE, so the client exits
(Game.cpp:1003-1008, verified). Neither 2.02 nor 2.23 ships it; supply an empty or real Curse.txt ([SETUP-012](../issues/setup-2.20.md#setup-012-cursetxt-is-required-by-the-english-client)).
badword.txt has the same check (995-999).

## Installed but never referenced

44 sprite files in 2.02 or 2.23 that 2.20 never opens: ABS, CruObj1, maptiles3, mdfyobjects,
mdfytiles, Mptwk, NewIconP, ShopKpr, Structures2, tile422-426, Mhelm5, Whelm2/3/5, Mmantle04-05,
Wmantle04-05, Tile462-464, Tile462-475 and the MH*/WH* cap, hauberk, helm, leggings, plate mail and
robe sheets from the patch.
