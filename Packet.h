#ifndef packetH
#define packetH

//====================================================================================
//constants
#define MAX_PACKET_DATA_SIZE 8192 //was 4096, 0xffff
#define MAX_TEST_PACKET_SIZE 8192
#define FLAP_HEADER_SIZE 6
#define SNAC_HEADER_SIZE 10
#define TLV_HEADER_SIZE 4
#define TLV_WORD_SIZE 6
#define ICQ_SIGN 0x2a
#define MAX_UIN_STR_SIZE 16
#define MAX_NICKNAME_STR_SIZE 64

//RATES
#define RATE_CLASS_SIZE 35
#define RATE_GROUP_HEADER_SIZE 4

//LOCATE_RIGHTS_REPLY_TAGS
#define MAX_SIG_LEN	1
#define MAX_CAPABILITIES_LEN 2
#define MAX_FIND_BY_EMAIL_LIST 3
#define MAX_CERTS_LEN 4	
#define MAX_SHORT_CAPABILITIES 5

//BUDDY_RIGHTS_REPLY_TAGS
#define MAX_BUDDIES	1
#define MAX_WATCHERS 2
#define MAX_TEMP_BUDDIES 4

//PD_RIGHTS_REPLY_TAGS
#define MAX_PERMITS	1
#define MAX_DENIES 2
#define MAX_TEMP_PERMITS 3

//FEEDBAG_RIGHTS_FLAGS
//#define INTERACTION_SUPPORTED 0x0001
//#define AUTHORIZATION_SUPPORTED	0x0002
//#define DOMAIN_SN_SUPPORTED	0x0004
//#define ICQ_NUM_SUPPORTED 0x0008
//#define SMS_NUM_SUPPORTED 0x0010
//#define ALIAS_ATTR_SUPPORTED 0x0020
//#define SMARTGRP_SUPPORTED 0x0040

//====================================================================================
//structures and types

typedef WORD WORD2[2];
typedef WORD2 *PWORD2;
typedef BYTE BYTE16[16];
typedef BYTE16 *PBYTE16;
typedef char **MSTR;
typedef BYTE BYTE4096[4096];
typedef BYTE4096 *MBYTE;

#pragma pack(push, 1)
struct FlapPacket
{
	BYTE Sign;//Command Start always 0x2a
	BYTE ChID;//Channel ID
	WORD SeqNum;//Sequence Number
	WORD DFLen;//Data Field Length
	BYTE Data[MAX_PACKET_DATA_SIZE];//Data
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SnacHeader//10 bytes
{
	WORD FamID;//Family ID
	WORD SubTypeID;//Sub type ID
	BYTE Flag0;//Flag 0
	BYTE Flag1;//Flag 1
	DWORD ReqID;//Request ID
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TLVPacket
{
	WORD TypeCode;//Type
	WORD LengthCode;//Length
	BYTE Data[MAX_PACKET_DATA_SIZE];//Data
};
#pragma pack(pop)

struct ProtocolGroup
{
	BYTE Count;
	WORD *FamilyNumber;
	WORD *FamilyVersion;
};

struct LocateRights
{
	WORD MaxSigLen;
	WORD MaxCapabilitiesLen;
	WORD MaxFindByEmailList;
	WORD MaxCertsLen;
	WORD MaxShortCapabilities;
};

struct BuddyRights
{
	WORD MaxBuddies;
	WORD MaxWatchers;
	WORD MaxTempBuddies;
};

struct PdRights
{
	WORD MaxPermits;
	WORD MaxDenies;
	WORD MaxTempPermits;
};

struct RateClass
{
	WORD ClassID;
	DWORD WindowSize;
	DWORD ClearThreshold;
	DWORD AlertThreshold;
	DWORD LimitThreshold;
	DWORD DisconnectThreshold;
	DWORD CurrentAverage;
	DWORD MaxAverage;
	DWORD LastArrivalDelta;
	BYTE DroppingSNACs;
};

struct RateGroup
{
	WORD GroupID;
	WORD NumberOfPairs;
	DWORD *SnacID;
};

struct LimitsInfo
{
	WORD NumOfRateClasses;
	RateClass *mRateClass;
	RateGroup *mRateGroup;
};

struct ICBMInfo//Inter Client Basic Message
{
	WORD maxSlots;
	DWORD icbmFlags;
	WORD maxIncomingICBMLen;
	WORD maxSourceEvil;
	WORD maxDestinationEvil;
	DWORD minInterICBMInterval;
};

#pragma pack(push, 1)
struct DCInfo
{
	DWORD Ip;
	DWORD Port;
	BYTE Type;
	WORD Version;
	DWORD Cookie;
	DWORD WebPort;
	DWORD ClientFeatures;
	DWORD LastTime[3];
	WORD Unknown;
};
#pragma pack(pop)

struct UserInfo
{
	char UIN[MAX_UIN_STR_SIZE];
	DCInfo mDCInfo;
	WORD UserClass;
	DWORD Status;
	DWORD ExternalIp;
	DWORD RegTime;
	DWORD LastOnline;
	DWORD OnlineTime;
	vector<PBYTE16> Capability;
};

struct Buddie
{
	WORD Id;
	char UIN[MAX_UIN_STR_SIZE];
	char Nickname[MAX_NICKNAME_STR_SIZE];
	bool Online;
	static bool EqualId(Buddie inBuddie, WORD inId)
	{
		return inBuddie.Id == inId;
	};
	static bool EqualUIN(Buddie inBuddie, char *inUIN)
	{
		return strcmp(inBuddie.UIN, inUIN) == 0;
	};
	static bool EqualOnlineStatus(Buddie inBuddie, bool IsOnline)
	{
		return inBuddie.Online == IsOnline;
	};
};

struct Group
{
	WORD Id;
	vector<Buddie> mBuddie;
	static bool EqualId(Group inGroup, WORD inId)
	{
		return inGroup.Id == inId;
	};
};

struct MasterGroup
{
	vector<Group> mGroup;
	BYTE PrivacyFlag;//1,2,3,4,5
	DWORD Visibility;//0xffffffff, 0x00000004
};

//====================================================================================
//functions

//Swap functions
WORD SwapWord(WORD value);
DWORD SwapDWord(DWORD value);

//Packet operations functions
void AddByteToPacket(FlapPacket *inFlapPacket, BYTE value);
void AddWordToPacket(FlapPacket *inFlapPacket, WORD value);
void AddDWordToPacket(FlapPacket *inFlapPacket, DWORD value);
void AddStringToPacket(FlapPacket *inFlapPacket, char *value);
void AddTLVToPacket(FlapPacket *inFlapPacket, TLVPacket *inTLVPacket);

void AddTLVByteMassToPacket(FlapPacket *inFlapPacket, WORD TypeCode, BYTE *value, WORD Size);
void AddTLVStringToPacket(FlapPacket *inFlapPacket, WORD TypeCode, char *value);
void AddTLVByteToPacket(FlapPacket *inFlapPacket, WORD TypeCode, BYTE value);
void AddTLVWordToPacket(FlapPacket *inFlapPacket, WORD TypeCode, WORD value);
void AddTLVDWordToPacket(FlapPacket *inFlapPacket, WORD TypeCode, DWORD value);
void AddSnacHeaderToPacket(FlapPacket *inFlapPacket, WORD inFamID, WORD inSubTypeID, BYTE inFlag0, BYTE inFlag1, DWORD inRegId);

void AddByteToTLV(TLVPacket *inTLVPacket, BYTE value);
void AddWordToTLV(TLVPacket *inTLVPacket, WORD value);
void AddDWordToTLV(TLVPacket *inTLVPacket, DWORD value);
void AddByteMassToTLV(TLVPacket *inTLVPacket, BYTE *value, WORD size);

//Create packets functions
FlapPacket *CreateHelloPacket();
FlapPacket *CreateSnac1706Packet(char *inUIN, WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1702Packet(char *inUIN, char *inPassword, char *Key, WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateCookiePacket(BYTE *inCookie, WORD CookieSize, WORD inSequence);
FlapPacket *CreateSnac0117Packet(WORD inSequence, DWORD ReqID=0x00000001);
//FlapPacket *CreateSnac0106Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0108Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0202Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0204Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0302Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0404Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0402Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0902Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1302Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1304Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1305Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1307Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac011ePacket(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0102Packet(WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac1502Packet(char *UIN, WORD DataType, WORD inSequence, DWORD ReqID=0x00000001);
FlapPacket *CreateSnac0406Packet(char *UIN, char *Message, WORD Type, WORD inSequence, DWORD ReqID=0x00000001);

//Analyse packets functions
void AnalyseSnac1703Packet(PBYTE inBuffer, sockaddr_in &outIP, PBYTE &Cookie, WORD &CookieSize);
void AnalyseSnac0103Packet(PBYTE inBuffer, ProtocolGroup &Groups);
void AnalyseSnac0118Packet(PBYTE inBuffer, ProtocolGroup &Groups);
void AnalyseSnac0107Packet(PBYTE inBuffer, LimitsInfo &outLimitsInfo);
void AnalyseSnac0203Packet(PBYTE inBuffer, LocateRights &Rights);
void AnalyseSnac0303Packet(PBYTE inBuffer, BuddyRights &Rights);
void AnalyseSnac0405Packet(PBYTE inBuffer, ICBMInfo &outICBMInfo);
void AnalyseSnac0903Packet(PBYTE inBuffer, PdRights &Rights);
void AnalyseSnac1303Packet(PBYTE inBuffer, int inSizeData, PWORD &outParams, WORD &Count);//get only tlv4
void AnalyseSnac1306Packet(PBYTE inBuffer, MasterGroup &mMasterGroup);//without vis/invis list
void AnalyseSnac1503Packet(PBYTE inBuffer, PCHAR &UIN, PCHAR &Message, bool &NoMoreMessages);
void AnalyseSnac030bPacket(PBYTE inBuffer, UserInfo &outUserInfo);
void AnalyseSnac030cPacket(PBYTE inBuffer, int inSizeData, MSTR &UIN, WORD &Count);
void AnalyseSnac0407Packet(PBYTE inBuffer, int inSizeData, PCHAR &UIN, PCHAR &Message);
void AnalyseSnac030aPacket(PBYTE inBuffer, int inSizeData, MSTR &BuddieId, WORD &Count);
void AnalyseSnac1319Packet(PBYTE inBuffer, PCHAR &UIN, PCHAR &Reason);
//not tested 2
void AnalyseSnac131bPacket(PBYTE inBuffer, PCHAR &UIN, bool &Accept, PCHAR &Message);
void AnalyseSnac040bPacket(PBYTE inBuffer, PCHAR &UIN, PCHAR &Status);

//Help functions
void PrintErrorStrTLV8(WORD inErrorCode);
bool IsThisSNAC(BYTE *inBuffer, WORD inFamID, WORD inSubTypeID);
DWORD GetSNACFamily(BYTE *inBuffer);
DWORD GetSNACRequest(BYTE *inBuffer);
WORD GetSnacFlag01(BYTE *inBuffer);
char *GetSnacFamFromBuffer(PBYTE Buffer);
void PrintMasterGroup(MasterGroup *pMasterGroup);
bool TestMasterGroup(MasterGroup *pMasterGroup);
bool WriteBufferToFile(char *FileName, PBYTE Buffer, WORD Size);
bool WritePacketToFile(char *FileName, FlapPacket *Packet, char *Hint);
WORD GetPacketSize(PBYTE Buffer);

//Conversions functions
wchar_t *utf8_to_unicode(char *utf8_string);
char *unicode_to_1251(wchar_t *unicode_string);
char *ByteWcharToChar(BYTE *bMass);
tm *DWordToPtm(DWORD dwTime);

#endif