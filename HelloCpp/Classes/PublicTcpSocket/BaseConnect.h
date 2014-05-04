#ifndef __BASECONNECT__
#define __BASECONNECT__
#include "cocos2d.h"
#include "../PublicTcpSocket/TCPSocket.h"
USING_NS_CC;
class BaseConnect
{
public:
	BaseConnect(CCString* ipaddres,int port,int iptype);
	~BaseConnect(void);

	// ע�ᵥ��Э��ص�����������)��������SOCKET_ID��ʶ�����ݰ�
    void process_login(int _tag, WorldPacket & packet);
    // ��Ϣ������
    void process_openbackpack(int _tag, WorldPacket & packet);
    // ע�ᵥ��Э��ص�����������)��������SOCKET_ID��ʶ��Э��ͷ�����ݰ�
    bool process_all(int _tag, int _cmd, WorldPacket & packet);
    // �����¼�
    void OnConnect(int _tag, bool isConnect);
    // �����¼�
    void onDisconnect(int _tag);
};

#endif __BASECONNECT__