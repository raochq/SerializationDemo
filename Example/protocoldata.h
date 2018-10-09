#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H
#include "msgobj.h"

class TCM_REQ_LoginSDK : public MsgObj
{
public:
	std::string Openid;
	std::string Version;
	RTTIWRITE(MsgObj,
		JsonWrite(Openid)
		JsonWrite(Version));
	RTTIPARSE(MsgObj,
		JsonParse(Openid)
		JsonParse(Version));
};

class TSM_RES_LoginSDK : public MsgObj
{
public:
	int GSState;
	std::string Openid;
	std::string SessionID;
	std::string GSVersion;
	std::string GSAddr;
	std::string GSWSS;
	std::list<TCM_REQ_LoginSDK> iarr;
	RTTIWRITE(MsgObj,
		JsonWrite(GSState)
		JsonWrite(Openid)
		JsonWrite(SessionID)
		JsonWrite(GSVersion)
		JsonWrite(GSAddr)
		JsonWrite(GSWSS)
		JsonWrite(iarr));

	RTTIPARSE(MsgObj,
		JsonParse(GSState)
		JsonParse(Openid)
		JsonParse(SessionID)
		JsonParse(GSVersion)
		JsonParse(GSAddr)
		JsonParse(GSWSS)
		JsonParse(iarr));		
};

REGISTER_CLASS(4000000, TCM_REQ_LoginSDK);
REGISTER_CLASS(3000002, TSM_RES_LoginSDK);

#endif
