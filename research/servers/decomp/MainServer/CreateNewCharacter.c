/* MainServer.exe  CreateNewCharacter @ 0x00408a00 and ResponseLogFromWLS @ 0x00409520   [V]
 * Reconstructed pseudo-code, not original source (hand-written from disassembly). */

void CMainLog::CreateNewCharacter(int iClientH, char *pData, char cKey)
{
    char cName[11] = {0}, cAccount[11] = {0}, cPassword[11] = {0}, cWorld[31] = {0};
    memcpy(cName, pData + 6, 10); memcpy(cAccount, pData + 16, 10);
    memcpy(cPassword, pData + 26, 10); memcpy(cWorld, pData + 36, 30);
    char cStr = pData[71], cVit = pData[72], cDex = pData[73], cInt = pData[74], cMag = pData[75], cChr = pData[76];

    char cCount = ReadAccountFile(cFn, cWorld, cFileAccount, cFilePassword, cCharNames, ...);
    if (strcmp(cFilePassword, cPassword) != 0) {
        PutLogList("(X) Cannot create new character(%s) - Account password not match!");
        SendLogResult(iClientH, /* failure type */); return;
    }
    m_pClientList[iClientH]->m_dwCookie = rand() % 1000000 + timeGetTime();
    if (!CheckValidName(cName) /* 0x411b70 */ || cStr + cVit + cDex + cInt + cMag + cChr != 70 ||
        cStr < 10 || cStr > 14 || cVit < 10 || cVit > 14 || cDex < 10 || cDex > 14 ||
        cInt < 10 || cInt > 14 || cMag < 10 || cMag > 14 || cChr < 10 || cChr > 14) {
        PutLogList("(X) Cannot create new character(%s) - HACKED character data!!!");
        SendLogResult(iClientH, /* failure type */); return;
    }
    char cBuf[3000] = {0};
    memcpy(cBuf, pData + 6, 71);                         /* the client request body, unchanged */
    *(DWORD *)(cBuf + 71) = m_pClientList[iClientH]->m_dwCookie;
    cBuf[75] = cCount;
    for (int i = 0; i < cCount; i++) memcpy(cBuf + 76 + i * 11, cCharNames + i * 11, 11);
    SendMsgToWorldServer(cWorld, MSGID_REQUEST_CREATENEWCHARACTER, DEF_MSGTYPE_CONFIRM,
                         cBuf, 0x4c + cCount * 11, -1);
}

void CMainLog::ResponseLogFromWLS(int iWorldSockH, char *pData, DWORD dwSize)
{
    char  cAccount[11] = {0};
    memcpy(cAccount, pData + 6, 10);
    DWORD dwCookie   = *(DWORD *)(pData + 16);
    DWORD dwInnerID  = *(DWORD *)(pData + 20);
    WORD  wInnerType = *(WORD  *)(pData + 24);
    char *pInner     = pData + 26;

    PutLogList("(TestLog) LogMsgFromWLS(%s)-ID(%d)...");
    if (m_pClientList[iWorldSockH]->m_iWorldIndex == -1) {
        PutLogList("(ErrorLog) LogMsgFromWLS Error! Account(%s) - Msg(%x)"); return; }
    if (wInnerType == DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED) {               /* 0x0F1C */
        char cNewName[11] = {0}; memcpy(cNewName, pInner, 10);
        /* adds "account-character  = <name>" (world WS1) or "account-character-<world>  = <name>"
           under [CHARACTERS] in Account\AscII<n>\<account>.txt (0x4097e0) */
        AddCharacterToAccountFile(m_pWorldList[...]->m_cName, cAccount, cNewName);
    }
    for (int i = 1; i < 10000; i++)
        if (m_pClientList[i] && strcmp(m_pClientList[i]->m_cAccountName, cAccount) == 0 &&
            m_pClientList[i]->m_dwCookie == dwCookie) {
            SendToClient(i, dwInnerID, wInnerType, pInner, dwSize /* minus header */);   /* 0x406670 */
            break;
        }
}
