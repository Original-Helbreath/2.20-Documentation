# Issues: client-resolution

Defects found in the client while analysing the screen-size dependence (640x480 assumptions) of the HUD, dialogs and input. Behaviour and the change plan live in [client/resolution.md](../client/resolution.md) and `research/client/resolution/`. All were verified by reading, not run. All entries: program = client, system = client-resolution, blocks 2.20 setup = no.

### CLIENT-RESOLUTION-001 Observer panning right and bottom never fires

- **Program**: client
- **System**: client-resolution
- **Kind**: dead code
- **Intent**: an observer pans the view when the mouse reaches any screen edge.
- **Now**: the right and bottom tests are `msX == 640` and `msY == 480`, but the mouse clamps to 639/479, so the observer can pan only left and up [V by reading].
- **Where**: `Game.cpp:35718-35730`; `DXC_dinput.cpp:82-88`
- **Evidence**: [V by reading]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/change-plan.md "Bugs found on the way"; client/resolution/window-setup.md

### CLIENT-RESOLUTION-002 Dialog drag is unclamped

- **Program**: client
- **System**: client-resolution
- **Kind**: bug
- **Intent**: dialogs stay on screen.
- **Now**: a dialog can be dragged off screen; only re-open clamps it (the max clamps at 18576-18577, and 18578-18579 push a dialog back by 20 when its right or bottom edge is above 10) [V by reading].
- **Where**: `Game.cpp:35849-35850`, `18576-18579`
- **Evidence**: [V by reading]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/change-plan.md; client/resolution/variables.md

### CLIENT-RESOLUTION-003 Item-drop query is clamped to the full screen

- **Program**: client
- **System**: client-resolution
- **Kind**: bug
- **Intent**: keep the query box clear of the HUD.
- **Now**: the box is clamped to the full screen, so it may cover the HUD (30 rows) [V by reading].
- **Where**: `Game.cpp:4717-4768`
- **Evidence**: [V by reading]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/change-plan.md; client/resolution/variables.md

### CLIENT-RESOLUTION-004 Item-drop clamp uses the wrong dialog size

- **Program**: client
- **System**: client-resolution
- **Kind**: bug
- **Intent**: clamp with the dialog's real size.
- **Now**: the clamp uses 235x100 while dialog 20 is 252x87; at the right edge the box reaches x=656, 16 px past the screen, and the bottom clamp leaves 13 px unused [V by reading].
- **Where**: `Game.cpp:4717-4719` vs `669-670`
- **Evidence**: [V by reading]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/change-plan.md

### CLIENT-RESOLUTION-005 Default dialog rectangles overlap the icon panel or leave the screen

- **Program**: client
- **System**: client-resolution
- **Kind**: bug
- **Intent**: default dialog positions fit above the HUD panels.
- **Now**: dialog 10's default bottom is y 435 (273 + 162), overlapping the icon panel by 8 px at 640x480; dialog 22's is y 434 (88 + 346), overlapping the icon panel (starts at 427) by 7 px; dialog 29 (gauge panel) ends at y 487 (434 + 53), 7 px below the 480-row screen (harmless, clipped art) [V by reading].
- **Where**: `Game.cpp:606-609`, `679-682`, `721-724`
- **Evidence**: [V by reading]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/change-plan.md

### CLIENT-RESOLUTION-006 _DrawBlackRect is dead code

- **Program**: client
- **System**: client-resolution
- **Kind**: dead code
- **Intent**: draw a black border rectangle on the screen edge.
- **Now**: its only caller is commented out; its 4 rows of screen-size literals are not live [V].
- **Where**: `Game.cpp:22826-22839`, caller `35316`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/method.md; client/resolution/values-screen-game.md

### CLIENT-RESOLUTION-007 PutSpriteFastFrontBuffer is unused and bypasses the back buffer

- **Program**: client
- **System**: client-resolution
- **Kind**: dead code
- **Intent**: blit to the primary surface.
- **Now**: `CSprite::PutSpriteFastFrontBuffer` blits straight to the primary `m_lpFrontB4`, has no caller and no literal size; it would be wrong after any scaling change [V]. `PutSpriteFastDst` is also unused (blitters.md).
- **Where**: `Sprite.cpp:483-572`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: client/resolution/window-setup.md; client/rendering/blitters.md
