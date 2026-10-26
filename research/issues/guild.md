# Issues: guild

Scope: defects and divergences in guild creation, admission, departure, disband, the guild file and guild notifications across the 2.20 game server and world log server. Split out of `research/systems/guild.md`.

### GUILD-001 Join and dismiss approval have no server-side authority check
- **Program**: game server
- **System**: guild (admission)
- **Kind**: client-only check / security
- **Intent**: only a guild master can approve a pending applicant.
- **Now**: `JoinGuildApproveHandler` does not check that the caller is a master, that a request is pending, or that the target is guildless; it only needs the same side (S 19050-19066). A crafted packet can pull any online same-side player (even another guild's member) into the caller's guild without consent; a guildless caller would stamp "NONE"/GUID -1/rank 12 on the target. Likewise `DismissGuildApproveHandler` removes any named player [V doc]. The ticket handshake exists only in the client. [V] Join approval copies the caller's guild name, GUID and location onto the named online player and sets rank DEF_GUILDSTARTRANK 12 (S 19053-19066, 19039-19090); dismiss approval removes the named online player from their guild with no check that they belong to the caller's guild (S 19124-19142) (merged from GSO-029 and GSO-030). [V]
- **Where**: S Game.cpp:19050-19066, 19113-19175; S Game.cpp:19039-19090, 19124-19142
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:157; research/game-server/functions/guild-party.md:67, :90

### GUILD-002 Kennedy leave does not update the guild file
- **Program**: game server / world log
- **System**: guild (departure)
- **Kind**: bug
- **Intent**: a member leaving by ticket 89 at Kennedy is removed from the guild file (as the master path sends `DELGUILDSMAN`).
- **Now**: no `DELGUILDSMAN` is sent (S 17610-17623); the file keeps a stale member line. Harmless for login validation (which compares GUID only) but the member list drifts. [V]
- **Where**: S Game.cpp:17610-17623
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:162

### GUILD-003 Disband and guild events reach only this server
- **Program**: game server
- **System**: guild (disband, notifications)
- **Kind**: incomplete
- **Intent**: a disbanded guild is cleared for every member on every server.
- **Now**: `SendGuildMsg` is local and `GuildNotifyHandler` (MSGID_GUILDNOTIFY, received and ignored) is unimplemented, so members on other servers keep the guild until relog [I effect]; their nearby observers are not sent `CLEARGUILDNAME` either (only the master's position is broadcast, S 16779). [V]
- **Where**: S Game.cpp:20497-20514, 16779
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:164

### GUILD-004 Master can /ban himself
- **Program**: game server
- **System**: guild (departure)
- **Kind**: bug
- **Intent**: the master bans members.
- **Now**: the same-guild-name test passes for the master (S 36755-36779), leaving a guild file whose master is guildless [I]. [V]
- **Where**: S Game.cpp:36755-36779
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:167

### GUILD-005 Create response is silent when the gate fails, and the gold is deducted without a re-check
- **Program**: game server
- **System**: guild (creation)
- **Kind**: bug
- **Intent**: a completed Log reply creates the guild, charges gold and answers the client.
- **Now**: the requested name is written to `m_cGuildName` with rank -1 before the Log answers (S 16704); a reject resets it. `ResponseCreateNewGuildHandler` acts only for a client that still has level >= 100 (>= 20 without the Chinese limit) and Charisma >= 20; otherwise no branch runs and no reply is sent, although the Log server has already created the guild file, and the stored name survives indefinitely if no answer comes. The deduction is `dwGoldCount - 1000000` with no re-check of the gold at this point. [V]
- **Where**: S Game.cpp:16563-16586, 16593-16601, 16704
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:103-107, 169

### GUILD-006 Weak guild GUID
- **Program**: game server
- **System**: guild (creation)
- **Kind**: data
- **Intent**: a unique guild id.
- **Now**: sum of the date fields plus `timeGetTime()` (S 16711); collisions across servers are possible in principle [I]. [V]
- **Where**: S Game.cpp:16710-16711
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:173

### GUILD-007 Dead or unwired guild features
- **Program**: game server
- **System**: guild
- **Kind**: dead code / incomplete
- **Intent**: dismiss command, guild co-op exp share, join cap notices, ban success notice.
- **Now**: `/dissmiss` is empty (`UserCommand_DissmissGuild`); `CalculateGuildEffect` (guild exp share) is disabled by an early return; `CANNOTJOINMOREGUILDSMAN`, `WAITFORGUILDOPERATION` and `SUCCESSBANGUILDMAN` are never sent (no SendNotifyMsg case) and `EVENTMSGSTRING` has no caller; `CGuildsMan` is unused. [V doc] Detail: `CalculateGuildEffect` returns early at a "### ERROR POINT, temporarily blocked" comment (the co-op share was 1d3 == 2 giving a third of the monster exp). `BANGUILD` and `SUCCESSBANGUILDMAN` are both passed to SendNotifyMsg (S 36782, 36785), which has no case for them and no default in the main switch 17715-19025 (the `default:` at 17800 belongs to an inner switch), so nothing is sent. `GuildNotifyHandler` is not implemented (see GUILD-003). Merged from COMMANDS-005, COMMANDS-014 (`/dissmiss` part), GSO-031 and GSO-033. [V]
- **Where**: S Game.cpp:36804-36807, 20249-20295; S GuildsMan.h; S Game.cpp:36782, 36785, 17715-19025, 20497-20514
- **Evidence**: doc notify-types.md, guild-party.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:177; research/systems/commands/overloaded.md:79, 91; research/game-server/functions/guild-party.md:113, :133, :154

### GUILD-008 No guild member cap
- **Program**: game server
- **System**: guild (admission)
- **Kind**: incomplete
- **Intent**: `DEF_MAXGUILDSMAN` 128 limits members.
- **Now**: the constant is defined (S Client.h:27) and never referenced in .cpp, so there is no member cap and `CANNOTJOINMOREGUILDSMAN` is never sent. [V grep] The `CANNOTJOINMOREGUILDSMAN` notify has a packing case (S 18948) and a client case but no server call sends it (merged from COMMANDS-009). [V]
- **Where**: S Client.h:27; S Game.cpp:18948
- **Evidence**: grep.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:93, 138, 178; research/systems/commands/overloaded.md:85

### GUILD-009 Silent failures on create and disband
- **Program**: game server / client
- **System**: guild
- **Kind**: bug
- **Intent**: tell the player why a request failed.
- **Now**: already-in-guild create and non-master disband only log on the server; the client waits. [V]
- **Where**: S Game.cpp:16661-16665, 16736-16739
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/guild.md:180
