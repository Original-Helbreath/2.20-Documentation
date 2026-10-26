# Issues: arena

Scope: defects and divergences in fight zones (guild-booked arenas: reservation, tickets, entry, slot clock) in the 2.20 game server. Split out of `research/systems/arena.md`.

### ARENA-001 No guild check on the server for booking and tickets
- **Program**: game server / client
- **System**: arena (reservation)
- **Kind**: client-only check / security
- **Intent**: only a guild master books a fight zone.
- **Now**: booking and tickets are per player; only the client hides the menu from non-masters (C 5240); `FightzoneReserveHandler` has no rank test (S 27414-27528). Any player with a crafted packet can book. [V]
- **Where**: S Game.cpp:27414-27528; C Game.cpp:5240
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:119

### ARENA-002 Booking owner is a client index
- **Program**: game server
- **System**: arena (reservation)
- **Kind**: bug
- **Intent**: the booking belongs to the character.
- **Now**: after the booker logs out the slot index can be reused by a different player; the persisted per-player booking survives, but `m_iFightZoneReserve` still holds the old index until the 2-hour clear (S 27483) [I effect]. [V]
- **Where**: S Game.cpp:27483
- **Evidence**: read in source; effect inferred.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:121

### ARENA-003 Booking clear timer runs on uptime, not the slot clock
- **Program**: game server
- **System**: arena (slot clock)
- **Kind**: bug
- **Intent**: bookings expire at the end of their 2-hour slot.
- **Now**: `FightzoneReserveProcessor` clears every 7,200,000 ms of uptime, so a booking can be cleared minutes after it was made. [V doc]
- **Where**: S Game.cpp:3786, 37440-37460
- **Evidence**: doc crusade-quests.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:124

### ARENA-004 Stale-booking notify is sent to the wrong client
- **Program**: game server
- **System**: arena (expiry)
- **Kind**: bug
- **Intent**: tell the player whose booking was dropped.
- **Now**: the stale-booking blocks call `SendNotifyMsg(NULL, i, ...)` with `i` instead of `iClientH` (`i` is an unrelated loop variable) [I]. [V]
- **Where**: S Game.cpp:6333, 8821
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:126

### ARENA-005 Expired tickets are not consumed
- **Program**: game server
- **System**: arena (entry)
- **Kind**: bug / dead code
- **Intent**: the comment says an expired ticket disappears.
- **Now**: the branch is empty (S 30867-30868). Minor: the date test uses `|` for `||` (S 30864), the same result for booleans. [V]
- **Where**: S Game.cpp:30864-30868
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:128

### ARENA-006 Ticket exhaustion toggles the zone number's sign
- **Program**: game server
- **System**: arena (tickets)
- **Kind**: bug
- **Intent**: after 50 tickets, no more are issued.
- **Now**: each extra "get ticket" with 0 left multiplies the zone number by -1 again (S 40159), flipping it back to positive. While the number is non-zero (either sign) a new booking is refused with -4 (S 27466, 40156-40160). [V]
- **Where**: S Game.cpp:40156-40161, 27466
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:130

### ARENA-007 Zone counts disagree between server, client and maps
- **Program**: game server / client
- **System**: arena (reservation)
- **Kind**: bug / data
- **Intent**: one consistent set of zones.
- **Now**: 10 are bookable, 9 maps are whitelisted, 8 are offered by the client; booking zone 10 would take gold for a map not in the list [I]. [V]
- **Where**: S Game.cpp:27438, 37151-37160; C Game.cpp:5345-5402
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:133

### ARENA-008 Fight-zone auto-resurrect removed but dead time still recorded
- **Program**: game server
- **System**: arena (in-zone rules)
- **Kind**: dead code
- **Intent**: a 30 s auto-resurrect in fight zones (removed for item duplication risk per comment).
- **Now**: `m_dwFightzoneDeadTime` is still set and logged. [V] Merged from COMBAT-005 (combat view of the same commented block). [V]
- **Where**: S Game.cpp:4080-4089, 19264-19269
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:135; research/systems/combat.md:214

### ARENA-009 Dead pointer arithmetic after the reserve result
- **Program**: game server
- **System**: arena (reservation)
- **Kind**: dead code
- **Intent**: advance the write pointer.
- **Now**: `ip += 4` after the result int is dead arithmetic. [V]
- **Where**: S Game.cpp:27513
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/arena.md:137
