#ifndef WGSERVER_WORLDPACKET_H
#define WGSERVER_WORLDPACKET_H


#include "ByteBuffer.h"


class WorldPacket : public ByteBuffer
{
public:
    __inline WorldPacket() : ByteBuffer(), m_opcode(0) { }
    __inline WorldPacket(uint16 opcode, size_t res) : ByteBuffer(res), m_opcode(opcode) {}
    __inline WorldPacket(size_t res) : ByteBuffer(res), m_opcode(0) { }
    __inline WorldPacket(const WorldPacket &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode) {}

    //! Clear packet and set opcode all in one mighty blow
    __inline void Initialize(uint16 opcode )
    {
        clear();
        m_opcode = opcode;
    }

    __inline uint16 GetOpcode() const { return m_opcode; }
    __inline void SetOpcode(const uint16 & opcode) { m_opcode = opcode; }
    __inline void SetLength(const uint16 & len)	{ 
		uint16 * plen = (uint16 * ) &(contents()[2]);
		*plen = len;
	}

	__inline std::string getString()
	{
		//std::string buff = 
		return (const char*)contents();
	}
	
 template<typename T>
   	void SetOffset(const uint16 & offset, const T value ) {
		T * pval = (T *) &(contents()[offset]);
		*pval = value;
	}

public:
	/**
	 * @����ʱ�� 2011-08-31
	 * @������ ��־��
	 * @�������� �����ݰ�׷���ַ���
	 * @����
	 * 	@packet ���ݷ��ָ��
	 *  @str ׷�ӵ��ַ���
	 */
	template<typename T> void AppendPacketString(std::string str)
	{
		T ilen = (T)str.size();
		*this << ilen;
		if (ilen > 0)
			Write((const uint8 *) str.c_str(), ilen);
	}
	/**
	 * @����ʱ�� 2011-08-31
	 * @������ ��־��
	 * @�������� ��ȡ�ַ���
	 * @����
	 * 	@packet ���ݷ��
	 * @���� �Ƿ�ɹ�
	 */
	template<typename T> bool GetPacketString(std::string &str)
	{
		T ilen;
		*this >> ilen;
		if (ilen == 0)
			return true;
		uint8 *buf = new uint8[ilen+1];
		uint16 plen = size();
		if (plen < ilen)
			return false;
		read(buf, ilen);
		buf[ilen] = 0;
		str = (char*)buf;
		return true;
	}


protected:
    uint16 m_opcode;
};

/*
template<uint32 Size>
class SERVER_DECL StackWorldPacket : public StackBuffer<Size>
{
	uint16 m_opcode;
public:
	__inline StackWorldPacket(uint16 opcode) : StackBuffer<Size>(), m_opcode(opcode) { }

	//! Clear packet and set opcode all in one mighty blow
	__inline void Initialize(uint16 opcode )
	{
		StackBuffer<Size>::Clear();
		m_opcode = opcode;
	}

	uint16 GetOpcode() { return m_opcode; }
	__inline void SetOpcode(uint16 opcode) { m_opcode = opcode; }
};
*/

#endif
