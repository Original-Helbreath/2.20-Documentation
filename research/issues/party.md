# Issues: party

Scope: defects and divergences in the party system (join handshake, gate party registry, local mirror, exp share) across the 2.20 game server and gate. Split out of `research/systems/party.md`.

### PARTY-001 Party master exists in UI wording only
- **Program**: game server / gate
- **System**: party (join)
- **Kind**: incomplete
- **Intent**: a party master who approves newcomers.
- **Now**: any CONFIRM member can accept a newcomer (S 51542); the gate does not record a master; `m_iPartyRank` is legacy [I]. [V]
- **Where**: S Game.cpp:51542; G PartyManager.h
- **Evidence**: read in source / doc gate 5.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:144

### PARTY-002 /joinparty skips consent and leaves a ghost gate member
- **Program**: game server / gate
- **System**: party (join)
- **Kind**: bug
- **Intent**: join a named player's party.
- **Now**: the target must be a CONFIRM member of a party, else the command returns silently (S 51376-51379); a party-full refusal (gate reply 4, success 0) goes only to the requesting game server, not broadcast (G GateCore.cpp:1209-1221). It sends op 3 for the target's party with no prompt and no master check, but leaves the caller's status at NULL (S 51349, 51365-51389); when the gate's add reply (code 4) returns, `PartyOperationResult_Join` drops it because the caller is not PROCESSING (S 51195), so the gate lists a member the game server never registered (ghost member, counts toward the 8) [I effect]. A leftover of the older command-line party flow. [V] `RequestJoinPartyHandler` needs only that the named player is a confirmed member of some party and sends op 3 with that party id straight to the gate; the accept prompt exists only in `JoinPartyHandler` iV1 == 1 (merged from GSO-035). [V]
- **Where**: S Game.cpp:51349-51389, 51195; G GateCore.cpp:1209-1221; S Game.cpp:51338-51395, 46805-46808
- **Evidence**: read in source; ghost effect inferred.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:146; research/game-server/functions/guild-party.md:241

### PARTY-003 Bounds check in PartyOperationResultHandler case 3 is never true
- **Program**: game server
- **System**: party (gate replies)
- **Kind**: bug
- **Intent**: reject an out-of-range client index.
- **Now**: `(iClientH < 0) && (iClientH > DEF_MAXCLIENTS)` is never true. [V] With the check dead, an out-of-range client id from the gate indexes `m_pClientList` unchecked (merged from GSO-034). [V]
- **Where**: S Game.cpp:51021
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:153; research/game-server/functions/guild-party.md:190

### PARTY-004 Gate party registry weak points

Merged into [GATE-013](gate.md#gate-013-party-id-from-the-wire-is-an-unchecked-array-index), [GATE-014](gate.md#gate-014-baddmember-decrements-the-wrong-party) and [GATE-017](gate.md#gate-017-member-server-index-is-never-updated-on-a-crossing).

### PARTY-005 Dead legacy per-server party path
- **Program**: game server
- **System**: party
- **Kind**: dead code
- **Intent**: v2.06 per-server party.
- **Now**: `CreateNewPartyHandler` S Game.cpp:46728 (call site commented 15439-15441), `CClient::bCreateNewParty` S Client.cpp:317 and `DEF_NOTIFY_RESPONSE_CREATENEWPARTY` are left behind; the per-client `m_iPartyRank`, `m_iPartyMemberCount`, `m_stPartyMemberName[8]` are legacy and not used by the gate path [I]. [V doc] The reply notify `DEF_NOTIFY_RESPONSE_CREATENEWPARTY` has a client handler (C Game.cpp:30829) but no server sender, and its value 0x0A30 is shared with REQUEST_ACCEPTJOINPARTY (S 18236). `CClient::bCreateNewParty` is called only by `CreateNewPartyHandler` (S Client.cpp:317-333) (merged from COMMANDS-002 and GSO-028). [V]
- **Where**: S Game.cpp:46728, 15439-15441; S Client.cpp:317; S Game.cpp:46735-46736, 18236; S Client.cpp:317-333; C Game.cpp:30829
- **Evidence**: doc guild-party.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:157; research/systems/commands/overloaded.md:76, 80; research/game-server/functions/guild-party.md:13, :161

### PARTY-006 Exp bonus factor left unset when every counted member is dead
- **Program**: game server
- **System**: party (exp share)
- **Kind**: bug
- **Intent**: apply the party exp bonus for the living local members.
- **Now**: if every counted member is dead (`iPartyTotalMember == 0`) no bonus case matches and the bonus factor is left unset. [V]
- **Where**: S Game.cpp:51698-51727
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:110

### PARTY-007 Gate member registry is one flat array, capacity 4,999 members
- **Program**: gate
- **System**: party (registry)
- **Kind**: bug / data
- **Intent**: up to 8 members in each of 4,999 parties.
- **Now**: the gate's `m_iMemberNumList[5000]` / `m_stMemberNameList[5000]` use one flat member-name array shared by all parties, so total capacity is 4,999 members, not 8 per party x 4,999; an add into a full registry is rejected silently. [V]
- **Where**: G PartyManager.h:20, 37-43; PartyManager.cpp:114-140
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/party.md:79
