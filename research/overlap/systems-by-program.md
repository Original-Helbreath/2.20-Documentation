# systems x programs

Conventions: see `../overlap.md`. Systems from `../systems.md` (30 systems; `commands.md` is a
cross-index and is left out). Roles: **owns** (authoritative logic), **relays** (forwards without
interpreting), **mirrors** (keeps a display copy of state), **stores** (persists), **displays**
(renders/plays), **-** (no part). Coupling points come from each system doc's "Couplings" section
unless cited otherwise. Main takeaway: the game server owns every gameplay rule; the WLS stores and
pushes; the gate touches only chat, party, guild, crusade, admin and topology; the MLS touches only
session and persistence (accounts).

| System | Client | Game server | Gate | MLS | WLS | Coupling point |
|---|---|---|---|---|---|---|
| map | displays (.amd, own loader) | owns | - (map list skipped) | stores routing table | stores routing table | `.amd` format; map list in REGISTERGAMESERVER; `DEF_NOTIFY_SERVERCHANGE` |
| tile | mirrors (flags from .amd) | owns | - | - | - | .amd flag byte (0x80/0x40; 0x20 server only) [V amd-map.md:29] |
| movement | mirrors (prediction) | owns | - | - | - | MOTION messages; move confirm / reject; `DEF_OBJECTDAMAGEMOVE` |
| entity | mirrors (view list) | owns | - | - | stores Npc.cfg, pushes | view snapshot 21x16 [V entity.md:28]; `Npc.cfg` push |
| gathering | displays | owns | - | - | - | NOTIFY sub-codes |
| combat | displays | owns | relays (crusade GSM) | - | - | `bStockMsgToGateServer` from kill handlers |
| magic | mirrors (name cfg) | owns | - | - | stores Magic.cfg, pushes | `MSGID_MAGICCONFIGURATIONCONTENTS`; client magic cfg is display-only |
| skills | mirrors (name cfg) | owns | - | - | stores Skill.cfg + mastery in char file | Skill.cfg push; mastery/SSN record keys |
| character | mirrors | owns | - | - | stores char file | record codec; autosave `REQUEST_SAVEPLAYERDATA` |
| pk-law | displays | owns | relays (force recall, GM) | - | stores PK/EK counts | record fields; GSM |
| item | mirrors (ItemName.cfg) | owns | logs (`ITEMLOG`, no caller) | - | stores Item*.cfg, bag/bank in char file, item log | Item.cfg push; `MSGID_GAMEITEMLOG` [V game-gate-protocol.md 2] |
| crafting | mirrors (BItem cfg) | owns | - | - | stores BuildItem/Potion cfg | BuildItem.cfg, Potion.cfg push |
| shop | displays | owns | - | - | - | COMMON REQ_PURCHASEITEM etc. |
| trade | displays | owns | - | - | logs | item log via log link |
| warehouse | mirrors (bank list) | owns | - | - | stores bank in char file | `DEF_MAXBANKITEMS` (see shared-definitions.md) |
| chat | displays | owns (filter, local delivery) | relays (GSM) | - | stores GM log | GSM bus; `MSGID_GAMEMASTERLOG` |
| party | displays (dialog 32) | owns local mirror | **owns registry** | - | stores party-id in char file | `MSGID_PARTYOPERATION`; party is the only gameplay state in the gate |
| guild | displays | owns live membership | relays (GSM guild ops) | - | **stores guild files** | guild create/disband via log link; guild-status byte in RESPONSE_PLAYERDATA |
| crusade | displays | owns (per server) | relays global start/end | - | stores occupy flags | GSM; `Crusade.cfg`/`CrusadeGUID.txt` local to GS |
| arena | displays | owns | - | - | stores booking in char file | record field |
| quest | displays | owns | - | - | stores Quest.cfg + quest fields | Quest.cfg push |
| network | owns own XSocket copy | owns XSocket + dispatch | owns XSocket + relay | binary XSocket | binary XSocket | identical framing everywhere (shared-code.md) |
| session | owns login screens | owns enter/leave | relays party op 4/5/7 | **owns accounts, coarse state** | **owns INIT/WAIT/PLAYING** | ENTERGAME; account status messages |
| persistence | - | owns codec | - | stores account files | **stores char/guild files** | record text format; WLS new-character template |
| server-topology | - | owns readiness gate | owns registration, shutdown | owns world list | owns GS list, push | REGISTERGAMESERVER (two different IDs: gate 0x0512A3F4 vs WLS, `../issues/setup-2.20.md#setup-005-game-server-registration-id-differs-from-the-wls-byte-patch`) |
| admin | displays | owns GM commands | owns operator shutdown, monitors | admin/billing msgs | GM/item/EK logs | GSM for cross-server GM; log messages |
| rendering | owns | - | - | - | - | assets only |
| ui | owns | - | - | - | - | - |
| audio | owns | - | - | - | - | - |
| client-state | owns mirror | source of truth | - | - | - | NOTIFY stream |

## Doc discrepancies
None found in this pass; roles were cross-checked against the Couplings sections and
`../servers/topology.md`.
