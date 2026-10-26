# Issues: chat

Scope: defects and divergences in chat (prefix modes, local fan-out, whisper, cross-server relay, mute, curse filter, notices) in the 2.20 client, game server and gate relay. Split out of `research/systems/chat.md`.

### CHAT-001 Language filter is client-side only
- **Program**: client / game server
- **System**: chat (moderation)
- **Kind**: client-only check
- **Intent**: Curse.txt filtering of chat; evident intent is that enforcement is the external BWM monitor plus GM mute [I].
- **Now**: Curse.txt rewrites outgoing text in the sender's client (C Game.cpp:35063); the server's `bCheckBadWord` is a stub. A modified client bypasses it. [V]
- **Where**: C Game.cpp:35063; S Game.cpp:36035-36047
- **Evidence**: read in source / doc chat-gm.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:164; research/game-server/functions/chat-gm.md:64 (merged from GSO-016)

### CHAT-002 A curse match squashes the whole line
- **Program**: client
- **System**: chat (curse filter)
- **Kind**: bug
- **Intent**: replace the bad word.
- **Now**: `ConvertString` rebuilds the result from `filterChar(str, "\t _-.:^")` (C Curse.cpp:147-158), so a sentence with one bad word arrives with all spaces and punctuation stripped. `^` is in the strip set, so a `^` guild-relay line that contains a curse loses its prefix and becomes local spoken chat [I from code]. [V]
- **Where**: C Curse.cpp:147-158
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:167

### CHAT-003 LoadCurse passes file text as a printf format
- **Program**: client
- **System**: chat (curse filter)
- **Kind**: bug
- **Intent**: copy a Curse.txt line into a buffer.
- **Now**: `LoadCurse` calls `sprintf(ptr, buf)` with file text as the format (C Curse.cpp:47); a `%` in Curse.txt corrupts the list [I]. [V]
- **Where**: C Curse.cpp:47
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:170

### CHAT-004 Guild relay (^) is sent before the level and SP gate
- **Program**: game server
- **System**: chat (guild relay)
- **Kind**: bug
- **Intent**: the level/SP gate decides who may use the channel (shouting is a resource decision, new characters cannot spam).
- **Now**: the GSM records go out before the level/SP gate, so a level-2 guild member with no SP still reaches guildmates on other servers; when the local gate fails the line is then spoken as ordinary area chat (mode 0), not refused. The SP check is 5 but the charge is 3. The same downgrade applies to `@`, `$`, `~` and `!` (a documented rule). [V]
- **Where**: S Game.cpp:10656-10717, 10703-10706
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:172

### CHAT-005 World shout (GSM type 2) has a receiver but no sender
- **Program**: game server
- **System**: chat (relay)
- **Kind**: incomplete
- **Intent**: a global message across servers.
- **Now**: `!` never relays; `ChatMsgHandlerGSM` handles type 2 ("global message") but no sender builds a type-2 record - a half-finished world shout. [V]
- **Where**: S Game.cpp:11274, 10654-10700
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:176

### CHAT-006 Remote guild delivery stops at the first uninitialised client
- **Program**: game server
- **System**: chat (relay)
- **Kind**: bug
- **Intent**: deliver to every guild member on the receiving server.
- **Now**: in `ChatMsgHandlerGSM` type 1, `break` on the first not-yet-initialised client ends the whole loop (S 11266); type 2/10 sends even to uninitialised sockets (11276-11279). [V]
- **Where**: S Game.cpp:11266, 11276-11279
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:178; research/game-server/functions/chat-gm.md:26 (merged from GSO-014)

### CHAT-007 Whisper target lookup has no failure reply
- **Program**: game server
- **System**: chat (whisper)
- **Kind**: bug
- **Intent**: tell the user when the target is not found.
- **Now**: if no server answers, `m_bIsCheckingWhisperPlayer` stays TRUE and later plain lines are mode 0, silently public (S 34797; 11071 needs index != -1) [I effect]. [V]
- **Where**: S Game.cpp:34797, 11071
- **Evidence**: read in source; effect inferred.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:180

### CHAT-008 GSM_DISCONNECT clears only the first whisperer
- **Program**: game server
- **System**: chat (whisper)
- **Kind**: bug
- **Intent**: clear every whisper target that points at the logged-out name.
- **Now**: `goto SSMH_LOOPBREAK` after one match (S 49360-49364); other players keep index 10000 to a logged-out name and their lines go to the bus unanswered. [V]
- **Where**: S Game.cpp:49353-49366
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:182

### CHAT-009 Mute units disagree between /shutup and the BWM monitor
- **Program**: game server
- **System**: chat (moderation)
- **Kind**: bug
- **Intent**: BWM mute of 10 minutes (code comment).
- **Now**: `/shutup` uses minutes x 20 ticks; BWM uses 600 ticks and its notify value; at 20 ticks per minute that is 30 minutes while the comment on the same line says 10. [V] Detail: `_BWM_Command_Shutup` sets `m_iTimeLeft_ShutUp = 20*3*10 = 600`, decremented by 1 per 3 s check, about 30 minutes (derived, not run); `ShutUpPlayer` notifies minutes (S 36132) while the BWM path notifies 600, a unit mismatch (client display unverified) (merged from ADMIN-008 and GSO-019). [V]
- **Where**: S Game.cpp:40710, 36131; S Game.cpp:40697-40720, 36132, 3872
- **Evidence**: read in source / doc chat-gm.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:184; systems/admin.md section 9; research/game-server/functions/chat-gm.md:210

### CHAT-010 Crusade chat secrecy is partial
- **Program**: game server
- **System**: chat (crusade filter)
- **Kind**: incomplete
- **Intent**: during a crusade enemies cannot hear you (war secrecy).
- **Now**: `~`, `$` and whispers ignore the side filter, and remote `^` lines are not filtered on the receiving server. [V]
- **Where**: S Game.cpp:11146-11155, 11262-11271
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:186

### CHAT-011 Confuse garble writes double-byte text over English text
- **Program**: game server
- **System**: chat (magic confuse)
- **Kind**: data
- **Intent**: garble the speaker's text under the confuse-language spell.
- **Now**: the garble writes Korean double bytes (cp949 in the original; UTF-8 bytes in this export, 3 bytes copied as 2) over English text [I display]. [V]
- **Where**: S Game.cpp:11059-11061
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/chat.md:188
