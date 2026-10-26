# Main Log Server and World Log Server (the 2.20 split)

Tags and [V]/[I]/[U] as in `topology.md`. No source for either program exists; everything here is
read from what the client and game server expect, plus the two config files left in the HGserver
tree.

## Contents
1. Two configs, two shapes
2. Who talks to which
3. Division of work (inferred)
4. Candidate MLS <-> WLS messages
5. Recommendation

## 1. Two configs, two shapes

`SRV Lserver.cfg` - single Log server (also in 2.03) [V]:

```
log-server-address       = 203.234.215.198
log-server-port          = 2848      clients
internal-log-server-port = 3840      game servers
gate-server-address      = 203.234.215.198
gate-server-port         = 6502      Log server connects to the gate as a monitor [I]
```

`SRV WLserver.cfg` - world log server "WS1" [V]:

```
world-server-name          = WS1
world-server-address       = 211.115.208.50
world-server-port          = 2501    clients after a server change
internal-world-server-port = 3840    game servers
main-log-server-address    = 211.115.208.39
main-log-server-port       = 4500    WLS -> MLS link
```

No HGserver code opens either file (`topology.md`). The game server handles `MSGID_WLSERVERFILECONTENTS` and reads only
`world-server-port` from it, to fill the port in `DEF_NOTIFY_SERVERCHANGE` (`SRV Game.cpp:25019-25066, 18646-18667`) [V,
receive side]. That the WLS sends it is [I]: the only WLS binary that exists never sends 0x0FA40007 (`official-compat-220.md`
section 6).

Neither file names a gate for the WLS; `Lserver.cfg` does. Whether the WLS also connected to the gate
is unknown. [U]

**MLS port the WLS dials: answered [V].** `WorldServer.exe` dials `main-log-server-port` from `WLserver.cfg`, nothing hard-coded
(`ReadWLServerConfig` 0x401938 region, keyword string 0x4170a4 stores the port at this+0x64; `InitializeWorldServer` 0x4014e0 (connect loop at 0x401590) loops 10 times
calling `FUN_0040ca50` = `inet_addr` + `htons` + `connect`; reconnect `SendMsgToMLS` 0x4041dd reuses the fields; trace in
`research/review/round2/wls-port.md`). The pack config
`.claude/scratch/v382core/WLserver.cfg:9-10` has `main-log-server-address = 195.154.77.227`, `main-log-server-port = 2981`, so the
WLS dials the MLS **public** port 2981 (`HMLServer.cfg:3`; MLS ports are config-driven: public `log-server-port` 2981..+2, internal
3840). The 4500 above is the 2.20 tree sample value only; no MLS listener matches it and it is not assumed for this setup. The
`WLserver.cfg` `internal-world-server-port = 3840` is the WLS's own game-server listener, not an MLS target. The internal-port
allow-list (patch 4, MainServer 0x51224) is checked only for connections accepted on the internal port (flag 1 at 0x40289c in accept
handler 0x402770), so patch 4 is **not needed** when the WLS dials 2981. **Still unverified [U, runtime test needed]:** that
MainServer accepts a WLS registration arriving on a public socket (the dispatcher is not known to check socket type).

## 2. Who talks to which

| Peer | Talks to | Evidence |
|---|---|---|
| Client, login and character screens | MLS (address hardcoded in the 2.20 client, port 2848) | `client-log-protocol.md` section 1 [V] |
| Client, first entry into the world | MLS: `REQUEST_ENTERGAME` with world name "WS1"; the reply carries the game server address | `client-log-protocol.md` section 8 [V] |
| Client, after a server change | WLS at `world-server-port` (2501), `ENTERGAME` kind `NEW_TOWLSBUTMLS` | `SRV Game.cpp:18646-18667`, `CLI Game.cpp:45039-45090`, `SRV NetMessages.h:315` [V] |
| Game servers | WLS at `internal-world-server-port` 3840 (the game server calls it `internal-log-server-port`) | `SRV Lserver.cfg:5` and `SRV WLserver.cfg:8` (both 3840), `SRV Game.cpp:799-801`; the sample `SRV gserver.cfg:8, 11` holds placeholder ports 111 / 22 (`internal-log-server-port`, `gate-server-port`), not 3840 [V values / I that this is the WLS] |
| WLS | MLS public port: the WLS dials `main-log-server-port` = 2981 in the pack's `WLserver.cfg:10`, the MLS `log-server-port` (`HMLServer.cfg:3`); 4500 is only the 2.20 sample value (`SRV WLserver.cfg:11`) | [V cfg and WLS connect path, `wls-port.md`; U whether the MLS accepts registration on a public socket, runtime test] |

The kind name "to WLS but MLS" (`DEF_ENTERGAMEMSGTYPE_NEW_TOWLSBUTMLS`) says the client sends the
MLS-style request to the WLS. [V name / I reading]

## 3. Division of work (inferred)

Every item here is [I], argued from the evidence above.
- **MLS**: accounts and passwords, account status (playing / not), billing time and IP blocks (see the
  message names in 4), the character list for an account in a given world, create/delete character,
  password change, and the first enter-game, which it forwards to the right WLS.
- **WLS**: game server registration and map lists, config pushes, player data load/save, guilds,
  routing by map for server changes, world user cap (`world-server-max-user`), shutdown gating
  (`SRV Game.cpp:14917` comment: once the shutdown message reaches the WLS it refuses new users).
- (Confirmed: corrected from official binary, WorldServer.exe 0x403640 / 0x404370.) Character files most likely lived with the WLS (it serves `REQUEST_PLAYERDATA`), and the MLS asked
  the WLS for the character list (`MSGID_REQUEST/RESPONSE_CHARINFOLIST`) [I].

## 4. Candidate MLS <-> WLS messages

Defined in `SRV NetMessages.h` and used by neither HGserver 2.20 nor client 2.20 (grep) [V]. Names
suggest the purpose; **no layout is known** [U]. Superseded: the official binaries give the real Main <-> World protocol and layouts in `official-mainserver.md` section 5 (corrected from official binary, MainServer.exe 0x403220 / WorldServer.exe 0x4021b0). The WLS listens on one client port only; the three public ports belong to the MLS (corrected from official binary, MainServer.exe 0x413344).

| Value | Name | Likely purpose [I] |
|---|---|---|
| 0x0512A3F6 / F7 | `MSGID_REQUEST/RESPONSE_REGISTERDBSERVER` (`:264-265`) | a DB back end registering |
| 0x23AA210E | `MSGID_REGISTER_WORLDSERVER` (`:410`) | WLS announces itself to the MLS |
| 0x23AA210F | `MSGID_REGISTER_WORLDSERVERSOCKET` (`:411`) | extra WLS sockets (like the game server's sub sockets) |
| 0x280120A0 | `MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET` (`:430`) | reply |
| 0x23AB211F | `MSGID_REGISTER_WORLDSERVER_GAMESERVER` (`:412`) | WLS tells the MLS about a game server and its maps, so the MLS can answer the first enter-game |
| 0x27049D0C | `MSGID_WORLDSERVERACTIVATED` (`:424`) | world open for players |
| 0x2400000A | `MSGID_REQUEST_REMOVEGAMESERVER` (`:417`) | game server gone |
| 0x23AB2200 / 01 | `MSGID_REQUEST/RESPONSE_CHARINFOLIST` (`:414-415`) | character list for the login screen |
| 0x24021EE0 | `MSGID_REQUEST_CLEARACCOUNTSTATUS` (`:418`) | account no longer in game |
| 0x13000000 | `DEF_MSGID_ANNOUNCEACCOUNT` (`:365`) | account entering the world |
| 0x13000001 / 02 | `MSGID_ACCOUNTINFOCHANGE`, `MSGID_IPINFOCHANGE` (`:367-368`) | billing data change |
| 0x14000010 | `MSGID_ANNOUNCEACCOUNTNEWPASSWORD` (`:371`) | password change pushed to the WLS (which would then send `CHECKACCOUNTPASSWORD` to game servers) |
| 0x14E91200..05 | `IPIDSTATUS`, `ACCOUNTCONNSTATUS`, `CLEARACCOUNTCONNSTATUS` (`:373-378`) | connection-state queries |
| 0x15000001 | `MSGID_REQUEST_NOCOUNTINGSAVELOGOUT` (`:381`) | logout that does not stop billing |
| 0x2900AD10..31 | `BLOCKACCOUNT`, `IPTIMECHANGE`, `ACCOUNTTIMECHANGE`, `REQUEST/RESPONSE_IPTIME` (`:432-436`) | blocks and paid time |

## 5. Recommendation

Build **one** Log process that is both MLS and WLS [I]:
- listen on 2848 (clients), 2501 (clients after a server change; or reuse 2848 and give that port in
  the pushed `WLserver.cfg`) and 3840 (game servers);
- skip the MLS <-> WLS link entirely; nothing outside the pair depends on it;
- push a `WLserver.cfg` whose `world-server-port` is the port where the merged process accepts
  `NEW_TOWLSBUTMLS`, so the game server's `DEF_NOTIFY_SERVERCHANGE` points back at it;
- the client accepts any world name in its enter-game request; answer for "WS1" (the name the
  client sends) and ignore others or treat all as one world.

The only cost is that a multi-world setup is lost, which no known private setup needs.

Server-change address detail [V, `SRV Game.cpp:18646-18667`]: `DEF_NOTIFY_SERVERCHANGE` carries the
game server's own `log-server-address` (15 bytes) and, in `DEF_DEFENCEHACKING` builds, the pushed
`world-server-port`; in other builds it carries `internal-log-server-port` (`SRV Game.cpp:5290, 5336`),
the very port the game servers use. So a Log server for a non-`DEFENCEHACKING` build must accept
client `ENTERGAME` requests on its game-server port and tell the two apart by message. Clients always
encrypt (key != 0) and send `MSGID_REQUEST_ENTERGAME`; game servers send key 0 and their own MsgIDs.
Also, the address must be one the client can reach, so `log-server-address` in `GServer.cfg` must be the
Log server's public address, not a LAN-only one. [I consequence]
