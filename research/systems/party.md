# System: party

*Verdict (size review): one cohesive system (join handshake, gate registry, local mirror and exp share are one loop); single file with a Contents block.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`, `G` = `sources/gateserver-librehelbreath/`.
Tags: [V] read in source at the cited line, [I] inferred, [U] unverified. Function bodies:
`research/game-server/functions/guild-party.md`; gate side: `research/gate/subsystems.md` section 5, `research/gate/functions.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules - 8. Couplings -
9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

A party is a small (up to 8) hunting group that shares experience, cannot hurt each other and has its own chat channel
(`$`). It must survive members walking through map gates onto other game servers, so the authoritative member list
lives in the **gate**, the only process every game server shares; each game server keeps a local mirror of its own
members for exp sharing [V S Game.cpp:50929-50968, 51107-51224; G PartyManager.h:20-43]. The player experience: click a
player with the party cursor to ask to join, the other side gets a yes/no box; the party stays across server changes and
even across logout (party id saved in the character file) unless the gate forgot it [V S Game.cpp:9255, 4994-5011].

## 2. Subsystems

| # | Subsystem | Intent |
|---|---|---|
| 2.1 | Join handshake (client UI + server) | requester asks a target; target accepts or rejects; implicit party creation if the target has none |
| 2.2 | Gate party registry | ids 1..4999, member names, 8 per party, broadcast add/remove/disband to all servers |
| 2.3 | Local party mirror | `m_stPartyInfo[id]` holds the client indexes of members on this server |
| 2.4 | Server-change continuity | stamp member on leave (op 7), confirm on arrival (op 5), 20 s timeout removes |
| 2.5 | Party benefits | exp split with bonus, no friendly fire, `$` chat |
| 2.6 | Leave / disband / info | slash commands and dialog; citizenship change forces leave |

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `CGame::PointCommandHandler` | client | C Game.cpp:34768-34800 | party cursor click sends `COMMONTYPE_REQUEST_JOINPARTY` [V doc client/functions/chat.md] |
| 2.1 | `CGame::DlgBoxClick_Party`, `DrawDialogBox_Party` | client | C Game.cpp:19043-19138, 39971-40199 | dialog 32: accept/reject/leave; cannot be closed by right click until the server answers [V doc] |
| 2.1 | `NotifyMsgHandler` cases `DEF_NOTIFY_PARTY`, `QUERY_JOINPARTY` | client | C Game.cpp:30207, 30820-30827 | open dialog 32 in the right mode [V] |
| 2.1 | `CGame::JoinPartyHandler` | game server | S Game.cpp:46739-46843 | iV1 0 leave, 1 ask to join (local target only), 2 info [V doc + V 46749-46775] |
| 2.1 | `CGame::RequestAcceptJoinPartyHandler` | game server | S Game.cpp:51498-51634 | target answers: 0 reject, 1 accept (create first if needed), 2 cancel [V] |
| 2.1 | `CGame::RequestJoinPartyHandler` | game server | S Game.cpp:51338-51395 | `/joinparty name`: op 3 straight to the gate, no accept prompt [V doc] |
| 2.2 | `CGame::RequestCreatePartyHandler` | game server | S Game.cpp:50929-50968 | op 1 create [V doc] |
| 2.2 | `CGateCore::PartyOperation` | gate | G GateCore.cpp:1110-1273 | op switch 1..7, replies and broadcasts [V doc gate] |
| 2.2 | `PartyManager::iCreateNewParty / bAddMember / bRemoveMember / bDeleteParty / bGetPartyInfo / bCheckPartyMember` | gate | G PartyManager.cpp:35-250 | registry [V signatures] |
| 2.3 | `CGame::PartyOperationResultHandler` + `_Create/_Join/_Dismiss/_Delete/_Info` | game server | S Game.cpp:50972-51466 | apply gate replies, maintain `m_stPartyInfo`, notify clients [V] |
| 2.4 | `CGame::DeleteClient` party block | game server | S Game.cpp:2775-2873 | logout -> op 4 remove; server change -> op 7 stamp; drop from local mirror [V] |
| 2.4 | `CGame::RequestInitDataHandler` party block | game server | S Game.cpp:4993-5020 | arrival -> op 5 check, re-add to local mirror [V] |
| 2.4 | `PartyManager::SetServerChangeStatus / CheckMemberActivity / GameServerDown` | gate | G PartyManager.cpp:250-316 (`GameServerDown` 250, `SetServerChangeStatus` 268, `CheckMemberActivity` 279-316) | stamp, 20 s timeout, dead-server cleanup [V doc gate] |
| 2.5 | `CGame::GetExp` party branch | game server | S Game.cpp:51678-51768 | exp split [V] |
| 2.5 | `iCalculateAttackEffect` and magic effect party checks | game server | S Game.cpp:12606, 31219, 31770, 32228 | no damage to own party [V] |
| 2.5 | `ChatMsgHandler` mode 4 | game server | S Game.cpp:10589-10600, 11152-11155 | `$` chat [V] |
| 2.6 | `RequestDismissPartyHandler`, `RequestDeletePartyHandler`, `GetPartyInfoHandler` | game server | S Game.cpp:51399-51496 | op 4 / info [V doc] |
| 2.6 | `RequestCivilRightHandler` | game server | S Game.cpp:27608 | gaining citizenship leaves the party [V] |

Dead: `CGame::CreateNewPartyHandler` S Game.cpp:46728 (call site commented 15439-15441), `CClient::bCreateNewParty`
S Client.cpp:317 [V doc] ([PARTY-005](../issues/party.md#party-005-dead-legacy-per-server-party-path)).

## 4. Commands and messages

- Client -> server: `MSGID_COMMAND_COMMON` with `DEF_COMMONTYPE_REQUEST_JOINPARTY` (iV1 0/1/2, name) [V S Game.cpp:15435-15437]
  and `DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY` (iV1 0/1/2) [V S 15394-15397]. Slash: `/createparty`, `/joinparty`,
  `/dismissparty`, `/getpartyinfo`, `/deleteparty` (S Game.cpp:10986-11010) [V doc chat-commands.md]. Chat prefix `$`.
- Server -> client: `DEF_NOTIFY_PARTY` (sub-codes: 1 create, 4 join, 6 dismiss, 2 disband, 5 info, 7 join refused)
  [V S 51125, 51188, 46763], `DEF_NOTIFY_QUERY_JOINPARTY`, `DEF_NOTIFY_PLAYERNOTONGAME`.
- Game <-> gate: `MSGID_PARTYOPERATION` request types 1 create, 3 add, 4 remove, 5 check, 6 info, 7 server-change stamp;
  reply codes 1 create, 2 disband, 3 not a member, 4 add, 5 info, 6 remove; add/remove/disband/timeout replies are
  broadcast to every server [V doc gate/subsystems.md 5].

## 5. Data

| Field | Where | Meaning |
|---|---|---|
| `m_iPartyID` | S Client.h:339 | gate party id, 0 = none; saved as `party-id` [V S Game.cpp:9255, 8571] |
| `m_iPartyStatus` | S Client.h:340, 39-41 | NULL 0 / PROCESSING 1 / CONFIRM 2; set CONFIRM on load if an id is present [V S 8572] |
| `m_iReqJoinPartyClientH`, `m_cReqJoinPartyName` | S Client.h | pending handshake partner (both sides point at each other) [V S 51522] |
| `m_iPartyRank`, `m_iPartyMemberCount`, `m_stPartyMemberName[8]` | S Client.h:202-209 | legacy per-client party (v2.06 and before); not used by the gate path [I] |
| `m_stPartyInfo[5000] {iTotalMembers, iIndex[8]}` | S Game.h:856-859 | local mirror of members on this server [V] |
| gate `m_iMemberNumList[5000]`, `m_stMemberNameList[5000]` | G PartyManager.h:37-43 | authoritative registry, memory only; one flat member-name array shared by all parties, so total capacity is 4,999 members, not 8 per party x 4,999; an add into a full registry is rejected silently ([PARTY-007](../issues/party.md#party-007-gate-member-registry-is-one-flat-array-capacity-4999-members)) [V G PartyManager.h:20, 37-43; PartyManager.cpp:114-140] |
| `DEF_MAXPARTYMEMBERS` 8 | S Game.h:203, Client.h:35; G PartyManager.h:21 (`DEF_MAXPARTYMEMBER`) | cap [V] |

## 6. Flow

**F1 Ask, accept, create.** A clicks B with the party cursor -> `REQUEST_JOINPARTY iV1=1, name B` -> `JoinPartyHandler`:
reject (NOTIFY_PARTY 7) if A already has a party or a pending status [V S 46761-46771]; find B on this server; reject if B
is in combat appearance, of another side, or itself PROCESSING [V doc 46777-46796]; else A = PROCESSING, both handshake
fields cross-linked, B gets `QUERY_JOINPARTY` with A's name [V doc 46805-46808] -> B's client opens dialog 32 mode 1
[V C 30820-30827] -> B clicks yes -> `REQUEST_ACCEPTJOINPARTY 1` -> `RequestAcceptJoinPartyHandler` validates the
cross-link and A still PROCESSING [V S 51543-51559]:
- B already CONFIRM in a party -> op 3 add A to B's party id [V 51562-51578];
- B has no party -> `RequestCreatePartyHandler(B)` op 1 [V 51599-51601] -> gate creates id -> reply code 1 ->
  `PartyOperationResult_Create`: B = CONFIRM, mirror add, NOTIFY_PARTY 1,1, then op 3 add A [V 51128-51170].
-> gate adds A, broadcasts code 4 to all servers -> on A's server `PartyOperationResult_Join`: A = CONFIRM, mirror add,
NOTIFY_PARTY 4,1 to A and to every local member [V 51194-51221].

**F2 Reject.** B clicks no -> `ACCEPTJOINPARTY 0` -> A gets NOTIFY_PARTY 7, A's status and links cleared [V S 51508-51539].

**F3 Server change.** A walks into a gate to another server -> `DeleteClient` with `m_bIsOnServerChange` -> op 7 stamp
[V S 2830-2850], local mirror slot removed and compacted [V 2856-2873] -> new server `RequestInitDataHandler` -> op 5
check + mirror re-add [V 4993-5020] -> gate clears the stamp; if 20 s pass without op 5, the gate broadcasts remove
(code 6) and a party left with one member is disbanded (code 2) [V doc gate 5].

**F4 Logout / leave.** Logout (save or no-save, not a server change) -> op 4 remove [V S 2779-2826]. `/dismissparty`,
dialog leave, or citizenship gain -> op 4 [V 27608].

**F5 Exp.** Monster exp reaches `GetExp(attacker)` -> if in a CONFIRM party, the attacker acted within 5 min and the gain
is >= 10 and the local mirror has > 1 member: count living local members N, unit = exp x (1 + bonus(N)) / N, every living
local member gets the unit into ExpStock, and the attacker also gets unit/10 when the call is for the attacker's own hit
and N > 1 [V S 51678-51761]. When the gain is under 10, or the mirror has a single member, the attacker alone gets the full
`iExp` with no unit/10 bonus and nobody else is paid [V 51767-51771]; if every counted member is dead
(`iPartyTotalMember == 0`) no bonus case matches ([PARTY-006](../issues/party.md#party-006-exp-bonus-factor-left-unset-when-every-counted-member-is-dead)).

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| members per party | 8 (gate cap) | G PartyManager.h:21 [V] |
| party ids | 1..4999 | doc gate 5 [V doc] |
| join needs | same side, target not in combat appearance, neither side pending | S 46761-46796 [V doc] |
| exp bonus (DEF_V219 on) | N=2 +2%, 3 +5%, 4 +7%, 5 +10%, 6 +14%, 7 +17%, 8 +20% of the gain, then / N | S 51708-51716 [V]; GlobalDef.h:24 defines DEF_V219 [V] |
| exp bonus (DEF_V219 off) | +10% .. +30% | S 51717-51725 [V] |
| exp share gates | attacker `m_dwLastActionTime` < 5 min; gain >= 10; recipients alive (HP > 0); no distance or map check | S 51678-51688 [V] |
| attacker extra | unit/10 when `bIsAttackerOwn` and N > 1 | S 51760-51761 [V] |
| friendly fire | party members cannot damage each other (melee and magic) | S 12606, 31219, 31770, 32228 [V] |
| `$` chat | SP 3, any level | S 10589-10600 [V] |
| crossing timeout | 20 s | doc gate 5 [V doc] |

## 8. Couplings

- **combat / character (other cluster)** - `GetExp` owns the party exp split; `iCalculateAttackEffect` and magic effects
  read `m_iPartyID` for friendly-fire immunity (coupling field `m_iPartyID`).
- **chat** - mode 4 delivery and `$` prefix; slash commands parsed in `ChatMsgHandler`.
- **session / persistence (infrastructure)** - `DeleteClient` and `RequestInitDataHandler` drive ops 4/5/7; `party-id`
  saved in the character file by the world log.
- **gate (infrastructure)** - `PartyOperation`, `PartyManager`; party state is the only gameplay state in the gate.
- **pk-law / citizenship** - `RequestCivilRightHandler` forces leave (side change would break the same-side rule).
- **client presentation** - dialog 32 and party cursor.

## 9. Current behaviour vs intent

Defects are in [issues/party.md](../issues/party.md); the neutral lines are:

1. **Exp share is per server, not per party.** The split counts only members in the local mirror, with no map or
   distance check: a member on another map of the same server shares, a member on another server gets nothing even
   though the gate says they are in the party [V S 51686-51692, 51752-51757] [I design].
2. "Party master" exists in UI wording only: any CONFIRM member can accept a newcomer (S 51542) and the gate records no master ([PARTY-001](../issues/party.md#party-001-party-master-exists-in-ui-wording-only)).
3. `/joinparty name` sends op 3 for the target's party with no prompt and leaves the caller's status at NULL, so the gate's add reply is dropped and the gate lists a member the game server never registered ([PARTY-002](../issues/party.md#party-002-joinparty-skips-consent-and-leaves-a-ghost-gate-member)).
4. The case 3 bounds check in `PartyOperationResultHandler` `(iClientH < 0) && (iClientH > DEF_MAXCLIENTS)` is never true (S 51021) ([PARTY-003](../issues/party.md#party-003-bounds-check-in-partyoperationresulthandler-case-3-is-never-true)).
5. The gate uses the party id unchecked as an array index, decrements the wrong party in the duplicate-name step and does not update `m_iIndex` on crossing ([PARTY-004](../issues/party.md#party-004-gate-party-registry-weak-points)).
6. `CreateNewPartyHandler` / `bCreateNewParty` / `DEF_NOTIFY_RESPONSE_CREATENEWPARTY` (v2.06 per-server party) are left behind ([PARTY-005](../issues/party.md#party-005-dead-legacy-per-server-party-path)).
7. [V doc] Join is refused while the target is "in combat appearance", which reads an appearance nibble, not an actual combat timer [I intent: stop recruiting in the middle of a fight].
8. The exp bonus factor is left unset when every counted member is dead ([PARTY-006](../issues/party.md#party-006-exp-bonus-factor-left-unset-when-every-counted-member-is-dead)).
9. The gate's member list is one flat array of 4,999 names ([PARTY-007](../issues/party.md#party-007-gate-member-registry-is-one-flat-array-capacity-4999-members)).

## 10. Open questions

- What the client shows for each NOTIFY_PARTY sub-code beyond 1/4/7 (client cluster).
- Whether the gate's 20 s crossing window is long enough for slow map loads on real servers [U].

## 11. Doc discrepancies

None found against the code for the functions used here.
