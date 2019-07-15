#include "stdafx.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//gloabal pointer to CRigidICQDlg class
//CRigidICQDlg *pRigidICQDlg;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//PUBLIC
MICQClient::MICQClient()
{
	srand((unsigned)time(NULL));
	m_cUIN = NULL;
	m_cPassword = NULL;
	m_wSequence = 0;
	m_dwRequestId = 0;
	m_pbBuffer = new BYTE[MAX_PACKET_DATA_SIZE];
	m_Server = NULL;
	m_NetTimeout.tv_sec = 4000;
	m_NetTimeout.tv_usec = 0;
	LimitAttempts = 10; //10*4000 = 20000secs
	ResultConnectCode = 0;
	m_pcCurrentDirectory = new char[_MAX_PATH];
	memset(m_pcCurrentDirectory, 0, _MAX_PATH);
	GetCurrentDirectoryA(_MAX_PATH-1, m_pcCurrentDirectory);
	UserTerminateFlag = false;
	m_pcPacketLogFileName = NULL;
	EnablePacketLog = false;
	RewritePacketLog = false;
	m_bCookie = NULL;
	m_ProtocolGroup.FamilyNumber = NULL;
	m_ProtocolGroup.FamilyVersion = NULL;
}
MICQClient::~MICQClient()
{
	m_wSequence = 0;
	if(m_cUIN) delete [] m_cUIN;
	if(m_cPassword) delete [] m_cPassword;
	//m_Log.clear();
	if(!m_MasterGroup.mGroup.empty())
	{
		for(WORD i=0; i<m_MasterGroup.mGroup.size(); i++)
			m_MasterGroup.mGroup[i].mBuddie.clear();
		m_MasterGroup.mGroup.clear();
	}
	m_Server = NULL;
	if(m_pbBuffer) delete m_pbBuffer;
	ResultConnectCode = 0;
	if(m_pcCurrentDirectory) delete [] m_pcCurrentDirectory;
	if(m_pcPacketLogFileName) delete [] m_pcPacketLogFileName;
	if(m_bCookie) delete [] m_bCookie;

	if(m_ProtocolGroup.FamilyNumber) delete [] m_ProtocolGroup.FamilyNumber;
	if(m_ProtocolGroup.FamilyVersion) delete [] m_ProtocolGroup.FamilyVersion;
}
bool MICQClient::bConnect()
{
	//init HWND
	MainWnd = AfxGetApp()->GetMainWnd()->m_hWnd;

	//load params from settings.ini
	if(!bGetSettingsFromIni("Settings.ini")) return 0;
	//test name of packet log file
	if(!m_pcPacketLogFileName && EnablePacketLog)
	{
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Packet log file name is null while writing to log enable");
		return 0;
	}
	//clear and create PacketLog file
	if(RewritePacketLog)
	{
		FILE *pFile = fopen(m_pcPacketLogFileName, "w");
		fclose(pFile);
	}
	//strating WS...
	if(WSAStartup(0x101,&wsaData) != 0)
	{
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Initialize sockets failed");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Initialize sockets passed");

	//get ip of login.icq.com
	const char* host = "login.icq.com";
	const in_addr* address;
    const hostent* host_info = 0 ;
    for( int attempt=0; (host_info==0) && (attempt<3); ++attempt )
		host_info = gethostbyname(host);
    if(host_info)
    {
		for( int i=0; host_info->h_addr_list[i]; ++i )
			address = (in_addr*)host_info->h_addr_list[i];
	}
	else
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Host info error");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Get ip of login.icq.com passed");

	//connecting to login.icq.com
	m_AddressPort.sin_addr = *address;
	m_AddressPort.sin_family = AF_INET;
	m_AddressPort.sin_port = htons(5190);
	m_Server = socket(AF_INET, SOCK_STREAM, 0);
	if(m_Server == INVALID_SOCKET)
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Invalid socket error while connect");
		return 0;
	}
	//set timeout for recv
	setsockopt(m_Server,SOL_SOCKET,SO_RCVTIMEO,(char*)&m_NetTimeout,sizeof(m_NetTimeout));
	//connect
	if(connect(m_Server,(sockaddr*)&(m_AddressPort),sizeof(m_AddressPort))==SOCKET_ERROR)
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Connection to login.icq.com failed");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Connection to login.icq.com passed");

	return 1;
}
bool MICQClient::bLogin(char *cUIN, char* cPassword)
{
	//authorizing and connect
	m_cUIN = new char[MAX_UIN_STR_SIZE];
	m_cPassword = new char[MAX_NICKNAME_STR_SIZE];
	memset(m_cUIN, 0, MAX_UIN_STR_SIZE);
	memset(m_cPassword, 0, MAX_NICKNAME_STR_SIZE);
	memcpy(m_cUIN, cUIN, strlen(cUIN));
	memcpy(m_cPassword, cPassword, strlen(cPassword));
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Copy UIN and Password to memory");

	//stage 1
	BYTE *Cookie = NULL;
	WORD CookieSize = 0;
	if(!_Stage1Authorization())
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 1 failed");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 1 of 4 passed");

	//disconnect
	closesocket(m_Server);
	m_Server = socket(AF_INET, SOCK_STREAM, 0);
	//set timeout for recv
	setsockopt(m_Server,SOL_SOCKET,SO_RCVTIMEO,(char*)&m_NetTimeout,sizeof(m_NetTimeout));
	//connect
	if(connect(m_Server,(sockaddr*)&(m_AddressPort),sizeof(m_AddressPort))==SOCKET_ERROR)
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Connection to new ip of login.icq.com failed");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Connection to new ip of login.icq.com passed");

	//stage 2
	if(!_Stage2ProtocolNegotiation())
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 2 failed");
		return 0;
	}
	if(Cookie) delete Cookie;
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 2 of 4 passed");

	//stage 3
	Sleep(100);
	if(!_Stage3ServicesSetup())
	{
		ResultConnectCode = 2;
		SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 3 failed");
		return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 3 of 4 passed");

	//stage 4
	if(!_Stage4FinalActions())
	{
		 ResultConnectCode = 2;
		 SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 4 failed");
		 return 0;
	}
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Stage 4 of 4 passed");
	Sleep(500);
	ResultConnectCode = 1;

	return 1;
}
bool MICQClient::bDisconnect()
{
	if(m_Server) closesocket(m_Server);
	WSACleanup();
	ResultConnectCode = 0;
	return 1;
}
bool MICQClient::bMainLoop()
{
	//first - test offline messages
	//second - loop
	bool Terminate = false;
	PBYTE Buf = new BYTE[MAX_PACKET_DATA_SIZE];
	while(!Terminate)
	{
		if(UserTerminateFlag) break;
		switch(ReciveBufferNew(Buf))
		{
			case NET_CLOSED_CONNECTION:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_CLOSED_CONNECTION");
				break;
			case NET_EMPTY_BUFFER:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_EMPTY_BUFFER");
				break;
			case NET_TIMEOUT:
				break;
			case NET_NULL_PACKET:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_NULL_PACKET");
				break;
			case NET_NOT_ICQ_PACKET:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_NOT_ICQ_PACKET");
				break;
			case NET_WRONG_SIZE:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_WRONG_SIZE");
				break;
			case NET_TOO_MANY_NULLS:
				Terminate = true;
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in MainLoop: NET_TOO_MANY_NULLS");
				break;
			case NET_SUCCESS:
				AnalyseIncomePacket((FlapPacket*)Buf);
				break;
		}
		Sleep(50);
	}
	if(Buf) delete [] Buf;
	bDisconnect();
	return 1;
}
bool MICQClient::bSendSimpleMessage(char *inUIN, char *inMessage)
{
	if(!SendPacket(CreateSnac0406Packet(inUIN, inMessage, 0x0001, ++m_wSequence))) return 0;
	return 1;
}
BYTE MICQClient::GetResultConnectCode()
{
	return ResultConnectCode;
}
MasterGroup *MICQClient::GetMasterGroup()
{
	if(!m_MasterGroup.mGroup.empty()) return &m_MasterGroup;
	return NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//PRIVATE
bool MICQClient::SendPacket(FlapPacket *Packet)
{
	if(!Packet) return 0;
	int SizeData = send(m_Server, (const char*)Packet, FLAP_HEADER_SIZE+SwapWord(Packet->DFLen), 0);
	if(SizeData == SOCKET_ERROR)
	{
		delete Packet;
		return 0;
	}
	//update sequence
	m_wSequence = SwapWord(Packet->SeqNum);
	//write to packet log
	if(EnablePacketLog) WritePacketToFile(m_pcPacketLogFileName, Packet, "CLIENT");
	//write to window log
	char *pText = GetSnacFamFromBuffer((PBYTE)Packet);
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)"Sended ", (LPARAM)pText);
	if(pText) delete [] pText;
	Sleep(100);
	if(Packet) delete Packet;
	return 1;
}
BYTE MICQClient::ReciveBufferNew(PBYTE &Buffer)
{
	if(!Buffer) return NET_EMPTY_BUFFER;
	int RecvSize;

	//read and test flap header
	memset(Buffer, 0x00, MAX_PACKET_DATA_SIZE);
	RecvSize = recv(m_Server, (char*)Buffer, FLAP_HEADER_SIZE, 0);
	if(RecvSize == SOCKET_ERROR)
	{
		if(WSAGetLastError()==10054) return NET_CLOSED_CONNECTION;
		if(WSAGetLastError()==10060) return NET_TIMEOUT;
	}
	if(RecvSize <= 0) return NET_NULL_PACKET;
	if(Buffer[0] != ICQ_SIGN) return NET_NOT_ICQ_PACKET;
	if(RecvSize != FLAP_HEADER_SIZE) return NET_WRONG_SIZE;

	//read and test flap data
	RecvSize = 0;
	while(RecvSize < GetPacketSize(Buffer))
	{
		RecvSize += recv(m_Server, (char*)Buffer+FLAP_HEADER_SIZE+RecvSize, GetPacketSize(Buffer)-RecvSize, 0);
		if(RecvSize == SOCKET_ERROR)
		{
			if(WSAGetLastError()==10054) return NET_CLOSED_CONNECTION;
			if(WSAGetLastError()==10060) return NET_TIMEOUT;
		}
	}
	if(EnablePacketLog) WritePacketToFile(m_pcPacketLogFileName, (FlapPacket*)Buffer, "SERVER");

	return NET_SUCCESS;
}
bool MICQClient::WaitAndReciveSnac(PBYTE &Buffer, WORD Fam, WORD Type, DWORD RequestId)
{
	BYTE CurrentAttempt = 0;
	while(1)
	{
		if(CurrentAttempt > LimitAttempts)
		{
			SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: Exceeded limit reciving attempts");
			return 0;
		}
		switch(ReciveBufferNew(Buffer))
		{
			case NET_CLOSED_CONNECTION:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_CLOSED_CONNECTION");
				return 0;
			case NET_TIMEOUT:
				CurrentAttempt ++;
				Sleep(1);
				continue;
			case NET_EMPTY_BUFFER:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_EMPTY_BUFFER");
				return 0;
			case NET_NULL_PACKET:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_NULL_PACKET");
				return 0;
			case NET_NOT_ICQ_PACKET:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_NOT_ICQ_PACKET");
				return 0;
			case NET_WRONG_SIZE:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_WRONG_SIZE");
				return 0;
			case NET_TOO_MANY_NULLS:
				SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)0, (LPARAM)"Error in WaitAndReciveSnac: NET_TOO_MANY_NULLS");
				return 0;
		}
		if(GetSNACRequest(Buffer) != RequestId && RequestId != 0x00000001)
		{
			Sleep(1);
			continue;
		}
		if(IsThisSNAC(Buffer, Fam, Type)) break;
		Sleep(50);//was 1 milisecond
	}
	char *pText = GetSnacFamFromBuffer(Buffer);
	SendMessage(MainWnd, MSGDATA_ADDTOLOG, (WPARAM)"Recived ", (LPARAM)pText);
	if(pText) delete [] pText;

	return 1;
}
bool MICQClient::_Stage1Authorization()
{
	//recv hello from server
	if(!WaitAndReciveSnac(m_pbBuffer, 0x00, 0x01)) return 0;
	//send hello packet
	if(!SendPacket(CreateHelloPacket())) return 0;
	//send SNAC(17,06)
	if(!SendPacket(CreateSnac1706Packet(m_cUIN, ++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(17,07)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x17, 0x07, m_dwRequestId)) return 0;
	char *Key = new char[64];
	memset(Key, 0, 64);
	memcpy(Key, m_pbBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2, 
		m_pbBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE]*0x100 + m_pbBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+1]);
	//send SNAC(17,02)
	if(!SendPacket(CreateSnac1702Packet(m_cUIN, m_cPassword, Key, ++m_wSequence, ++m_dwRequestId))) return 0;
	if(Key) delete [] Key;
	//recv SNAC(17,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x17, 0x03, m_dwRequestId)) return 0;
	AnalyseSnac1703Packet(m_pbBuffer, m_AddressPort, m_bCookie, m_wCookieSize);
	if(!m_wCookieSize || !m_bCookie) return 0;

	return 1;
}
bool MICQClient::_Stage2ProtocolNegotiation()
{
	//recv hello from server
	if(!WaitAndReciveSnac(m_pbBuffer, 0x00, 0x01)) return 0;
	//send cookie packet
	//sometimes error here
	if(!SendPacket(CreateCookiePacket(m_bCookie, m_wCookieSize, ++m_wSequence))) return 0;
	//recv SNAC(01,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x01, 0x03)) return 0;
	AnalyseSnac0103Packet(m_pbBuffer, m_ProtocolGroup);
	if(m_ProtocolGroup.FamilyNumber) delete [] m_ProtocolGroup.FamilyNumber;
	//send SNAC(01,17)
	if(!SendPacket(CreateSnac0117Packet(++m_wSequence))) return 0;
	//recv SNAC(01,18)/SNAC(01,15)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x01, 0x18)) return 0;
	AnalyseSnac0118Packet(m_pbBuffer, m_ProtocolGroup);

	return 1;
}
bool MICQClient::_Stage3ServicesSetup()
{
	//send SNAC(03,02)
	if(!SendPacket(CreateSnac0302Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(03,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x03, 0x03, m_dwRequestId)) return 0;
	AnalyseSnac0303Packet(m_pbBuffer, m_BuddieRights);//get buddie rights
	//send SNAC(09,02)
	if(!SendPacket(CreateSnac0902Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(09,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x09, 0x03, m_dwRequestId)) return 0;
	AnalyseSnac0903Packet(m_pbBuffer, m_PdRights);//get PD rights
	//send SNAC(02,02)
	if(!SendPacket(CreateSnac0202Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(02,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x02, 0x03, m_dwRequestId)) return 0;
	AnalyseSnac0203Packet(m_pbBuffer, m_LocateRights);//get locate rights
	//send SNAC(02,04)
	if(!SendPacket(CreateSnac0204Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//send SNAC(04,04)
	if(!SendPacket(CreateSnac0404Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(04,05)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x04, 0x05, m_dwRequestId)) return 0;
	AnalyseSnac0405Packet(m_pbBuffer, m_ICBMInfo);//get ICBM info
	//send SNAC(13,02)
	if(!SendPacket(CreateSnac1302Packet(++m_wSequence, ++m_dwRequestId))) return 0;
	//recv SNAC(13,03)
	if(!WaitAndReciveSnac(m_pbBuffer, 0x13, 0x03, m_dwRequestId)) return 0;
	//no one normal description of this snac, can't analyse
	//send SNAC(13,04)
	if(!SendPacket(CreateSnac1304Packet(++m_wSequence, ++m_dwRequestId))) return 0;//was 13, 05
	//recv SNAC(13,06) that can went more than once
	//very important: do not skip no one of SNAC(13,06). If skip at least one SNAC then UIN and Nickname will be TEMPLATED!
	//Template UIN = "000". See AnalyseSnac1306Packet() for details
	while(1)
	{
		if(!WaitAndReciveSnac(m_pbBuffer, 0x13, 0x06, m_dwRequestId)) return 0;
		AnalyseSnac1306Packet(m_pbBuffer, m_MasterGroup);
		if(!GetSnacFlag01(m_pbBuffer)) break;
		Sleep(1);
	}
	//send SNAC(13,07)
	if(!SendPacket(CreateSnac1307Packet(++m_wSequence, ++m_dwRequestId))) return 0;

	return 1;
}
bool MICQClient::_Stage4FinalActions()
{
	//send SNAC(01,1e)
	if(!SendPacket(CreateSnac011ePacket(++m_wSequence, ++m_dwRequestId))) return 0;
	//send SNAC(01,02)
	if(!SendPacket(CreateSnac0102Packet(++m_wSequence, ++m_dwRequestId))) return 0;

	return 1;
}
bool MICQClient::_OfflineMessagesRequest()
{
	if(!SendPacket(CreateSnac1502Packet(m_cUIN, 0x3c, ++m_wSequence))) return 0;
	return 1;
}
void MICQClient::AnalyseIncomePacket(FlapPacket *recvPacket)
{
	WORD SnacFam = recvPacket->Data[0]*0x100 + recvPacket->Data[1];
	WORD SnacType = recvPacket->Data[2]*0x100 + recvPacket->Data[3];
	char *UIN = NULL;
	char *Message = NULL;
	UserInfo mUserInfo;
	//MSTR HelpStr;
	//WORD Count;
	bool NoMoreMessages;
	vector<Buddie>::iterator BuddieIter;

	switch(SnacFam)
	{
		case 0x15:
			switch(SnacType)
			{
				case 0x03: //offline message
					AnalyseSnac1503Packet((PBYTE)recvPacket, UIN, Message, NoMoreMessages);
					if(NoMoreMessages)
					{
						if(!SendPacket(CreateSnac1502Packet(m_cUIN, 0x3e, ++m_wSequence))) return;
					}
					else cout<<endl<<"Offline msg: "<<Message<<" from "<<UIN<<endl;
					if(UIN) delete [] UIN;
					break;
			}
			break;
		case 0x03:
			switch(SnacType)
			{
				case 0x0a: //can't send notification about some buddies
					//AnalyseSnac030aPacket((PBYTE)recvPacket, FLAP_HEADER_SIZE+SwapWord(recvPacket->DFLen), HelpStr, Count);
					//for(int i=0; i<Count; i++)
					//{
					//	strcat(HelpStr[i], " can't notify");
					//	cout<<HelpStr[i]<<endl;
					//}
					//clear HelpStr
					//for(int i=0; i<Count; i++)
					//	if(HelpStr[i]) delete [] HelpStr[i];
					//if(HelpStr) delete [] HelpStr;
					break;
				case 0x0b: //someone online
					AnalyseSnac030bPacket((PBYTE)recvPacket, mUserInfo);
					//find contact and set him online status to 1
					for(WORD i=0; i<m_MasterGroup.mGroup.size(); i++)
					{
						BuddieIter = find_if(m_MasterGroup.mGroup[i].mBuddie.begin(), m_MasterGroup.mGroup[i].mBuddie.end(),
							bind2nd( ptr_fun(Buddie::EqualUIN), (char*)mUserInfo.UIN ));
						if(BuddieIter != m_MasterGroup.mGroup[i].mBuddie.end())
						{
							BuddieIter->Online = true;
							SendMessage(MainWnd, MSGDATA_BOLDCONTACT, (WPARAM)i, (LPARAM)(BuddieIter-m_MasterGroup.mGroup[i].mBuddie.begin()));
							break;
						}
						Sleep(1);
					}
					break;
				case 0x0c: //someone offline
					//Need to find and fix error in this function
					//AnalyseSnac030cPacket((PBYTE)recvPacket, FLAP_HEADER_SIZE+SwapWord(recvPacket->DFLen), HelpStr, Count);
					//if(!HelpStr || !Count) break;
					//do some
					//clear HelpStr
					//for(int i=0; i<Count; i++)
					//	if(HelpStr[i]) delete [] HelpStr[i];
					//if(HelpStr) delete [] HelpStr;
					//cout<<"Someone(s) go offline"<<endl;
					break;
			}
			break;
		case 0x04:
			switch(SnacType)
			{
				case 0x07: //get incomming msg
					AnalyseSnac0407Packet((PBYTE)recvPacket,FLAP_HEADER_SIZE+SwapWord(recvPacket->DFLen), UIN, Message);
					//Add message directly to GUI [FEEDBACK]
					SendMessage(MainWnd, MSGDATA_DATAMESSAGE, (WPARAM)UIN, (LPARAM)Message);
					if(Message) delete [] Message;
					break;
				case 0x0b: //get incoming autoresponse
					break;
			}
			break;
		case 0x13:
			switch(SnacType)
			{
				case 0x19: //someone want your authorization
					AnalyseSnac1319Packet((PBYTE)recvPacket, UIN, Message);
					if(Message) delete [] Message;
					if(UIN) delete [] UIN;
					break;
				case 0x1b: //authorize reply
					break;
			}
			break;
		default: //unknown packets
			break;
	}
}
bool MICQClient::bGetSettingsFromIni(char *FileName)
{
	//prepare path with ini file name
	DWORD PathSize = strlen(m_pcCurrentDirectory)+strlen(FileName)+2; 
	char *FullPath = new char[PathSize];
	memset(FullPath, 0, PathSize);
	strcpy(FullPath, m_pcCurrentDirectory);
	strcat(FullPath, "\\");
	strcat(FullPath, FileName);
	//test on existing file
	FILE *pFile = fopen(FullPath, "r");
	if(!pFile)
	{
		delete [] FullPath;
		return 0;
	}
	//reading settings
	//[PacketLog]
	EnablePacketLog = (bool)GetPrivateProfileIntA("PacketLog", "Enable", 1, FullPath);
	RewritePacketLog = (bool)GetPrivateProfileIntA("PacketLog", "Rewrite", 1, FullPath);
	m_pcPacketLogFileName = new char[64];
	memset(m_pcPacketLogFileName, 0, 64);
	GetPrivateProfileStringA("PacketLog", "Name", "PackLog.txt", m_pcPacketLogFileName, 63, FullPath);
	delete [] FullPath;
	//[Net]
	m_NetTimeout.tv_sec = GetPrivateProfileIntA("Net", "RecvTimeout", 4000, FullPath);
	LimitAttempts = GetPrivateProfileIntA("Net", "RecvLimitAttempts", 10, FullPath);
	return 1;
}
