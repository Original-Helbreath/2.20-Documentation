# Timers and delay events

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 9

## CGame::OnTimer - SRV Game.cpp:3668-3825

Multimedia-timer handler. The base tick is 300 ms (Wmain.cpp:254 `_StartTimer(300)`), so "every tick" means every 300 ms and the 600 ms MobGenerator fires on every second tick. Every tick GameProcess; 3 s client checks, gate heartbeat (MSGID_GAMESERVERALIVE), day/night, start-game gate; 1 s delay events + stock flush + sub-log socket bring-up + final shutdown countdown; 3 s dynamic objects, notices, special events, energy spheres; 600 ms MobGenerator; 3 min occupy-flag save (SaveOccupyFlagData only if m_iMiddlelandMapIndex >= 0, 3760; the same block reseeds srand, 3769; SaveOccupyFlagData itself is a no-op); 4 s fish, collected mana, crusade starter; 20 s weather; 2 h fightzone reset; shutdown drain; 10 s map-sector info and minerals (AgingMapSectorInfo every 5th pass, 3819); also InvalidateRect each 3 s (3687). The shutdown drain polls every 2 s (3791): shutdown codes 3/4 call bInit and bump m_iAutoRebootingCount, otherwise a 20-step final countdown ends in SendMessage(WM_DESTROY) (3797-3805, 3716-3723). GServer.cfg is read only indirectly through bInit on auto-reboot.

- sig: `void CGame::OnTimer(char cType)`
- callers: WndProc (Wmain.cpp:89) only. The name-scan caller ThreadProc never reaches OnTimer: its condition `if (G_pGame = NULL)` (Wmain.cpp:61) is an assignment, so the branch is never taken, and no thread is ever created (no _beginthread/CreateThread; only an `extern` at Game.cpp:265) ([GSO-040](../../issues/game-server-other.md#gso-040-threadproc-never-runs)).
- callees: AgingMapSectorInfo, CheckClientResponseTime, CheckDayOrNightMode, CheckDynamicObjectList, CrusadeWarStarter, DelayEventProcessor, DynamicObjectEffectProcessor, EnergySphereProcessor, FightzoneReserveProcessor, FishGenerator, FishProcessor, GameProcess, MineralGenerator, MobGenerator, NoticeHandler, PutLogFileList, PutLogList, SaveOccupyFlagData, SendCollectedMana, SendMsgToGateServer, SendStockMsgToGateServer, SpecialEventHandler, SyncMiddlelandMapInfo, UpdateMapSectorInfo, WhetherProcessor, XSocket, _CheckGateSockConnection, _iForcePlayerDisconect, bConnect, bInit ...
- messages: MSGID_GAMESERVERALIVE
- cfg: none read directly (GServer.cfg only through bInit at 3799); the callee list above is cut off at 30 of 31 callees (bInitBufferSize is the hidden one)
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 6, untranslated quote): 화면 갱신 / 2002-12-8 World server 보안 설정을 위해 / 게임을 시작할 준비가 되었다. 메시지를 보낸다.

## CGame::CheckClientResponseTime - SRV Game.cpp:3827-4142

3 s per-client tick: timeout, timers, regen, poison, autosave, exp stock, auto exp, abilities, locked-map timer (the "jail" counter, 3986-3992), force recall, level limits, criticals. Also: hourly speed-hack check, where more than 300000 exp gained in an hour sets a one-month PenaltyBlock and calls DeleteClient (4027-4057; `#ifndef DEF_TESTSERVER`, so live in this build); skill-message flood kick (4016-4021); hunger tick (3880-3894) which also stretches HP/MP/SP regen (3897-3919); hunter recall from mapped zones (4097-4110).

- sig: `void CGame::CheckClientResponseTime()`
- callers: CGame::OnTimer
- callees: CalcExpStock, CheckCommanderConstructionPoint, CheckCrusadeResultCalculation, CheckUniqueItemEquipment, DeleteClient, GetExp, PoisonEffect, PutLogFileList, PutLogList, RequestTeleportHandler, SendEventToNearClient_TypeA, SendNotifyMsg, TimeHitPointsUp, TimeManaPointsUp, TimeStaminarPointsUp, _SendMapStatus, bSendMsgToLS
- messages: MSGID_EVENT_MOTION, MSGID_REQUEST_SAVEPLAYERDATA
- notifies: DEF_NOTIFY_HUNGER, DEF_NOTIFY_SPECIALABILITYENABLED, DEF_NOTIFY_SPECIALABILITYSTATUS, DEF_NOTIFY_SUPERATTACKLEFT, DEF_NOTIFY_TOBERECALLED
- types: DEF_OBJECTNULLACTION
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): 시간을 구한다음 iWarPeriod를 구한다. / 월요일 / 화요일

## CGame::DelayEventProcess - SRV Game.cpp:20414-20417

Empty stub ([GSO-041](../../issues/game-server-other.md#gso-041-delayeventprocess-is-an-empty-stub-and-the-damageobject-delay-event-does-nothing)).

- sig: `void CGame::DelayEventProcess()`
- callers: CGame::GameProcess

## CGame::bRegisterDelayEvent - SRV Game.cpp:32991-33019

Adds a delayed event. Returns FALSE when DEF_MAXDELAYEVENTS is full; stores dwLastTime as an absolute trigger time (33013).

- sig: `BOOL CGame::bRegisterDelayEvent(int iDelayType, int iEffectType, DWORD dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3)`
- callers: CGame::DynamicObjectEffectProcessor, CGame::MeteorStrikeHandler, CGame::MeteorStrikeMsgHandler, CGame::NpcBehavior_Attack, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::UseItemHandler, CGame::iCalculateAttackEffect

## CGame::DelayEventProcessor - SRV Game.cpp:33021-33134

Fires due delayed events. Handled types (33033-33128): CALCMETEORSTRIKEEFFECT, DOMETEORSTRIKEDAMAGE, METEORSTRIKE, USEITEM_SKILL, MAGICRELEASE; DAMAGEOBJECT is a no-op (33071) ([GSO-041](../../issues/game-server-other.md#gso-041-delayeventprocess-is-an-empty-stub-and-the-damageobject-delay-event-does-nothing)).

- sig: `void CGame::DelayEventProcessor()`
- callers: CGame::OnTimer
- callees: CalcMeteorStrikeEffectHandler, DoMeteorStrikeDamageHandler, MeteorStrikeHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetBerserkFlag, SetIceFlag, SetInvisibilityFlag, iCalculateUseSkillItemEffect
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_SKILLUSINGEND
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 6, untranslated quote): 이벤트가 동작할 시간이 됐다. 동작후 삭제된다. / 아이템 사용에 따른 결과 계산, 통보 / 기술 사용이 무효화 되었다면 무시.

## CGame::bRemoveFromDelayEventList - SRV Game.cpp:33138-33163

Removes delayed events of a target; removes all effects when iEffectType is NULL (33145).

- sig: `BOOL CGame::bRemoveFromDelayEventList(int iH, char cType, int iEffectType)`
- callers: CGame::DeleteClient, CGame::DeleteNpc, CGame::DoMeteorStrikeDamageHandler, CGame::DynamicObjectEffectProcessor, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::UseItemHandler, CGame::UseSkillHandler, CGame::_CheckMiningAction, CGame::iCalculateAttackEffect, CGame::iClientMotion_Magic_Handler
- Korean comments (UTF-8, first 2, untranslated quote): Effect 종류에 상관없이 모두 삭제 / 해당 Effect만 삭제.

## _TimerFunc - SRV Wmain.cpp:343-346

timeSetEvent callback: posts WM_USER_TIMERSIGNAL to the window.

- sig: `void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)`

## _StartTimer - SRV Wmain.cpp:350-360

timeSetEvent periodic timer: timeBeginPeriod(min resolution) then timeSetEvent(dwTime, 0, _TimerFunc, ..., TIME_PERIODIC) (Wmain.cpp:355-357); Initialize passes 300 ms.

- sig: `MMRESULT _StartTimer(DWORD dwTime)`
- callers: Initialize

## _StopTimer - SRV Wmain.cpp:364-374

timeKillEvent, then timeEndPeriod (Wmain.cpp:369-372).

- sig: `void _StopTimer(MMRESULT timerid)`
- callers: OnDestroy
