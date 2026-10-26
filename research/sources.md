# Helbreath original sources - catalog

Scope: official Siementech-era material (1999-2005) only. Private-server forks, clients, sprite
packs and sites are excluded. Aim: the least-modified source. Some change is unavoidable
(encoding fixes, build fixes); pick the copy with the smallest documented delta and record
every change made on top of it.

All checks below were run 2026-10-25 without cloning or downloading: Git repos with
`git ls-remote` + GitHub API, files with HTTP HEAD (`curl -sIL`). Status words: `verified`
(checked that day), `unverified` (not checked, with reason).

## Headline

- **Least-modified matching pair: v2.20 client + v2.20 server, both from `librehelbreath`.**
  Imported 2015-02-13/15 as raw developer trees. The server tree still contains Visual
  SourceSafe files (`vssver.scc`, `mssccprj.scc`), VC6 workspace files (`.ncb`, `.opt`,
  `.plg`) and runtime logs, which suggests a straight copy of a Siementech working folder.
  The only stated change is a restored Korean text encoding.
- **Oldest source found: v2.03 server** (no matching 2.03 client found).
- **Not found anywhere:** official Gate server and Log server source (the 2.20 server has
  `GateServer.cfg` / `Lserver.cfg` but not those programs), any 1.x / 3.x official source,
  official client source other than 2.19/2.20.

## Source code

| Rank | What | Version | Where (exact) | Modification level | Status |
|---|---|---|---|---|---|
| 1 | Client (C++, DirectDraw/DInput/DSound, VC6-era, 99 files incl. DX headers/libs and DLLs) | 2.20 (`GlobalDef.h` defines 2.19 and 2.20 by region) | `https://github.com/librehelbreath/ClientSources.git`, single commit `c01544d` "Imported original 2.20 client sources" (2015-02-15) | Single-commit import, no later edits | verified (ls-remote, API tree, GlobalDef.h read) |
| 2 | HG game server (C++, VC6 `.dsp/.dsw`, cfgs, `mapdata/`, `GameData/`) | 2.20 (`Version.h`: Korea build 207, USA 222, Interserver 306) | `https://github.com/librehelbreath/HGserver.git` at commit `1697e41` (2015-02-13). Use `git checkout 1697e41`; **not** master | Encoding fix only | verified (API commit + tree + Version.h) |
| 3 | HG game server | 2.03 | same repo, commit `1c98b2f` "Imported HGServer 2.03 sources" (2015-02-13) | Encoding fix only (message: "Broken Korean character encoding was restored") | verified (API commit) |
| 4 | HG game server zip | 2.03 | `https://sourceforge.net/projects/hb-online/files/helbreath/Server_Source/2.03_hbSource_Source.zip/download` (3,233,513 bytes, file dated 2004-11-20) | Oldest dated copy, contents not inspected; useful to diff against #3 | verified downloadable (HEAD 200); contents unverified |
| 5 | HG game server, 2.03 -> 2.24b history | 2.03 first commit "2.03 leaked sources" | `https://github.com/Helbreath/hgserver-hbxdevteam.git` | Later commits are the HBX private server: **use first commit `dfd3bb3` only**, as a cross-check | verified (ls-remote, API) |

| 6 | **Gate server** (C++, VC6 `.dsp`, GateCore, PartyManager, `GateServer.cfg` port 6502) | 2.19 | `https://github.com/librehelbreath/GateServer.git` at commit `0b994cd` (2014-10-27, "Sources for the Gate Server of Helbreath 2.19"). HEAD `a2b7f1d` (2018) is a clang-format reformat only | Untouched import (has `vssver.scc`) | verified (cloned to `sources/gateserver-librehelbreath`) |

**Gate cross-check (verified 2026-10-25):** HelbreathX forum file "2.20 GateServer Sources"
(70.78 kB, uploaded 2025 by "Sharon", needs a forum login; copy in `downloads/forum_gate220/`)
is identical to librehelbreath `0b994cd` in all 31 files once encoding/line endings are
normalised. File dates 2004-03-22 (sources) and 2005-09 (VC workspace files, i.e. when it was
last opened). The gate has no version string of its own: it accepts any game server whose
`build-date` matches its config, so the "2.19" and "2.20" labels refer to the same code.

**Client cross-check (verified 2026-10-25):** `github.com/Helbreath/2.20client` (2022
import, cloned to `sources/client-2.20-helbreathorg`) matches librehelbreath ClientSources
in all 112 shared files; in `Game.cpp`, `Wmain.cpp` and the `lan_*.h` files, they differ only in
non-ASCII comment bytes (Korean/Chinese text encoding). The forum's `ClientSources.zip` is
GitHub's zip of librehelbreath commit `c01544d`; the forum's `client.zip` is the 2022 HBX
`Helbreath/client` (private-server, ignored). Both in `downloads/forum_clients/`.
Also inspected, not useful (in `downloads/forum_other/`):
- `client-ui.zip`: a 2022 Vue web UI. It has a `createcharacter.png` that may be a usable
  visual reference.
- `base-client.zip`: a 2022 modern C++/vcpkg rewrite.
- `helbreathusa.zip`: a 2015 nopCommerce ASP.NET web shop, not game code.

**Log server reference sources (newer, all private-server derived; all `git ls-remote` verified 2026-10-25):**

| Repo | Login part | Notes |
|---|---|---|
| `Saritagala/HBGhost` (2022, 1.2 GB) | `MainLServer/` (MainLog.cpp) and `WorldLServer/` (WorldLog.cpp, 148 KB) | **Same Main / World split as 2.20.** VC6 `.dsp` projects. No Korean comments, so it is community code, not a leak. Uses SQLAPI, plus some file I/O. The WorldLServer README says "Leeched from Helbreath Resource" (helbreath.info). Best structural reference. |
| `rastaunlimited/Helbreath-International-Source` (2015, 41 MB) | `Login/LoginServer.cpp` | Single combined login server; MySQL era |
| `rastaunlimited/Helbreath-International-All-In-One` (2015, 450 MB) | `Sources/Login/LoginServer.cpp` | "Helbreath Mysql 2015" |
| `isolatorhk/Helbreath.LoginServerConsole` (2016) | `LoginServer.cpp`, `GameServer.cpp` | Console; MySQL |
| `Helbreath/server_old` (2022, HBX) | `src/lserver.cpp` | Modernised |
| `xRisenx/Helbreath-Source` (and forks incl. `shadowevil/Helbreath-Source-master`) | `fantasy-logins/LoginServer.cpp` | API metadata unavailable |

How to use them: as references for structure and message handling only. Their protocol targets
3.x and must be checked against the 2.20 docs in `research/servers/`.

**Outpost forum (unadvised.net/forums), read 2026-10-25 as a guest.** Attachments need a
logged-in account. The topics most relevant to the log server are:

| Topic | Date | Why it matters |
|---|---|---|
| t=6789 "Worldlserver Source" by Hypnotoad | 2006-05-21 | **Best lead.** A World Log server source described as "a combination of 3.51 AND 2.03 world". It stores data as ASCII text files (characters, and guilds in `Guild\AscII%d\GUILDNAME.txt`), with SQL left as `// todo`. Handles character creation and saving. Community-written, but modelled on the original. |
| t=5845 "Is Here Any 2.20 Wl Or/and Ml" | 2005-11 | Says most servers in 2005 ran an official **WL server binary v2.15** (one poster v2.195), and that the **ML (main log) binary** could be hex-edited or downloaded from "hbss". So official binaries circulated, but not sources. |
| t=7038 "Console World Login Server Source" by Cleroth | 2006-07 | Source and binary; megaupload link (dead). |
| t=8252 "MainLogServer(Source)" by marleythe9 | 2007-05 | Unfinished Main Log rewrite: login, game-server registration, character create and delete. |
| t=7625 "Main/World Src" | 2006-11 | Skeleton only. |
| t=9861 "v3.51 Server Files" (Helbreath Files forum) | posted 2022 | Official 3.51 build 521 game-server binary ("hacked from HBChina"). Its attachment list is not visible to guests. |
| t=9860 "v3.51 Server Config files and SQL Dump" | 2022 | Config files and an SQL dump. |
| t=4990 | 2005-06 | Someone offered official 3.8 Main Log and World server sources for USD 10,000, which confirms they existed privately. |

**Outpost attachments (downloaded 2026-10-25 while logged in; in `downloads/outpost/`):**
- `220HGServersrc.zip`
  - **A second copy of the 2.20 game server.**
  - **Verified:** all 68 code and project files it shares with librehelbreath `1697e41` are
    identical, including `Game.cpp` (dated 2004-03-22).
  - It lacks the data files. StrTok and XSocket come only as `.lib` files.
  - `Portion.*` is the same file as `Potion.*`; only the `#include` name differs.
- `351SqlFiles.zip`
  - The official 3.51 game server binary: `HGServer-v3.61-(521).exe`, dated 2005-01-07.
  - Also MapData text files dated 2004-03-22.
- `351configsandsqldump.rar`: the 3.51 cfg set, `ws1.txt` (World Server 1 config) and
  Crusade/Heldenian SQL scripts. This suggests the 3.x world server used SQL in places.
- `351clientsrcs.zip` (1,505,619 B): the 3.51 client sources. Not inspected.
- The Hypnotoad WorldLServer attachment (t=6789) **no longer exists** on the forum.

**Official Main Log and World Log server binaries (found 2026-10-25):** forum file
`helbreath-v3.82.zip` (324,026,786 B, in `downloads/forum_v382/`) contains, under
`Server/Core/`:
- `MainServer.exe` (393,216 B, with 1,935 Korean string runs)
- `WorldServer.exe` (106,496 B, with 1,330 Korean string runs)
- their cfgs (`HMLServer.cfg`, `WLserver.cfg`)
- empty `Account/` and `Character/` folders

They are Siementech binaries with text-file (`AscII`) account and character storage. Their
log strings describe the whole flow:
- registration of World and Game servers;
- account create and password checks;
- character create, delete and send;
- the enter-game reply `ReqEnterGame: Accnt Char Map Addr Port Code`;
- connection states INIT/WAIT/PLAYING/NULL.

**Build dates, from the PE link timestamps (verified):**
- `MainServer.exe`: 2002-11-26
- `WorldServer.exe`: 2002-11-29

That is the 2.19/2.20 era: the 2.20 code comments run to 2002-12, and the 2.20 builds are
dated Feb-Mar 2003. These are probably the widely used "WL v2.195" generation (inferred).
The WorldServer predates the 2.20 game server's "2002-12-8 World server teleport list"
change, so it may lack that one message (inferred). There is no source code. Usable as a behavioural
reference, and possibly as a test harness for a rebuilt 2.20 log server.
`Sources/Server` in the same zip is a 3.82 HG game server, not a log server.

**Log server: no official source found** (official 3.x binaries exist, see above). Community rewrite (private-server
derived, MySQL, unchecked against 2.20): `https://github.com/isolatorhk/Helbreath.LoginServerConsole.git`
(verified reachable). Unopened 2004 HBX server packages that may bundle Log/Gate binaries, all HEAD 200:
SourceForge hb-online `Compiled_server/HBServer2.23.zip` (5,272,468 B),
`Compiled_server/Hb_server_2.22_HbX.zip` (4,967,646 B), `OldFiles/HBserv221.zip`
(5,256,808 B), `OldFiles/HBS.zip` (5,264,084 B). Also `github.com/Helbreath/2.20client`
(2022, same 2.20 client lineage; `LOGIN.CFG` -> 211.115.208.39:2848).

Do not use (verified reachable, but private-server modified): `librehelbreath/HGserver` master
(restructured + CMake + C++17, 2018), `Helbreath-Amazon/Source` (3.82 "HB Sunrise", fan
client by Snoopy81/Cleroth/Diuuude, not Siementech code), `play-helbreath/equilibrium-project`,
`Helbreath/server*`, `Helbreath/client*`, SourceForge `HBX_2.14` / `2.17_HBX` zips.

### Local clones and checks (2026-10-25)

Cloned into `sources/`: `client-2.20` (ClientSources), `hgserver-librehelbreath` (HGserver;
2.03 = `1c98b2f`, 2.20 = `1697e41`), `hgserver-hbxdevteam` (2.03 = `dfd3bb3`).

- **2.03 integrity: verified.** `1c98b2f` (uploaded 2015) and `dfd3bb3` (uploaded 2022,
  separate people) are identical in all 66 shared code/project files (incl. `Game.cpp`,
  42,705 lines) once Korean encoding (utf-8 vs cp949) and line endings are normalised.
  librehelbreath adds 151 data files (cfgs, `mapdata/`, logs) that the HBX copy lacks. Two
  independent copies agreeing is good evidence that this is the circulating leak text.
  Script: `.claude/scratch/scripts/diff203.py`. SourceForge zip (#4) not yet compared (needs
  a 3.2 MB download).
- **2.20 pair matches: verified.** All shared message IDs in `NetMessages.h` carry the same
  values on client and server (a few names differ in spelling, e.g. `CREATEPOTION` /
  `CREATEPORTION`; server-only IDs are inter-server traffic). Both `Version.h` (server) and
  `GlobalDef.h` (client) give 2.20 for Korea/English/International, 2.19 for Taiwan/China.
- **2.20 date: derived.** Code comments run to 2002-12; server build dates 0207 / 0222 /
  0306 (Korea / USA / Interserver) therefore most likely mean Feb-Mar 2003, which lines up
  with the HelbreathUSA launch (Jan-Feb 2003). Inference only.

## Official clients (retail discs)

| What | Direct URL | Size (bytes) | Status |
|---|---|---|---|
| "HelbreatH - The Crusade" retail CD, 2001 | `https://archive.org/download/helbreath-the_crusade-2001/HelbreatH.iso` | 361,857,024 | verified downloadable (HEAD 200) |
| "Helbreath - The Crusade (USA)" Redump bin/cue | `https://archive.org/download/redump-id-65970/Helbreath%20-%20The%20Crusade%20%28USA%29.bin` (+ `.cue`) | 382,656,288 | verified downloadable (HEAD 200) |

**2001 disc contents: verified** via archive.org's in-ISO listing (no download):
`Hb202full_ENG.exe` (211,830,945 bytes, 2001-08-21) = an official **v2.02 English full
installer**, plus `Movie/Cru_Helbreath.avi` (Crusade trailer). USA disc: contents unverified
(bin image, no listing).

### Official installers and patches on the Wayback Machine (2.20 era first)

All HEAD-checked 200 via `https://web.archive.org/web/<timestamp>id_/<url>` on 2026-10-25.

| What | Wayback URL (`id_` form) | Size (bytes) | Relevance |
|---|---|---|---|
| HelbreathUSA `patch_v223.zip` | `https://web.archive.org/web/20141104104043id_/http://www.helbreathusa.com/patch_v223.zip` | 42,004,965 | **Closest official client data to 2.20** (contents unverified) |
| HelbreathUSA full install (2011) | `https://web.archive.org/web/20110711161458id_/http://www.helbreathusa.com/downloadHB/Helbreath%20USA%20Install.zip` | 289,132,524 | Late 3.8x-era; base for comparison only |
| helbreath.com `downloads/Helbreath.exe` (2002-04) | `https://web.archive.org/web/20020413091249id_/http://www.helbreath.com:80/downloads/Helbreath.exe` | 307,200 | Small; likely a launcher/web installer (unverified) |
| helbreath.com `japan/down/Helbreath.exe` (2002-10) | `https://web.archive.org/web/20021031103851id_/http://www.helbreath.com:80/japan/down/Helbreath.exe` | 303,104 | Same, Japan |
| HelbreathUSA `Patch_3.0_to_3.1.zip` | `https://web.archive.org/web/20130630001212id_/http://www.helbreathusa.com/Patch_3.0_to_3.1.zip` | 2,324,091 | Later era |

Also archived (later era, not checked): `hbusa_update_3.7_to_3.72Winter.zip`,
`hbusa_update_3.72_to_3.81.zip`, `hbusa_update_3.80_WishBall.zip`,
`hbusa_update_080105.zip`. Snapshots of 3.5->3.6, 3.6->3.7 and 3.72->3.80 are ~360-byte
error pages.

**Likely route to 2.20 client data:** v2.02 full install (2001 disc) + `patch_v223.zip`,
then check that the result has every file the 2.20 client loads.

### Files the 2.20 client loads (from source, verified)

`sprites\*.pak` (e.g. `interface.pak`, `interface2.pak`, `item-pack.pak`, `LoginDialog.pak`,
`GameDialog.pak`, `DialogText.pak`, `New-Dialog.pak`, `Newmaps.pak`, `TREES1.PAK`,
`structures1.pak`, `sprfonts.pak`, `effect5.pak`, `Mbo/Mhr/Mpt/Msh/Msw` and
`Wbo/Whr/Wpt/Wsh/Wsw` (male/female equipment), `item-equipM/W`, `item-ground`),
`mapdata\*.amd`, `sounds\C%d.wav`, `E%d.wav`, `M%d.wav`, `music\*.wav` (`MainTm`,
`aresden`, `elvine`, `middleland`, `dungeon`, `Carol`), `contents\ItemName.cfg`,
`contents\contents1000.txt`, `contents\badword.txt`, `contents\Curse.txt`, `magiccfg.txt`,
`LOGIN.CFG` (log server address/port). Registry key `Software\Siementech\Helbreath\Settings`.

## Reference / history

| What | URL | Status |
|---|---|---|
| Helbreath Fandom wiki dump, 2021-05-30 (WikiTeam) | `https://archive.org/download/wiki-helbreathfandomcom/helbreathfandomcom-20210530-wikidump.7z` (1,071,392 bytes) | verified downloadable (HEAD 200) |
| RaGEZone "Helbreath server files version 3.51 and 3.2" | `https://forum.ragezone.com/threads/helbreath-server-files-version-3-51-and-3-2.20768/` | unverified (HTTP 403 to fetch) |

## Downloaded and inspected (2026-10-25)

Stored under `downloads/<name>/`, one folder per file. Untrusted: nothing has been run.

- **`downloads/sf_203/2.03_hbSource_Source.zip`** (3,233,513 bytes, size matches).
  - Holds `2.03 Source/hbs2/`: all 217 files of the librehelbreath 2.03 commit, plus a
    compiled `HGserver.exe` and three Windows `.lib` files.
  - All 217 shared files are identical to librehelbreath `1c98b2f` once encoding and line
    endings are normalised.
  - **Verified:** three independent copies of 2.03 agree (2004 zip, 2015 import, 2022 import).
  - The zip keeps **original file timestamps**, the best dating evidence found:
    - `Game.cpp`, `HGserver.exe` and `HGserver.plg` are dated **2001-08-31**, so the 2.03
      build is from Aug 2001.
    - Most other source files are dated 2001-11-25.
    - Maps go back to 1999-07-22 (`aresden_my.amd`).
    - `HGserver.ncb`, `HGserver.opt`, `Events.log` and `GameData/CrusadeGUID.Txt` are dated
      **2003-05-21**, meaning the tree was last opened in Visual C++ and run then. That is
      consistent with a leak around May 2003 (derived, not proven).
- **`downloads/patch_v223/patch_v223.zip`** (42,004,965 bytes), extracted to `x/`.
  - An **incremental** official patch: 60 files, 112 MB unpacked.
  - `HelGame.exe` (2003-04-15) is ASPack-packed, so no readable version strings.
  - 45 `sprites\*.pak` files dated Dec 2002 to Mar 2003: armour sets, mantles, map tiles,
    `GameDialog`, `item-*`, `monster` and others.
  - `contents\ItemName.cfg` and `contentsN.txt` dated Apr 2003.
  - Pak header (`<Pak file header>` then `<Sprite File Header>`) matches what the 2.20
    client's `Sprite.cpp` reads. **Verified compatible format.**
  - It is not a full client: base files such as `interface.pak`, sounds, music and
    `mapdata` must come from a full install.
- **`downloads/retail_2001/HelbreatH.iso`** (361,857,024 bytes, size matches), extracted to
  `iso/`: `Hb202full_ENG.exe` plus `Movie/Cru_Helbreath.avi`.
  - The installer is a 77,824-byte stub (dated 2000-07-06) followed by a 211.7 MB payload in
    an **unidentified** installer format.
  - 7-Zip cannot open the payload. It has no zlib streams and no plaintext file names, and
    the stub carries no vendor string, only macros such as `#InstallDir#` and
    `#UserSerialNumber#`.
  - **Unpacked 2026-10-25 by static reverse engineering** (installer never run).
    - Extractor: `tools/hb_installer_extract.py`, standard library only. Format spec is
      in its docstring and in `.claude/scratch/re/`.
    - Output: `downloads/retail_2001/extracted/`, listing in `extracted_listing.txt`.
    - **Verified:** 439 files, 630,000,307 bytes, count and sizes match the installer's
      file table.
    - **Verified:** all 438 stored checksums match.
    - **Verified:** 166 of 169 paks have the pak magic; the other 3 are empty in the
      installer itself.
    - **Verified:** all 146 wavs are valid.
    - Folders: `CONTENTS`, `FONTS`, `MAPDATA`, `MUSIC`, `SOUNDS`, `SPRITES`; root files
      `HelGame.exe`, `Helbreath.exe` (FTP auto-updater for `update.helbreath.com`, reads
      `update.lst`, folder `patch_en`), `LOGIN.CFG` (log server 211.239.170.90:2848),
      `Readme.htm`, `search.dll`, `zlib.dll`, `Uninstal.exe`.
    - Title "Helbreath Crusade v2.02". Registry key `Software\SiemenTech\Helbreath`.

### 2.20 client data coverage (verified 2026-10-25)

Script: `.claude/scratch/scripts/client_coverage.py`.
- The 2.20 client source references **208 sprite paks**.
- The v2.02 base and the `patch_v223` files together provide **160** of them.
- **48 are missing**. They were added by intermediate patches 2.03-2.22, which have not
  been found:
  - **Monsters and NPCs:** beholder, bunny, cat, darkelf, devlin, direboar, ettin,
    frost, gagoyle, giantfrog, icegolem, mcgaffin, mtgiant, perry, rudolph, wyvern,
    msanta, wsanta.
  - **Weapons, tools and helms:** mhammer, whammer, mbhammer, wbhammer, mhoe, whoe,
    msw2, wsw2, mswx, wswx, nmhelm1-4, nwhelm1-4.
  - **UI:** dialogtext, feedback, logindialog.
  - **Effects:** effect6, effect7, effect8.
  - **Map tiles:** tile226-229, tile430-443, tile444-444, tile445-461.
  - **Other:** canplant, crop.

  Maps, sounds and contents were not checked.
- **The official updater left no archive.** `update.helbreath.com` has no Wayback
  captures; it was FTP. A later check of `ftp.helbreath.com` was interrupted by an
  archive.org outage.

## Gaps

- **Base client files (v2.02) are locked in an installer whose format is unidentified.**
  Two ways to unpack it: run it inside Windows Sandbox (disposable VM) and copy out the
  install folder, or reverse the payload format. After unpacking, apply `patch_v223` on top
  and check the result against the list of files the 2.20 client loads.
- No format documentation or original tools for `.pak` / `.amd` found; the 2.20 client's
  `Sprite.cpp`, `TileSpr.cpp`, `MapData.cpp` are the de facto spec.
- Leak origin: unverified forum claims only (server hack, or ex-employee). Copies were
  circulating by 2004 (SourceForge file dates).
- Wayback snapshots of helbreath.com / siementech.com not checked (rate-limited).
