# Server window UI (template for a matching Log server window)

Tags and [V]/[I]/[U] as in `topology.md`. Both official server programs share one Win32 skeleton,
evidently from the same team (the HGserver and gate `Wmain.cpp` files are near copies). A Log server
built from the gate's code inherits it directly.

## Contents
1. Common skeleton
2. Gate server window
3. Game server window
4. Log files
5. Proposed Log server window

## 1. Common skeleton

| Element | Value | Gate | HGserver |
|---|---|---|---|
| Window class | `"Gate-Server%d"` / `"GameServer%d"` with the `hInstance` value | `GATE Wmain.cpp:125` | `SRV Wmain.cpp:146` |
| Class style | `CS_HREDRAW \| CS_VREDRAW \| CS_OWNDC \| CS_DBLCLKS`; no icon; arrow cursor; `COLOR_WINDOW+1` background (system window colour, normally white); no menu; `cbWndExtra = sizeof(int)` | `GATE Wmain.cpp:137-153` | `SRV Wmain.cpp:161-170` |
| Window style | `WS_VISIBLE \| WS_POPUP \| WS_CAPTION \| WS_SYSMENU \| WS_MINIMIZEBOX`, ex-style 0, position `CW_USEDEFAULT`: fixed size, no maximise, no resize border | `GATE Wmain.cpp:164-178` | `SRV Wmain.cpp:191-206` |
| Message loop | `PeekMessage(PM_NOREMOVE)` -> `GetMessage` / `Translate` / `Dispatch`, then `UpdateScreen()`; otherwise `WaitMessage()` | `GATE Wmain.cpp:191-207` | `SRV Wmain.cpp:217-232` |
| Timer | multimedia `timeSetEvent(periodic)`; callback posts `WM_USER+500` to the window | `GATE Wmain.cpp:26, 238, 323-341` | `SRV Wmain.cpp:30, 254, 350-358` |
| Sockets | `WSAAsyncSelect` messages: accept `WM_USER+100`; one message number per peer slot | gate clients from `WM_ONCLIENTSOCKETEVENT` (`GATE Wmain.cpp:84, 111`) | log main `WM_USER+200`, subs +1..+10, gate `WM_USER+1000`, clients `WM_USER+1100`.. (`SRV Wmain.cpp:117-133`) |
| Log list | `char G_cMsgList[120*50]`: 50 lines of up to 120 bytes, newest at index 0; `PutLogList` shifts the buffer down one line and sets an "updated" flag; `UpdateScreen` invalidates the window when the flag is set | `GATE Wmain.cpp:30, 258-276` | `SRV Wmain.cpp:34, 288-304` |
| Painting | `BeginPaint`, `SetBkMode(TRANSPARENT)`, default system font, plain `TextOut`; line i drawn at `y = 5 + base - i*16`, so the newest line is at the bottom of the log area and older lines go up | `GATE Wmain.cpp:279-315` | `SRV Wmain.cpp:307-330` |
| Close | `WM_CLOSE` -> Yes/No `MessageBox` with `MB_ICONEXCLAMATION` | `GATE Wmain.cpp:104-108` | `SRV Wmain.cpp:109-115`, `SRV Game.cpp:38183-38191` |
| Tray icon, menus, accelerators, dialogs | none (the "tray" comment on `WS_SYSMENU` refers to the taskbar button) | [V by grep: no `Shell_NotifyIcon`, no menu resource] | same |
| Hotkeys | raw `WM_KEYDOWN`/`WM_KEYUP` with F1 (and F4/F12) held as modifier flags | `GATE GateCore.cpp:579-668, 852-862` | `SRV Game.cpp:32869-32973` |

## 2. Gate server window

- Title `"Helbreath GateServer 2002-6-20"`, 620 x 700 (`GATE Wmain.cpp:162, 172-173`). [V]
- Layout, top to bottom (`GATE Wmain.cpp:279-315`) [V]:

```
y=10   [F1 + F4]  Shut down all game servers
y=25   [F2] Shows not activated game server list
y=40   [F3] Shows activated game server list
       ... log lines: 35 lines, i=0 (newest) at y=605, i=34 at y=61 ...
y=620  ________________________________________________________________________________
y=640   0    1    2    3    4    5    6    7    8    9        monitor slot numbers, x = 21 + i*30
y=655   ?    ?    O    X    ...                                 slot status: ? none, X = 1, O = 2
```

- Startup log lines: "(!) Reading configuration file...", "(*) Gate server port : n",
  "(*) Build Date : n", "(*) Gate server address : a" (`GATE GateCore.cpp:97-134`), then
  "(!) Gate Server Listening..." (`GATE Wmain.cpp:240`). [V]
- Typical runtime lines: "<n> Accept! Add New Client.", "(!) Game Server Registration - Success( Name:%s
  | Hd:%lx | date %d)", "*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", "WARNING!
  Monitor(%d) connection lost!" (`GATE GateCore.cpp:181-283, 451-537`). [V]
- Close prompt: text "Quit gate server?", caption "GATE-SERVER". [V]

## 3. Game server window

- Title `"Helbreath GameServer V%s.%s %d (Executed at: %d %d %d)"` = version, build date, month,
  day, hour of start (`SRV Wmain.cpp:189`); 800 x 600 (`SRV Wmain.cpp:201-202`). [V]
- Layout (`SRV Wmain.cpp:307-330`, `SRV Game.cpp:888-972`) [V]:

```
y=10   Server-Name(%s) Max.Level(%d) Players(%d/%d - %d/%d) Crusade(%d:%d) SLSock(%d:%d) GTSock(%d) RBT(%d)
y=25   ********** CHINESE SERVER MODE **********   (x=5, one banner per build flag)
       ********** DB GAME SERVER MODE ********** (x=300)
y=40   further banners (BALL EVENT, BEGINNER EVENT, ITEM EVENT)
       ... log lines: 20 lines, newest at y=355, oldest at y=51 ...
y=400  Map(%s)    Object(%d)    P(%d, %d)    N(%d, %d)    A(%d, %d)    E(%d, %d)    M(%d, %d)
       one line per hosted map, 15 px apart
```

  Taiwan builds use a different status line with `Lmt.WorldUser` and `CruID` (`SRV Game.cpp:894-896`).
- Repaint: the 3-second game timer calls `InvalidateRect` (`SRV Game.cpp:3682-3687`), besides log updates. [V]
- Hotkeys (`SRV Game.cpp:32869-32973, 14852-14858`) [V]:

| Keys | Effect |
|---|---|
| F1 + F4 (held) | local shutdown, saving all players ("GAME SERVER SHUTDOWN PROCESS BEGIN(by Local command)!!!") |
| F1 + F6 / F1 + F7 | shutdown announcement 1 / 2 to players |
| F1 + F9 | resume crusade |
| F1 + F11 | end crusade (side 0) |
| F1 + Home | start crusade world-wide |
| Insert / Delete (no modifier) | test meteor strike on side 1 / 2 |
| F2 | body commented out |

- Close prompt: "Player data not saved! Shutdown server now?" with the server name as caption,
  skipped once the shutdown has completed (`SRV Game.cpp:38183-38191`). [V]

## 4. Log files

All in the working folder, appended with `fopen(..., "at")`. Lines start `"(%4d:%2d:%2d:%2d:%2d) - "`
(year:month:day:hour:minute, space-padded) unless noted. [V]

| Program | File | Writer |
|---|---|---|
| Gate | `ItemEvents.log`, `HackAttacker.log`, `GameServerLog\GameServerLogYYYYMMDD.log` | `GATE Wmain.cpp:356-423` |
| HGserver | `Events.log` | `SRV Wmain.cpp:378-396` |
| HGserver | `AdminEvents.log` | `SRV Wmain.cpp:398-416` |
| HGserver | `LogData\ItemEvents%04d%02d%02d.log`, lines start `HH:MM:SS<tab>`, buffered and flushed every 100 lines or 10 s | `SRV Wmain.cpp:420-458` |
| HGserver | `LogEvents.log` | `SRV Wmain.cpp:460-478` |
| HGserver | `DebugMsg.log` | `SRV Wmain.cpp:480-498` |

The on-screen list is memory only; file logging is a separate call. [V]

## 5. Proposed Log server window [I]

Built on the gate's `Wmain.cpp` so it looks like its siblings:
- class `"Log-Server%d"` (hInstance), same class and window styles;
- title `"Helbreath LogServer V2.20 (Executed at: %d %d %d)"` following the game server's pattern;
- 800 x 600 like the game server, or 620 x 700 like the gate;
- top status line at (5,10), same style as the game server:
  `Accounts(%d) Online(%d) GameServers(%d) Maps(%d) ClientConn(%d) GTSock(%d)`;
- hotkey help lines at y = 25/40 like the gate (for example `[F2] Game server list`,
  `[F3] Online accounts`, `[F1 + F4] Save all and shut down`);
- log list painted bottom-up, 16 px per line, `TextOut` on a transparent background;
- 3-second multimedia timer for housekeeping (account timeouts, server liveness), 1-second timer if
  needed for the server-change reservation timeout;
- `WM_CLOSE` Yes/No: "Quit log server?";
- file logs `Events.log` (general) and one daily file for logins, using the shared line prefix;
- startup lines in the `(*) key : value` style for each config value read.
