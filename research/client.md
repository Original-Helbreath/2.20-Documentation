# client (index)

Helbreath v2.20 client (`sources\client-2.20\`, librehelbreath ClientSources c01544d): function map
and subsystem notes. Login protocol lives in `servers\client-log-protocol.md`; message layouts in
`shared\messages\`; file formats in `shared\formats\`.

- `client/build-config.md` - DEF_LANGUAGE and every switch, the hard-coded log-server address and how to change it; read before building or pointing the client at a server.
- `client/assets.md` - every file the client loads, sprite id ranges, what 2.02 + 2.23 lack (including the required Curse.txt); read before assembling a data folder or porting the loader.
- `client/data-structures.md` - CGame fields, array limits, dialog ids, CMapData/CTile and record classes; read before touching state or sizing a port.
- `client/functions/startup.md` - WinMain, bInit load order, main loop, OnTimer, registry settings; read when tracing startup or shutdown.
- `client/functions/game-modes.md` - game modes and UpdateScreen_On* screens; read when following screen flow.
- `client/functions/networking.md` - XSocket framing and cipher, receive/send handlers, every NotifyMsg_*; read when matching client behaviour to a message.
- `client/functions/ui-dialogs.md` - the 38 dialog boxes and their draw/press/click handlers; read when working on any dialog.
- `client/functions/input.md` - DirectInput mouse, keyboard, IME text entry, shortcuts; read when tracing input.
- `client/functions/world-map.md` - CMapData, CTile, map loading, object placement, view point; read for client-side world logic.
- `client/functions/characters.md` - DrawObjects, DrawObject_On* and name/text drawing; read when working on character display.
- `client/functions/items.md` - inventory, bank, shop, exchange, item drops, build items; read for item flows.
- `client/functions/magic-effects.md` - effects, magic and skills, crusade UI; read for spells and effects.
- `client/functions/sound.md` - DirectSound wrapper, sound loading, BGM choice; read for audio.
- `client/functions/chat.md` - chat, local commands, guild/party, word filters; read for chat and social code.
- `client/functions/config-loading.md` - cfg/contents parsers and the sprite loading screen; read when changing data files.
- `client/functions/support.md` - CMisc, CStrTok, WEBCTRL, CGameMonitor and unclassified helpers; read when a helper is called from elsewhere.
- `client/functions/rendering.md` - index of DXC_ddraw/CSprite/CMyDib functions; read to find a draw routine, then the rendering docs below.
- `client/rendering/directdraw.md` - DirectDraw 7 surfaces, pixel formats, flip, lost surfaces; read before porting presentation.
- `client/rendering/blitters.md` - every Put* blit, blend tables, .pak read/skip vs PAKLib, helper DLLs; read before reproducing pixel output.
- `client/rendering/frame-pipeline.md` - per-frame order, draw layering, night/weather, timing; read when ordering draws.
- `client/rendering/effect-transparency.md` - why effects are additive on black, what the colour key does for them, why re-keying them fails; read before editing, re-keying or porting effect sprites.
- `client/rendering/porting-notes.md` - what a modern port must replicate, can drop, and the rendering bugs; read before starting a port.
