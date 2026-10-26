/* MainServer.exe  message dispatcher @ 0x00403220   [V, from disassembly]
 * Reconstructed pseudo-code, not original source. The switch tests MsgID only. */

void CMainLog::MsgProcess()
{
    char *pData = m_pMsgBuffer;                 /* this+0x27164, 30001 bytes */
    char  cFrom, cKey; DWORD dwSize; int iClientH;

    memset(pData, 0, 30001);
    while (bGetMsgQuene(&cFrom, pData, &dwSize, &iClientH, &cKey) == TRUE) {   /* 0x403000 */
        if (m_pClientList[iClientH] == NULL) continue;                          /* this+0x48 */
        switch (*(DWORD *)pData) {
        case MSGID_REQUEST_LOGIN:              RequestLogin(iClientH, pData, cKey);            break; /* 0x4036f0 */
        case MSGID_REQUEST_CREATENEWACCOUNT:   CreateNewAccount(iClientH, pData, cKey);        break; /* 0x40b890 */
        case MSGID_RESPONSE_LOG:               ResponseLogFromWLS(iClientH, pData, dwSize);    break; /* 0x409520 */
        case MSGID_REQUEST_CREATENEWCHARACTER: CreateNewCharacter(iClientH, pData, cKey);      break; /* 0x408a00 */
        case MSGID_REQUEST_ENTERGAME:          RequestEnterGame(iClientH, pData, cKey);        break; /* 0x406840 */
        case MSGID_REQUEST_DELETECHARACTER:    DeleteCharacter(iClientH, pData, cKey);         break; /* 0x40adc0 */
        case MSGID_REQUEST_CHANGEPASSWORD:     ChangePassword(iClientH, pData, cKey);          break; /* 0x40caa0 */
        case MSGID_REQUEST_REGISTERDBSERVER:   RegisterDBServer(iClientH, pData);              break; /* 0x40d360 */
        case MSGID_ACCOUNTINFOCHANGE:          AccountInfoChange(iClientH, pData);             break; /* 0x40d3e0 */
        case MSGID_REQUEST_ACCOUNTCONNSTATUS:  AccountConnStatus(iClientH, pData);             break; /* 0x40d930 */
        case MSGID_REQUEST_CLEARACCOUNTCONNSTATUS: ClearAccountConnStatus(iClientH, pData);    break; /* 0x40de50 */
        case MSGID_REQUEST_BLOCKACCOUNT:       BlockAccount(iClientH, pData);                  break; /* 0x40ebd0 */
        case MSGID_ACCOUNTTIMECHANGE:          AccountTimeChange(iClientH, pData);             break; /* 0x40f720 */
        case MSGID_SENDSERVERSHUTDOWNMSG:      WorldServerShutdown(iClientH, pData);           break; /* 0x40f5d0 */
        case MSGID_REGISTER_WORLDSERVER:       RegisterWorldServer(iClientH, pData);           break; /* 0x405820 */
        case MSGID_REGISTER_WORLDSERVERSOCKET: RegisterWorldServerSocket(iClientH, pData);     break; /* 0x405cb0 */
        case MSGID_REGISTER_WORLDSERVER_GAMESERVER: RegisterWorldServerGameServer(iClientH, pData); break; /* 0x405110 */
        case MSGID_RESPONSE_CHARINFOLIST:      ResponseCharInfoList(iClientH, pData, dwSize);  break; /* 0x406510 */
        case MSGID_REQUEST_REMOVEGAMESERVER:   RemoveGameServer(iClientH, pData);              break; /* 0x407f40 */
        case MSGID_REQUEST_CLEARACCOUNTSTATUS: ClearAccountStatus(iClientH, pData);            break; /* 0x408900 */
        case MSGID_WORLDSERVERACTIVATED:       WorldServerActivated(iClientH, pData);          break; /* 0x40e630 */
        }
    }
    UpdateScreen();                                                            /* 0x413000 */
}
