# System: arena (fight zones)

*Verdict (size review): small and cohesive; single file.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`. Tags: [V] read in
source, [I] inferred, [U] unverified. Function bodies: `research/game-server/functions/crusade-quests.md` (fightzone rows).

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules - 8. Couplings -
9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Fight zones (`fightzone1`..`fightzone9` maps) are rentable arenas for organised guild battles. A guild master pays 1,500
gold at the City Hall to book a zone for the current two-hour slot, then collects up to 50 dated entry tickets to hand
to guild members; using a ticket teleports into the zone [V S Game.cpp:27414-27528, 40145-40305, 30858-30879]. Inside,
the usual same-nation protection is lifted between guilds: a member of another guild (even of your own nation) is an
enemy [V S Game.cpp:37613-37668]. Fight zones are not auto-saved periodically and everyone is force-recalled at the end of the slot
[V S Game.cpp:3927-3933, 2151-2162]; the evident intent is softer death consequences there, but no death-consequence
branch was found [U, see section 10]. The nation-parity rule
alternates which zones each nation may book on odd and even days so the two nations do not collide [V S 27443-27448].

## 2. Subsystems

| # | Subsystem | Intent |
|---|---|---|
| 2.1 | Reservation | book one zone per slot per player; gold cost; slot and parity rules |
| 2.2 | Ticket issue | 50 tickets per booking, stamped with the slot's expiry (month, day, hour) |
| 2.3 | Ticket use / entry | ticket teleports into its zone if the stamp is still valid |
| 2.4 | Slot clock and expiry | 2-hour slots from midnight: force-recall timer on entry, periodic clear of bookings, stale bookings dropped at login |
| 2.5 | In-zone rules | guild-vs-guild relationship, no auto-save, fight-zone death branches |
| 2.6 | GM controls | `/reservefightzone` (block a zone, toggle no-force-recall), `/getticket` |

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `DlgBoxClick_GuildMenu` modes 13-22 | client | C Game.cpp:5240-5243, 5341-5422 | master-only menu; zones 1..8 offered; result modes 14-22 [V] |
| 2.1 | response handler `MSGID_RESPONSE_FIGHTZONE_RESERVE` | client | C Game.cpp:2170 | maps result code to a dialog mode [V grep] |
| 2.1 | `CGame::FightzoneReserveHandler` | game server | S Game.cpp:27414-27528 | checks, gold, booking, reply [V] |
| 2.2 | `DrawDialogBox_GuildMenu` (sends `REQ_GETOCCUPYFIGHTZONETICKET`) | client | C Game.cpp:38907-39216 | "get ticket" button [V doc] |
| 2.2 | `CGame::GetFightzoneTicketHandler` | game server | S Game.cpp:40145-40305 | one ticket per call, date stamp [V] |
| 2.3 | `UseItemHandler` fixed-location teleport branch (effect value 2 > 10) | game server | S Game.cpp:30836-30880 | validate stamp, deplete, teleport to `fightzone<N>` [V] |
| 2.4 | `RequestInitDataHandler` / teleport arrival force-recall | game server | S Game.cpp:2146-2163 (also 23756) | recall timer to the end of the slot, unless GM mode or admin [V] |
| 2.4 | `CGame::FightzoneReserveProcessor` | game server | S Game.cpp:37440-37460 | every 7,200,000 ms of uptime: clear non-GM bookings, notify holders [V doc] |
| 2.4 | `_bDecodePlayerDatafileContents` stale-booking check (two build variants, S 5551 and 6945) | game server | S Game.cpp:6326-6338, 8814-8825 | drop booking whose stamp has passed when character data loads [V] |
| 2.5 | map init `m_bIsFightZone` | game server | S Game.cpp:25584-25587 | any map whose name starts "fight" [V] |
| 2.5 | `iGetPlayerRelationship` / `iGetPlayerABSStatus` fight-zone branches | game server | S Game.cpp:37613-37668, 37705 | other guilds are enemies [V] |
| 2.5 | `ClientKilledHandler` fight-zone dead time | game server | S Game.cpp:19264-19270 | records time of death; 30 s auto-resurrect commented out [V; S 4080-4089] |
| 2.6 | `AdminOrder_ReserveFightzone` | game server | S Game.cpp:36587-36647 | GM block (-1) / unblock, toggles `m_iFightzoneNoForceRecall` [V doc] |
| 2.6 | `AdminOrder_GetFightzoneTicket` | game server | S Game.cpp:50801-50849 | GM gets 3 tickets for the zone he stands in (or zone 1) [V] |

## 4. Commands and messages

- `MSGID_REQUEST_FIGHTZONE_RESERVE` (iV1 = zone) client -> game [V C 5345; S 15063]; reply
  `MSGID_RESPONSE_FIGHTZONE_RESERVE` (CONFIRM/REJECT + int result 1, 0 slot ending, -1 taken, -2 gold, -3 parity day,
  -4 already booked) [V S 27449-27512].
- `DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET` -> `GetFightzoneTicketHandler` [V S 15589].
- `DEF_NOTIFY_FIGHTZONERESERVE` (-1 tickets exhausted, -2 booking cancelled/expired, -4/-5 GM toggle) [V S 40160, 6333, 37457].
- `DEF_NOTIFY_ITEMOBTAINED` / `CANNOTCARRYMOREITEM` for tickets.
- Slash (GM): `/reservefightzone <n>`, `/getticket`.

## 5. Data

| Field | Where | Meaning |
|---|---|---|
| `m_iFightZoneReserve[DEF_MAXFIGHTZONE=10]` | S Game.h:170, 764 | per zone: 0 free, client index of the booker, -1 GM-blocked [V] |
| `m_iFightzoneNoForceRecall` | S Game.h:766 | GM switch: 1 = no recall timer in fight zones [V S 2152] |
| `m_iFightzoneNumber`, `m_iReserveTime`, `m_iFightZoneTicketNumber` | S Client.h:153 | booking: zone (negated once tickets run out), expiry `MMDDHH`, tickets left; saved as `reserved-fightzone-id = n t c` [V S 9216, 8695] |
| `m_dwFightzoneDeadTime` | S Client.h:331 | time of death in a fight zone [V] |
| ticket item `ArenaTicket` / `ArenaTicket(%d)` | S englishitem.h:8-9 | touch effect `DEF_ITET_DATE` with month/day/hour; effect value 2 = 10 + zone [V S 40184-40192, 30872] |
| `Map::m_bIsFightZone` | S Map.h:153 | map flag [V] |

## 6. Flow

**F1 Book.** Master opens the City Hall guild menu: rank 0 only, mode 13 if no booking else mode 19 (get tickets)
[V C 5240-5243] -> picks zone N -> `FightzoneReserveHandler`: minutes left in slot > 5 [V S 27429, 27449]; zone free
[V 27454]; gold >= 1500 [V 27458]; parity `(day + side + N) % 2 == 0` [V 27448, 27462]; player holds no booking
[V 27466] -> take 1500 gold, `m_iFightZoneReserve[N-1] = clientH`, zone and expiry stored (current hour +1 if odd,
+2 if even = slot end), 50 tickets [V 27476-27499] -> reply.

**F2 Tickets.** "Get ticket" -> `GetFightzoneTicketHandler`: if tickets <= 0, negate the zone number and notify -1
[V S 40156-40161]; else create `ArenaTicket(N)`, add to bag, decrement, stamp month/day/hour from the expiry
[V 40164-40192]. The master hands tickets to members (normal item give).

**F3 Enter.** Member uses the ticket -> date check: month and day equal today and stamped hour > current hour
[V S 30864-30866] -> if the holder is not already on `fightzone<N>`: deplete the ticket and teleport; if already there nothing is consumed
[V 30869-30878] -> on arrival the
recall timer is set to the time left in the 2-hour slot minus 2 minutes (in 3 s ticks) [V S 2151-2162].

**F4 Expire.** Every 2 h of server uptime all non-GM bookings are cleared and holders notified [V doc S 37440-37460, 3786];
at login, a booking whose `MMDDHH` has passed is dropped [V S 6326-6338].

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| price | 1,500 gold | S 27458, 27479 [V] |
| slots | 2 h from 00:00; booking closes 5 min before slot end | S 27429 [V] |
| zones | server accepts 1..10, client offers 1..8, maps exist 1..9 (teleport whitelist) | S 27438; C 5345-5402; S 37151-37160 [V] |
| parity | Aresden (side 1) and Elvine (side 2) alternate zone parity by day | S 27443-27448 [V] |
| tickets | 50 per booking, valid only for the booked slot (same month/day, hour < stamp) | S 27499, 30864-30866 [V] |
| recall | at slot end - 2 min; skipped for admins, with GM no-recall, or under DEF_GUILDWARMODE | S 2150-2163 [V] |
| in-zone | no auto-save | S 3928-3933 [V] |
| relationship | other guild master/member -> enemy (2) in fight zones | S 37613, 37627, 37654, 37668 [V] |

## 8. Couplings

- **guild** - menu lives in the guild dialog; client restricts booking to rank 0 (C 5240); relationship uses
  `m_cGuildName`/`m_iGuildRank`.
- **combat / pk-law (other cluster)** - `iGetPlayerRelationship*`, fight-zone branches in attack and kill handlers
  (S 12621, 13114, 13785, 31140-32237 read `m_bIsFightZone`) decide criminal/EK/drop consequences.
- **items (other cluster)** - ticket item, `DEF_ITET_DATE` touch effect, `UseItemHandler` teleport branch.
- **map / movement (other cluster)** - `RequestTeleportHandler`, force-recall timer `m_iTimeLeft_ForceRecall`, map flag.
- **persistence** - booking persisted in the character file.

## 9. Current behaviour vs intent

Defects are in [issues/arena.md](../issues/arena.md); the neutral lines are:

1. `FightzoneReserveHandler` has no rank or guild test; only the client hides the menu from non-masters ([ARENA-001](../issues/arena.md#arena-001-no-guild-check-on-the-server-for-booking-and-tickets)).
2. The booking owner is a client index, which can be reused after the booker logs out ([ARENA-002](../issues/arena.md#arena-002-booking-owner-is-a-client-index)).
3. The clearing timer runs on server uptime, not on the slot clock ([ARENA-003](../issues/arena.md#arena-003-booking-clear-timer-runs-on-uptime-not-the-slot-clock)).
4. The stale-booking blocks call `SendNotifyMsg(NULL, i, ...)` with a loop variable instead of `iClientH` (S 6333, 8821) ([ARENA-004](../issues/arena.md#arena-004-stale-booking-notify-is-sent-to-the-wrong-client)).
5. An expired ticket is not consumed: the branch is empty (S 30867-30868) ([ARENA-005](../issues/arena.md#arena-005-expired-tickets-are-not-consumed)).
6. Each extra "get ticket" with 0 left multiplies the zone number by -1 again; while it is non-zero a new booking is refused with -4 ([ARENA-006](../issues/arena.md#arena-006-ticket-exhaustion-toggles-the-zone-numbers-sign)).
7. 10 zones are bookable, 9 maps are whitelisted and 8 are offered by the client ([ARENA-007](../issues/arena.md#arena-007-zone-counts-disagree-between-server-client-and-maps)).
8. The fight-zone auto-resurrect was removed (item duplication risk per comment) but `m_dwFightzoneDeadTime` is still set and logged ([ARENA-008](../issues/arena.md#arena-008-fight-zone-auto-resurrect-removed-but-dead-time-still-recorded)).
9. `ip += 4` after the result int is dead arithmetic (S 27513) ([ARENA-009](../issues/arena.md#arena-009-dead-pointer-arithmetic-after-the-reserve-result)).

## 10. Open questions

- Which drop/penalty rules apply on fight-zone deaths (combat cluster owns `ClientKilledHandler` / `ApplyPKpenalty`
  branches) [U here].
- Whether fight-zone maps can live on a different game server than the City Hall (the comment at S 2146 suggests so);
  bookings are server-local arrays, so cross-server entry depends only on the ticket stamp [I].

## 11. Doc discrepancies

None against the code.
