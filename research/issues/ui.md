# Issues: ui

Defects and divergences in the client's dialog-box system, input handling and shortcuts. Behaviour lives in [systems/ui.md](../systems/ui.md). Click-to-move rubber-banding is recorded in [CLIENT-STATE-002](client-state.md#client-state-002-prediction-without-a-reconciliation-queue).

### UI-001 Double-click branch assigns dwSelectClickTime to itself

- **Program**: client
- **System**: ui
- **Kind**: bug
- **Intent**: reset the click time so a triple click is not a second double click [I].
- **Now**: the branch assigns `dwSelectClickTime` to itself; harmless because it is overwritten at 35792, but the reset is lost [V code].
- **Where**: `Game.cpp:35782`, `35792`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/ui.md section 9

### UI-002 Panel id comments are swapped

- **Program**: client
- **System**: ui
- **Kind**: bug
- **Intent**: comments name the right panel.
- **Now**: comments swap the panel ids (slot 40 = 29 commented as icon panel 30) [V].
- **Where**: `Game.cpp:949-950`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/ui.md section 9

### UI-003 ClearInputString zeroes only pointer-size bytes

- **Program**: client
- **System**: ui
- **Kind**: bug
- **Intent**: clear the whole input buffer.
- **Now**: it zeroes only pointer-size bytes [Vd].
- **Where**: `Game.cpp:19456-19463`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/ui.md section 9

### UI-004 Function-key layout is a build switch

- **Program**: client
- **System**: ui
- **Kind**: build
- **Intent**: one key layout.
- **Now**: `DEF_SHORTCUT` (Taiwan/China only) decides whether a function key opens a dialog (English) or fires a shortcut [V].
- **Where**: `GlobalDef.h:163-176`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/ui.md section 9
