# shared code across the sources

Conventions: see `../overlap.md`. Method: `.claude/scratch/overlap/diff.py` strips all whitespace,
drops blank lines, then diffs every pair (C = client, H = game server, G = gate @ 0b994cd). "Lines"
= differing normalized lines; ratio = difflib similarity. MLS/WLS: no source, but their binaries
use the same XSocket class and framing [V official-worldserver.md 1].

## Summary

| File | Copies | C-H | C-G | H-G | Verdict |
|---|---|---|---|---|---|
| XSocket.cpp | C, H, G | 115 (0.88) | 104 (0.89) | 9 (0.99) | one class, client branch diverged; server/gate near-identical [V] |
| XSocket.h | C, H, G | 18 | 17 | 1 | same; client adds heap `operator new/delete`, blocking send/connect [V] |
| StrTok.cpp/.h | C, H, G | 20 / 7 | 20 / 7 | **0 / 0** | server = gate identical; client rewritten branch order, 256-byte token vs 1024 [V CLI StrTok.h:22, SRV StrTok.h:21] |
| NetMessages.h | C, H, G | 43 | 50 | 59 | one catalogue, three snapshots; values never disagree (shared-definitions.md) [V] |
| MessageIndex.h | H, G | - | - | **0** | identical [V] |
| ActionID.h | C, H, G | 4 | 6 | 2 | same table, client extras [V] |
| UserMessages.h | H, G | - | - | 6 | per-process window messages; intentionally different [V] |
| WINMAIN.H / Wmain.cpp | C, H, G | 14 / 993 | 19 / 935 | 5 / 266 | same Win32 skeleton, program-specific body [V] |
| DynamicObjectID.h | C, H | 2 | - | - | server adds flag DOs 16/17 [V] |
| Misc.cpp/.h | C, H | 639 (0.27) | - | - | same class name, different helper sets [V] |
| Msg.cpp/.h, Item.*, Magic.h, Skill.h, BuildItem.h, Tile.h | C, H | 0.30-0.69 | - | - | same names, different roles: server = rule records, client = display records [V] |
| GlobalDef.h | C, H | 446 (0.00) | - | - | unrelated contents under one name [V] |
| Client.h | H, G | - | - | 263 (0.11) | unrelated: GS player object vs gate peer object [V] |

## What the divergence means
- **XSocket** is the one true shared library. Cipher code is the same in all three copies (send:
  `+= (i ^ key)` then `^= key ^ (size - i)`, CLI XSocket.cpp:486-489, SRV XSocket.cpp:435-438) [V].
  The gate copy forces key 0 and keeps the write-enabled queue; the game-server copy sends direct
  (the 9 H-G lines) [V diff]. The client copy adds connect-time queueing until FD_WRITE
  (`m_bIsWriteEnabled`, CLI XSocket.cpp:37, 108, 494) and blocking helpers [V]. Security view:
  `../issues/network.md#net-001-cipher-is-obfuscation-not-security`.
- **StrTok** server and gate share one copy byte-for-byte after whitespace; the client copy is an
  older or independently edited branch with a smaller token buffer [V/I].
- **NetMessages.h** was copied into each project and then extended locally: 374 / 377 / 340 defines
  (C / H / G) [V netmsg.py]. Only-in-H names are server-to-log IDs (config push, logs, manager);
  only-in-C names are login result codes for the client; only-in-G names are monitor shutdown IDs.
  The gate snapshot predates GSM 0x0D-0x12 [V game-gate-protocol.md 8].
- **Same file name, different module** (Item, Magic, Skill, BuildItem, Msg, Misc, Tile, GlobalDef,
  Client.h): these were not copies; client and server each built their own class for the same noun.
  Sharing them would mean designing a common record, not merging files [I].

## Doc discrepancies
None found: `../servers/wire-format.md` cipher description matches all three copies.
