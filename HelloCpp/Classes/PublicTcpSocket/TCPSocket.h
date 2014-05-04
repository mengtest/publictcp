#ifndef __CC_TCPSOCKET_H__
#define __CC_TCPSOCKET_H__

#include <functional>
#include "WorldPacket.h"

#ifdef WIN32
	#include <windows.h>
	#include <WinSock.h>
	#include <process.h>
	#pragma comment( lib, "ws2_32.lib" )
	#pragma comment( lib, "libzlib.lib" )
#else
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <pthread.h>

	#define SOCKET int
	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1
#endif


#ifndef CHECKF
#define CHECKF(x) \
	do \
{ \
	if (!(x)) { \
	CCLOG("CHECKF:%s", #x, __FILE__, __LINE__); \
	return 0; \
	} \
} while (0)
#endif

#define _MAX_MSGSIZE 16 * 1024		// �ݶ�һ����Ϣ���Ϊ16k
#define BLOCKSECONDS	30			// INIT��������ʱ��
#define INBUFSIZE	(64*1024)		//?	����ߴ�������汨�����  �������ݵĻ���
#define OUTBUFSIZE	(8*1024)		//? ����ߴ�������汨������� �������ݵĻ��棬��������8Kʱ��FLUSHֻ��ҪSENDһ��

class  TCPSocket
{
public:
	TCPSocket(void);
	bool	Create(const char* pszServerIP, int nServerPort, int tagid, int nBlockSec = BLOCKSECONDS, bool bKeepAlive = false);
	bool	SendMsg(void* pBuf, int nSize);
	bool	ReceiveMsg(void* pBuf, int& nSize);
	bool	Flush(void);
	bool	Check(void);
	void	Destroy(void);
	SOCKET	GetSocket(void) const { return m_sockClient; }
	
	int		getTagID(){ return m_tag; }
private:
	bool	recvFromSock(void);		// �������ж�ȡ�����ܶ������
	bool    hasError();			// �Ƿ�������ע�⣬�첽ģʽδ��ɷǴ���
	void    closeSocket();

	SOCKET	m_sockClient;

	// �������ݻ���
	char	m_bufOutput[OUTBUFSIZE];	//? ���Ż�Ϊָ������
	int		m_nOutbufLen;

	// ���λ�����
	char	m_bufInput[INBUFSIZE];
	int		m_nInbufLen;
	int		m_nInbufStart;				// INBUFʹ��ѭ��ʽ���У��ñ���Ϊ������㣬0 - (SIZE-1)
	int		m_tag;
};

typedef std::function<bool(int,int,WorldPacket&)> ProAllFunc;	// ��������Э�飬���д���@socket��ʶ,@Э��ͷ��@���ݰ�,�����Ƿ�ַ�
typedef std::function<void(int,WorldPacket&)> ProFunc;	// ���յ���Э�飬@socket��ʶ,@���ݰ�
typedef std::function<void(int,bool)> sckConnectFunc;	// ���ӳɹ�/ʧ��
typedef std::function<void(int)> sckDisconnectFunc;	// ���ӶϿ�


 // ˵����func������, _Objectָ��, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3��ʾ�ص���������������
#define SCT_CALLBACK_1(func, _Object) std::bind(&func,_Object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define SCT_CALLBACK_2(func, _Object) std::bind(&func,_Object, std::placeholders::_1, std::placeholders::_2)
#define SCT_CALLBACK_3(func, _Object) std::bind(&func,_Object, std::placeholders::_1)
// ����SOCKET������
#define CREATE_TCPSOCKETMGR()	new TCPSocketManager()
// PING������
#define CMSG_PING		0x01
// PING�����ͼ��
#define PINGINTERVAL_TIMER	10

struct SocketAddress
{
	std::string pserverip;
	int nPort;
	int nTag;
	SocketAddress()
	{
		pserverip = "";
		nPort = 0;
		nTag = 0;
	}
};

class TCPSocketManager
{
public:
	TCPSocketManager();
	~TCPSocketManager();
	// ����socket����ӵ�������
	void createSocket(const char* pszServerIP,	// IP��ַ
							int nServerPort,			// �˿�
							int _tag,					// ��ʶID
							int nBlockSec = BLOCKSECONDS, // ����ʱ��ms
							bool bKeepAlive = false);
	// ���ӷ�����
	bool	connect(SocketAddress *pAddress);
	// ע��Э���
	void	register_process(const uint16 &entry, ProFunc callback);
	// ע���������Э��
	void	register_all_process(ProAllFunc callback){ _pProcess = callback; }
	// ע��socket���ӳɹ��¼�
	void	register_connect(sckConnectFunc callback){ _pOnConnect = callback; }
	// ע��socket�����¼�
	void	register_disconnect(sckDisconnectFunc callback){ _OnDisconnect = callback; }
	// �������socket��������
	bool	addSocket(TCPSocket *pSocket);
	// ɾ��socket
	bool	removeSocket(int _tag);
	// �Ͽ�socket
	void	disconnect(int _tag);
	// ��ȡsocket
	TCPSocket	*GetSocket(int _tag);
	// ������Ϣ
	bool	SendPacket(int _tag, WorldPacket *packet);

	void	update();

	static TCPSocketManager &getSingleton(){ assert(mSingleton); return *mSingleton;}

#ifdef WIN32
	static void socket_thread(void *mgr);
	CRITICAL_SECTION mCs;	
#else
	static void *socket_thread(void *mgr);
	mutable pthread_mutex_t m_mutex;  
#endif
protected:
	// ����PING��
	void	SendPingPacket(WorldPacket &packet);
	
private:	
	ProAllFunc _pProcess;
	sckConnectFunc _pOnConnect;
	sckDisconnectFunc _OnDisconnect;
	std::list<TCPSocket*> m_lstSocket;
	std::map<uint16, ProFunc> _mapProcess;
	static TCPSocketManager * mSingleton;
	time_t m_sendping_last_time;
	std::list<SocketAddress> m_lstAddress;
};

#define sSocketMgr	TCPSocketManager::getSingleton()


#endif //__CC_TCPSOCKET_H__
