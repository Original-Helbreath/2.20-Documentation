# Issues: warehouse

Scope: defects and divergences in the bank/warehouse (deposit via Howard, withdraw, persistence flag) of the 2.20 game server. Split out of `research/systems/warehouse.md`.

### WAREHOUSE-001 Deposit accepts any NPC named Howard at any distance

Merged into [TRADE-008](trade.md#trade-008-no-server-side-distance-check-for-give-and-exchange).

### WAREHOUSE-002 m_bIsBankModified is only ever set, never reset
- **Program**: game server
- **System**: warehouse (persistence)
- **Kind**: bug / incomplete
- **Intent**: the flag is "to reduce DB load" (v2.13 comment, 27744): save the bank only when it changed.
- **Now**: it is only ever set TRUE, never reset after a save: the only FALSE assignment is the constructor (S Client.cpp:288; TRUE at 27745, 27822, 27966, 32512), so after the first bank change in a session every save carries the flag. [V]
- **Where**: S Game.cpp:27744-27745, 27822, 27966, 32512; S Client.cpp:288
- **Evidence**: read in source; how the world log uses the byte is unknown (no official source).
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/warehouse.md:96
