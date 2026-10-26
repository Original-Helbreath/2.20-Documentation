# Issues: admin

Defects and divergences in GM/admin tooling, the monitor and manager links, the anti-hack checks and item logging. Behaviour lives in [systems/admin.md](../systems/admin.md). The dead `_Manager_Shutdown` is recorded in [TOPO-007](server-topology.md#topo-007-manager-shutdown-is-dead).

### ADMIN-001 Monitor messages carry no login check

- **Program**: game server
- **System**: admin
- **Kind**: security
- **Intent**: only the bad-word monitor program may mute a player.
- **Now**: `MSGID_BWM_COMMAND_SHUTUP` is dispatched for any client socket and `_BWM_Command_Shutup` never checks `m_bIsBWMonitor` (written only at `40688-40695`, read nowhere). It applies to any player name (no admin-level test, so a GM can be muted; no length check) for about 30 min. The only precondition is a socket that passed `bAccept` [V].
- **Where**: `SRV Game.cpp:14964, 14986-14988, 40697-40720`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-002 Manager password is a shared literal; _Manager_Init is repeatable

- **Program**: game server
- **System**: admin
- **Kind**: security
- **Intent**: authenticate the manager tool.
- **Now**: the password is the literal `"?BWordMtr?"`, and `_Manager_Init` can be repeated, decrementing `m_iTotalClients` each time [V].
- **Where**: `SRV Game.cpp:40739, 48987, 40741-40742`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-003 GM enable passwords are compiled-in literals

- **Program**: game server
- **System**: admin
- **Kind**: security
- **Intent**: restrict GM commands to authorised players.
- **Now**: the literals `0909114` and `147258` are matched in the chat dispatcher, the same for every GM and server [V]. The functions the passwords unlock (`/enableadmincreateitem`, `/enableadmincommand`) only check the admin level afterwards: create-item at least 3 (4 under DEF_TAIWAN), command at least 1 (S 46323, 46341) (merged from GSO-004). [V]
- **Where**: `SRV Game.cpp:10932, 10937`; S Game.cpp:46323, 46341
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9; research/game-server/mechanics.md:220

### ADMIN-004 Frequency checks use the client clock and run after the action

- **Program**: game server
- **System**: admin
- **Kind**: security
- **Intent**: reject speed hacks before they take effect.
- **Now**: the gap checks use the time the client put in the motion message and are called after the move/attack was applied. They act inside the function: on a gap below 250/500/1500 ms each calls `DeleteClient(i, TRUE, TRUE)` itself, and the return value (always FALSE) is unused. The 250 ms move check has two exemptions (`m_bIsMoveBlocked`, `m_bIsAttackModeChange`). The batch checks use the server clock [V].
- **Where**: `SRV Game.cpp:1047, 1058, 1082, 1101, 52615-52713`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-005 Magic speed check never wired

- **Program**: game server
- **System**: admin
- **Kind**: dead code
- **Intent**: limit cast frequency.
- **Now**: `bCheckClientMagicFrequency` has no caller [V doc].
- **Where**: `bCheckClientMagicFrequency`
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-006 Clock-skew check only catches a fast clock

- **Program**: game server
- **System**: admin
- **Kind**: bug
- **Intent**: detect a client clock that drifts from the server in either direction, and ban.
- **Now**: it compares elapsed time since the first CHECKCONNECTION on both clocks and fires only when the client's elapsed time exceeds the server's by >= `DEF_CLIENTTIMEOUT` (10000 ms). If the client's elapsed time is lower it returns early; the year-long ban that used to follow is commented out [V].
- **Where**: `SRV Game.cpp:48946-48962`, `48958`, `48965-48972`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-007 IP restriction on /createitem removed

- **Program**: game server
- **System**: admin
- **Kind**: security
- **Intent**: allow /createitem only from the office subnet.
- **Now**: the `203.234.215.` subnet check is commented out [V].
- **Where**: `SRV Game.cpp:46382`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9

### ADMIN-008 Shut-up units disagree

Merged into [CHAT-009](chat.md#chat-009-mute-units-disagree-between-shutup-and-the-bwm-monitor).

### ADMIN-009 Item-log coverage is narrow in this build

Merged into [ITEM-007](item.md#item-007-dead-taiwanlog-item-log-pair-the-shipped-pair-logs-only-good-items).

### ADMIN-010 RequestAdminUserMode is empty

- **Program**: game server
- **System**: admin
- **Kind**: incomplete
- **Intent**: a date-derived admin check.
- **Now**: the function is empty; the date-derived check never existed in this build [V doc]. The body is two comment lines only and checks nothing (kind: security, since any caller is accepted); it is dispatched from MSGID_ADMINUSER (S 15009-15011) and the message has no sender (merged from GSO-015 and COMMANDS-012). [V]
- **Where**: `research/game-server/functions/chat-gm.md`; S Game.cpp:35831-35836, 15009-15011
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9; research/game-server/functions/chat-gm.md:56; research/systems/commands/overloaded.md:88

### ADMIN-011 /polymorph parses and does nothing

- **Program**: game server
- **System**: admin
- **Kind**: incomplete
- **Intent**: polymorph a target.
- **Now**: the command parses and does nothing (comment: not implemented yet) [V doc]. Detail: S 38119-38148 parses the argument and ignores it (38143-38145); the source comment says it is not implemented until the polymorph routine is finished (merged from GSO-018 and COMMANDS-014 (`/polymorph` part)). [V]
- **Where**: `research/game-server/functions/chat-gm.md`; SRV Game.cpp:38119-38148
- **Evidence**: [V doc]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9; research/game-server/functions/chat-gm.md:172; research/systems/commands/overloaded.md:92; chat-commands.md:50

### ADMIN-012 /shutdownthisserverrightnow writes no GM log

- **Program**: game server
- **System**: admin
- **Kind**: bug
- **Intent**: every GM order is logged.
- **Now**: this one is not [V].
- **Where**: `SRV Game.cpp:11011-11027`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/admin.md section 9
