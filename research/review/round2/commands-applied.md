# Round 2 commands review: findings applied

Each finding was checked against source first (S = hgserver-librehelbreath/src/Game.cpp, C = client-2.20/Game.cpp, G = gateserver GateCore.cpp). Generated files were fixed in the generator scripts (`.claude/scratch/commands/annot.py, annot_notify.py, slash.py, gen.py, gen_bysys.py`, patched by `.claude/scratch/fix-cmd/fix_scripts*.py`) and regenerated; the regenerate-before-edit run reproduced the old docs byte for byte, so nothing hand-made was lost. Hand-written files (overloaded.md, method.md) edited directly. Result: 19 applied, 0 rejected.

| id | result | where | reason |
|---|---|---|---|
| F1 | applied | annot.py GIVEITEMTOCHAR -> common-types.md:13; overloaded.md:27 | S 17094, 17410-17440, 17598-17624, 17314/17576 re-read: branches (a)-(f), ids 88/89, Howard/Kennedy confirmed |
| F2 | applied | overloaded.md:17 | C 19070/19089/19127 send JOINPARTY/ACCEPTJOINPARTY; S 46749-46755 case 0 = RequestDeletePartyHandler |
| F3 | applied | annot_notify.py PLAYERNOTONGAME -> notify-types.md; overloaded.md:33 | S 34695 send is commented; /fi dropped from lists |
| F4 | applied | annot.py ITEMLOG -> msgids.md:109 (DEAD); overloaded.md dead table | S 20654-20657 only a packing case, comment "not used", no other use |
| F5 | applied | annot.py -> msgids.md:122 (DEAD); overloaded.md dead table | S 4281-4282 shared case label only |
| F6 | applied | annot_notify.py -> notify-types.md (DEAD); overloaded.md dead table | S 15440 call commented, 46728/46735 sender only |
| F7 | applied | gen.py DIR_OVERRIDE + fallbacks -> common-types.md, motion.md, notify-types.md | fallback `C->L`/`L->C` replaced: COMMONTYPE -> C->S, NOTIFY/OBJECT -> S->C; OBJECTDEAD = local; alias pairs share Dir |
| F8 | applied | gen.py -> gate-relay.md:3 | G 287-289, 1017-1031: `i != iClientH` when bIsOwnSend FALSE |
| F9 | applied | overloaded.md dead table | SKILLTRAINFAIL split into own row (S 25443-25525); range 17715-19025 verified, default at 17800 is inner |
| F10 | applied | slash.py `~` -> chat-commands.md:18; overloaded.md straddle row | S 10724-10741, 11146-11149: only m_cSide compared, no crusade test |
| F11 | applied (decision) | annot.py new system `envelope` | MSGID_COMMAND_COMMON and MSGID_NOTIFY moved out of session (session 36 -> 30 primary). CHATMSG and MOTION stay with their dominant domain; EVENT_COMMON and SERVERSTOCKMSG stay (one family each, domain-bound) |
| F12 | applied (decision) | annot.py/annot_notify.py/slash.py new system `gathering` | 5 fishing + 3 agriculture notifies, GETFISHTHISTIME, /createfish secondary moved; crafting = alchemy/build/upgrade. No mineral rows exist, so none moved |
| F13 | applied (decision) | annot.py/annot_notify.py | RETRIEVEITEM, RESPONSE_RETRIEVEITEM, ITEMTOBANK, CANNOTITEMTOBANK -> primary `item`, secondary `shop`; no separate bank system (4 rows, deposit rides GIVEITEMTOCHAR); scopes updated |
| F14 | applied (decision) | annot.py | REGISTERDBSERVER req/resp and NOCOUNTINGSAVELOGOUT (all defined-only, grep verified) -> account; REQUEST_RESTART -> character, secondary combat |
| F15 | applied (decision) | gen.py REG, gen_bysys.py, method.md | the 8 prefix modes are now registered and placed; total is 432 = 371 + 45 + 8 + 8, other-subtypes (36) stated as unplaced |
| F16 | applied | overloaded.md collisions | S NetMessages.h:4-5, 293-294, 309; C 278-279, 296-302 verified; 0x0A03 also collides with dead REQ_LISTCONTENTS |
| F17 | applied | gen.py DIR_OVERRIDE -> msgids.md:104-107 | X->S |
| F18 | applied | gen.py, method.md | C 34121 is the function end (34137 is not); canonical tree stated. Identical line numbers in client-2.20-helbreathorg is the reviewer's claim, not re-run (unverified here) |
| F19 | applied | slash.py -> chat-commands.md:14-18 | S 10595-10599 etc. SP deduct under FirmStaminar == 0; S 11098-11128 crusade side filter on mode 1 (@ and ^) |

Also applied: NOTIFY_HELP/HELPFAILED marked unreachable (reviewer's side note).
