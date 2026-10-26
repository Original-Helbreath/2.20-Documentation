/* MainServer.exe  RequestEnterGame @ 0x00406840   [V constants and branch structure;
 *                 meaning of some branches I, see official-mainserver.md 3.5]
 * Reconstructed pseudo-code, not original source (hand-written from disassembly). */

void CMainLog::RequestEnterGame(int iClientH, char *pData, char cKey)
{
    char cCharName[11] = {0}, cMapName[11] = {0}, cAccount[11] = {0}, cPassword[11] = {0};
    char cWorldName[31] = {0}, cToken[121] = {0}, cGSAddr[17]; int iGSPort;
    int  iLevel;
    WORD wEnterType = *(WORD *)(pData + 4);
    char cMsg[256] = {0};

    memcpy(cCharName, pData + 6, 10);  memcpy(cMapName, pData + 16, 10);
    memcpy(cAccount,  pData + 26, 10); memcpy(cPassword, pData + 36, 10);
    iLevel = *(int *)(pData + 46);
    memcpy(cWorldName, pData + 50, 30); memcpy(cToken, pData + 80, 120);

    *(DWORD *)cMsg = MSGID_RESPONSE_ENTERGAME;
    int iRoute = FindWorldAndGameServer(cWorldName, cMapName, cGSAddr, &iGSPort);  /* 0x4078a0 */
    if (iRoute == 1 || iRoute == 2) {           /* world / game server not registered */
        *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_REJECT; cMsg[6] = 3; Send(iClientH, cMsg, 7); return; }
    if (iRoute == -2) {
        *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_REJECT; cMsg[6] = 5; Send(iClientH, cMsg, 7); return; }

    int iFileResult; /* ... */
    ReadAccountFile(cFn, cWorldName, ..., &iFileResult, ...);                 /* 0x404400 */
    if (iFileResult == 0) {
        *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_REJECT; cMsg[6] = 4;
        PutLogList("(X) Enter game rejected! (%s/%s) (%s/%s)"); Send(iClientH, cMsg, 7); return; }
    if (iFileResult == 4) {
        *(DWORD *)cMsg = MSGID_RESPONSE_LOG;
        *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_FORCECHANGEPASSWORD;           /* 0x0A05 */
        Send(iClientH, cMsg, 6); return; }
    if (!CharacterBelongsToAccount(cCharName) || strcmp(cPassword, cFilePassword) != 0) {
        *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_REJECT; cMsg[6] = 4;
        PutLogList("(X) Enter game rejected! (%s/%s) (%s/%s) %d"); Send(iClientH, cMsg, 7); return; }

    int iConn = GetAccountConnState(cAccount);                                 /* 0x407ac0 */
    if (wEnterType == DEF_ENTERGAMEMSGTYPE_NEW) {                              /* 0x0F1C */
        if (iConn == 0) {
            *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_CONFIRM;                /* 0x0F22 */
            memcpy(cMsg + 6, cGSAddr, 16);
            *(WORD *)(cMsg + 22) = (WORD)iGSPort;
            Send(iClientH, cMsg, 24);                                          /* no game-server name */
            SetAccountConnStatus(1, cAccount, cWorldName, ...);                /* 0x408430 */
            char cInit[24] = {0};
            memcpy(cInit, cAccount, 10); memcpy(cInit + 10, cPassword, 10);
            *(int *)(cInit + 20) = iLevel;
            SendMsgToWorldServer(cWorldName, MSGID_REQUEST_SETACCOUNTINITSTATUS,
                                 DEF_MSGTYPE_CONFIRM, cInit, 24, -1);
        } else if (iConn == 1) {
            *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_PLAYING; Send(iClientH, cMsg, 6);   /* 0x0F20 */
        }
    }
    else if (wEnterType == DEF_ENTERGAMEMSGTYPE_NOENTER_FORCEDISCONN) {        /* 0x0F1D */
        RequestForceDisconnect(cAccount, cWorldName);   /* 0x40e420: MSGID_REQUEST_FORCEDISCONECTACCOUNT to WLS */
        *(WORD *)(cMsg + 4) = DEF_ENTERGAMERESTYPE_FORCEDISCONN; Send(iClientH, cMsg, 6);  /* 0x0F23 */
    }
}
