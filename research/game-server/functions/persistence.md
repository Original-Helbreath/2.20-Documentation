# Player data encode/decode and saving

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 6

## CGame::_bDecodePlayerDatafileContents - SRV Game.cpp:5551-6426

DB-mode decoder: binary player record from the Log server. The `_bInitItemAttr` calls (5953, 6119) are the by-name overload (9857); the name check `m_Misc.bCheckValidName` is at 6315; `_bItemLog` DUPITEMID calls are at 6099, 6239 (handle form).

- sig: `BOOL CGame::_bDecodePlayerDatafileContents(int iClientH, char * pData, DWORD dwSize)`
- callers: CGame::InitPlayerData
- callees: GetMapInitialPoint, PutLogFileList, PutLogList, SendNotifyMsg, _AdjustRareItemValue, _bCheckDupItemID, _bInitItemAttr, _bItemLog, _iCalcMaxLoad, bCheckValidName, bEquipItemHandler, bIsValidLoc, iCalcTotalWeight, iGetItemWeight
- notifies: DEF_NOTIFY_FIGHTZONERESERVE
- Korean comments (UTF-8, first 6, untranslated quote): 소지품 검사용 / v2.14 DB 저장 속도 향상 / 맵의 인덱스는 i이다.

## CGame::_iComposePlayerDataFileContents - SRV Game.cpp:6428-6941

DB-mode encoder.

- sig: `int CGame::_iComposePlayerDataFileContents(int iClientH, char * pData)`
- callers: CGame::LocalSavePlayerData, CGame::bSendMsgToLS
- callees: iGetMaxHP
- Korean comments (UTF-8, first 5, untranslated quote): GuildRank가 -1이면 길드이름은 무의미하다. / GuildRank가 -1이면 길드GUID는 무의미하다. / V2.1 성후니 파티제를 위해서 추가

## CGame::_bDecodePlayerDatafileContents - SRV Game.cpp:6945-8914 (inactive: else(DEF_DBGAMESERVER))

File-mode decoder (inactive). Its `_bInitItemAttr` calls (7060, 7636) are the by-name overload; the name check `bCheckValidName` for this build is at 8803 (a separate site from 6315 in the DB decoder).

- sig: `BOOL CGame::_bDecodePlayerDatafileContents(int iClientH, char * pData, DWORD dwSize)`
- callers: CGame::InitPlayerData
- callees: CStrTok, GetMapInitialPoint, PutLogFileList, PutLogList, SendNotifyMsg, _AdjustRareItemValue, _bCheckDupItemID, _bGetIsStringIsNumber, _bInitItemAttr, _bItemLog, _iCalcMaxLoad, bCheckAndConvertPlusWeaponItem, bCheckValidName, bEquipItemHandler, bIsValidLoc, iCalcTotalWeight, iGetItemWeight, pGet
- notifies: DEF_NOTIFY_FIGHTZONERESERVE
- Korean comments (UTF-8, first 6, untranslated quote): 소지품 검사용 / 플레이어가 속해있는 맵 이름을 얻는다. / 이제 이 맵의 인덱스값을 찾아 할당한다.

## CGame::_iComposePlayerDataFileContents - SRV Game.cpp:8917-9475 (inactive: else(DEF_DBGAMESERVER))

File-mode encoder (inactive).

- sig: `int CGame::_iComposePlayerDataFileContents(int iClientH, char * pData)`
- callers: CGame::LocalSavePlayerData, CGame::bSendMsgToLS
- callees: iGetMaxHP
- Korean comments (UTF-8, first 6, untranslated quote): 이름 저장 / 캐릭터 프로필 / 캐릭터 프로필이 설정되지 않았다면

## CGame::ResponseSavePlayerDataReplyHandler - SRV Game.cpp:36354-36372

Save confirmed: tells client to change server.

- sig: `void CGame::ResponseSavePlayerDataReplyHandler(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_SERVERCHANGE
- Korean comments (UTF-8, first 2, untranslated quote): 이제 이 이름을 갖는 클라이언트를 찾아 접속을 끊어도 좋다는 메시지를 보낸다. / 같은 이름을 가진 캐릭터를 찾았다. 접속을 종료하라는 메시지를 보낸다.

## CGame::LocalSavePlayerData - SRV Game.cpp:38736-38794

Fallback local save, called from DeleteClient only when bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT) returns FALSE (Game.cpp:2800). Writes `Character_<month>_<day>_<hour>_<min>\AscII<first byte of name>\<name>.txt` (38755-38768), opened "wt" (text mode) although the active record is the binary DB form (_iComposePlayerDataFileContents 6428) ([GSO-039](../../issues/game-server-other.md#gso-039-local-fallback-save-writes-the-binary-record-in-text-mode)).

- sig: `void CGame::LocalSavePlayerData(int iClientH)`
- callers: CGame::DeleteClient
- callees: PutLogList, _iComposePlayerDataFileContents
- Korean comments (UTF-8, first 3, untranslated quote): 로그 서버로의 연결이 종료되어 임시로 게임서버 내의 폴더에 저장한다. / 디렉토리를 만든다. / (char*)cp 부터 (dwMsgSize - 36)크기까지가 파일에 저장될 데이터이다.
