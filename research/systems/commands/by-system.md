# Commands by system

System -> command matrix built from the per-family files (generated, `gen_bysys.py`). Prefixes: C = DEF_COMMONTYPE_, M = motion DEF_OBJECT*, N = DEF_NOTIFY_, ID = MSGID_, GSM = gate stock record, / = server slash command, local = client-local slash command, pfx = chat prefix mode. `(x)` = not live (dead, unused, never sent or not compiled; see the family file). Generic confirm/reject, Log result codes and enter-game modes (`other-subtypes.md`) are left out.

Straddlers: a command listed in another system's 'also serves' column has that system as a secondary owner. The worst offenders are discussed in `overloaded.md`.

## Systems

| System | Scope | Primary count | Also serves (secondary) count |
|---|---|---|---|
| session | connect, login hand-off, init, keep-alive, save/logout, server change, shutdown | 30 | 37 |
| envelope | pure multiplexing envelopes whose jobs live in the sub-command files: MSGID_COMMAND_COMMON (C->S) and MSGID_NOTIFY (S->C); no domain of their own | 2 | 0 |
| config | Log server pushing content configs into the game server at start-up | 13 | 3 |
| movement | walk/run/stop/turn, move validation, panning | 11 | 6 |
| map | map data, teleport, weather, day/night, map status/minimap | 9 | 28 |
| entity | NPCs, dynamic objects (fire, fish, minerals), full-object refresh, NPC talk | 11 | 23 |
| combat | melee/ranged attacks, damage, death, super-attacks, safe-attack mode | 13 | 14 |
| pk-law | criminal status, PK penalties/rewards, hunter mode, force-recall | 9 | 8 |
| magic | spell learning, casting, effects | 12 | 5 |
| skills | skill mastery, skill use, skill caps, special abilities | 11 | 3 |
| crafting | alchemy (potions), manufacturing (BuildItem), item upgrade | 15 | 4 |
| gathering | fishing and farming (agriculture): skill-use driven resource gathering | 9 | 1 |
| item | inventory, equip, drop, pick-up, use, item counts, item positions, warehouse (bank) storage | 22 | 24 |
| trade | player-to-player give and two-way exchange | 12 | 0 |
| shop | NPC buy/sell/repair | 15 | 11 |
| character | stats, level, exp, hunger, reputation, profile, citizenship | 19 | 6 |
| party | party create/join/dismiss/info (relayed through the gate) | 14 | 1 |
| guild | guild create/disband/join/dismiss/ban, guild names | 30 | 16 |
| crusade | crusade war, duties, war units, construction, occupy flags, meteor/grand magic, energy sphere | 45 | 5 |
| arena | fightzone reservation and tickets | 6 | 0 |
| quest | quest contents, accept, cancel, rewards | 7 | 2 |
| chat | chat modes, whisper, notices, mute | 26 | 26 |
| gm | admin commands and admin-only info | 36 | 17 |
| monitor | external tools: bad-word monitor, manager program, server monitor | 11 | 0 |
| logging | item/GM/crusade logs sent to Log server or gate | 4 | 0 |
| account | Log-server account/character services (client<->Log), not seen by the game server | 38 | 0 |
| ui-only | client-local, never leaves the client | 2 | 2 |

## session

connect, login hand-off, init, keep-alive, save/logout, server change, shutdown.

- msgids.md (msgid, 26): ID:COMMAND_CHECKCONNECTION; ID:REQUEST_INITPLAYER; ID:RESPONSE_INITPLAYER; ID:REQUEST_INITDATA [+map]; ID:RESPONSE_INITDATA [+map]; ID:REQUEST_REGISTERGAMESERVER; ID:RESPONSE_REGISTERGAMESERVER [+config]; ID:REQUEST_PLAYERDATA; ID:RESPONSE_PLAYERDATA; ID:RESPONSE_SAVEPLAYERDATA_REPLY [+map]; ID:REQUEST_SAVEPLAYERDATALOGOUT; ID:REQUEST_NOSAVELOGOUT; ID:REQUEST_SAVEPLAYERDATA; ID:REQUEST_SAVEPLAYERDATA_REPLY [+map]; ID:GAMESERVERALIVE; ID:GAMESERVERDOWN; ID:TOTALGAMESERVERCLIENTS [+chat]; ID:ENTERGAMECONFIRM; ID:GAMESERVERSHUTDOWNED [+gm]; ID:REQUEST_FORCEDISCONECTACCOUNT; ID:SENDSERVERSHUTDOWNMSG [+chat]; ID:REQUEST_SETACCOUNTINITSTATUS(x); ID:REQUEST_SETACCOUNTWAITSTATUS; ID:REQUEST_CHECKACCOUNTPASSWORD; ID:SERVERSTOCKMSG [+chat,crusade,gm,guild]; ID:GATEWAY_CURRENTCONN
- gate-relay.md (gsm, 1): GSM:GSM_DISCONNECT
- notify-types.md (notify, 3): N:SERVERCHANGE [+map]; N:SERVERSHUTDOWN [+chat]; N:FORCEDISCONN
- also serves (primary owner after `<-`): ID:RESPONSE_CHECKCONNECTION<-monitor; ID:REQUEST_TELEPORT<-map; ID:EVENT_LOG<-entity; ID:PLAYERITEMLISTCONTENTS<-item; ID:PLAYERCHARACTERCONTENTS<-character; ID:WLSERVERFILECONTENTS<-config; ID:REQUEST_ENTERGAME<-account; ID:RESPONSE_ENTERGAME<-account; ID:REQUEST_SAVEARESDENOCCUPYFLAGDATA(x)<-crusade; ID:REQUEST_SAVEELVINEOCCUPYFLAGDATA(x)<-crusade; ID:MANAGER_SHUTDOWN(x)<-monitor; ID:REQUEST_NOTICEMENT<-chat; ID:RESPONSE_NOTICEMENT<-chat; ID:REGISTER_WORLDSERVER(x)<-account; ID:REGISTER_WORLDSERVERSOCKET(x)<-account; ID:REGISTER_WORLDSERVER_GAMESERVER(x)<-account; ID:REQUEST_CHARINFOLIST(x)<-account; ID:RESPONSE_CHARINFOLIST(x)<-account; ID:REQUEST_REMOVEGAMESERVER(x)<-account; ID:REQUEST_CLEARACCOUNTSTATUS(x)<-account; ID:WORLDSERVERACTIVATED(x)<-account; ID:RESPONSE_REGISTER_WORLDSERVERSOCKET(x)<-account; ID:REQUEST_BLOCKACCOUNT(x)<-account; ID:IPTIMECHANGE(x)<-account; ID:ACCOUNTTIMECHANGE(x)<-account; ID:REQUEST_IPTIME(x)<-account; ID:RESPONSE_IPTIME(x)<-account; ID:REQUEST_ALL_SERVER_REBOOT<-monitor; ID:REQUEST_EXEC_1DOTBAT<-monitor; ID:REQUEST_EXEC_2DOTBAT<-monitor; ID:REQUEST_GAMESERVER_SHUTDOWN<-monitor; ID:REQUEST_ALLGAMESERVER_SHUTDOWN<-monitor; ID:GETMINIMUMLOADGATEWAY<-account; /:/closeconn <name><-gm; /:/checkip <ip><-gm; /:/disconnectall <-gm; /:/shutdownthisserverrightnow <-gm

## envelope

pure multiplexing envelopes whose jobs live in the sub-command files: MSGID_COMMAND_COMMON (C->S) and MSGID_NOTIFY (S->C); no domain of their own.

- msgids.md (msgid, 2): ID:NOTIFY; ID:COMMAND_COMMON

## config

Log server pushing content configs into the game server at start-up.

- msgids.md (msgid, 13): ID:ITEMCONFIGURATIONCONTENTS [+item]; ID:NPCCONFIGURATIONCONTENTS [+entity]; ID:MAGICCONFIGURATIONCONTENTS [+magic]; ID:SKILLCONFIGURATIONCONTENTS [+skills]; ID:PORTIONCONFIGURATIONCONTENTS [+crafting]; ID:QUESTCONFIGURATIONCONTENTS [+quest]; ID:BUILDITEMCONFIGURATIONCONTENTS [+crafting]; ID:DUPITEMIDFILECONTENTS [+item]; ID:NOTICEMENTFILECONTENTS [+chat]; ID:WORLDCONFIGCONTENTS; ID:NPCITEMCONFIGCONTENTS [+entity,item]; ID:WLSERVERFILECONTENTS [+session]; ID:TELEPORTLISTCONTENTS [+map]
- also serves (primary owner after `<-`): ID:RESPONSE_REGISTERGAMESERVER<-session; ID:ARESDENOCCUPYFLAGSAVEFILECONTENTS(x)<-crusade; ID:ELVINEOCCUPYFLAGSAVEFILECONTENTS(x)<-crusade

## movement

walk/run/stop/turn, move validation, panning.

- msgids.md (msgid, 4): ID:COMMAND_MOTION [+combat,magic,item]; ID:RESPONSE_MOTION [+combat]; ID:REQUEST_PANNING [+gm,map]; ID:RESPONSE_PANNING [+map]
- motion.md (motion, 7): M:OBJECTSTOP [+entity]; M:OBJECTMOVE; M:OBJECTMOVE_CONFIRM [+map]; M:OBJECTMOVE_REJECT; M:OBJECTMOTION_CONFIRM; M:OBJECTMOTION_REJECT [+combat]; M:OBJECTRUN [+character]
- also serves (primary owner after `<-`): M:OBJECTGETITEM<-item; M:OBJECTDAMAGEMOVE<-combat; M:OBJECTATTACKMOVE<-combat; N:DAMAGEMOVE<-combat; ID:EVENT_MOTION<-entity; /:/setobservermode <-gm

## map

map data, teleport, weather, day/night, map status/minimap.

- msgids.md (msgid, 5): ID:REQUEST_TELEPORT [+session]; ID:REQUEST_TELEPORT_LIST [+shop]; ID:RESPONSE_TELEPORT_LIST [+shop]; ID:REQUEST_CHARGED_TELEPORT [+shop]; ID:RESPONSE_CHARGED_TELEPORT [+shop]
- notify-types.md (notify, 4): N:SHOWMAP [+item]; N:TIMECHANGE; N:WHETHERCHANGE; N:LOCKEDMAP [+pk-law]
- also serves (primary owner after `<-`): C:REQ_USEITEM<-item; C:REQUEST_MAPSTATUS<-crusade; C:SETGUILDTELEPORTLOC<-crusade; C:GUILDTELEPORT<-crusade; C:SETGUILDCONSTRUCTLOC<-crusade; M:OBJECTMOVE_CONFIRM<-movement; N:SERVERCHANGE<-session; N:TOBERECALLED<-pk-law; N:MAPSTATUSNEXT<-crusade; N:MAPSTATUSLAST<-crusade; N:TCLOC<-crusade; N:FORCERECALLTIME<-pk-law; ID:REQUEST_INITDATA<-session; ID:RESPONSE_INITDATA<-session; ID:RESPONSE_SAVEPLAYERDATA_REPLY<-session; ID:REQUEST_SAVEPLAYERDATA_REPLY<-session; ID:TELEPORTLISTCONTENTS<-config; ID:REQUEST_CIVILRIGHT<-character; ID:DYNAMICOBJECT<-entity; ID:REQUEST_PANNING<-movement; ID:RESPONSE_PANNING<-movement; GSM:GSM_MIDDLEMAPSTATUS<-crusade; GSM:GSM_REQUEST_SUMMONPLAYER<-gm; /:/teleport <map> <x> <y><-gm; /:/tp <map> <x> <y><-gm; /:/summonplayer <name><-gm; /:/setforcerecalltime <min><-gm; /:/setobservermode <-gm

## entity

NPCs, dynamic objects (fire, fish, minerals), full-object refresh, NPC talk.

- msgids.md (msgid, 5): ID:REQUEST_FULLOBJECTDATA; ID:EVENT_MOTION [+movement,combat]; ID:EVENT_LOG [+session]; ID:EVENT_COMMON [+item,magic,guild]; ID:DYNAMICOBJECT [+map]
- common-types.md (common, 1): C:TALKTONPC [+quest,shop]
- motion.md (motion, 2): M:OBJECTNULLACTION; M:OBJECTDEAD
- notify-types.md (notify, 3): N:NEWDYNAMICOBJECT(x); N:DELDYNAMICOBJECT(x); N:MONSTEREVENT_POSITION
- also serves (primary owner after `<-`): C:ITEMDROP<-item; C:SETITEM<-item; C:CLEARGUILDNAME<-guild; C:SUMMONWARUNIT<-crusade; C:REQGUILDNAME<-guild; M:OBJECTSTOP<-movement; M:OBJECTDYING<-combat; N:NPCTALK<-quest; N:REQGUILDNAMEANSWER<-guild; ID:NPCCONFIGURATIONCONTENTS<-config; ID:NPCITEMCONFIGCONTENTS<-config; /:/hold<-magic; /:/tgt <name><-magic; /:/free<-magic; /:/attack <name><-gm; /:/createfish <x> <y> <item><-gm; /:/summondemon <-gm; /:/unsummonall <-gm; /:/unsummonboss <-gm; /:/polymorph <name>(x)<-gm; /:/gns <npc><-gm; /:/summon <npc> <n><-gm; /:/energysphere <-crusade

## combat

melee/ranged attacks, damage, death, super-attacks, safe-attack mode.

- common-types.md (common, 2): C:TOGGLECOMBATMODE; C:TOGGLESAFEATTACKMODE [+pk-law]
- motion.md (motion, 6): M:OBJECTDYING [+entity]; M:OBJECTMOTION_ATTACK_CONFIRM; M:OBJECTATTACK; M:OBJECTDAMAGE; M:OBJECTDAMAGEMOVE [+movement]; M:OBJECTATTACKMOVE [+movement]
- notify-types.md (notify, 5): N:HP [+character]; N:KILLED; N:SAFEATTACKMODE; N:SUPERATTACKLEFT; N:DAMAGEMOVE [+movement]
- also serves (primary owner after `<-`): C:MAGIC<-magic; C:REQUEST_ACTIVATESPECABLTY<-skills; M:OBJECTMOTION_REJECT<-movement; N:ITEMLIFESPANEND<-item; N:MAGICEFFECTON<-magic; N:MAGICEFFECTOFF<-magic; N:GLOBALATTACKMODE<-gm; ID:COMMAND_MOTION<-movement; ID:RESPONSE_MOTION<-movement; ID:EVENT_MOTION<-entity; ID:REQUEST_RESTART<-character; /:/tgt <name><-magic; /:/attack <name><-gm; /:/setattackmode <0/1><-gm

## pk-law

criminal status, PK penalties/rewards, hunter mode, force-recall.

- common-types.md (common, 2): C:REQ_GETREWARDMONEY [+crusade]; C:REQUEST_HUNTMODE [+character]
- notify-types.md (notify, 7): N:PKPENALTY; N:PKCAPTURED; N:ENEMYKILLREWARD [+crusade]; N:TOBERECALLED [+map]; N:REWARDGOLD; N:FORCERECALLTIME [+map]; N:RESPONSE_HUNTMODE
- also serves (primary owner after `<-`): C:TOGGLESAFEATTACKMODE<-combat; C:REQ_GETHEROMANTLE<-crusade; N:ENEMYKILLS<-crusade; N:LOCKEDMAP<-map; ID:GAMEITEMLOG<-logging; GSM:GSM_REQUEST_SETFORCERECALLTIME<-gm; /:/setattackmode <0/1><-gm; /:/setforcerecalltime <min><-gm

## magic

spell learning, casting, effects.

- common-types.md (common, 3): C:MAGIC [+combat]; C:REQ_STUDYMAGIC [+shop]; C:GETMAGICABILITY [+skills]
- motion.md (motion, 1): M:OBJECTMAGIC
- chat-commands.md (slash, 3): /:/hold [+entity]; /:/tgt <name> [+entity,combat]; /:/free [+entity]
- notify-types.md (notify, 5): N:MAGICSTUDYSUCCESS; N:MAGICSTUDYFAIL; N:MP [+character]; N:MAGICEFFECTON [+combat]; N:MAGICEFFECTOFF [+combat]
- also serves (primary owner after `<-`): C:REQ_USEITEM<-item; ID:COMMAND_MOTION<-movement; ID:EVENT_COMMON<-entity; ID:MAGICCONFIGURATIONCONTENTS<-config; /:/setinvi <0/1><-gm

## skills

skill mastery, skill use, skill caps, special abilities.

- common-types.md (common, 4): C:REQ_TRAINSKILL(x); C:REQ_USESKILL [+crafting]; C:REQ_SETDOWNSKILLINDEX; C:REQUEST_ACTIVATESPECABLTY [+combat]
- notify-types.md (notify, 7): N:SKILLTRAINSUCCESS [+item]; N:SKILLTRAINFAIL(x); N:SKILL; N:SKILLUSINGEND; N:DOWNSKILLINDEXSET; N:SPECIALABILITYENABLED; N:SPECIALABILITYSTATUS
- also serves (primary owner after `<-`): C:REQ_GETFISHTHISTIME<-gathering; C:GETMAGICABILITY<-magic; ID:SKILLCONFIGURATIONCONTENTS<-config

## crafting

alchemy (potions), manufacturing (BuildItem), item upgrade.

- common-types.md (common, 4): C:REQ_CREATEPORTION; C:REQ_CREATEPOTION; C:BUILDITEM; C:UPGRADEITEM [+item]
- notify-types.md (notify, 11): N:NOMATCHINGPORTION; N:LOWPORTIONSKILL; N:PORTIONFAIL; N:PORTIONSUCCESS; N:BUILDITEMSUCCESS; N:BUILDITEMFAIL; N:ITEMATTRIBUTECHANGE [+item]; N:GIZONITEMUPGRADELEFT; N:GIZONEITEMCHANGE [+item]; N:GIZONITEMCANGE [+item]; N:ITEMUPGRADEFAIL
- also serves (primary owner after `<-`): C:REQ_USEITEM<-item; C:REQ_USESKILL<-skills; ID:PORTIONCONFIGURATIONCONTENTS<-config; ID:BUILDITEMCONFIGURATIONCONTENTS<-config

## gathering

fishing and farming (agriculture): skill-use driven resource gathering.

- common-types.md (common, 1): C:REQ_GETFISHTHISTIME [+skills]
- notify-types.md (notify, 8): N:EVENTFISHMODE; N:FISHCHANCE; N:FISHSUCCESS; N:FISHFAIL; N:FISHCANCELED; N:NOMOREAGRICULTURE; N:AGRICULTURESKILLLIMIT; N:AGRICULTURENOAREA
- also serves (primary owner after `<-`): /:/createfish <x> <y> <item><-gm

## item

inventory, equip, drop, pick-up, use, item counts, item positions, warehouse (bank) storage.

- msgids.md (msgid, 4): ID:REQUEST_RETRIEVEITEM [+shop]; ID:RESPONSE_RETRIEVEITEM [+shop]; ID:PLAYERITEMLISTCONTENTS [+session]; ID:REQUEST_SETITEMPOS [+ui-only]
- common-types.md (common, 5): C:ITEMDROP [+entity]; C:EQUIPITEM [+character]; C:RELEASEITEM [+character]; C:SETITEM [+entity]; C:REQ_USEITEM [+magic,map,crafting]
- motion.md (motion, 1): M:OBJECTGETITEM [+movement]
- notify-types.md (notify, 12): N:ITEMOBTAINED; N:CANNOTCARRYMOREITEM; N:ITEMLIFESPANEND [+combat]; N:ITEMTOBANK [+shop]; N:DROPITEMFIN_ERASEITEM; N:ITEMDEPLETED_ERASEITEM; N:SETITEMCOUNT; N:CANNOTITEMTOBANK [+shop]; N:ITEMPOSLIST [+ui-only]; N:ITEMRELEASED; N:DROPITEMFIN_COUNTCHANGED; N:ITEMCOLORCHANGE
- also serves (primary owner after `<-`): C:REQ_PURCHASEITEM<-shop; C:GIVEITEMTOCHAR<-trade; C:REQ_GETOCCUPYFLAG(x)<-crusade; C:REQ_GETHEROMANTLE<-crusade; C:UPGRADEITEM<-crafting; N:ITEMPURCHASED<-shop; N:SKILLTRAINSUCCESS<-skills; N:GIVEITEMFIN_ERASEITEM<-trade; N:SHOWMAP<-map; N:ITEMREPAIRED<-shop; N:ITEMSOLD<-shop; N:GIVEITEMFIN_COUNTCHANGED<-trade; N:ITEMATTRIBUTECHANGE<-crafting; N:GIZONEITEMCHANGE<-crafting; N:GIZONITEMCANGE<-crafting; ID:COMMAND_MOTION<-movement; ID:ITEMCONFIGURATIONCONTENTS<-config; ID:EVENT_COMMON<-entity; ID:DUPITEMIDFILECONTENTS<-config; ID:NPCITEMCONFIGCONTENTS<-config; ID:ITEMLOG(x)<-logging; ID:GAMEITEMLOG<-logging; /:/createitem <name> ...<-gm; /:/enableadmincreateitem 147258 <-gm

## trade

player-to-player give and two-way exchange.

- common-types.md (common, 5): C:GIVEITEMTOCHAR [+item,guild,shop]; C:EXCHANGEITEMTOCHAR; C:SETEXCHANGEITEM; C:CONFIRMEXCHANGEITEM; C:CANCELEXCHANGEITEM
- notify-types.md (notify, 7): N:GIVEITEMFIN_ERASEITEM [+item]; N:OPENEXCHANGEWINDOW; N:SETEXCHANGEITEM; N:CANCELEXCHANGEITEM; N:EXCHANGEITEMCOMPLETE; N:CANNOTGIVEITEM; N:GIVEITEMFIN_COUNTCHANGED [+item]

## shop

NPC buy/sell/repair.

- msgids.md (msgid, 1): ID:REQUEST_SELLITEMLIST
- common-types.md (common, 6): C:REQ_LISTCONTENTS(x); C:REQ_PURCHASEITEM [+item]; C:REQ_SELLITEM; C:REQ_REPAIRITEM; C:REQ_SELLITEMCONFIRM; C:REQ_REPAIRITEMCONFIRM
- notify-types.md (notify, 8): N:ITEMPURCHASED [+item]; N:NOTENOUGHGOLD; N:CANNOTSELLITEM; N:SELLITEMPRICE; N:CANNOTREPAIRITEM; N:REPAIRITEMPRICE; N:ITEMREPAIRED [+item]; N:ITEMSOLD [+item]
- also serves (primary owner after `<-`): C:GIVEITEMTOCHAR<-trade; C:REQ_STUDYMAGIC<-magic; C:TALKTONPC<-entity; N:ITEMTOBANK<-item; N:CANNOTITEMTOBANK<-item; ID:REQUEST_RETRIEVEITEM<-item; ID:RESPONSE_RETRIEVEITEM<-item; ID:REQUEST_TELEPORT_LIST<-map; ID:RESPONSE_TELEPORT_LIST<-map; ID:REQUEST_CHARGED_TELEPORT<-map; ID:RESPONSE_CHARGED_TELEPORT<-map

## character

stats, level, exp, hunger, reputation, profile, citizenship.

- msgids.md (msgid, 5): ID:PLAYERCHARACTERCONTENTS [+session]; ID:REQUEST_CIVILRIGHT [+map]; ID:RESPONSE_CIVILRIGHT; ID:LEVELUPSETTINGS; ID:REQUEST_RESTART [+combat]
- chat-commands.md (slash, 4): /:/setpf <text> [+chat]; /:/pf <name> [+chat]; /:/rep+ <name>; /:/rep- <name>
- notify-types.md (notify, 10): N:EXP; N:SP [+chat]; N:LEVELUP; N:LIMITEDLEVEL(x); N:CHARISMA(x); N:PLAYERPROFILE [+chat]; N:TRAVELERLIMITEDLEVEL; N:HUNGER; N:CANNOTRATING; N:RATINGPLAYER
- also serves (primary owner after `<-`): C:EQUIPITEM<-item; C:RELEASEITEM<-item; C:REQUEST_HUNTMODE<-pk-law; M:OBJECTRUN<-movement; N:HP<-combat; N:MP<-magic

## party

party create/join/dismiss/info (relayed through the gate).

- msgids.md (msgid, 1): ID:PARTYOPERATION
- common-types.md (common, 4): C:REQUEST_ACCEPTJOINPARTY; C:REQUEST_CREATENEWPARTY(x); C:REQUEST_JOINPARTY; C:RESPONSE_JOINPARTY(x)
- chat-commands.md (slash, 5): /:/createparty [+chat]; /:/joinparty <name> [+chat]; /:/dismissparty [+chat]; /:/getpartyinfo [+chat]; /:/deleteparty [+chat]
- chat-commands.md (prefix, 1): pfx:$ [+chat]
- notify-types.md (notify, 3): N:RESPONSE_CREATENEWPARTY(x); N:QUERY_JOINPARTY; N:PARTY
- also serves (primary owner after `<-`): ID:COMMAND_CHATMSG<-chat

## guild

guild create/disband/join/dismiss/ban, guild names.

- msgids.md (msgid, 7): ID:GUILDNOTIFY; ID:REQUEST_CREATENEWGUILD; ID:RESPONSE_CREATENEWGUILD; ID:REQUEST_DISBANDGUILD; ID:RESPONSE_DISBANDGUILD; ID:REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN; ID:REQUEST_UPDATEGUILDINFO_DELGUILDSMAN
- common-types.md (common, 7): C:JOINGUILDAPPROVE; C:JOINGUILDREJECT; C:DISMISSGUILDAPPROVE; C:DISMISSGUILDREJECT; C:CLEARGUILDNAME [+entity]; C:BANGUILD(x); C:REQGUILDNAME [+entity]
- chat-commands.md (slash, 2): /:/ban <name> [+chat]; /:/dissmiss <name>(x)
- chat-commands.md (prefix, 2): pfx:@ [+chat]; pfx:^ [+chat,gm]
- notify-types.md (notify, 12): N:QUERY_JOINGUILDREQPERMISSION; N:QUERY_DISMISSGUILDREQPERMISSION; N:WAITFORGUILDOPERATION(x); N:GUILDDISBANDED; N:EVENTMSGSTRING(x) [+chat]; N:CANNOTJOINMOREGUILDSMAN(x); N:NEWGUILDSMAN; N:DISMISSGUILDSMAN; N:NOGUILDMASTERLEVEL; N:SUCCESSBANGUILDMAN(x); N:CANNOTBANGUILDMAN; N:REQGUILDNAMEANSWER [+entity]
- also serves (primary owner after `<-`): C:GIVEITEMTOCHAR<-trade; C:REQ_GETOCCUPYFIGHTZONETICKET<-arena; C:REQUEST_SELECTCRUSADEDUTY<-crusade; C:REQUEST_HELP(x)<-crusade; C:SETGUILDTELEPORTLOC<-crusade; C:GUILDTELEPORT<-crusade; C:SETGUILDCONSTRUCTLOC<-crusade; N:HELP(x)<-crusade; ID:COMMAND_CHATMSG<-chat; ID:EVENT_COMMON<-entity; ID:REQUEST_FIGHTZONE_RESERVE<-arena; ID:SERVERSTOCKMSG<-session; GSM:GSM_SETGUILDTELEPORTLOC<-crusade; GSM:GSM_CONSTRUCTIONPOINT<-crusade; GSM:GSM_SETGUILDCONSTRUCTLOC<-crusade; GSM:GSM_CHATMSG<-chat

## crusade

crusade war, duties, war units, construction, occupy flags, meteor/grand magic, energy sphere.

- msgids.md (msgid, 7): ID:OCCUPYFLAGDATA(x); ID:REQUEST_SAVEARESDENOCCUPYFLAGDATA(x) [+session]; ID:REQUEST_SAVEELVINEOCCUPYFLAGDATA(x) [+session]; ID:ARESDENOCCUPYFLAGSAVEFILECONTENTS(x) [+config]; ID:ELVINEOCCUPYFLAGSAVEFILECONTENTS(x) [+config]; ID:COLLECTEDMANA(x); ID:METEORSTRIKE(x)
- common-types.md (common, 9): C:REQ_GETOCCUPYFLAG(x) [+item]; C:REQ_GETHEROMANTLE [+item,pk-law]; C:REQUEST_SELECTCRUSADEDUTY [+guild]; C:REQUEST_MAPSTATUS [+map]; C:REQUEST_HELP(x) [+guild]; C:SETGUILDTELEPORTLOC [+guild,map]; C:GUILDTELEPORT [+map,guild]; C:SUMMONWARUNIT [+entity]; C:SETGUILDCONSTRUCTLOC [+guild,map]
- chat-commands.md (slash, 3): /:/energysphere  [+gm,entity]; /:/begincrusadetotalwar  [+gm]; /:/endcrusadetotalwar  [+gm]
- gate-relay.md (gsm, 9): GSM:GSM_GRANDMAGICRESULT; GSM:GSM_GRANDMAGICLAUNCH; GSM:GSM_COLLECTEDMANA; GSM:GSM_BEGINCRUSADE [+gm]; GSM:GSM_ENDCRUSADE [+gm]; GSM:GSM_MIDDLEMAPSTATUS [+map]; GSM:GSM_SETGUILDTELEPORTLOC [+guild]; GSM:GSM_CONSTRUCTIONPOINT [+guild]; GSM:GSM_SETGUILDCONSTRUCTLOC [+guild]
- notify-types.md (notify, 17): N:ENEMYKILLS [+pk-law]; N:NOTFLAGSPOT; N:ENERGYSPHERECREATED; N:ENERGYSPHEREGOALIN; N:CRUSADE; N:DUTYSELECTED(x); N:MAPSTATUSNEXT [+map]; N:MAPSTATUSLAST [+map]; N:HELP(x) [+guild]; N:HELPFAILED(x); N:METEORSTRIKECOMING; N:METEORSTRIKEHIT; N:GRANDMAGICRESULT; N:NOMORECRUSADESTRUCTURE; N:CONSTRUCTIONPOINT; N:TCLOC [+map]; N:CANNOTCONSTRUCT
- also serves (primary owner after `<-`): C:REQ_GETREWARDMONEY<-pk-law; N:ENEMYKILLREWARD<-pk-law; ID:COMMAND_CHATMSG<-chat; ID:GAMECRUSADELOG<-logging; ID:SERVERSTOCKMSG<-session

## arena

fightzone reservation and tickets.

- msgids.md (msgid, 2): ID:REQUEST_FIGHTZONE_RESERVE [+guild]; ID:RESPONSE_FIGHTZONE_RESERVE
- common-types.md (common, 1): C:REQ_GETOCCUPYFIGHTZONETICKET [+guild]
- chat-commands.md (slash, 2): /:/reservefightzone <n> [+gm]; /:/getticket  [+gm]
- notify-types.md (notify, 1): N:FIGHTZONERESERVE

## quest

quest contents, accept, cancel, rewards.

- common-types.md (common, 2): C:QUESTACCEPTED; C:REQUEST_CANCELQUEST
- notify-types.md (notify, 5): N:NPCTALK [+entity]; N:QUESTCONTENTS; N:QUESTABORTED; N:QUESTCOMPLETED; N:QUESTREWARD
- also serves (primary owner after `<-`): C:TALKTONPC<-entity; ID:QUESTCONFIGURATIONCONTENTS<-config

## chat

chat modes, whisper, notices, mute.

- msgids.md (msgid, 3): ID:COMMAND_CHATMSG [+gm,party,guild,crusade]; ID:REQUEST_NOTICEMENT [+session]; ID:RESPONSE_NOTICEMENT [+session]
- chat-commands.md (slash, 2): /:/fi <name> [+gm]; /:/to [name]
- chat-commands.md (local, 6): local:/whon; local:/whoff; local:/shon; local:/shoff; local:/tooff <name>; local:/toon <name>
- chat-commands.md (prefix, 5): pfx:(none); pfx:!; pfx:~; pfx:#; pfx:whisper
- gate-relay.md (gsm, 4): GSM:GSM_REQUEST_FINDCHARACTER; GSM:GSM_RESPONSE_FINDCHARACTER; GSM:GSM_CHATMSG [+guild,gm]; GSM:GSM_WHISFERMSG
- notify-types.md (notify, 6): N:PLAYERONGAME [+gm]; N:PLAYERNOTONGAME; N:WHISPERMODEON; N:WHISPERMODEOFF; N:PLAYERSHUTUP [+gm]; N:NOTICEMSG
- also serves (primary owner after `<-`): N:EVENTMSGSTRING(x)<-guild; N:SP<-character; N:TOTALUSERS<-gm; N:PLAYERPROFILE<-character; N:SERVERSHUTDOWN<-session; ID:NOTICEMENTFILECONTENTS<-config; ID:TOTALGAMESERVERCLIENTS<-session; ID:BWM_INIT<-monitor; ID:BWM_COMMAND_SHUTUP<-monitor; ID:SENDSERVERSHUTDOWNMSG<-session; ID:SERVERSTOCKMSG<-session; GSM:GSM_REQUEST_SHUTUPPLAYER<-gm; GSM:GSM_RESPONSE_SHUTUPPLAYER<-gm; /:/who<-gm; /:/setpf <text><-character; /:/pf <name><-character; /:/shutup <name> <time><-gm; /:/ban <name><-guild; /:/createparty<-party; /:/joinparty <name><-party; /:/dismissparty<-party; /:/getpartyinfo<-party; /:/deleteparty<-party; pfx:$<-party; pfx:@<-guild; pfx:^<-guild

## gm

admin commands and admin-only info.

- msgids.md (msgid, 1): ID:ADMINUSER(x)
- chat-commands.md (slash, 24): /:/who [+chat]; /:/shutup <name> <time> [+chat]; /:/closeconn <name> [+session]; /:/attack <name> [+entity,combat]; /:/createfish <x> <y> <item> [+gathering,entity]; /:/teleport <map> <x> <y> [+map]; /:/tp <map> <x> <y> [+map]; /:/summondemon  [+entity]; /:/summonplayer <name> [+map]; /:/unsummonall  [+entity]; /:/unsummonboss  [+entity]; /:/checkip <ip> [+session]; /:/polymorph <name>(x) [+entity]; /:/setinvi <0/1> [+magic]; /:/gns <npc> [+entity]; /:/setattackmode <0/1> [+combat,pk-law]; /:/setforcerecalltime <min> [+pk-law,map]; /:/summon <npc> <n> [+entity]; /:/disconnectall  [+session]; /:/createitem <name> ... [+item]; /:/enableadmincreateitem 147258  [+item]; /:/enableadmincommand 0909114 ; /:/shutdownthisserverrightnow  [+session]; /:/setobservermode  [+movement,map]
- gate-relay.md (gsm, 4): GSM:GSM_REQUEST_SUMMONPLAYER [+map]; GSM:GSM_REQUEST_SHUTUPPLAYER [+chat]; GSM:GSM_RESPONSE_SHUTUPPLAYER [+chat]; GSM:GSM_REQUEST_SETFORCERECALLTIME [+pk-law]
- notify-types.md (notify, 7): N:TOTALUSERS [+chat]; N:ADMINUSERLEVELLOW; N:DEBUGMSG(x); N:IPACCOUNTINFO; N:ADMINIFO; N:OBSERVERMODE; N:GLOBALATTACKMODE [+combat]
- also serves (primary owner after `<-`): N:PLAYERONGAME<-chat; N:PLAYERSHUTUP<-chat; ID:COMMAND_CHATMSG<-chat; ID:GAMESERVERSHUTDOWNED<-session; ID:GAMEMASTERLOG<-logging; ID:REQUEST_PANNING<-movement; ID:SERVERSTOCKMSG<-session; GSM:GSM_BEGINCRUSADE<-crusade; GSM:GSM_ENDCRUSADE<-crusade; GSM:GSM_CHATMSG<-chat; /:/fi <name><-chat; /:/reservefightzone <n><-arena; /:/energysphere <-crusade; /:/begincrusadetotalwar <-crusade; /:/endcrusadetotalwar <-crusade; /:/getticket <-arena; pfx:^<-guild

## monitor

external tools: bad-word monitor, manager program, server monitor.

- msgids.md (msgid, 11): ID:RESPONSE_CHECKCONNECTION [+session]; ID:BWM_INIT [+chat]; ID:BWM_COMMAND_SHUTUP [+chat]; ID:MANAGER_INIT; ID:MANAGER_SHUTDOWN(x) [+session]; ID:REQUEST_ALL_SERVER_REBOOT [+session]; ID:REQUEST_EXEC_1DOTBAT [+session]; ID:REQUEST_EXEC_2DOTBAT [+session]; ID:MONITORALIVE; ID:REQUEST_GAMESERVER_SHUTDOWN [+session]; ID:REQUEST_ALLGAMESERVER_SHUTDOWN [+session]

## logging

item/GM/crusade logs sent to Log server or gate.

- msgids.md (msgid, 4): ID:ITEMLOG(x) [+item]; ID:GAMEMASTERLOG [+gm]; ID:GAMEITEMLOG [+item,pk-law]; ID:GAMECRUSADELOG [+crusade]

## account

Log-server account/character services (client<->Log), not seen by the game server.

- msgids.md (msgid, 38): ID:REQUEST_REGISTERDBSERVER(x); ID:RESPONSE_REGISTERDBSERVER(x); ID:REQUEST_LOGIN; ID:REQUEST_CREATENEWACCOUNT; ID:RESPONSE_LOG; ID:REQUEST_CREATENEWCHARACTER; ID:REQUEST_ENTERGAME [+session]; ID:RESPONSE_ENTERGAME [+session]; ID:REQUEST_DELETECHARACTER; ID:REQUEST_CHANGEPASSWORD; ID:RESPONSE_CHANGEPASSWORD; ID:REQUEST_INPUTKEYCODE; ID:RESPONSE_INPUTKEYCODE; ID:ACCOUNTINFOCHANGE(x); ID:IPINFOCHANGE(x); ID:ANNOUNCEACCOUNTNEWPASSWORD(x); ID:REQUEST_IPIDSTATUS(x); ID:RESPONSE_IPIDSTATUS(x); ID:REQUEST_ACCOUNTCONNSTATUS(x); ID:RESPONSE_ACCOUNTCONNSTATUS(x); ID:REQUEST_CLEARACCOUNTCONNSTATUS(x); ID:RESPONSE_CLEARACCOUNTCONNSTATUS(x); ID:REQUEST_NOCOUNTINGSAVELOGOUT(x); ID:REGISTER_WORLDSERVER(x) [+session]; ID:REGISTER_WORLDSERVERSOCKET(x) [+session]; ID:REGISTER_WORLDSERVER_GAMESERVER(x) [+session]; ID:REQUEST_CHARINFOLIST(x) [+session]; ID:RESPONSE_CHARINFOLIST(x) [+session]; ID:REQUEST_REMOVEGAMESERVER(x) [+session]; ID:REQUEST_CLEARACCOUNTSTATUS(x) [+session]; ID:WORLDSERVERACTIVATED(x) [+session]; ID:RESPONSE_REGISTER_WORLDSERVERSOCKET(x) [+session]; ID:REQUEST_BLOCKACCOUNT(x) [+session]; ID:IPTIMECHANGE(x) [+session]; ID:ACCOUNTTIMECHANGE(x) [+session]; ID:REQUEST_IPTIME(x) [+session]; ID:RESPONSE_IPTIME(x) [+session]; ID:GETMINIMUMLOADGATEWAY [+session]

## ui-only

client-local, never leaves the client.

- chat-commands.md (local, 2): local:/showframe; local:/enabletogglescreen
- also serves (primary owner after `<-`): N:ITEMPOSLIST<-item; ID:REQUEST_SETITEMPOS<-item

Total commands placed: 432 (each exactly once as primary) = 371 family rows (MSGID 139, COMMONTYPE 56, motion 17, NOTIFY 141, GSM 18) + 45 server slash commands + 8 chat prefix modes + 8 client-local commands. The 36 other-subtypes (`other-subtypes.md`) are not placed. Entry-string and alias counts are in `method.md`.
