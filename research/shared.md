# shared (index)

Cross-program reference for the Helbreath v2.20 stack (client, game server, gate, Main/World Log):
the message-ID catalogue and the file formats. Per-program maps point here. Each entry marks claims
[V] verified in code or [I] inferred, with file:line. Message layouts stay in `servers/` and are
linked, not repeated.

- `shared/messages/README.md` - sources, column meanings, counts and how each message's link was
  guessed; read before trusting any row in the message files.
- `shared/messages/client-game.md` - 237 client <-> game server IDs and sub-types (MOTION, COMMON,
  NOTIFY, ...) with handler lines on both sides; read when tracing gameplay traffic.
- `shared/messages/client-log.md` - 39 client <-> Log (MLS/WLS) IDs and result codes; read when
  working on login, character select or enter-game.
- `shared/messages/game-log.md` - 41 game server <-> Log IDs (player data, config push, guilds);
  read when building the Log server's game-server side.
- `shared/messages/game-gate.md` - 17 game server <-> gate IDs; read when touching the gate link.
- `shared/messages/game-game.md` - 18 GSM records relayed game-to-game through the gate; read when
  handling cross-server stock messages.
- `shared/messages/main-world.md` - 10 MLS <-> WLS IDs known only from the official binaries; read
  when splitting the Log server into Main and World.
- `shared/messages/admin-log.md` - 6 admin/DB-tool messages of the official MLS; read only if
  reusing those tools.
- `shared/messages/unused.md` - 22 IDs defined but used nowhere; read before assigning a new ID.
- `shared/formats/persistence-files.md` - account, character and guild files: paths, owners,
  game-server encoder/decoder, the 77 character keys; read before touching saved player data.
- `shared/formats/server-program-cfg.md` - gserver.cfg, GateServer.cfg, Lserver.cfg, WLserver.cfg,
  Crusade/WarSchedule/CrusadeGUID, OccupyFlag, notice.txt; read when configuring or starting a
  server process.
- `shared/formats/server-content-cfg.md` - Item/Npc/Magic/Skill/Quest/Potion/BuildItem/DupItemID/
  NpcItem/Teleport/World configs pushed by the Log server, field by field; read when editing game
  content or implementing the config push.
- `shared/formats/client-files.md` - LOGIN.CFG, ItemName.cfg, contents*.txt, magic/skill/BItem
  cfgs, GameMsgList, XOR-obfuscated badword and Curse; read when changing client data files.
- `shared/formats/amd-map.md` - binary .amd map layout (header, 10-byte tiles, flags) and loader
  differences; read before reading or writing maps.
- `shared/formats/config-files.md` - every server cfg/data file: reader line, copies by version, chosen copy,
  encoding and name-spelling hazards; read before picking a config copy.
- `shared/formats/mapdata-txt.md` - server map-info .txt keys (teleports, spawners, waypoints,
  crusade points); read when editing a map's server behaviour.
