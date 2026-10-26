# Client build configuration (v2.20)

Compile-time switches of the 2.20 client and how the log-server address is chosen. Read this before
building the client or pointing it at a server. All entries are verified against
`sources\client-2.20\` (commit c01544d) unless marked *inferred*.

## Contents
- Where switches live
- DEF_LANGUAGE
- Derived and optional switches
- Log server address (and how to change it)
- Version check
- Build inputs present in the tree

## Where switches live

Everything is in `GlobalDef.h`, included by Game.h:24, Wmain.cpp:20, Misc.cpp:6 and DXC_ddraw.h:19.
The tree has no project file (.dsp/.vcxproj; [SETUP-014](../issues/setup-2.20.md#setup-014-no-client-project-file-in-the-tree)), so preprocessor defines from a project are
unknown; `_DEBUG` is the only switch expected from the compiler. StdAfx.h defines nothing relevant.

## DEF_LANGUAGE

`#define DEF_LANGUAGE 4` (GlobalDef.h:1). Values (GlobalDef.h:2-6, 9-26):

| Value | Build | Version | Strings header (Game.cpp:9-17) |
|---|---|---|---|
| 1 | Taiwan | 2.19 | lan_tai.h |
| 2 | China | 2.19 | lan_chi.h |
| 3 | Korea | 2.20 | lan_kor.h |
| 4 | English (shipped setting) | 2.20 | lan_eng.h |
| 5 | Japan | 2.20 | lan_jap.h |

Undefined or >5 falls back to 3 (GlobalDef.h:157-162), but since line 1 defines it first, the
fallback only applies if line 1 is removed. Use counts in the source: `== 3` 24 blocks, `== 5` 23,
`== 4` 20, `== 2` 15, `== 1` 9 (all counts are `#if` lines only, measured by grep over *.cpp and *.h; `#elif` lines excluded, `== 1` has 11 with them), plus `> 2` (4 `#if`, e.g. Curse.txt loading), `> 3`, `!= 3`, `< 5`, `>= 3`,
`< 3` and one `% 2 == 0` (China + English, Game.cpp:43371). Korea-only code includes the ISP billing
thread `ThreadProc` (Wmain.cpp:429-785). `lan_eng.h` is the only non-UTF-8 source file.

## Derived and optional switches

Set per language (GlobalDef.h:164-197):

| Switch | Languages | Effect |
|---|---|---|
| DEF_USING_WIN_IME | 1, 2, 5 | Windows IME text input (11 blocks) |
| DEF_ENGLISHITEM | 1, 2, 4, 5 | item names from ItemName.cfg instead of Korean |
| DEF_USING_GATEWAY | 1 (removed under `_DEBUG`, 208-210) | ask a gateway for the least-loaded log server (`MSGID_GETMINIMUMLOADGATEWAY`, Game.cpp:14786-14801) |
| DEF_SHORTCUT | 1, 2 | extra shortcut key code (8 blocks) |
| DEF_GIZON | 1, 2, 3 | only changes how dialog 34 (item upgrade) opens (Game.cpp:18507-18538, the only use of the macro). With it: set cMode from the caller and open. **Off in English**: opens in cMode 6 if the inventory holds sprite 6 frame 128/129 items, otherwise shows DRAW_DIALOGBOX_ITEMUPGRADE30 and returns. Opened from the inventory dialog's upgrade button (Game.cpp:22651-22654), also in English; cType is ignored there (verified) |
| DEF_SELECTSERVER | 2, 3, 4, 5 | world-server select screen; when undefined, `WS1` is forced (Game.cpp:15516-15520) |
| DEF_MAKE_ACCOUNT | 2 | in-client account creation |
| DEF_FEEDBACKCARD | 2 | feedback dialog |
| DEF_HTMLCOMMOM | 2 | HTML notices via WEBCTRL (21 blocks) |
| DEF_ACCOUNTLONG / DEF_ACCOUNTLEN 16 | 5 with DEF_JAPAN_FOR_TERRA | long account names |

Optional, all commented out (GlobalDef.h:7, 28-33): DEF_COLOR (colour test), DEF_XMAS (snow, trees,
`music\Carol`), DEF_TESTSERVER (undefines DEF_SELECTSERVER, internal test log server),
DEF_JAPAN_FOR_TERRA (44 conditional lines in *.cpp/*.h, GlobalDef.h excluded; measured), DEF_FUCK_USA (US service build: English items, no server select,
US log server), DEF_SHOWCURSORPOS (Game.cpp:35310). `DEF_FULL_SCREEN` appears only commented out
(Wmain.cpp:329).

## Log server address (and how to change it)

`bReadLoginConfigFile` (Game.cpp:15526), called from `bInit` (Game.cpp:882) with `"login.cfg"`:

1. `DEF_TESTSERVER`: hard-coded `203.234.215.200:2848` (Game.cpp:15528-15533).
2. `DEF_JAPAN_FOR_TERRA`: hard-coded `211.43.213.43:2849` (15536-15540).
3. **`DEF_LANGUAGE > 3` (English and Japan): hard-coded, login.cfg is never read.**
   `61.100.0.103:2848` (Game.cpp:15549), or `216.27.13.36:2848` with DEF_FUCK_USA (15546). The
   v2.15 change log gives the reason: English and Japanese clients were forced to a fixed IP because
   players edited the port (GlobalDef.h:145).
4. Otherwise (Taiwan, China, Korea): parse `login.cfg` with `strtok` on `"= ,\t\n"`; the address
   must be at most 15 characters and the port non-zero, else bInit fails.

So the shipped English source ignores `LOGIN.CFG` ([SETUP-011](../issues/setup-2.20.md#setup-011-the-clients-log-server-address-is-hard-coded)) (whose value in the tree is `211.115.208.39:2848`;
the retail 2.02 one is `211.239.170.90:2848`). To point the English client at your own server,
either:

- change the literal at Game.cpp:15549 (and the port at 15552) and rebuild, or
- change the `#if DEF_LANGUAGE > 3` at Game.cpp:15543 so the file parser runs, then edit `login.cfg`
  (same keys as the existing file; see `..\servers\client-log-protocol.md`).

With `DEF_USING_GATEWAY` the address may later be replaced at runtime by the gateway reply
(Game.cpp:14797-14801). The game server address always comes from the log server reply, not from
a file (`..\servers\client-log-protocol.md`).

The address buffer is `m_cLogServerAddr[16]` (Game.h:859): IPv4 dotted only, no host names
(`XSocket::bConnect` uses `inet_addr`, verified at XSocket.cpp:191; `bBlockConnect`, line 137, uses `gethostbyname`, so host names would work only on the blocking path).

## Version check

The client compares the server's version words with DEF_UPPERVERSION/DEF_LOWERVERSION and switches
to `DEF_GAMEMODE_ONVERSIONNOTMATCH` on mismatch (Game.cpp:15096). The check is wrapped in `#ifndef _DEBUG` (Game.cpp:15095), so debug builds skip it. The English build is 2.20.

## Build inputs present in the tree

Libraries and DLLs shipped beside the source: DirectX 7 headers and libs (DDRAW, DINPUT, DSOUND,
DXGUID), WS2_32, WSOCK32, WINMM, IMM32, `search.dll`/`search.lib` (anti-hack, loaded by WinMain),
`Cint.lib` (the `CInt` guard, no source), `StrTok.lib`, `DesDll.dll/.lib`, `ClientLogon.dll`
(`#import`ed at Wmain.cpp:28, a COM type library; its use is in the Korean login path, *inferred*),
and the unused helper DLLs `MMXCPY.DLL`, `Dib16.dll`, `Offs.dll`, `DDRAW7.DLL`
(`rendering\blitters.md`).
