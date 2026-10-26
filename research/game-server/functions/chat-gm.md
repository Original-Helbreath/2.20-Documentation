# Chat and GM commands

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1]. Admin level numbers below are the minimum `m_iAdminUserLevel` measured in the active build (DEF_TAIWAN and DEF_TESTSERVER off).
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 35

## CGame::ChatMsgHandler - SRV Game.cpp:10539-11225

Chat dispatcher: modes by prefix ($ party, @ guild, ! shout, ^ guild/all servers, ~ side, / command), SP costs, confuse-language, whisper, GM logging.

- sig: `void CGame::ChatMsgHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: AdminOrder_CallGuard, AdminOrder_CheckIP, AdminOrder_CloseConn, AdminOrder_CreateFish, AdminOrder_CreateItem, AdminOrder_DisconnectAll, AdminOrder_EnableAdminCommand, AdminOrder_EnableAdminCreateItem, AdminOrder_GetFightzoneTicket, AdminOrder_GetNpcStatus, AdminOrder_Polymorph, AdminOrder_ReserveFightzone, AdminOrder_SetAttackMode, AdminOrder_SetForceRecallTime, AdminOrder_SetInvi, AdminOrder_SetObserverMode, AdminOrder_Summon, AdminOrder_SummonDemon, AdminOrder_SummonPlayer, AdminOrder_Teleport, AdminOrder_UnsummonAll, AdminOrder_UnsummonBoss, CheckAndNotifyPlayerConnection, EnergySphereProcessor, GetPartyInfoHandler, GetPlayerProfile, GlobalStartCrusadeMode, ManualEndCrusadeMode, PutLogList, RequestCreatePartyHandler, RequestJoinPartyHandler, RequestDismissPartyHandler, RequestDeletePartyHandler, SaveOccupyFlagData, SetPlayerProfile, SetPlayerReputation, SetSummonMobAction, ShutUpPlayer, ToggleWhisperPlayer, UserCommand_BanGuildsman, UserCommand_DissmissGuild, SendNotifyMsg, bSendMsgToLS, bStockMsgToGateServer, iDice, iSendMsg (callee list completed from the body; the scan listed it truncated with "...").
- commands handled inline (level gates): `/begincrusadetotalwar` and `/endcrusadetotalwar` (>= 3), `/energysphere` (>= 2), `/shutdownthisserverrightnow` (== 4), `/setobservermode` (>= 3; >= 1 under DEF_TAIWAN), `/getticket` (>= 2), `/who` (>= 3 under DEF_CHINESE, 10753) (10956-11043)
- messages: MSGID_GAMEMASTERLOG, MSGID_GAMESERVERSHUTDOWNED
- notifies: DEF_NOTIFY_SP, DEF_NOTIFY_TOTALUSERS
- switches: DEF_CHINESE, DEF_TAIWAN, DEF_TESTSERVER, defined
- Korean comments (UTF-8, first 6, untranslated quote): v1.41 ShutUp Time이 걸려 있으면 완전히 말을 할 수 없다. 강력 제재 / 본인의 이름이 기입되어 있지 않다면 리턴 / 관람자라면 채팅을 할 수 없다.

## CGame::ChatMsgHandlerGSM - SRV Game.cpp:11228-11282

Chat arriving from other servers (guild/global). Quirk: in guild mode (case 1) the loop uses `break` (not `continue`) on the first client with m_bIsInitComplete == FALSE (11266), so later guild members are skipped. Global mode (2/10) sends to every client including uninitialised ones, with no error handling (11276-11279) ([GSO-014](../../issues/game-server-other.md#gso-014-guild-chat-from-other-servers-stops-at-the-first-uninitialised-client)).

- sig: `void CGame::ChatMsgHandlerGSM(int iMsgType, int iV1, char * pName, char * pData, DWORD dwMsgSize)`
- callers: CGame::ServerStockMsgHandler
- callees: iSendMsg
- messages: MSGID_COMMAND_CHATMSG
- Korean comments (UTF-8, first 4, untranslated quote): 다른 서버에서 온 길드 메시지 / 같은 길드 메시지 전송 / 다른 서버에서 온 전체 메시지

## CGame::CheckAndNotifyPlayerConnection - SRV Game.cpp:34638-34722

/fi: is a player online and where.

- sig: `void CGame::CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, DWORD dwSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, bStockMsgToGateServer, pGet
- notifies: DEF_NOTIFY_PLAYERONGAME
- Korean comments (UTF-8, first 6, untranslated quote): cName의 이름을 가진 플레이어가 접속중인지 찾는다. / 같은 이름을 가진 캐릭터를 찾았다. / 01-12-10 접속 위치 확인

## CGame::ToggleWhisperPlayer - SRV Game.cpp:34726-34808

/to: sets the whisper target (local or remote).

- sig: `void CGame::ToggleWhisperPlayer(int iClientH, char * pMsg, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, bStockMsgToGateServer, pGet
- notifies: DEF_NOTIFY_WHISPERMODEOFF, DEF_NOTIFY_WHISPERMODEON
- Korean comments (UTF-8, first 6, untranslated quote): 귓속말 상대가 지정되지 않았다. 귓속말 모드를 해제한다. / 귓속말 상태가 해제되었음을 통보. / 같은 이름을 가진 플레이어를 찾았다. // 자기 자신이라면 할당하지 않는다.

## CGame::RequestAdminUserMode - SRV Game.cpp:35831-35836

Empty body: two comment lines only (35831-35836), so it checks nothing; the comment says a date-derived code should match. Dispatched from MSGID_ADMINUSER (15009-15011) ([GSO-015](../../issues/game-server-other.md#gso-015-requestadminusermode-has-an-empty-body)).

- sig: `void CGame::RequestAdminUserMode(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- Korean comments (UTF-8, first 2, untranslated quote): 운영자 모드를 확인하기 위해서는 일단 이 메시지를 전송하는 클라이언트를 갖고 있어야 한다. / 또한 날짜를 조합해서 생성되는 확인 코드가 일치해야 한다.

## CGame::bCheckBadWord - SRV Game.cpp:36035-36047

Bad word filter (stub) ([GSO-016](../../issues/game-server-other.md#gso-016-bcheckbadword-is-a-stub)).

- sig: `BOOL CGame::bCheckBadWord(char * pString)`
- Korean comments (UTF-8, first 1, untranslated quote): 스트링을 검색하며 욕설이 있는지를 찾는다.

## CGame::ShutUpPlayer - SRV Game.cpp:36077-36189

/shutup: GM mutes a player for `minutes` x 20 ticks (3 s each); admin level 2 (1 under DEF_TAIWAN) and m_bIsAdminCommandEnabled; if the player is not on this server it forwards GSM_REQUEST_SHUTUPPLAYER through the gate (36157-36180).

- sig: `void CGame::ShutUpPlayer(int iClientH, char * pMsg, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, _bItemLog, bSendMsgToLS, bStockMsgToGateServer, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_PLAYERSHUTUP
- switches: DEF_TAIWAN, DEF_TAIWANLOG, DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / Admin user level이 낮아서 이 기능을 사용할 수 없다. / token이 곧 채팅을 불가능하게 만들 사용자 이름

## CGame::NoticeHandler - SRV Game.cpp:36323-36351

Rotates notice messages every 80 s.

- sig: `void CGame::NoticeHandler()`
- callers: CGame::OnTimer
- callees: Get, SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_NOTICEMSG
- cfg: notice.txt
- Korean comments (UTF-8, first 2, untranslated quote): 공지사항이 1개 이하라면 보낼 필요가 없다. / 공지사항을 전송할 시간이 되었다.

## CGame::AdminOrder_CallGuard - SRV Game.cpp:36449-36533

/attack: spawns a guard on a player. Admin level 2 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_CallGuard(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, bSendMsgToLS, bSetNpcAttackMode, iGetEmptyNamingValue, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_PLAYERNOTONGAME
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / token이 곧 채팅을 불가능하게 만들 사용자 이름

## CGame::AdminOrder_SummonDemon - SRV Game.cpp:36535-36584

/summondemon. Admin level 3 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_SummonDemon(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, bSendMsgToLS, iGetEmptyNamingValue
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 5, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / 더이상 이 맵에 NPC를 만들수 없다. 이름을 할당할 수 없기 때문.

## CGame::AdminOrder_ReserveFightzone - SRV Game.cpp:36587-36647

/reservefightzone (admin level 2): every call toggles m_iFightzoneNoForceRecall (0/1) and notifies -5/-4, even with no argument (36611-36617); with an argument it flips zone N between reserved-by-GM (-1) and free (0) (36626-36642) ([GSO-017](../../issues/game-server-other.md#gso-017-reservefightzone-toggles-a-flag-on-every-call)).

- sig: `void CGame::AdminOrder_ReserveFightzone(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, bSendMsgToLS, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_FIGHTZONERESERVE
- Korean comments (UTF-8, first 6, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / 운영자가 예약한 사투장은 -1 값이 들어간다. / 이제부터 계속 다른 유저는 예약이 불가능하다.

## CGame::AdminOrder_CloseConn - SRV Game.cpp:36650-36719

/closeconn. Admin level 2 (3 under the typo macro DEF_TIAWAN, never defined) and m_bIsAdminCommandEnabled ([GSO-011](../../issues/game-server-other.md#gso-011-misspelt-build-switches-leave-blocks-dead)).

- sig: `void CGame::AdminOrder_CloseConn(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, DeleteClient, SendNotifyMsg, _bItemLog, bSendMsgToLS, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_PLAYERNOTONGAME
- switches: DEF_TAIWANLOG, DEF_TESTSERVER, DEF_TIAWAN
- Korean comments (UTF-8, first 6, untranslated quote): v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / Admin user level이 낮아서 이 기능을 사용할 수 없다. / token이 곧 채팅을 불가능하게 만들 사용자 이름

## CGame::AdminOrder_CreateFish - SRV Game.cpp:36990-37055

/createfish. Admin level 2.

- sig: `void CGame::AdminOrder_CreateFish(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, _bInitItemAttr, iCreateFish, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- Korean comments (UTF-8, first 4, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / 이 좌표에 물고기를 만든다. / v2.17 2002-7-31 아이템을 아이템 고유번호로 생성할 수 있게 한다.

## CGame::AdminOrder_Teleport - SRV Game.cpp:37057-37198

/teleport, /tp. Admin level 1.

- sig: `void CGame::AdminOrder_Teleport(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, RequestTeleportHandler, SendNotifyMsg, bSendMsgToLS, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- Korean comments (UTF-8, first 4, untranslated quote): v2.13 옵저버 모드이면 텔레포트 않되게 수정 / v2.13 운영자 레벨 조절 / Admin user level이 낮아서 이 기능을 사용할 수 없다.

## CGame::AdminOrder_CheckIP - SRV Game.cpp:38008-38052

/checkip. Admin level 2.

- sig: `void CGame::AdminOrder_CheckIP(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_IPACCOUNTINFO
- Korean comments (UTF-8, first 3, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / token이 IP address이다. / 요구한 주소와 일치하는 클라이언트 발견.

## CGame::AdminOrder_Polymorph - SRV Game.cpp:38119-38148

/polymorph (parses and does nothing). Admin level 3 ([GSO-018](../../issues/game-server-other.md#gso-018-polymorph-parses-and-does-nothing)).

- sig: `void CGame::AdminOrder_Polymorph(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- Korean comments (UTF-8, first 2, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / 아직 구현되지 않았다. 폴리모프 루틴이 완성된 후 동작할 것임.

## CGame::AdminOrder_SetInvi - SRV Game.cpp:38150-38181

/setinvi. Admin level 1.

- sig: `void CGame::AdminOrder_SetInvi(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, SetInvisibilityFlag, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- Korean comments (UTF-8, first 3, untranslated quote): v2.13 운영자 레벨 조절 / Admin user level이 낮아서 이 기능을 사용할 수 없다. / 이 값이 문자 '1'이면 투명으로 세트. '0'이면 해제

## CGame::AdminOrder_GetNpcStatus - SRV Game.cpp:39269-39306

/gns. Admin level 2.

- sig: `void CGame::AdminOrder_GetNpcStatus(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_ADMINIFO, DEF_NOTIFY_ADMINUSERLEVELLOW
- Korean comments (UTF-8, first 1, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다.

## CGame::_BWM_Init - SRV Game.cpp:40688-40695

Registers a client as the bad-word monitor (MSGID_BWM_INIT).

- sig: `void CGame::_BWM_Init(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- callees: PutLogList

## CGame::_BWM_Command_Shutup - SRV Game.cpp:40697-40720

Bad-word monitor order: mutes a named player. Sets m_iTimeLeft_ShutUp = 20*3*10 = 600 (40710), which CheckClientResponseTime decrements by 1 per call (every >3000 ms, 3682-3683, 3872), i.e. about 30 min (derived, not run); the source comment says 10 min and ShutUpPlayer uses minutes x 20. The notify also sends 600 whereas ShutUpPlayer sends minutes (36132), a unit mismatch (client display unverified) ([GSO-019](../../issues/game-server-other.md#gso-019-bad-word-monitor-mute-uses-a-different-time-unit-and-the-notify-sends-600)).

- sig: `void CGame::_BWM_Command_Shutup(char *pData)`
- callers: CGame::MsgProcess
- callees: PutAdminLogFileList, SendNotifyMsg
- notifies: DEF_NOTIFY_PLAYERSHUTUP
- Korean comments (UTF-8, first 1, untranslated quote): 1이 3초다. 20이면 1분 욕을 하면 무조건 페널티 10분

## CGame::AdminOrder_SetAttackMode - SRV Game.cpp:43946-44003

/setattackmode. Admin level 3 (2 under DEF_TAIWAN).

- sig: `void CGame::AdminOrder_SetAttackMode(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_GLOBALATTACKMODE
- switches: DEF_TAIWAN
- Korean comments (UTF-8, first 6, untranslated quote): v1.42 길드전용 임시 코드 / 2002-7-4 운영자 레벨을 3으로 올린다. / Admin user level이 낮아서 이 기능을 사용할 수 없다.

## CGame::AdminOrder_SetForceRecallTime - SRV Game.cpp:44006-44067

/setforcerecalltime. Admin level 3 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_SetForceRecallTime(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, PutLogList, SendNotifyMsg, bStockMsgToGateServer, pGet
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- cfg: World cfg
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 3, untranslated quote): v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다.

## CGame::AdminOrder_UnsummonAll - SRV Game.cpp:44069-44095

/unsummonall. Admin level 3 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_UnsummonAll(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: NpcKilledHandler, SendNotifyMsg
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 3, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / v2.17 2002-8-5 모든 몬스터를 죽인다. 그러나 아이템은 나오지 않는다.

## CGame::AdminOrder_UnsummonBoss - SRV Game.cpp:44099-44127

/unsummonboss. Admin level 3 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_UnsummonBoss(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: NpcKilledHandler, SendNotifyMsg
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 4, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / 모든 배틀골램과 고즈핸드ㅡ, 데몬, 가고일, 헬클라우드, 타이거웜을 죽인다.

## CGame::AdminOrder_Summon - SRV Game.cpp:44209-44303

/summon. Admin level 3 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_Summon(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, PutLogList, SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, bSendMsgToLS, bSetNpcFollowMode, iGetEmptyNamingValue, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / 메시지 출력

## CGame::AdminOrder_SummonPlayer - SRV Game.cpp:44306-44414

/summonplayer. Admin level 2 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_SummonPlayer(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, RequestTeleportHandler, SendNotifyMsg, _bItemLog, bSendMsgToLS, bStockMsgToGateServer, pGet
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TAIWANLOG, DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / 같은 서버에 있는 플레이어 중

## CGame::AdminOrder_DisconnectAll - SRV Game.cpp:44618-44636

/disconnectall. Admin level 4 and m_bIsAdminCommandEnabled.

- sig: `void CGame::AdminOrder_DisconnectAll(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: SendNotifyMsg, _iForcePlayerDisconect
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 2, untranslated quote): v2.18 2002-10-15 GM 패스워드를 치지 않으면 동작하지 않는다. / Admin user level이 낮아서 이 기능을 사용할 수 없다.

## CGame::RequestNoticementHandler - SRV Game.cpp:45990-46029

Sends the noticement text if the client's size differs.

- sig: `void CGame::RequestNoticementHandler(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: iSendMsg
- messages: MSGID_RESPONSE_NOTICEMENT
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 2, untranslated quote): 클라이언트가 갖고 있는 파일 사이즈와 다르면 내용을 모두 보낸다. / 에러 발생해도 끊지 않는다.

## CGame::AdminOrder_SetObserverMode - SRV Game.cpp:46224-46244

/setobservermode. No level test inside; ChatMsgHandler requires level 3 (1 under DEF_TAIWAN) (11033).

- sig: `void CGame::AdminOrder_SetObserverMode(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: ClearOwner, SendEventToNearClient_TypeA, SendNotifyMsg, SetOwner
- messages: MSGID_EVENT_LOG
- notifies: DEF_NOTIFY_OBSERVERMODE
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 2, untranslated quote): 새 위치에 표시한다. / 현재 위치에서 지운다. 없어진 행위를 알려줘야 한다. 아직 미구현

## CGame::AdminOrder_EnableAdminCreateItem - SRV Game.cpp:46323-46338

/enableadmincreateitem <pw>. Admin level 3 (4 under DEF_TAIWAN); sets m_bIsAdminCreateItemEnabled ([GSO-004](../../issues/game-server-other.md#gso-004-two-gm-enable-commands-are-protected-by-hard-coded-passwords-in-source)).

- sig: `void CGame::AdminOrder_EnableAdminCreateItem(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- switches: DEF_TAIWAN

## CGame::AdminOrder_EnableAdminCommand - SRV Game.cpp:46341-46352

/enableadmincommand <pw>. Admin level 1; sets m_bIsAdminCommandEnabled ([GSO-004](../../issues/game-server-other.md#gso-004-two-gm-enable-commands-are-protected-by-hard-coded-passwords-in-source)).

- sig: `void CGame::AdminOrder_EnableAdminCommand(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler

## CGame::AdminOrder_CreateItem - SRV Game.cpp:46354-46599

/createitem. Admin level 4 (3 under DEF_TAIWAN) and m_bIsAdminCreateItemEnabled.

- sig: `void CGame::AdminOrder_CreateItem(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, _bAddClientItemList, _bInitItemAttr, _bItemLog, bSendMsgToLS, iDice, iSendMsg, pGet
- messages: MSGID_GAMEMASTERLOG, MSGID_NOTIFY
- notifies: DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_ITEMOBTAINED
- switches: DEF_TAIWAN, DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): Admin user level이 낮아서 이 기능을 사용할 수 없다. / 01-12-10 아이피 검사. 회사 내에서만 아이템 생성이 가능 / 아이템 생성 확인 코드가 입력되지 않았다면 만들 수 없다.

## CGame::GSM_RequestShutupPlayer - SRV Game.cpp:49767-49819

Remote shutup request.

- sig: `void CGame::GSM_RequestShutupPlayer(char * pGMName,WORD wReqServerID, WORD wReqClientH, WORD wTime,char * pPlayer )`
- callers: CGame::ServerStockMsgHandler
- callees: SendNotifyMsg, _bItemLog, bSendMsgToLS, bStockMsgToGateServer
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_PLAYERSHUTUP
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 4, untranslated quote): 찾았다. / v2.14 셧업시간을 분으로 수정 / 1이 3초다. 20이면 1분

## CGame::GSM_RequestFindCharacter - SRV Game.cpp:49821-49864

Remote /fi request.

- sig: `void CGame::GSM_RequestFindCharacter(WORD wReqServerID, WORD wReqClientH, char *pName,char * cRequestCharName)`
- callers: CGame::ServerStockMsgHandler
- callees: bStockMsgToGateServer
- Korean comments (UTF-8, first 2, untranslated quote): 찾았다. / v2.14 /fi 기능

## CGame::AdminOrder_GetFightzoneTicket - SRV Game.cpp:50801-50849

/getticket: GM gets fightzone tickets. No level test inside; ChatMsgHandler requires level 2 (11040).

- sig: `void CGame::AdminOrder_GetFightzoneTicket(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: GetFightzoneTicketHandler
