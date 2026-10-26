# Messages: Admin/DB tool <-> Main Log server (official only)

Generated from the NetMessages.h files by `.claude/scratch/shared-ref/gen_messages.py`; conventions in `README.md`. Sender -> receiver: admin tool -> main log (and reverse for responses; per-row direction is in the name: REQUEST/COMMAND = toward the server side, RESPONSE/NOTIFY/EVENT = back). 
Byte layouts: `../../servers/official-mainserver.md` (not repeated here).

| Hex | Name | Parent | Ver | Link basis (inferred) | Client 2.20 | Game 2.20 | Game 2.03 | Gate 2.19 | Official | Docs |
|---|---|---|---|---|---|---|---|---|---|---|
| 0x0512A3F6 | `MSGID_REQUEST_REGISTERDBSERVER` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin/db tool | - | - | - | - | admin/DB tool (`official-mainserver.md:75`) | game-log-protocol.md, official-mainserver.md |
| 0x13000001 | `MSGID_ACCOUNTINFOCHANGE` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin, ip-checked | - | - | - | - | admin, IP-checked (`official-mainserver.md:76`) | game-log-protocol.md, official-mainserver.md, world-log-server.md |
| 0x14E91202 | `MSGID_REQUEST_ACCOUNTCONNSTATUS` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin, ip-checked | - | - | - | - | admin, IP-checked (`official-mainserver.md:77`) | official-mainserver.md |
| 0x14E91204 | `MSGID_REQUEST_CLEARACCOUNTCONNSTATUS` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin, ip-checked | - | - | - | - | admin, IP-checked (`official-mainserver.md:78`) | official-mainserver.md |
| 0x2900AD10 | `MSGID_REQUEST_BLOCKACCOUNT` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin, ip-checked | - | - | - | - | admin, IP-checked (`official-mainserver.md:79`) | game-log-protocol.md, official-mainserver.md |
| 0x2900AD22 | `MSGID_ACCOUNTTIMECHANGE` | - | 2.03 2.19g 2.20c 2.20s off | official table sender: admin | - | - | - | - | admin (`official-mainserver.md:80`) | game-log-protocol.md, official-mainserver.md |
