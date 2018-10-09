#include "ProtocolData.h"
#include <iostream>
int main(int argc, char const *argv[])
{
    TSM_RES_LoginSDK *sm = new TSM_RES_LoginSDK;
    sm->Openid = "1233";
    sm->GSAddr = "127.0.0.1:8080";
    sm->SessionID = "sfasfasfda";
    sm->GSState = 1;
	TCM_REQ_LoginSDK aa, bb;
	aa.Openid = "AA";
	bb.Openid = "BB";
	sm->iarr.push_back(aa);
	sm->iarr.push_back(bb);
    std::string msg = sm->ToString();
    std::cout << msg.c_str() << std::endl;

    MsgObj *obj = MsgObj::CreateObject(msg);
    std::cout << ((TSM_RES_LoginSDK *)obj)->SessionID.c_str();
    delete obj;

    return 0;
}
