/* MainServer.exe  RegisterWorldServer @ 0x00405820, RegisterWorldServerSocket @ 0x00405cb0,
 *                 RegisterWorldServerGameServer @ 0x00405110, WorldServerActivated @ 0x0040e630   [V]
 * Reconstructed pseudo-code, not original source (hand-written from disassembly). */

BOOL CMainLog::RegisterWorldServer(int iClientH, char *pData)
{
    char cName[31] = {0}, cAddr[17] = {0};
    memcpy(cName, pData + 6, 30);
    memcpy(cAddr, pData + 36, 16);
    int iPort = *(int *)(pData + 52);

    for (int i = 1; i < 5; i++)
        if (m_pWorldList[i] && strcmp(m_pWorldList[i]->m_cName, cName) == 0) {   /* +0x328 */
            PutLogList("(XXX) World Server(%s) registration failed! Already existing world server name.");
            return FALSE;
        }
    for (int i = 1; i < 5; i++)                 /* 4 world slots, this+0x9c88 */
        if (m_pWorldList[i] == NULL) {
            m_pWorldList[i] = new CWorldServer(cName, cAddr, iPort);            /* 0x390 bytes */
            PutLogList("(OOO) World Server(%s:%s:%d) registration success!");
            PutLogList("(O) Client(%d) is socket to World Server(%s).");
            m_pClientList[iClientH]->m_cType = 2;                               /* world socket */
            m_pClientList[iClientH]->m_iWorldIndex = i;                         /* client+0x2c */
            m_pClientList[iClientH]->m_dw30 = 0;
            char cZero[10] = {0};
            SendMsgToWorldServer(cName, MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET,
                                 DEF_MSGTYPE_CONFIRM, cZero, 10, iClientH);
            return TRUE;
        }
    return FALSE;
}

/* RegisterWorldServerSocket: same body; finds the world by name, marks the socket type 2 with that
 * world index, and answers MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET (10 zero bytes) on it. */

/* RegisterWorldServerGameServer: world 30 (+6), game server name 10 (+36), address 16 (+46, 15 used),
 * port u32 (+62), map count u32 (+66), maps 11 each (+70). Duplicate game-server name in that world:
 * "(XXX) World Server(%s)-Game Server(%s) registration fail! Duplicated game server name."; else
 * stores the server (0x4052d0) and each map (0x405590, log "(!) Map(%s) registration: %s-%s MapNum(%d)"). */

void CMainLog::WorldServerActivated(int iClientH, char *pData)
{
    char cName[31] = {0};
    memcpy(cName, pData + 6, 30);
    for (int i = 1; i < 5; i++)
        if (m_pWorldList[i] && strcmp(m_pWorldList[i]->m_cName, cName) == 0) {
            m_pWorldList[i]->m_bActivated = 1;      /* +0x38c: LOGIN for this world now allowed */
            PutLogList("(!) WorldServer(%s) Activated.");
        }
}
