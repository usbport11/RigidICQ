#include "stdafx.h"
#include "Packet.h"
#include "md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WORD SwapWord(WORD value)
{
	return ((value & 0x00FF) << 8) + ((value & 0xFF00) >> 8);
}
DWORD SwapDWord(DWORD value)
{
	DWORD Result=0;
	__asm
	{
		MOV EAX, value
		BSWAP EAX
		MOV Result,EAX
	};
	return Result;
}
void AddByteToPacket(FlapPacket *inFlapPacket, BYTE value)
{
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	memcpy(inFlapPacket->Data+DataLen, (void*)&value, sizeof(BYTE));
	inFlapPacket->DFLen = SwapWord(DataLen + sizeof(BYTE));
}
void AddWordToPacket(FlapPacket *inFlapPacket, WORD value)
{
	WORD SwapValue = SwapWord(value);
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	memcpy(inFlapPacket->Data+DataLen, (void*)&SwapValue, sizeof(WORD));
	inFlapPacket->DFLen = SwapWord(DataLen + sizeof(WORD));
}
void AddDWordToPacket(FlapPacket *inFlapPacket, DWORD value)
{
	DWORD SwapValue = SwapDWord(value);
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	memcpy(inFlapPacket->Data+DataLen, (void*)&SwapValue, sizeof(DWORD));
	inFlapPacket->DFLen = SwapWord(DataLen + sizeof(DWORD));
}
void AddStringToPacket(FlapPacket *inFlapPacket, char *value)
{
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	memcpy(inFlapPacket->Data+DataLen, (void*)value, strlen(value));
	inFlapPacket->DFLen = SwapWord(DataLen + (WORD)strlen(value));
}
void AddTLVToPacket(FlapPacket *inFlapPacket, TLVPacket *inTLVPacket)
{
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(inTLVPacket->LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)inTLVPacket, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddTLVByteMassToPacket(FlapPacket *inFlapPacket, WORD TypeCode, BYTE *value, WORD Size)
{
	//create tlvpacket
	TLVPacket Result;
	memset((void*)&Result, 0, sizeof(TLVPacket));
	Result.TypeCode = SwapWord(TypeCode);
	Result.LengthCode = SwapWord(Size);
	memcpy((void*)Result.Data, (void *)value, Size);
	//insert tlv in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(Result.LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)&Result, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddTLVStringToPacket(FlapPacket *inFlapPacket, WORD TypeCode, char *value)
{
	//create tlvpacket
	TLVPacket Result;
	memset((void*)&Result, 0, sizeof(TLVPacket));
	Result.TypeCode = SwapWord(TypeCode);
	Result.LengthCode = SwapWord((WORD)strlen(value));
	memcpy((void*)Result.Data, (void *)value, strlen(value));
	//insert tlv in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(Result.LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)&Result, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddTLVWordToPacket(FlapPacket *inFlapPacket, WORD TypeCode, WORD value)
{
	//create tlvpacket
	TLVPacket Result;
	memset((void*)&Result, 0, sizeof(TLVPacket));
	Result.TypeCode = SwapWord(TypeCode);
	Result.LengthCode = SwapWord(sizeof(WORD));
	WORD SwapValue = SwapWord(value);
	memcpy((void*)Result.Data, (void *)&SwapValue, sizeof(WORD));
	//insert tlv in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(Result.LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)&Result, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddTLVDWordToPacket(FlapPacket *inFlapPacket, WORD TypeCode, DWORD value)
{
	//create tlvpacket
	TLVPacket Result;
	memset((void*)&Result, 0, sizeof(TLVPacket));
	Result.TypeCode = SwapWord(TypeCode);
	Result.LengthCode = SwapWord(sizeof(DWORD));
	DWORD SwapValue = SwapDWord(value);
	memcpy((void*)Result.Data, (void *)&SwapValue, sizeof(DWORD));
	//insert tlv in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(Result.LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)&Result, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddTLVByteToPacket(FlapPacket *inFlapPacket, WORD TypeCode, BYTE value)
{
	//create tlvpacket
	TLVPacket Result;
	memset((void*)&Result, 0, sizeof(TLVPacket));
	Result.TypeCode = SwapWord(TypeCode);
	Result.LengthCode = SwapWord(sizeof(BYTE));
	memcpy((void*)Result.Data, (void *)&value, sizeof(WORD));
	//insert tlv in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	WORD SizeOfTLV = TLV_HEADER_SIZE + SwapWord(Result.LengthCode);
	memcpy(inFlapPacket->Data+DataLen, (void*)&Result, SizeOfTLV);
	inFlapPacket->DFLen = SwapWord(DataLen + SizeOfTLV);
}
void AddSnacHeaderToPacket(FlapPacket *inFlapPacket, WORD inFamID, WORD inSubTypeID, BYTE inFlag0, BYTE inFlag1, DWORD inRegId)
{
	//create snac header
	SnacHeader Result;
	memset((void*)&Result, 0, sizeof(SnacHeader));
	Result.FamID = SwapWord(inFamID);
	Result.SubTypeID = SwapWord(inSubTypeID);
	Result.Flag0 = inFlag0;
	Result.Flag1 = inFlag1;
	Result.ReqID = SwapDWord(inRegId);
	//insert snac in the end
	WORD DataLen = SwapWord(inFlapPacket->DFLen);
	memcpy(inFlapPacket->Data, (void*)&Result, SNAC_HEADER_SIZE);
	memcpy(inFlapPacket->Data+FLAP_HEADER_SIZE, &Result.ReqID, sizeof(DWORD));
	inFlapPacket->DFLen = SwapWord(SNAC_HEADER_SIZE);
}
void AddByteToTLV(TLVPacket *inTLVPacket, BYTE value)
{
	WORD DataLen = SwapWord(inTLVPacket->LengthCode);
	memcpy(inTLVPacket->Data+DataLen, (void*)&value, sizeof(BYTE));
	inTLVPacket->LengthCode = SwapWord(DataLen + sizeof(BYTE));
}
void AddWordToTLV(TLVPacket *inTLVPacket, WORD value)
{
	WORD DataLen = SwapWord(inTLVPacket->LengthCode);
	WORD SwapValue = SwapWord(value);
	memcpy(inTLVPacket->Data+DataLen, (void*)&SwapValue, sizeof(WORD));
	inTLVPacket->LengthCode = SwapWord(DataLen + sizeof(WORD));
}
void AddDWordToTLV(TLVPacket *inTLVPacket, DWORD value)
{
	WORD DataLen = SwapWord(inTLVPacket->LengthCode);
	DWORD SwapValue = SwapDWord(value);
	memcpy(inTLVPacket->Data+DataLen, (void*)&SwapValue, sizeof(DWORD));
	inTLVPacket->LengthCode = SwapWord(DataLen + sizeof(DWORD));
}
void AddByteMassToTLV(TLVPacket *inTLVPacket, BYTE *value, WORD size)
{
	WORD DataLen = SwapWord(inTLVPacket->LengthCode);
	memcpy(inTLVPacket->Data+DataLen, (void*)value, size);
	inTLVPacket->LengthCode = SwapWord(DataLen + size);
}
FlapPacket *CreateHelloPacket()
{
	BYTE Need[4]={0x00,0x00,0x00,0x01};
	FlapPacket *inFlapPacket;
	inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x01;
	inFlapPacket->SeqNum = SwapWord(rand()%0x7FFF);
	inFlapPacket->DFLen = SwapWord(0x0004);
	memcpy(inFlapPacket->Data, Need, 0x04);
	//add hello tlv
	AddTLVDWordToPacket(inFlapPacket,0x8003,0x00100000);//0x8003
	return inFlapPacket;
}
FlapPacket *CreateSnac1706Packet(char *inUIN, WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	memset(inFlapPacket, 0x00, sizeof(FlapPacket));
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);//increment sequence
	inFlapPacket->DFLen = 0;
	//add snac and uin
	AddSnacHeaderToPacket(inFlapPacket,0x0017,0x0006,0x00,0x00,ReqID);//SNAC(17,06)
	AddTLVStringToPacket(inFlapPacket, 0x0001, inUIN);
	return inFlapPacket;
}
FlapPacket *CreateSnac1702Packet(char *inUIN, char *inPassword, char *Key, WORD inSequence, DWORD ReqID)
{
	//create md5 hash string
	WORD KeyLen = (WORD)strlen(Key);
	char AIM_MD5_STRING[] = "AOL Instant Messenger (SM)";
	MMD5Interface Md5Interface;
	mir_md5_state_t MD5Context;
	mir_md5_byte_t Digest[16+1]={0};//+1
	Md5Interface.md5_init(&MD5Context);//error
	Md5Interface.md5_append(&MD5Context, (const mir_md5_byte_t *)inPassword, (int)strlen(inPassword));
	Md5Interface.md5_finish(&MD5Context, Digest);
	Md5Interface.md5_init(&MD5Context);
	Md5Interface.md5_append(&MD5Context, (const mir_md5_byte_t *)Key, KeyLen);
	Md5Interface.md5_append(&MD5Context, (const mir_md5_byte_t *)Digest, 16);
	Md5Interface.md5_append(&MD5Context, (const mir_md5_byte_t *)AIM_MD5_STRING, (int)strlen(AIM_MD5_STRING));
	Md5Interface.md5_finish(&MD5Context, Digest);

	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);//increment sequence
	inFlapPacket->DFLen = 0;
	//add authorize information
	AddSnacHeaderToPacket(inFlapPacket,0x0017,0x0002,0x00,0x00,ReqID);//SNAC(17,02)
	AddTLVStringToPacket(inFlapPacket, 0x0001, inUIN);//screen name or strUIN
	AddTLVStringToPacket(inFlapPacket, 0x0025, (char*)Digest);//md5 hash string
	AddTLVStringToPacket(inFlapPacket, 0x0003, "ICQBasic");//client name "MIMWin32"
	AddTLVWordToPacket(inFlapPacket, 0x0016, 0x010a);//client id = 0x010a
	AddTLVWordToPacket(inFlapPacket, 0x0017, 0x0014);//major number 1
	AddTLVWordToPacket(inFlapPacket, 0x0018, 0x0022);//minor number 0
	AddTLVWordToPacket(inFlapPacket, 0x0019, 0x0001);//lesser number 0
	AddTLVWordToPacket(inFlapPacket, 0x001a, 0xffff);//build number 1
	AddTLVDWordToPacket(inFlapPacket, 0x0014, 0x00000666);//distribution number 85
	AddTLVStringToPacket(inFlapPacket, 0x000f, "us");//language
	AddTLVStringToPacket(inFlapPacket, 0x000e, "en");//country
	AddTLVByteToPacket(inFlapPacket, 0x004a, 0x00);//0x0 - not using SSL
	AddDWordToPacket(inFlapPacket, 0x004C0000);//???? (0_0)
	return inFlapPacket;
}
FlapPacket *CreateCookiePacket(PBYTE inCookie, WORD CookieSize, WORD inSequence)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x01;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//add cookie
	AddDWordToPacket(inFlapPacket, 0x00000001);
	AddTLVByteMassToPacket(inFlapPacket, 0x0006, inCookie, CookieSize);
	return inFlapPacket;
}
FlapPacket *CreateSnac0117Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//add needed services
	AddSnacHeaderToPacket(inFlapPacket,0x0001,0x0017,0x00,0x00,ReqID);//SNAC(01,17)
	AddDWordToPacket(inFlapPacket, 0x00220001);//?
	AddDWordToPacket(inFlapPacket, 0x00010004);//Generic service controls v.4
	AddDWordToPacket(inFlapPacket, 0x00020001);//Location services v.1
	AddDWordToPacket(inFlapPacket, 0x00030001);//Buddy List management service v.1
	AddDWordToPacket(inFlapPacket, 0x00040001);//ICBM (messages) service v.1
	AddDWordToPacket(inFlapPacket, 0x00060001);//Invitation service v.1
	AddDWordToPacket(inFlapPacket, 0x00090001);//Privacy management service v.1
	AddDWordToPacket(inFlapPacket, 0x000b0001);//Usage stats service v.1
	AddDWordToPacket(inFlapPacket, 0x00130004);//Server Side Information (SSI) service v.4
	AddDWordToPacket(inFlapPacket, 0x00150001);//ICQ specific extensions service v.1
	AddDWordToPacket(inFlapPacket, 0x000a0001);//User lookup service v.1
	return inFlapPacket;
}
/*
FlapPacket *CreateSnac0106Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//ask for rate limits info
	AddSnacHeaderToPacket(inFlapPacket,0x0001,0x0006,0x00,0x00,ReqID);
	return inFlapPacket;
}
*/
FlapPacket *CreateSnac0108Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//accepted server limits
	AddSnacHeaderToPacket(inFlapPacket,0x0001,0x0008,0x00,0x00,ReqID);
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0002);
	AddWordToPacket(inFlapPacket, 0x0003);
	AddWordToPacket(inFlapPacket, 0x0004);
	AddWordToPacket(inFlapPacket, 0x0005);
	return inFlapPacket;
}
FlapPacket *CreateSnac0202Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request location service parameters and limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0002,0x0002,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac0204Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//location information (like client profile string, client directory profile string, client capabilities)
	AddSnacHeaderToPacket(inFlapPacket,0x0002,0x0004,0x00,0x00,ReqID);
	//create TLV5
	TLVPacket inTLVPacket;
	BYTE UUID_SupportICQ[16] = {0x09, 0x46,0x13, 0x4D, 0x4C, 0x7F, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
	inTLVPacket.TypeCode = SwapWord(0x0005);
	inTLVPacket.LengthCode = 0;
	memset(inTLVPacket.Data, 0, MAX_PACKET_DATA_SIZE);
	//maybe need other UUID's?
	AddByteMassToTLV(&inTLVPacket, UUID_SupportICQ, 16);
	AddTLVToPacket(inFlapPacket, &inTLVPacket);
	return inFlapPacket;
}
FlapPacket *CreateSnac0302Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request buddylist service parameters and limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0003,0x0002,0x00,0x00,ReqID);
	AddTLVWordToPacket(inFlapPacket, 0x05, 0x03);//want to get ARRIVE/DEPART for all buddies?
	return inFlapPacket;
}
FlapPacket *CreateSnac0404Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request location service parameters and limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0004,0x0004,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac0402Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//set icbm parameters
	AddSnacHeaderToPacket(inFlapPacket,0x0004,0x0002,0x00,0x00,ReqID);
	AddWordToPacket(inFlapPacket, 0x0000);//channel
	AddDWordToPacket(inFlapPacket, 0x00000003);//icbmFlags
	AddWordToPacket(inFlapPacket, 0x1f40);//maxIncomingICBMLen
	AddWordToPacket(inFlapPacket, 0x03e7);//maxSourceEvil
	AddWordToPacket(inFlapPacket, 0x03e7);//maxDestinationEvil
	AddDWordToPacket(inFlapPacket, 0x00000000);//minInterICBMInterval
	return inFlapPacket;
}
FlapPacket *CreateSnac0902Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request buddylist service parameters and limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0009,0x0002,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac1302Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request server-stored information (SSI) service limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0013,0x0002,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac1304Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	AddSnacHeaderToPacket(inFlapPacket,0x0013,0x0004,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac1305Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//check your SSI local copy up-to-date
	AddSnacHeaderToPacket(inFlapPacket,0x0013,0x0005,0x00,0x00,ReqID);
	AddWordToPacket(inFlapPacket, 0x4c26);
	AddWordToPacket(inFlapPacket, 0x0dfa);
	AddWordToPacket(inFlapPacket, 0x00d9);
	return inFlapPacket;
}
FlapPacket *CreateSnac1307Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request server-stored information (SSI) service limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0013,0x0007,0x00,0x00,ReqID);
	return inFlapPacket;
}
FlapPacket *CreateSnac011ePacket(WORD inSequence, DWORD ReqID)
{
	DWORD DirectCookie = rand() ^ (rand() << 16);
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	AddSnacHeaderToPacket(inFlapPacket,0x0001,0x001e,0x00,0x00,ReqID);
	AddTLVDWordToPacket(inFlapPacket, 0x06, 0x10010000);//Status flags
	//create TLV(0C)
	TLVPacket TLV_0C;
	TLV_0C.TypeCode = SwapWord(0x000c);
	TLV_0C.LengthCode = 0;
	AddDWordToTLV(&TLV_0C, 0x00000000);//Direct connect IP
	AddDWordToTLV(&TLV_0C, 0x00000000);//Direct connect Port
	AddByteToTLV(&TLV_0C, 0x04);//Direct connect port
	AddWordToTLV(&TLV_0C, 0x0009);//ICQ Version
	AddDWordToTLV(&TLV_0C, DirectCookie);//Direct cookie
	AddDWordToTLV(&TLV_0C, 0x50);//Web port
	AddDWordToTLV(&TLV_0C, 0x01);//Client features
	AddDWordToTLV(&TLV_0C, 0xffffffff);//Last info update time ?
	AddDWordToTLV(&TLV_0C, 0x00000451);//Last ext info update time ?
	AddDWordToTLV(&TLV_0C, 0x00000000);//Last ext status update time ?
	AddWordToTLV(&TLV_0C, 0x0000);//Unknown
	AddTLVToPacket(inFlapPacket, &TLV_0C);
	AddTLVWordToPacket(inFlapPacket, 0x1F, 0x0000);//Unknown
	//create TLV(1D)
	TLVPacket TLV_1D;
	TLV_1D.TypeCode = SwapWord(0x001d);
	TLV_1D.LengthCode = 0;
	AddWordToTLV(&TLV_0C, 0x0002);//Type item
	AddWordToTLV(&TLV_0C, 0x0404);//0x0400 + 4 + Note len(0)
	AddWordToTLV(&TLV_0C, 0x0000);//Emty note
	AddWordToTLV(&TLV_0C, 0x000e);//Type item
	AddWordToTLV(&TLV_0C, 0x0000);//Mood len(0)
	AddTLVToPacket(inFlapPacket, &TLV_1D);

	return inFlapPacket;
}
FlapPacket *CreateSnac0102Packet(WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//request server-stored information (SSI) service limitations
	AddSnacHeaderToPacket(inFlapPacket,0x0001,0x0002,0x00,0x00,ReqID);
	AddWordToPacket(inFlapPacket, 0x0022);//family number #22
	AddWordToPacket(inFlapPacket, 0x0001);//family #1 version
	AddWordToPacket(inFlapPacket, 0x0110);//family #1 tool id
	AddWordToPacket(inFlapPacket, 0x1246);//family #1 tool version
	AddWordToPacket(inFlapPacket, 0x0001);//1
	AddWordToPacket(inFlapPacket, 0x0004);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0013);//13
	AddWordToPacket(inFlapPacket, 0x0004);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0002);//2
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0003);//3
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0015);//15
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0004);//4
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0006);//6
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x0009);//9
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x000a);//a
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	AddWordToPacket(inFlapPacket, 0x000b);//b
	AddWordToPacket(inFlapPacket, 0x0001);
	AddWordToPacket(inFlapPacket, 0x0110);
	AddWordToPacket(inFlapPacket, 0x1246);
	return inFlapPacket;
}
FlapPacket *CreateSnac1502Packet(char *UIN, WORD DataType, WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	AddSnacHeaderToPacket(inFlapPacket,0x0015,0x0002,0x00,0x00,ReqID);
	TLVPacket TLV_01;
	TLV_01.TypeCode = SwapWord(0x0001);
	TLV_01.LengthCode = 0;
	AddWordToTLV(&TLV_01, 0x0800);//SwapWord(Length TLV - 2)
	AddDWordToTLV(&TLV_01, SwapDWord(atoi(UIN)));//UIN
	AddWordToTLV(&TLV_01, SwapWord(DataType));//type of request: 3c - get, 3e - delete
	AddWordToTLV(&TLV_01, 0x0000);//request sequence
	AddTLVToPacket(inFlapPacket, &TLV_01);
	return inFlapPacket;
}
FlapPacket *CreateSnac0406Packet(char *UIN, char *Message, WORD Type, WORD inSequence, DWORD ReqID)
{
	FlapPacket *inFlapPacket = new FlapPacket;
	inFlapPacket->Sign = ICQ_SIGN;
	inFlapPacket->ChID = 0x02;
	inFlapPacket->SeqNum = SwapWord(inSequence);
	inFlapPacket->DFLen = 0;
	//message snac
	AddSnacHeaderToPacket(inFlapPacket,0x0004,0x0006,0x00,0x00,ReqID);
	DWORD Cookie[2]={0};
	Cookie[0] = rand() ^ (rand() << 16);
	Cookie[1] = rand() ^ (rand() << 16);
	AddDWordToPacket(inFlapPacket, Cookie[0]);//cookie 1
	AddDWordToPacket(inFlapPacket, Cookie[1]);//cookie 2
	AddWordToPacket(inFlapPacket, Type);//type of msg
	AddByteToPacket(inFlapPacket, (BYTE)strlen(UIN));//length of UIN
	AddStringToPacket(inFlapPacket, UIN);//UIN
	if(Type == 0x0001)
	{
		//Create TLV(02)
		TLVPacket TLV_02;
		TLV_02.TypeCode = SwapWord(0x0002);
		TLV_02.LengthCode = 0;
		AddByteToTLV(&TLV_02, 0x05);//fragment id
		AddByteToTLV(&TLV_02, 0x01);//fragment ver
		AddWordToTLV(&TLV_02, 0x0001);//len of capabilities
		AddByteToTLV(&TLV_02, 0x01);//text only

		AddByteToTLV(&TLV_02, 0x01);//fragment id
		AddByteToTLV(&TLV_02, 0x01);//fragment ver
		AddWordToTLV(&TLV_02, (WORD)strlen(Message)+4);//str len + word + word
		AddWordToTLV(&TLV_02, 0x0000);//charset - ASCII
		AddWordToTLV(&TLV_02, 0x0000);//language
		AddByteMassToTLV(&TLV_02, (BYTE*)Message, (WORD)strlen(Message));
		AddTLVToPacket(inFlapPacket, &TLV_02);
		//Create TLV(06)
		AddDWordToPacket(inFlapPacket, 0x00060000);//Empty TLV
	}
	//here must be more hard type of message
	return inFlapPacket;
}

void AnalyseSnac1703Packet(PBYTE inBuffer, sockaddr_in &outIP, PBYTE &Cookie, WORD &CookieSize)
{
	//for test
	Cookie = NULL;
	CookieSize = 0;

	//test on errors -> TLV(1)+TLV(4)+TLV(8)
	WORD TLV1Len = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+3]+TLV_HEADER_SIZE;
	WORD TLVcode = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+1];
	if(TLVcode==0x04)
	{
		WORD TLV4Len = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+2]*0x100 +inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+3];
		char *ErrorLink = new char[TLV4Len+1];
		memset(ErrorLink, 0, TLV4Len+1);
		memcpy(ErrorLink, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+TLV_HEADER_SIZE, TLV4Len);
		cout<<"Error link: "<<ErrorLink<<endl;
		delete ErrorLink;
		TLVcode = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+TLV4Len+4]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+TLV4Len+5];
		if(TLVcode==0x08)
		{
			WORD ErrorCode = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+TLV4Len+8]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV1Len+TLV4Len+9];
			cout<<"Error code(after 04) = "<<dec<<ErrorCode<<endl;
			PrintErrorStrTLV8(ErrorCode);
		}
		return;
	}
	if(TLVcode==0x08)
	{
		WORD ErrorCode = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV_HEADER_SIZE+TLV1Len]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+TLV_HEADER_SIZE+TLV1Len+1];
		cout<<"Error code(only 08) = "<<dec<<ErrorCode<<endl;
		PrintErrorStrTLV8(ErrorCode);
		return;
	}

	//get new ip, port and cookie
	WORD LenBeforeIp = 25 + inBuffer[23]*0x100 + inBuffer[24];
	BYTE HeaderLen = 4;
	BYTE DataLen = inBuffer[LenBeforeIp+2]*0x100 + inBuffer[LenBeforeIp+3];
	//search ":"
	char *pdest = strchr((char*)inBuffer+LenBeforeIp+HeaderLen, ':');
	if(!pdest) return;
    WORD Pos = (unsigned int)(pdest - (char*)(inBuffer+LenBeforeIp+HeaderLen));
	//get ip
	char *IpAddress = new char[17];
	memset(IpAddress, 0, 17);
	memcpy(IpAddress, inBuffer+LenBeforeIp+HeaderLen, Pos);
	//get port
	char *Port = new char[6];
	memset(Port, 0, 6);
	memcpy(Port, inBuffer+LenBeforeIp+Pos+HeaderLen+1, DataLen-(Pos+1));
	WORD wPort=atoi(Port);
	outIP.sin_addr.S_un.S_addr = inet_addr(IpAddress);
	outIP.sin_family = AF_INET;
	outIP.sin_port = htons(wPort);
	//get cookie
	CookieSize = inBuffer[LenBeforeIp+4+DataLen+2]*0x100 + inBuffer[LenBeforeIp+4+DataLen+3];
	Cookie = new BYTE[CookieSize];
	memset(Cookie, 0, CookieSize);
	memcpy(Cookie, inBuffer+LenBeforeIp+4+DataLen+4, CookieSize);

	if(Port) delete [] Port;
	if(IpAddress) delete [] IpAddress;
}
void AnalyseSnac0103Packet(PBYTE inBuffer, ProtocolGroup &Groups)
{
	//Supported services
	Groups.Count = ((FLAP_HEADER_SIZE + inBuffer[4]*0x100 + inBuffer[5])-(FLAP_HEADER_SIZE+SNAC_HEADER_SIZE))/2;
	Groups.FamilyNumber = new WORD[Groups.Count];
	memset(Groups.FamilyNumber, 0, Groups.Count);
	for(int i=0; i<Groups.Count; i++)
		Groups.FamilyNumber[i] = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*2]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*2+1];
}
void AnalyseSnac0118Packet(PBYTE inBuffer, ProtocolGroup &Groups)
{
	//Services version numbers
	Groups.Count = ((FLAP_HEADER_SIZE + inBuffer[4]*0x100 + inBuffer[5])-(FLAP_HEADER_SIZE+SNAC_HEADER_SIZE))/2;
	Groups.FamilyNumber = new WORD[Groups.Count];
	memset(Groups.FamilyNumber, 0, Groups.Count);
	Groups.FamilyVersion = new WORD[Groups.Count];
	memset(Groups.FamilyVersion, 0, Groups.Count);
	for(int i=0; i<Groups.Count; i++)
	{
		Groups.FamilyNumber[i] = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*4]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*4+1];
		Groups.FamilyVersion[i] = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*4+2]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+i*4+3];
	}
}
void AnalyseSnac0107Packet(PBYTE inBuffer, LimitsInfo &outLimitsInfo)
{
	//Rate limits information
	outLimitsInfo.NumOfRateClasses = inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+1];
	outLimitsInfo.mRateClass = new RateClass[outLimitsInfo.NumOfRateClasses];
	outLimitsInfo.mRateGroup = new RateGroup[outLimitsInfo.NumOfRateClasses];
	int PrevGroupLen = 0;
	int Prefix0 = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2;
	int Prefix1 = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2+RATE_CLASS_SIZE*outLimitsInfo.NumOfRateClasses;
	for(int i=0; i<outLimitsInfo.NumOfRateClasses; i++)
	{
		//RateClass
		memcpy(&outLimitsInfo.mRateClass[i].ClassID, inBuffer + Prefix0 + i*RATE_CLASS_SIZE, sizeof(WORD));
		outLimitsInfo.mRateClass[i].ClassID = SwapWord(outLimitsInfo.mRateClass[i].ClassID);
		memcpy(&outLimitsInfo.mRateClass[i].WindowSize, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 2, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].WindowSize = SwapDWord(outLimitsInfo.mRateClass[i].WindowSize);
		memcpy(&outLimitsInfo.mRateClass[i].ClearThreshold, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 6, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].ClearThreshold = SwapDWord(outLimitsInfo.mRateClass[i].ClearThreshold);
		memcpy(&outLimitsInfo.mRateClass[i].AlertThreshold, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 10, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].AlertThreshold = SwapDWord(outLimitsInfo.mRateClass[i].AlertThreshold);
		memcpy(&outLimitsInfo.mRateClass[i].LimitThreshold, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 14, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].LimitThreshold = SwapDWord(outLimitsInfo.mRateClass[i].LimitThreshold);
		memcpy(&outLimitsInfo.mRateClass[i].DisconnectThreshold, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 18, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].DisconnectThreshold = SwapDWord(outLimitsInfo.mRateClass[i].DisconnectThreshold);
		memcpy(&outLimitsInfo.mRateClass[i].CurrentAverage, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 22, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].CurrentAverage = SwapDWord(outLimitsInfo.mRateClass[i].CurrentAverage);
		memcpy(&outLimitsInfo.mRateClass[i].MaxAverage, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 26, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].MaxAverage = SwapDWord(outLimitsInfo.mRateClass[i].MaxAverage);
		memcpy(&outLimitsInfo.mRateClass[i].LastArrivalDelta, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 30, sizeof(DWORD));
		outLimitsInfo.mRateClass[i].LastArrivalDelta = SwapDWord(outLimitsInfo.mRateClass[i].LastArrivalDelta);
		memcpy(&outLimitsInfo.mRateClass[i].DroppingSNACs, inBuffer + Prefix0 + i*RATE_CLASS_SIZE + 34, sizeof(BYTE));

		//RateGroup
		memcpy(&outLimitsInfo.mRateGroup[i].GroupID, inBuffer + Prefix1 + PrevGroupLen, sizeof(WORD));
		outLimitsInfo.mRateGroup[i].GroupID = SwapWord(outLimitsInfo.mRateGroup[i].GroupID);
		memcpy(&outLimitsInfo.mRateGroup[i].NumberOfPairs, inBuffer + Prefix1 + PrevGroupLen + 2, sizeof(WORD));
		outLimitsInfo.mRateGroup[i].NumberOfPairs = SwapWord(outLimitsInfo.mRateGroup[i].NumberOfPairs);
		outLimitsInfo.mRateGroup[i].SnacID = new DWORD[outLimitsInfo.mRateGroup[i].NumberOfPairs];
		for(int j=0; j<outLimitsInfo.mRateGroup[i].NumberOfPairs; j++)
		{
			memcpy(&outLimitsInfo.mRateGroup[i].SnacID[j], inBuffer + Prefix1 + PrevGroupLen + RATE_GROUP_HEADER_SIZE + j*4, sizeof(DWORD));
			outLimitsInfo.mRateGroup[i].SnacID[j] = SwapDWord(outLimitsInfo.mRateGroup[i].SnacID[j]);
		}
		PrevGroupLen += RATE_GROUP_HEADER_SIZE + outLimitsInfo.mRateGroup[i].NumberOfPairs*4;
	}
}
void AnalyseSnac0203Packet(PBYTE inBuffer, LocateRights &Rights)
{
	//Location service limitations
	int TLVChainLen = inBuffer[4]*0x100 + inBuffer[5] - SNAC_HEADER_SIZE;
	int i = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	WORD TLVLen = 0;
	WORD TLVType = 0;
	while(i < TLVChainLen+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE)
	{
		TLVType = inBuffer[i]*0x100 + inBuffer[i+1];
		TLVLen = inBuffer[i+2]*0x100 + inBuffer[i+3];
		switch(TLVType)
		{
			case MAX_SIG_LEN:
				Rights.MaxSigLen = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_CAPABILITIES_LEN:
				Rights.MaxCapabilitiesLen = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_FIND_BY_EMAIL_LIST:
				Rights.MaxFindByEmailList = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_CERTS_LEN:
				Rights.MaxCertsLen = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_SHORT_CAPABILITIES:
				Rights.MaxShortCapabilities = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
		}
		i += TLVLen;
		Sleep(1);
	}
}
void AnalyseSnac0303Packet(PBYTE inBuffer, BuddyRights &Rights)
{
	int TLVChainLen = inBuffer[4]*0x100 + inBuffer[5] - SNAC_HEADER_SIZE;
	int i = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	WORD TLVLen = 0;
	WORD TLVType = 0;
	while(i < TLVChainLen+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE)
	{
		TLVType = inBuffer[i]*0x100 + inBuffer[i+1];
		TLVLen = inBuffer[i+2]*0x100 + inBuffer[i+3];
		switch(TLVType)
		{
			case MAX_BUDDIES:
				Rights.MaxBuddies = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_WATCHERS:
				Rights.MaxWatchers = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_TEMP_BUDDIES:
				Rights.MaxTempBuddies= inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
		}
		i += TLVLen;
		Sleep(1);
	}
}
void AnalyseSnac0405Packet(PBYTE inBuffer, ICBMInfo &outICBMInfo)
{
	//ICBM service parameters
	//Fixed size packet
	memcpy(&outICBMInfo.maxSlots, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE, sizeof(WORD));
	outICBMInfo.maxSlots = SwapWord(outICBMInfo.maxSlots);
	memcpy(&outICBMInfo.icbmFlags, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2, sizeof(DWORD));
	outICBMInfo.icbmFlags = SwapDWord(outICBMInfo.icbmFlags);
	memcpy(&outICBMInfo.maxIncomingICBMLen, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+6, sizeof(WORD));
	outICBMInfo.maxIncomingICBMLen = SwapWord(outICBMInfo.maxIncomingICBMLen);
	memcpy(&outICBMInfo.maxSourceEvil, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+8, sizeof(WORD));
	outICBMInfo.maxSourceEvil = SwapWord(outICBMInfo.maxSourceEvil);
	memcpy(&outICBMInfo.maxDestinationEvil, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+10, sizeof(WORD));
	outICBMInfo.maxDestinationEvil = SwapWord(outICBMInfo.maxDestinationEvil);
	memcpy(&outICBMInfo.minInterICBMInterval, inBuffer+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+12, sizeof(DWORD));
	outICBMInfo.minInterICBMInterval = SwapDWord(outICBMInfo.minInterICBMInterval);
}
void AnalyseSnac0903Packet(PBYTE inBuffer, PdRights &Rights)
{
	int TLVChainLen = inBuffer[4]*0x100 + inBuffer[5] - SNAC_HEADER_SIZE;
	int i = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	WORD TLVLen = 0;
	WORD TLVType = 0;
	while(i < TLVChainLen+FLAP_HEADER_SIZE+SNAC_HEADER_SIZE)
	{
		TLVType = inBuffer[i]*0x100 + inBuffer[i+1];
		TLVLen = inBuffer[i+2]*0x100 + inBuffer[i+3];
		switch(TLVType)
		{
			case MAX_PERMITS:
				Rights.MaxPermits = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_DENIES:
				Rights.MaxDenies = inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
			case MAX_TEMP_PERMITS:
				Rights.MaxTempPermits= inBuffer[i+4]*0x100 + inBuffer[i+5];
				break;
		}
		i += TLVLen;
		Sleep(1);
	}
}
void AnalyseSnac1303Packet(PBYTE inBuffer, int inSizeData, PWORD &outParams, WORD &Count)
{
	//SSI service parameters
	//Need to get TLV4. Other unknown :(
	Count = (inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+2]*0x100 + inBuffer[FLAP_HEADER_SIZE+SNAC_HEADER_SIZE+3])/2;
	outParams = new WORD[Count];
	int Prefix = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE + TLV_HEADER_SIZE;
	for(int i=0; i<Count; i++)
		outParams[i] = inBuffer[Prefix + i*2]*0x100 + inBuffer[Prefix + i*2 + 1];
}
void AnalyseSnac1306Packet(PBYTE inBuffer, MasterGroup &mMasterGroup)
{
	WORD Pos = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	BYTE SSIVersion = inBuffer[Pos];
	WORD CountItems = inBuffer[Pos+1]*0x100 + inBuffer[Pos+2];
	Pos += 3;
	WORD NameLen = 0;
	WORD GroupID = 0;
	WORD BuddieID = 0;
	WORD TLVType = 0;
	WORD TLVChainLen = 0;
	WORD ItemType = 0;
	WORD j = 0;
	Group addGroup;
	Buddie addBuddie;
	vector<Group>::iterator GroupIter;
	vector<Buddie>::iterator BuddieIter;
	char *ConvertedStr = 0;

	for(WORD i=0; i<CountItems; i++)
	{
		NameLen = inBuffer[Pos]*0x100 + inBuffer[Pos+1];
		GroupID = inBuffer[Pos+2+NameLen]*0x100 + inBuffer[Pos+2+NameLen+1];
		BuddieID = inBuffer[Pos+2+NameLen+2]*0x100 + inBuffer[Pos+2+NameLen+3];
		ItemType = inBuffer[Pos+2+NameLen+4]*0x100 + inBuffer[Pos+2+NameLen+5];
		TLVChainLen = inBuffer[Pos+2+NameLen+6]*0x100 + inBuffer[Pos+2+NameLen+6+1];
		//cout<<NameLen<<" "<<GroupID<<" "<<BuddieID<<" "<<ItemType<<" "<<TLVChainLen<<endl;
		Pos += 2 + NameLen + 8;
		j = Pos;

		switch(ItemType)
		{
			case 0x00: //buddies
				GroupIter = find_if(mMasterGroup.mGroup.begin(), mMasterGroup.mGroup.end(), bind2nd( ptr_fun(Group::EqualId), GroupID ));
				if(GroupIter == mMasterGroup.mGroup.end())
				{
					cout<<"Group "<<GroupID<<" not found!"<<endl;
					break;
				}
				BuddieIter = find_if(GroupIter->mBuddie.begin(), GroupIter->mBuddie.end(), bind2nd( ptr_fun(Buddie::EqualId), BuddieID ));
				if(BuddieIter == GroupIter->mBuddie.end())
				{
					cout<<"Buddie "<<BuddieID<<" not found!"<<endl;
					break;
				}
				//Nickname and UIN
				memset(BuddieIter->Nickname, 0, MAX_NICKNAME_STR_SIZE);
				BuddieIter->Id = BuddieID;
				if(NameLen > MAX_UIN_STR_SIZE) NameLen = MAX_UIN_STR_SIZE;
				memset(BuddieIter->UIN, 0, MAX_UIN_STR_SIZE);
				memcpy(BuddieIter->UIN, inBuffer+j-NameLen-8, NameLen);
				BuddieIter->Online = false;
				while(j < Pos+TLVChainLen)
				{
					TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
					switch(TLVType)
					{
						case 0xca: //privacy setting
							mMasterGroup.PrivacyFlag = inBuffer[j+TLV_HEADER_SIZE];
							break;
						case 0xd0: //unknown
							break;
						case 0xd1: //unknown
							break;
						case 0xd2: //unknown
							break;
						case 0xd3: //unknown
							break;
						case 0xcb: //visibility
							memcpy(&mMasterGroup.Visibility, inBuffer+j+TLV_HEADER_SIZE, sizeof(DWORD));
							mMasterGroup.Visibility = SwapDWord(mMasterGroup.Visibility);
							break;
						//===================================
						case 0x13a: //unknown
							break;
						case 0x13c: //unknown
							break;
						case 0x137: //unknown
							break;
						case 0x15c: //unknown
							break;
						case 0x15d: //unknown
							break;
						case 0x6d: //unknown
							break;
						case 0x131: //nickname
							if(!strlen(BuddieIter->Nickname)) break;
							if(inBuffer[j+2]*0x100 + inBuffer[j+3] > 63) memcpy(BuddieIter->Nickname, inBuffer+j+TLV_HEADER_SIZE, 63);
							else memcpy(BuddieIter->Nickname, inBuffer+j+TLV_HEADER_SIZE, inBuffer[j+2]*0x100 + inBuffer[j+3]);
							ConvertedStr = unicode_to_1251(utf8_to_unicode(BuddieIter->Nickname));
							memset(BuddieIter->Nickname, 0, MAX_NICKNAME_STR_SIZE);
							memcpy(BuddieIter->Nickname, ConvertedStr, strlen(ConvertedStr));
							break;
					}
					j += TLV_HEADER_SIZE + inBuffer[j+2]*0x100 + inBuffer[j+3];
				}
				break;
			case 0x20: //unknown
				break;
			case 0x01: //groups
				while(j < Pos+TLVChainLen)
				{	
					TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
					switch(TLVType)
					{
						case 0xc8:
							if(GroupID == 0x00) //master group
							{
								mMasterGroup.mGroup.reserve((inBuffer[j+2]*0x100 + inBuffer[j+3])/2);
								for(WORD k=0; k<mMasterGroup.mGroup.capacity(); k++)
								{
									addGroup.Id = inBuffer[j+TLV_HEADER_SIZE+k*2]*0x100 + inBuffer[j+TLV_HEADER_SIZE+k*2+1];//error here
									addGroup.mBuddie.clear();
									mMasterGroup.mGroup.push_back(addGroup);
									Sleep(1);
								}
								break;
							}
							else//if(GroupID != 0x00) //not master group
							{
								GroupIter = find_if(mMasterGroup.mGroup.begin(), mMasterGroup.mGroup.end(), bind2nd( ptr_fun(Group::EqualId), GroupID ));
								//if group not exist create it or break
								if(GroupIter == mMasterGroup.mGroup.end())
								{
									cout<<"Group "<<GroupID<<" for add buddie not found!"<<endl;
									break;
								}
								GroupIter->mBuddie.reserve((inBuffer[j+2]*0x100 + inBuffer[j+3])/2);
								for(WORD k=0; k<GroupIter->mBuddie.capacity(); k++)
								{
									addBuddie.Id = inBuffer[j+TLV_HEADER_SIZE+k*2]*0x100 + inBuffer[j+TLV_HEADER_SIZE+k*2+1];
									//nulling Nick and UIN, and fill them by templates
									memset(addBuddie.Nickname, 0, MAX_NICKNAME_STR_SIZE);
									memcpy(addBuddie.Nickname, "NoNickname", 10);
									memset(addBuddie.UIN, 0, MAX_UIN_STR_SIZE);
									memcpy(addBuddie.UIN, "000", 3);
									addBuddie.Online = false;
									GroupIter->mBuddie.push_back(addBuddie);
								}
								break;
							}
							break;
						case 0x6a: //buddy was added automatically by the server
							break;
						case 0x76:
							break;
					}
					j += TLV_HEADER_SIZE + inBuffer[j+2]*0x100 + inBuffer[j+3];
				}
				break;
			case 0x03: //invisible list
				while(j < Pos+TLVChainLen)
				{
					TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
					switch(TLVType)
					{
						case 0x67: //unknown
							break;
						case 0x150: //number of IMs sent
							break;
						case 0x151: //number of IMs received
							break;
						case 0x152: //umber of times a user has the away message set
							break;
						case 0x153: //number of seconds a user is online
							break;
						case 0x160: //unknown
							break;
						case 0x164: //unknown
							break;
						case 0x16f: //unknown
							break;
						case 0x170: //unknown
							break;
						case 0x171: //unknown
							break;
						case 0x1c3: //unknown
							break;
						case 0x1c4: //unknown
							break;
						case 0x1c5: //unknown
							break;
						case 0x1c6: //unknown
							break;

					}
					j += TLV_HEADER_SIZE + inBuffer[j+2]*0x100 + inBuffer[j+3];
				}
				break;
			case 0x19: //deleted
				break;
			case 0x14: //buddie icon info
				TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
				switch(TLVType)
				{
					case 0xd5: //md5 checksum of icon?
						break;
				}
				break;
			case 0x02: //visible list
				TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
				switch(TLVType)
				{
					case 0x131: //nickname
						break;
				}
				break;
			case 0x05: //presence info
				TLVType = inBuffer[j]*0x100 + inBuffer[j+1];
				switch(TLVType)
				{
					case 0xc9: //can or not other see your idle time
						break;
				}
				break;
			case 0x1d: //unknown
				break;
			default:
				break;
		}
		Pos += TLVChainLen;
	}
}
void AnalyseSnac1503Packet(PBYTE inBuffer, PCHAR &UIN, PCHAR &Message, bool &NoMoreMessages)
{
	BYTE Prefix = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE;
	WORD TLVLen = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
	Prefix += TLV_HEADER_SIZE;
	WORD DataType = inBuffer[Prefix+6]*0x100 + inBuffer[Prefix+7];
	DWORD dwUIN = 0;
	char buffer[32] = {0};
	WORD MessageLen = 0;
	BYTE UINLen = 0;
	switch(SwapWord(DataType))
	{
		case 0x41: //offline message
			memcpy(&dwUIN, inBuffer+Prefix+10, sizeof(DWORD));
			dwUIN = SwapDWord(dwUIN);
			itoa(dwUIN, buffer, 10);
			UINLen = (BYTE)strlen(buffer);
			UIN = new char[UINLen+1];
			memset(UIN, 0, UINLen+1);
			memcpy(UIN, buffer, UINLen);
			MessageLen = SwapWord(inBuffer[Prefix+22]*0x100 + inBuffer[Prefix+23]);
			Message = new char[MessageLen];
			memset(Message, 0, MessageLen);
			memcpy(Message, inBuffer+Prefix+24, MessageLen);
			NoMoreMessages = false;
			break;
		case 0x42: //end
			NoMoreMessages = true;
			break;
	}
}
void AnalyseSnac030bPacket(PBYTE inBuffer, UserInfo &outUserInfo)
{
	BYTE Prefix = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE;
	BYTE UINLength = inBuffer[Prefix];
	memset(outUserInfo.UIN, 0, MAX_UIN_STR_SIZE);
	memcpy(outUserInfo.UIN, inBuffer+Prefix+1, UINLength);
	Prefix += 1 + UINLength;
	WORD CountTLV = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
	BYTE16 CLID;//BYTE16 *CLID
	WORD TLVType;
	WORD TLVLen;
	Prefix += 4;

	outUserInfo.Capability.clear(); //important!
	for(int i=0; i<CountTLV; i++)
	{
		TLVType = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
		TLVLen = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
		Prefix += TLV_HEADER_SIZE;
		switch(TLVType)
		{
			case 0x01: //user class
				outUserInfo.UserClass = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
				break;
			case 0x06: //status
				memcpy(&outUserInfo.Status, inBuffer+Prefix, sizeof(DWORD));
				outUserInfo.Status = SwapDWord(outUserInfo.Status);
				break;
			case 0x0c: //dc info
				memcpy(&outUserInfo.mDCInfo, inBuffer+Prefix, sizeof(DCInfo));
				outUserInfo.mDCInfo.Ip = SwapDWord(outUserInfo.mDCInfo.Ip);
				outUserInfo.mDCInfo.Port = SwapDWord(outUserInfo.mDCInfo.Port);
				outUserInfo.mDCInfo.Version = SwapWord(outUserInfo.mDCInfo.Version);
				outUserInfo.mDCInfo.Cookie = SwapDWord(outUserInfo.mDCInfo.Cookie);
				outUserInfo.mDCInfo.WebPort = SwapDWord(outUserInfo.mDCInfo.WebPort);
				outUserInfo.mDCInfo.ClientFeatures = SwapDWord(outUserInfo.mDCInfo.ClientFeatures);
				outUserInfo.mDCInfo.LastTime[0] = SwapDWord(outUserInfo.mDCInfo.LastTime[0]);
				outUserInfo.mDCInfo.LastTime[1] = SwapDWord(outUserInfo.mDCInfo.LastTime[1]);
				outUserInfo.mDCInfo.LastTime[2] = SwapDWord(outUserInfo.mDCInfo.LastTime[2]);
				break;
			case 0x0a: //external client ip address
				memcpy(&outUserInfo.ExternalIp, inBuffer+Prefix, sizeof(DWORD));
				outUserInfo.ExternalIp = SwapDWord(outUserInfo.ExternalIp);
				break;
			case 0x05: //time when this account was registered
				memcpy(&outUserInfo.RegTime, inBuffer+Prefix, sizeof(DWORD));
				outUserInfo.RegTime = SwapDWord(outUserInfo.RegTime);
				break;
			case 0x0f: //Client online time (in seconds)
				memcpy(&outUserInfo.OnlineTime, inBuffer+Prefix, sizeof(DWORD));
				outUserInfo.OnlineTime = SwapDWord(outUserInfo.OnlineTime);
				break;
			case 0x03: //Time when client gone online (unix time_t)
				memcpy(&outUserInfo.LastOnline, inBuffer+Prefix, sizeof(DWORD));
				outUserInfo.LastOnline = SwapDWord(outUserInfo.LastOnline);
				break;
			case 0x37: //unknown
				break;
			case 0x0d: //capabilities list - CLID's(16 bytes)
				if(TLVLen/sizeof(BYTE16) <= 0) break;
				outUserInfo.Capability.reserve(TLVLen/sizeof(BYTE16));
				for(unsigned int j=0; j<outUserInfo.Capability.capacity(); j++)
				{
					//CLID = new BYTE16[1];
					memcpy(CLID, inBuffer+Prefix+j*16, sizeof(BYTE16));
					outUserInfo.Capability.push_back((BYTE16*)CLID);
				}
				break;
			case 0x1d: //buddy icon info
				break;
		}
		Prefix += TLVLen;
		Sleep(1);
	}

	return;
}
void AnalyseSnac030cPacket(PBYTE inBuffer, int inSizeData, MSTR &UIN, WORD &Count)
{
	WORD Prefix = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE;
	BYTE UINLength = 0;
	WORD TLVCount = 0, TLVType = 0, TLVLen = 0;
	int i;
	Count = 0;
	UIN = NULL;
	if(Prefix >= inSizeData) return;

	//counting UINs that come offline
	while(Prefix < inSizeData)
	{
		UINLength = inBuffer[Prefix];
		Prefix += UINLength + 1;
		Prefix += 2; //+ evil
		TLVCount = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
		Prefix += 2; //+ TLVCount
		//tlv chain
		i = 0;
		while(i < TLVCount)
		{
			TLVLen = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];//Exception here! WHY???
			Prefix += TLV_HEADER_SIZE + TLVLen;
			i ++;
		}
		Count ++;
		Sleep(1);
	}

	//fill offline UINs
	UIN = new char* [Count];
	Prefix = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	Count = 0;
	while(Prefix < inSizeData)
	{
		UINLength = inBuffer[Prefix];
		UIN[Count] = new char [UINLength+1];
		memset(UIN[Count], 0, UINLength+1);
		memcpy(UIN[Count], inBuffer+Prefix+1, UINLength);
		Prefix += UINLength + 1;
		Prefix += 2; //+ evil
		TLVCount = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
		Prefix += 2; //+ TLVCount
		//tlv chain
		i = 0;
		while(i < TLVCount)
		{
			TLVLen = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
			Prefix += TLV_HEADER_SIZE + TLVLen;
			i ++;
		}
		Count ++;
		Sleep(1);
	}
}
void AnalyseSnac0407Packet(PBYTE inBuffer, int inSizeData, PCHAR &UIN, PCHAR &Message)
{
	BYTE Prefix = FLAP_HEADER_SIZE+SNAC_HEADER_SIZE;
	DWORD Cookie[2];
	WORD ChannelId;
	WORD WarningLevel;
	WORD CountTLV;
	WORD TLVType, TLVLen;
	WORD MsgType = 0, MsgLen;
	WORD Features;
	WORD CodePage, Language;
	BYTE *Msg;

	memcpy(&Cookie[0], inBuffer+Prefix, sizeof(DWORD));
	Cookie[0] = SwapDWord(Cookie[0]);
	memcpy(&Cookie[1], inBuffer+Prefix+4, sizeof(DWORD));
	Cookie[1] = SwapDWord(Cookie[1]);
	Prefix += 8;
	memcpy(&ChannelId, inBuffer+Prefix, sizeof(WORD));
	ChannelId = SwapWord(ChannelId);
	BYTE UINLength = inBuffer[Prefix+2];
	Prefix += 3;
	UIN = new char[UINLength+1];
	memset(UIN, 0, UINLength+1);
	memcpy(UIN, inBuffer+Prefix, UINLength);
	Prefix += UINLength;
	memcpy(&WarningLevel, inBuffer+Prefix, sizeof(WORD));
	WarningLevel = SwapWord(WarningLevel);
	memcpy(&CountTLV, inBuffer+Prefix+2, sizeof(WORD));
	CountTLV = SwapWord(CountTLV);
	Prefix += 4;

	for(int i=0; i<CountTLV; i++)
	{
		TLVType = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
		TLVLen = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
		Prefix += TLV_HEADER_SIZE;
		switch(TLVType)
		{
			case 0x01: //class
				break;
			case 0x06: //status
				break;
			case 0x05: //reg time
				break;
			case 0x1d: //icon info
				break;
			case 0x0f: //online time
				break;
			case 0x03: //connect time
				break;
			case 0x37: //unknown
				break;
		}
		Prefix += TLVLen;
	}
	for(int i=Prefix; i<inSizeData; i+=TLV_HEADER_SIZE+TLVLen)
	{
		TLVType = inBuffer[i]*0x100 + inBuffer[i+1];
		TLVLen = inBuffer[i+2]*0x100 + inBuffer[i+3];
		Prefix = i;
		switch(TLVType)
		{
			case 0x02: //IM_DATA
				Prefix += TLV_HEADER_SIZE;
				Features = inBuffer[Prefix+4]*0x100 + inBuffer[Prefix+5];
				Prefix += 2 + 2 + inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3] + 2;
				MsgLen = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
				CodePage = inBuffer[Prefix+2]*0x100 + inBuffer[Prefix+3];
				Language = inBuffer[Prefix+4]*0x100 + inBuffer[Prefix+5];
				Prefix += 2 + 4;
				switch(CodePage)
				{
					case 0x00: //ANSI
						Message = new char[MsgLen-4+1];
						memset(Message, 0, MsgLen-4+1);
						memcpy(Message, inBuffer+Prefix, MsgLen-4);
						break;
					case 0x01:
						break;
					case 0x02: //UNICODE
						Msg = new BYTE[MsgLen-4+1];
						memset(Msg, 0, MsgLen-4+1);
						memcpy(Msg, inBuffer+Prefix, MsgLen-4);
						Message = ByteWcharToChar(Msg);
						break;
					default:
						Message = NULL;
						break;
				}
				break;
			case 0x03: //REQUEST_HOST_ACK
				break;
			case 0x04: //AUTO_RESPONSE
				break;
			case 0x05: //DATA
				break;
			case 0x06: //STORE
				break;
			case 0x11: //WANT_EVENTS
				break;
			case 0x13: //BART
				break;
			case 0x16: //HOST_IM_ID
				break;
			case 0x17: //HOST_IM_ARGS
				break;
			case 0x22: //SEND_TIME
				break;
			case 0x23: //FRIENDLY_NAME
				break;
			case 0x24: //ANONYMOUS
				break;
			case 0x25: //WIDGET_NAME
				break;
		}
	}
}
void AnalyseSnac030aPacket(PBYTE inBuffer, int inSizeData, MSTR &BuddieId, WORD &Count)
{
	int DataLen = inSizeData - FLAP_HEADER_SIZE - SNAC_HEADER_SIZE;
	int CurrentPos = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	int StrLen = 0;
	Count = 0;
	while(CurrentPos < inSizeData)
	{
		StrLen = inBuffer[CurrentPos];
		CurrentPos += StrLen;
		Count ++;
		Sleep(1);
	}
	BuddieId = new char* [Count];
	Count = 0;
	CurrentPos = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	while(CurrentPos < inSizeData)
	{
		StrLen = inBuffer[CurrentPos];
		BuddieId[Count] = new char [StrLen+1];
		memset(BuddieId[Count], 0, StrLen+1);
		memcpy(BuddieId[Count], inBuffer+CurrentPos+1, StrLen);
		CurrentPos += StrLen + 1;
		Count ++;
		Sleep(1);
	}
}
void AnalyseSnac1319Packet(PBYTE inBuffer, PCHAR &UIN, PCHAR &Reason)
{
	int Prefix = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	BYTE UINLen;
	WORD ReasonLen;

	UINLen = inBuffer[Prefix];
	UIN = new char[UINLen+1];
	memset(UIN, 0, UINLen+1);
	Prefix += 1;
	memcpy(UIN, inBuffer + Prefix, UINLen);
	Prefix += UINLen;
	ReasonLen = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
	Reason = new char[ReasonLen+1];
	memset(Reason, 0, ReasonLen+1);
	ReasonLen += 2;
	memcpy(Reason, inBuffer + Prefix, ReasonLen);
}
void AnalyseSnac131bPacket(PBYTE inBuffer, PCHAR &UIN, bool &Accept, PCHAR &Message)
{
	int Prefix = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	BYTE UINLen;
	WORD MessageLen;

	UINLen = inBuffer[Prefix];
	UIN = new char[UINLen+1];
	memset(UIN, 0, UINLen+1);
	Prefix += 1;
	memcpy(UIN, inBuffer + Prefix, UINLen);
	Prefix += UINLen;
	Accept = (bool)inBuffer[Prefix];
	Prefix += 1;
	MessageLen = inBuffer[Prefix]*0x100 + inBuffer[Prefix+1];
	Message = new char[MessageLen+1];
	MessageLen += 2;
	memcpy(Message, inBuffer + Prefix, MessageLen);
}
void AnalyseSnac040bPacket(PBYTE inBuffer, PCHAR &UIN, PCHAR &Status)
{
	int PacketSize = GetPacketSize(inBuffer);
	int Prefix = FLAP_HEADER_SIZE + SNAC_HEADER_SIZE;
	DWORD Cookie[2];
	WORD ChannelId;
	BYTE UINLen = 0;
	WORD ReasonCode = 0;
	BYTE FragmentId;
	BYTE FragmentVer;
	WORD FragmentLen;

	memcpy(&Cookie[0], inBuffer+Prefix, sizeof(DWORD));
	Cookie[0] = SwapDWord(Cookie[0]);
	memcpy(&Cookie[1], inBuffer+Prefix+4, sizeof(DWORD));
	Cookie[1] = SwapDWord(Cookie[1]);
	Prefix += 8;
	memcpy(&ChannelId, inBuffer+Prefix, sizeof(WORD));
	ChannelId = SwapWord(ChannelId);
	BYTE UINLength = inBuffer[Prefix+2];
	Prefix += 3;
	UIN = new char[UINLen+1];
	memset(UIN, 0, UINLen+1);
	memcpy(UIN, inBuffer+Prefix, UINLen);
	Prefix += UINLen;
	memcpy(&ReasonCode, inBuffer+Prefix, sizeof(WORD));
	ChannelId = SwapWord(ReasonCode);
	Prefix += 2;

	int i = Prefix;
	switch(ChannelId)
	{
		case 0x01:
			while(i < PacketSize)
			{
				FragmentId = inBuffer[i];
				FragmentVer = inBuffer[i+1];
				FragmentLen = inBuffer[i+2]*0x100 + inBuffer[i+3];
				switch(FragmentId)
				{
					case 0x01: //text message
						break;
					case 0x05: //capabilities
						break;
				}
				i += FragmentLen;
			}
			break;
		case 0x02:
			break;
	}
}

void PrintErrorStrTLV8(WORD inErrorCode)
{
	switch(inErrorCode)
	{
		case 0x0001:
			cout<<"Invalid nick or password"<<endl;
			break;
		case 0x0002:
			cout<<"Service temporarily unavailable"<<endl;
			break;
		case 0x0003:
			cout<<"All other errors"<<endl;
			break;
		case 0x0004:
			cout<<"Incorrect nick or password, re-enter"<<endl;
			break;
		case 0x0005:
			cout<<"Mismatch nick or password, re-enter"<<endl;
			break;
		case 0x0006:
			cout<<"Internal client error (bad input to authorizer)"<<endl;
			break;
		case 0x0007:
			cout<<"Invalid account"<<endl;
			break;
		case 0x0008:
			cout<<"Deleted account"<<endl;
			break;
		case 0x0009:
			cout<<"Expired account"<<endl;
			break;
		case 0x000a:
			cout<<"No access to database"<<endl;
			break;
		case 0x000b:
			cout<<"No access to resolver"<<endl;
			break;
		case 0x000c:
			cout<<"Invalid database fields"<<endl;
			break;
		case 0x000d:
			cout<<"Bad database status"<<endl;
			break;
		case 0x000e:
			cout<<"Bad resolver status"<<endl;
			break;
		case 0x000f:
			cout<<"Internal error"<<endl;
			break;
		case 0x0010:
			cout<<"Service temporarily offline"<<endl;
			break;
		case 0x0011:
			cout<<"Suspended account"<<endl;
			break;
		case 0x0012:
			cout<<"DB send error"<<endl;
			break;
		case 0x0013:
			cout<<"DB link error"<<endl;
			break;
		case 0x0014:
			cout<<"Reservation map error"<<endl;
			break;
		case 0x0015:
			cout<<"Reservation link error"<<endl;
			break;
		case 0x0016:
			cout<<"The users num connected from this IP has reached the maximum"<<endl;
			break;
		case 0x0017:
			cout<<"The users num connected from this IP has reached the maximum (reservation)"<<endl;
			break;
		case 0x0018:
			cout<<"Rate limit exceeded (reservation). Please try to reconnect in a few minutes"<<endl;
			break;
		case 0x0019:
			cout<<"User too heavily warned"<<endl;
			break;
		case 0x001a:
			cout<<"Reservation timeout"<<endl;
			break;
		case 0x001b:
			cout<<"You are using an older version of ICQ. Upgrade required"<<endl;
			break;
		case 0x001c:
			cout<<"You are using an older version of ICQ. Upgrade recommended"<<endl;
			break;
		case 0x001d:
			cout<<"Rate limit exceeded. Please try to reconnect in a few minutes"<<endl;
			break;
		case 0x001e:
			cout<<"Can't register on the ICQ network. Reconnect in a few minutes"<<endl;
			break;
		case 0x0020:
			cout<<"Invalid SecurID"<<endl;
			break;
		case 0x0022:
			cout<<"Account suspended because of your age (age < 13)"<<endl;
			break;
	}
}
bool IsThisSNAC(BYTE *inBuffer, WORD inFamID, WORD inSubTypeID)
{
	return (inBuffer[FLAP_HEADER_SIZE]*0x100+inBuffer[FLAP_HEADER_SIZE+1]==inFamID &&
		inBuffer[FLAP_HEADER_SIZE+2]*0x100+inBuffer[FLAP_HEADER_SIZE+3]==inSubTypeID);
}
DWORD GetSNACFamily(BYTE *inBuffer)
{
	DWORD ReturnRes=0;
	memcpy(&ReturnRes, inBuffer+FLAP_HEADER_SIZE, sizeof(DWORD));
	ReturnRes = SwapDWord(ReturnRes);
	return ReturnRes;
}
DWORD GetSNACRequest(BYTE *inBuffer)
{
	DWORD ReturnRes=0;
	memcpy(&ReturnRes, inBuffer+FLAP_HEADER_SIZE+6, sizeof(DWORD));
	ReturnRes = SwapDWord(ReturnRes);
	return ReturnRes;
}
WORD GetSnacFlag01(BYTE *inBuffer)
{
	WORD ReturnRes=0;
	memcpy(&ReturnRes, inBuffer+FLAP_HEADER_SIZE+5, sizeof(WORD));
	ReturnRes = SwapWord(ReturnRes);
	return ReturnRes;
}
char *GetSnacFamFromBuffer(PBYTE Buffer)
{
	char *ResText = new char[12];
	memset(ResText, 0, 12);
	strcat(ResText, "SNAC(");
	char buf[10] = {0};
	int nSnac[2] = {Buffer[FLAP_HEADER_SIZE]*0x100+Buffer[FLAP_HEADER_SIZE+1], 
		Buffer[FLAP_HEADER_SIZE+2]*0x100+Buffer[FLAP_HEADER_SIZE+3]};
	itoa(nSnac[0], buf, 16);
	strcat(ResText, buf);
	memset(buf, 0, 10);
	strcat(ResText, ",");
	itoa(nSnac[1], buf, 16);
	strcat(ResText, buf);
	strcat(ResText, ")");
	return ResText;
}
void PrintMasterGroup(MasterGroup *pMasterGroup)
{
	char buf[10]={0};
	for(unsigned int i=0; i<pMasterGroup->mGroup.size(); i++)
	{
		itoa(pMasterGroup->mGroup[i].Id, buf, 10);
		cout<<buf<<endl;
		memset(buf, 0, 10);
		for(unsigned int j=0; j<pMasterGroup->mGroup[i].mBuddie.size(); j++)
			cout<<" "<<pMasterGroup->mGroup[i].mBuddie[j].UIN<<endl;
		Sleep(1);
	}
}
bool TestMasterGroup(MasterGroup *pMasterGroup)
{
	for(unsigned int i=0; i<pMasterGroup->mGroup.size(); i++)
	{
		for(unsigned int j=0; j<pMasterGroup->mGroup[i].mBuddie.size(); j++)
			if(pMasterGroup->mGroup[i].mBuddie[j].UIN[0] == '0') return 0;
		Sleep(1);
	}
	return 1;
}
bool WriteBufferToFile(char *FileName, PBYTE Buffer, WORD Size)
{
	if(!Buffer) return 0;
	if(!Size) return 0;
	fstream LogFile;
	LogFile.open(FileName, ios::in | ios::out | ios::app);
	LogFile<<"[Size: "<<Size<<"] Unknown data from server"<<endl;
	for(WORD i=0; i<Size; i++)
	{
		if(i && !(i%16)) LogFile<<endl;
		LogFile<<" ";
		LogFile.fill('0');
		LogFile.width(2);
		LogFile<<hex<<right<<uppercase<<(unsigned short int)Buffer[i];
	}
	LogFile<<endl<<endl;
	LogFile.close();

	return 1;
}
bool WritePacketToFile(char *FileName, FlapPacket *Packet, char *Hint)
{
	if(!Packet || !FileName)
	{
		return 0;
	}
	fstream LogFile;
	BYTE *pPacket = (BYTE*)Packet;
	WORD PacketSize = FLAP_HEADER_SIZE+SwapWord(Packet->DFLen);
	if(PacketSize > MAX_PACKET_DATA_SIZE)
	{
		return 0;
	}
	LogFile.open(FileName, ios::in | ios::out | ios::app);
	LogFile<<"[Size: "<<PacketSize<<"] ";
	char buffer[20] = {0};
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	strftime(buffer, 20, "%d.%m.%y %H:%M:%S", timeinfo);
	LogFile<<"["<<buffer<<"] ";
	char *SnacText = GetSnacFamFromBuffer((PBYTE)Packet);
	LogFile<<SnacText<<" ";
	delete [] SnacText;
	if(Hint) LogFile<<Hint<<endl;
	else LogFile<<endl;
	for(int i=0; i<PacketSize; i++)
	{
		if(i && !(i%16)) LogFile<<endl;
		LogFile<<" ";
		LogFile.fill('0');
		LogFile.width(2);
		LogFile<<hex<<right<<uppercase<<(unsigned short int)pPacket[i];
	}
	LogFile<<endl<<endl;
	LogFile.close();
	return true;
}
WORD GetPacketSize(PBYTE Buffer)
{
	return Buffer[4]*0x100 + Buffer[5];
}

wchar_t *utf8_to_unicode(char *utf8_string)
{
	int err;
	wchar_t *res;
	int res_len = MultiByteToWideChar(
		CP_UTF8,			// Code page
		0,					// No flags
		utf8_string,		// Multibyte characters string
		-1,					// The string is NULL terminated
		NULL,				// No buffer yet, allocate it later
		0					// No buffer
		);
	if (res_len == 0) 
	{
		printf("Failed to obtain utf8 string length\n");
		return NULL;
	}
	res = (wchar_t*)calloc(sizeof(wchar_t), res_len);
	if (res == NULL) 
	{
		printf("Failed to allocate unicode string\n");
		return NULL;
	}
	err = MultiByteToWideChar(
		CP_UTF8,			// Code page
		0,					// No flags
		utf8_string,		// Multibyte characters string
		-1,					// The string is NULL terminated
		res,				// Output buffer
		res_len				// buffer size
		);
	if (err == 0)
	{
		printf("Failed to convert to unicode\n");
		free(res);
		return NULL;
	}
	return res;
}

char *unicode_to_1251(wchar_t *unicode_string)
{
	int err;
	char *res;
	int res_len = WideCharToMultiByte(
		1251,				// Code page
		0,					// Default replacement of illegal chars
		unicode_string,		// Multibyte characters string
		-1,					// Number of unicode chars is not known
		NULL,				// No buffer yet, allocate it later
		0,					// No buffer
		NULL,				// Use system default
		NULL				// We are not interested whether the default char was used
		);
	if (res_len == 0) 
	{
		printf("Failed to obtain required cp1251 string length\n");
		return NULL;
	}
	res = (char*)calloc(sizeof(char), res_len);
	if (res == NULL) 
	{
		printf("Failed to allocate cp1251 string\n");
		return NULL;
	}
	err = WideCharToMultiByte(
		1251,				// Code page
		0,					// Default replacement of illegal chars
		unicode_string,		// Multibyte characters string
		-1,					// Number of unicode chars is not known
		res,				// Output buffer
		res_len,			// buffer size
		NULL,				// Use system default
		NULL				// We are not interested whether the default char was used
		);
	if (err == 0)
	{
		printf("Failed to convert from unicode\n");
		free(res);
		return NULL;
	}
	return res;
}
char *ByteWcharToChar(BYTE *bMass)
{
	int Length = (int)strlen((char*)bMass);
	char *Result = new char[Length];
	wchar_t *Mass = new wchar_t[Length/2+1];
	memcpy(Mass, bMass, Length);
	for(int i=0;i<Length/2+1;i++)
		Mass[i] = SwapWord(Mass[i]);
	memset(Mass+Length/2, 0, sizeof(wchar_t));
	char *un = unicode_to_1251(Mass);
	strset(Result,0);
	memcpy(Result, un, strlen(un));
	if(Mass) delete [] Mass;
	return Result;
}
tm *DWordToPtm(DWORD dwTime)
{
	time_t t = dwTime;
	tm *Result;
	Result = gmtime ( &t );
	return Result;
}