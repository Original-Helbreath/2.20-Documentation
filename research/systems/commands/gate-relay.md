# GSM_* records relayed game->gate->game (inside MSGID_SERVERSTOCKMSG)

Built by the sender and queued with `bStockMsgToGateServer`; the gate fans the stock buffer out to every OTHER game server (SendMsgToAllGameServers with bIsOwnSend FALSE skips the originator, G GateCore.cpp:287-289, 1017-1031), so the sender must also apply the effect locally; each receiver dispatches in `ServerStockMsgHandler` (S Game.cpp, case per GSM). These are how chat commands reach players on other servers (/fi, /to, ^ chat, /summonplayer, /shutup, /setforcerecalltime) and how crusade state stays in sync. Gate side never parses GSM contents.

Site format `Function@<S|C|G> file:line`: S = `sources/hgserver-librehelbreath/src/`, C = `sources/client-2.20/`, G = `sources/gateserver-librehelbreath/`. `[commented]` = inside // or /* */; `[not compiled]` = under a switch that is off in the shipped build. Dir: C client, S game server, L Log server (no 2.20 source), G gate, X external monitor. Columns and status rules: `method.md`.

| Name | Value | Dir | Sender sites (ref) | Handler sites (case) | Packing case | Effect / intent | System | Secondary | Build gates seen | Status | Params, overloading, notes | Headers |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `GSM_REQUEST_FINDCHARACTER` | 0x01 | - | CheckAndNotifyPlayerConnection@S Game.cpp:34700; ToggleWhisperPlayer@S Game.cpp:34779 | ServerStockMsgHandler@S Game.cpp:49682 | - | Ask other servers whether a player is online (/fi, /to). | chat | - | - | live | - | cli,gate,srv |
| `GSM_RESPONSE_FINDCHARACTER` | 0x02 | - | GSM_RequestFindCharacter@S Game.cpp:49832 | ServerStockMsgHandler@S Game.cpp:49701 | - | Answer: player is here (map, location). | chat | - | - | live | - | cli,gate,srv |
| `GSM_GRANDMAGICRESULT` | 0x03 | - | CalcMeteorStrikeEffectHandler@S Game.cpp:49994 | ServerStockMsgHandler@S Game.cpp:49530 | - | Meteor-strike damage result to all servers. | crusade | - | - | live | - | cli,gate,srv |
| `GSM_GRANDMAGICLAUNCH` | 0x04 | - | _GrandMagicLaunchMsgSend@S Game.cpp:50065 | ServerStockMsgHandler@S Game.cpp:49513 | - | Announce meteor launch. | crusade | - | - | live | - | cli,gate,srv |
| `GSM_COLLECTEDMANA` | 0x05 | - | SendCollectedMana@S Game.cpp:48153 | ServerStockMsgHandler@S Game.cpp:49501 | - | Share collected mana totals. | crusade | - | - | live | - | cli,gate,srv |
| `GSM_BEGINCRUSADE` | 0x06 | - | GlobalStartCrusadeMode@S Game.cpp:50291 | ServerStockMsgHandler@S Game.cpp:49488 | - | Start crusade on all servers (/begincrusadetotalwar or schedule). | crusade | gm | - | live | - | cli,gate,srv |
| `GSM_ENDCRUSADE` | 0x07 | - | CalcMeteorStrikeEffectHandler@S Game.cpp:49966; ManualEndCrusadeMode@S Game.cpp:50539 | ServerStockMsgHandler@S Game.cpp:49495 | - | End crusade on all servers. | crusade | gm | - | live | - | cli,gate,srv |
| `GSM_MIDDLEMAPSTATUS` | 0x08 | - | SyncMiddlelandMapInfo@S Game.cpp:50098 | ServerStockMsgHandler@S Game.cpp:49461 | - | Sync middleland structure status. | crusade | map | - | live | - | cli,gate,srv |
| `GSM_SETGUILDTELEPORTLOC` | 0x09 | - | RequestSetGuildTeleportLocHandler@S Game.cpp:48469 | ServerStockMsgHandler@S Game.cpp:49427 | - | Sync guild teleport point. | crusade | guild | - | live | - | cli,gate,srv |
| `GSM_CONSTRUCTIONPOINT` | 0x0A | - | NpcKilledHandler@S Game.cpp:14673; ClientKilledHandler@S Game.cpp:19501; CheckCommanderConstructionPoint@S Game.cpp:50336 | ServerStockMsgHandler@S Game.cpp:49416 | - | Sync commander construction points. | crusade | guild | - | live | - | cli,gate,srv |
| `GSM_SETGUILDCONSTRUCTLOC` | 0x0B | - | RequestSetGuildConstructLocHandler@S Game.cpp:48572 | ServerStockMsgHandler@S Game.cpp:49444 | - | Sync construction point. | crusade | guild | - | live | - | cli,gate,srv |
| `GSM_CHATMSG` | 0x0C | - | ChatMsgHandler@S Game.cpp:10663; ChatMsgHandler@S Game.cpp:10684 | ServerStockMsgHandler@S Game.cpp:49398 | - | Cross-server guild chat (^) and GM global notice. | chat | guild;gm | - | live | - | cli,gate,srv |
| `GSM_WHISFERMSG` | 0x0D | - | ChatMsgHandler@S Game.cpp:11179 | ServerStockMsgHandler@S Game.cpp:49368 | - | Cross-server whisper. | chat | - | - | live | - | cli,srv |
| `GSM_DISCONNECT` | 0x0E | - | DeleteClient@S Game.cpp:2624 | ServerStockMsgHandler@S Game.cpp:49353 | - | Player left; clear remote whisper targets. | session | - | - | live | - | cli,srv |
| `GSM_REQUEST_SUMMONPLAYER` | 0x0F | - | AdminOrder_SummonPlayer@S Game.cpp:44388 | ServerStockMsgHandler@S Game.cpp:49573 | - | /summonplayer target on another server. | gm | map | - | live | - | cli,srv |
| `GSM_REQUEST_SHUTUPPLAYER` | 0x10 | - | ShutUpPlayer@S Game.cpp:36159 | ServerStockMsgHandler@S Game.cpp:49626 | - | /shutup target on another server. | gm | chat | - | live | - | cli,srv |
| `GSM_RESPONSE_SHUTUPPLAYER` | 0x11 | - | GSM_RequestShutupPlayer@S Game.cpp:49778 | ServerStockMsgHandler@S Game.cpp:49651 | - | Mute confirmation. | gm | chat | - | live | - | cli,srv |
| `GSM_REQUEST_SETFORCERECALLTIME` | 0x12 | - | AdminOrder_SetForceRecallTime@S Game.cpp:44052 | ServerStockMsgHandler@S Game.cpp:49744 | - | /setforcerecalltime on all servers. | gm | pk-law | - | live | - | cli,srv |

Rows: 18. Status counts: live 18.
