# Reconstructed pseudo-C of the official log servers

**This is reconstructed pseudo-code, not original source.** It was written from a static analysis of
`MainServer.exe` (MLS, 2002-11-26, v3.51 "Extasis fix") and `WorldServer.exe` (WLS, 2002-11-29, V2.15);
neither binary was run. It does not compile and is not meant to.

- `WorldServer/*.c` - cleaned from the Ghidra decompiler output (the program is fully analysed in the
  Ghidra project; function names there match the file names).
- `MainServer/*.c` - written by hand from a capstone disassembly, because Ghidra did not create
  functions for this debug build. Expect more interpretation here.

Conventions:
- One file per function; the header comment gives the program, the address and the tag ([V] the
  control flow and constants were seen; [I] parts are interpretation).
- Field and message names follow the 2.20 `NetMessages.h` (`MSGID_*`, `DEF_LOGRESMSGTYPE_*`,
  `DEF_ENTERGAMERESTYPE_*`). Body offsets: 0 MsgID u32, 4 MsgType u16, 6 payload.
- C runtime and MFC helpers are not reconstructed; they appear by their usual names (`memcpy`,
  `strcmp`, `wsprintf`, `fopen`).
- `this->` members are named for their use; the hex offset is kept in a comment where it matters.

Read the protocol documents first: `../official-mainserver.md`, `../official-worldserver.md`,
`../official-compat-220.md`.
