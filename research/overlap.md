# overlap (index)

How the five Helbreath 2.20 programs - Client (C), Game Server / HGserver (GS), Gate Server (G),
Main Log Server (MLS), World Log Server (WLS) - connect and what they share. Describes intent and
current behaviour; defects live in `issues.md` and are linked, not described. Tags: [V] verified in
source or binary map, [I] inferred, [U] unverified. Source prefixes: `CLI` = `sources/client-2.20/`,
`SRV` = `sources/hgserver-librehelbreath/src/`, `GATE` = `sources/gateserver-librehelbreath` @ `0b994cd`.
MLS/WLS have no source; their claims come from `servers/official-mainserver.md` and
`servers/official-worldserver.md`. Scripts that produced the diffs and counts:
`.claude/scratch/overlap/{diff,defs,netmsg}.py`.

- `overlap/programs.md` - each program's job, what it owns (state, files, ports) and what it trusts the others for; read first.
- `overlap/connections.md` - every link: direction, port and cfg key, message families, who connects when, what happens on loss, text diagram; read before touching start-up or any socket.
- `overlap/systems-by-program.md` - matrix of the 30 gameplay/infrastructure systems x 5 programs (owns / relays / mirrors / stores / displays) with the coupling point; read to find where a system lives outside the game server.
- `overlap/shared-code.md` - XSocket, StrTok, headers, Misc/Msg/Item copies: identical vs diverged, measured; read before reusing a source file in another program.
- `overlap/shared-data.md` - account/character/guild files, config push, map files, client data, logs: who writes and who reads each; read before changing a file format.
- `overlap/shared-definitions.md` - constants defined in more than one program, values per program, agree or not; read before changing a limit or an ID.
- `overlap/rebuild-boundaries.md` - the shared libraries and contracts the overlaps suggest (protocol, persistence, content tables, routing); read when planning a rebuild.
