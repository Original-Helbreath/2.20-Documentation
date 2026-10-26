# WLS -> MLS port trace (R2 round 2)

All addresses are VAs in the Ghidra-imported binaries. [V] = verified in decompile/bytes.

## WorldServer.exe
- `ReadWLServerConfig` (0x401938 region): keyword `main-log-server-address` (str 0x4170bc) sets the MLS address (this+0x4d);
  `main-log-server-port` (str 0x4170a4, case 7) sets the MLS port (this+0x64); `world-server-port` / `internal-world-server-port`
  are the WLS's own listen ports. [V]
- `InitializeWorldServer` (entry 0x4014e0; connect loop at 0x401590): loops 10 times, creating a socket object and calling `FUN_0040ca50(addr = this+0x4d,
  port = this+0x64, msg = 0x464+i)` -> `inet_addr` + `htons` + `connect()`. Log "(!) Try to connect Main-log-socket(%d)... Addr:%s Port:%d". [V]
  Reconnect path (`SendMsgToMLS` 0x4041dd) reuses the same fields. [V]
- No hard-coded MLS port in the WLS: it dials exactly `main-log-server-port` (the pack: 2981). 4500 appears nowhere. [V]

## MainServer.exe (Ghidra has no functions/strings analysed; traced from raw bytes)
- Listeners created in the window-message setup around 0x412c44 and 0x413344-0x41346c; all ports config-driven:
  internal = this+0x1c (`internal-log-server-port`, 3840), public = this+0x18 (`log-server-port`, 2981), +1, +2. Nothing hard-coded. [V bytes]
- Accept handler 0x402770 (thunk 0x40123a). Msg switch at 0x412560: msg 0x446 (internal) calls it with flag 1; 0x43c/0x43d/0x43e
  (public) call it with flag 0. At 0x40289c `cmp [ebp+0xc],1; jne` skips the address check for flag 0; for flag 1 it does
  strncmp(peer, "211.115.208.", 12) (push of 0x451224 at 0x4028a1; next list entry 0x451214 at 0x4028c7). [V bytes]

## Conclusions
1. The shipped WLserver.cfg (main-log-server-port 2981) is consistent with the binary: the WLS dials the MLS PUBLIC port 2981.
2. The allow-list (patch 4, file offset 0x51224) applies ONLY to the internal port 3840. A WLS connecting to 2981 is never checked,
   so patch 4 is not needed for the 2981 setup. It would only matter if the WLS were pointed at 3840.
3. `internal-log-server-port = 3840` in WLserver.cfg is the WLS's own game-server listener, not an MLS target (same number, different host/role).
4. Unverified: that the MLS dispatcher accepts a WLS registration arriving on a public socket (docs say the switch does not check
   socket type; needs a runtime test).

Docs should say: WLS -> MLS = `main-log-server-port`, default 2981 (public); drop the "assumed 4500" and the 211.115.208. prefix requirement for this setup.
Confidence: high on WLS side and on the flag/allow-list logic; medium on end-to-end acceptance.

## Applied edits (docs pass for R2-05; paths under research/servers/)
- world-log-server.md:45-55 - "Open question (MLS port the WLS dials) [U]" -> "answered [V]" with function addresses; patch 4 not needed; only public-socket acceptance stays [U].
- world-log-server.md:65 (peers table, WLS row) - "port unverified" -> dials public 2981; 4500 demoted to sample value.
- official-compat-220.md:112-116 (section 5) - "open question" -> patch 4 not needed for 2981; addresses added.
- official-compat-220.md:~145 (section 7, patch 4 row) - "unresolved" -> optional, only if WLS pointed at 3840.
- official-compat-220.md:150-153 (section 8) - port question replaced by the remaining public-socket acceptance question.
- official-mainserver.md:54-59 (section 1) - open question -> answered; 0x451224 and config-driven ports noted; acceptance [U].
- official-worldserver.md:44-46 - open question -> answered with ReadWLServerConfig / InitializeWorldServer / SendMsgToMLS addresses.
- topology.md:66 (WLS -> MLS row) and :82 (diagram label 4500 -> 2981, 4500 in 2.20 sample) - same answer.
- recreation-plan.md:124-125 (section 7) - new item: patch 4 not needed, test public-socket acceptance [U].
- Not edited: sample-value mentions of 4500 (topology.md:39, world-log-server.md:34), correctly labelled as the 2.20 sample. servers.md has no matches.

Address discrepancies noted, left unchanged: official-mainserver.md says accept handler 0x4027ae and check range 0x402894-0x402957, this trace says 0x402770 (thunk 0x40123a) and 0x40289c; official-worldserver.md says InitializeWorldServer 0x4014e0, this trace says 0x401590. Reconciled 2026-10-25 (main session): Ghidra gives InitializeWorldServer entry 0x4014e0, body to 0x4015db, so 0x401590 is the connect loop inside it. MainServer bytes: 0x40123a is `jmp 0x402770`, 0x402770 starts with the `push ebp; mov ebp,esp` prologue, and 0x4027ae is mid-function. So the accept handler entry is 0x402770, and 0x40289c (flag test) lies inside the 0x402894-0x402957 check range. Docs updated.
