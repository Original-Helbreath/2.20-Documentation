/* MainServer.exe  RequestLogin @ 0x004036f0   [V control flow and constants; status meaning I]
 * Reconstructed pseudo-code, not original source (hand-written from disassembly). */

void CMainLog::RequestLogin(int iClientH, char *pData, char cKey)
{
    char cAccount[11] = {0}, cPassword[11] = {0}, cWorldName[31] = {0}, cFn[256] = {0}, cTmp[120];
    char cFileAccount[11], cFilePassword[11], cCharNames[4 * 11];
    int  iStatus = 0, iBlockY, iBlockM, iBlockD, iAccTime;
    short sAccY, sAccM, sAccD, sIpY, sIpM, sIpD;
    char  cMsg[2000];
    int   i, iRet;

    memcpy(cAccount,   pData + 6, 10);  DecodeField(cKey, cAccount);    /* 0x411ac0: returns at once */
    memcpy(cPassword,  pData + 16, 10); DecodeField(cKey, cPassword);
    memcpy(cWorldName, pData + 26, 30);

    strcat(cFn, "Account"); strcat(cFn, "\\");
    wsprintf(cTmp, "AscII%d", (unsigned char)cAccount[0]); strcat(cFn, cTmp);
    strcat(cFn, "\\"); strcat(cFn, cAccount); strcat(cFn, ".txt");

    char cCount = ReadAccountFile(cFn, cWorldName, cFileAccount, cFilePassword, cCharNames,
                                  &iStatus, &iBlockY, &iBlockM, &iBlockD,
                                  &sAccY, &sAccM, &sAccD, &iAccTime /* ... */);   /* 0x404400 */
    m_pClientList[iClientH]->m_pXSock->GetPeerAddress(m_pClientList[iClientH]->m_cIPaddress);

    BOOL bWorld = FALSE;                        /* world must be registered AND activated */
    for (i = 1; i < 5; i++)
        if (m_pWorldList[i] && strcmp(m_pWorldList[i]->m_cName, cWorldName) == 0 &&
            m_pWorldList[i]->m_bActivated == 1)                                  /* +0x38c */
            bWorld = TRUE;

    *(DWORD *)cMsg = MSGID_RESPONSE_LOG;
    if (!bWorld) { *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER;   /* 0x0A02 */
                   Send(iClientH, cMsg, 6); return; }
    if (cCount == -1 || strncmp(cAccount, cFileAccount, 10) != 0) {
        *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT; Send(iClientH, cMsg, 6); return; }  /* 0x0F17 */
    if (IsBlockedFor5Minutes(cAccount) == 1) {                                   /* 0x40eb20 */
        *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_REJECT;
        *(int *)(cMsg + 6) = 0; *(int *)(cMsg + 10) = 0; *(int *)(cMsg + 14) = 0;
        Send(iClientH, cMsg, 18); return; }
    if (iStatus == 0) {                                                          /* block date not passed */
        *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_REJECT;
        *(int *)(cMsg + 6) = iBlockY; *(int *)(cMsg + 10) = iBlockM; *(int *)(cMsg + 14) = iBlockD;
        Send(iClientH, cMsg, 18); return; }
    if (strncmp(cFilePassword, cPassword, 10) != 0) {
        *(WORD *)(cMsg + 4) = DEF_LOGRESMSGTYPE_PASSWORDMISMATCH; Send(iClientH, cMsg, 6);           /* 0x0F16 */
        PutLogList("(!) Account(%s) Password Wrong : %s, %s"); return; }

    /* success: remember account and a cookie, ask the WLS for the character list */
    strcpy(m_pClientList[iClientH]->m_cAccountName, cAccount);                   /* client+5 */
    m_pClientList[iClientH]->m_dwCookie = rand() % 1000000 + timeGetTime();      /* client+0x10 */

    char *cp = cMsg; memset(cMsg, 0, 2000);
    memcpy(cp, cAccount, 10);                         cp += 10;
    *(DWORD *)cp = m_pClientList[iClientH]->m_dwCookie; cp += 4;
    *(WORD *)cp = 4;      cp += 2;                    /* upper version: 2.20 client needs 2  (patch 0x3EF1) */
    *(WORD *)cp = 0x60;   cp += 2;                    /* lower version: 2.20 client needs 20 (patch 0x3F08) */
    *cp++ = (char)iStatus;                            /* 2 or 4 */
    *(short *)cp = sAccY; cp += 2; *(short *)cp = sAccM; cp += 2; *(short *)cp = sAccD; cp += 2;
    *(short *)cp = sIpY;  cp += 2; *(short *)cp = sIpM;  cp += 2; *(short *)cp = sIpD;  cp += 2;
    *cp++ = cCount;
    *(int *)cp = iAccTime; cp += 4;
    *(int *)cp = 0;        cp += 4;                   /* IP time */
    for (i = 0; i < cCount; i++, cp += 10) memcpy(cp, cCharNames + i * 11, 10);

    SendMsgToWorldServer(cWorldName, MSGID_REQUEST_CHARINFOLIST, DEF_MSGTYPE_CONFIRM,
                         cMsg, 0x28 + cCount * 10, -1);                           /* 0x406000 */
    /* the WLS answers MSGID_RESPONSE_CHARINFOLIST; ResponseCharInfoList (0x406510) finds this
       client by account + cookie and forwards the embedded RESPONSE_LOG CONFIRM unchanged */
}
