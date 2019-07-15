/*
Regards to ICQ users:
	Alex (tester)
	Nata (tester)
	demonnnne (tester)
	yyyyy (icq number)
	xxx (icq number)
	Dr Ago(spaming and tasks given)
Regards to sub (from www.rnq.ru)
Regards to sites:
	www.rnq.ru(forum, sources of rnq)
	www.iserverd.khstu.ru/oscar(a lot of info)
	www.oscar.asechka.ru(a lot of info)
	www.kingant.net/oscar(info)
	www.icq2000cc.hobi.ru(nice example)
	www.miranda-im.org(sources of miranda)
Date of project start: 24.06.2010
Date of release with minimal bugs: 31.08.2010
Date of release with fixed net code: 04.09.2010
Date of release GUI with Message Dialog: 17.09.2010
Date of stop coding: 20.09.2010
Date of continue coding: 23.10.2010
Using software: Notepad++, Dev C++, MVC++ 2010 Express, MVC++ 2005
All code written by own hands (0_0)
*/

#ifndef icqclientH
#define icqclientH

//net errors codes
#define NET_SUCCESS 1
#define NET_EMPTY_BUFFER 2
#define NET_TIMEOUT 3
#define NET_CLOSED_CONNECTION 4
#define NET_NULL_PACKET 5
#define NET_NOT_ICQ_PACKET 6
#define NET_WRONG_SIZE 7
#define NET_TOO_MANY_NULLS 8

#include "packet.h"

class MICQClient
{
	public:
		//constructor, destructor
		MICQClient();
		~MICQClient();

		//basic functions
		bool bConnect();
		bool bLogin(char *cUIN, char* cPassword);
		bool bDisconnect();
		bool bMainLoop();
		bool bSendSimpleMessage(char *inUIN, char *inMessage);

		//gets
		BYTE GetResultConnectCode();
		MasterGroup *GetMasterGroup();

		//flag to teminate Mainloop()
		bool UserTerminateFlag;

	private:
		//ICQ DATA
		MasterGroup m_MasterGroup;
		PdRights m_PdRights;
		ICBMInfo m_ICBMInfo;
		BuddyRights m_BuddieRights;
		LocateRights m_LocateRights;
		ProtocolGroup m_ProtocolGroup;
		char *m_cUIN;
		char *m_cPassword;
		WORD m_wSequence;
		DWORD m_dwRequestId;
		WORD m_wCookieSize;
		BYTE *m_bCookie;
		PBYTE m_pbBuffer;
		bool SendPacket(FlapPacket *Packet);
		BYTE ReciveBufferNew(PBYTE &Buffer);
		bool WaitAndReciveSnac(PBYTE &Buffer, WORD Fam, WORD Type, DWORD RequestId = 0x00000001);
		bool _Stage1Authorization();
		bool _Stage2ProtocolNegotiation();
		bool _Stage3ServicesSetup();
		bool _Stage4FinalActions();
		bool _OfflineMessagesRequest();
		void AnalyseIncomePacket(FlapPacket *recvPacket);

		//SYSTEM DATA
		HWND MainWnd;
		char *m_pcCurrentDirectory;
		bool EnablePacketLog;
		bool RewritePacketLog;
		char *m_pcPacketLogFileName;
		//bool EnableSystemLog;
		//bool RewriteSystemtLog;
		//char *m_pcSystemLogFileName;
		bool bGetSettingsFromIni(char *FileName);

		//NET DATA
		WSADATA wsaData;
		SOCKET m_Server;
		sockaddr_in m_AddressPort;
		struct timeval m_NetTimeout;
		BYTE LimitAttempts;
		BYTE ResultConnectCode; //0 - not connected, 1 - connected, 2 - error while connecting
};

#endif
