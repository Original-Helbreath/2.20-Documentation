# Messages: Main Log server <-> World Log server

Generated from the NetMessages.h files by `.claude/scratch/shared-ref/gen_messages.py`; conventions in `README.md`. Sender -> receiver: main log -> world log (and reverse for responses; per-row direction is in the name: REQUEST/COMMAND = toward the server side, RESPONSE/NOTIFY/EVENT = back). 
Byte layouts: `../../servers/official-mainserver.md` (not repeated here).

| Hex | Name | Parent | Ver | Link basis (inferred) | Client 2.20 | Game 2.20 | Game 2.03 | Gate 2.19 | Official | Docs |
|---|---|---|---|---|---|---|---|---|---|---|
| 0x0FC94203 | `MSGID_RESPONSE_LOG` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:70`) | client-log-protocol.md, official-mainserver.md |
| 0x23AA210E | `MSGID_REGISTER_WORLDSERVER` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:82`) | game-log-protocol.md, official-mainserver.md, official-worldserver.md, world-log-server.md |
| 0x23AA210F | `MSGID_REGISTER_WORLDSERVERSOCKET` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:83`) | official-mainserver.md, official-worldserver.md, world-log-server.md |
| 0x23AB211F | `MSGID_REGISTER_WORLDSERVER_GAMESERVER` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:84`) | official-mainserver.md, official-worldserver.md, world-log-server.md |
| 0x23AB2200 | `MSGID_REQUEST_CHARINFOLIST` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: mls | - | - | - | - | MLS (`official-worldserver.md:90`) | official-mainserver.md, official-worldserver.md |
| 0x23AB2201 | `MSGID_RESPONSE_CHARINFOLIST` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:85`) | official-mainserver.md |
| 0x2400000A | `MSGID_REQUEST_REMOVEGAMESERVER` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:86`) | game-log-protocol.md, official-mainserver.md, official-worldserver.md, world-log-server.md |
| 0x24021EE0 | `MSGID_REQUEST_CLEARACCOUNTSTATUS` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:87`) | game-log-protocol.md, official-mainserver.md, world-log-server.md |
| 0x27049D0C | `MSGID_WORLDSERVERACTIVATED` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: wls | - | - | - | - | WLS (`official-mainserver.md:88`) | game-log-protocol.md, official-mainserver.md, official-worldserver.md, world-log-server.md |
| 0x280120A0 | `MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: mls | - | - | - | - | MLS (`official-worldserver.md:93`) | game-log-protocol.md, official-worldserver.md, world-log-server.md |
