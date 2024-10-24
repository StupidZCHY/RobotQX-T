#ifndef _SEER_PROTOCOL_V1_H_
#define _SEER_PROTOCOL_V1_H_

#include <stdint.h>
#include <QtEndian>

class SeerHeader{
public:
    SeerHeader(){
        m_header = 0x5A;
        m_version = 0x01;
        m_number = 0;
        m_length = 0;
        m_type = 0;
        memset(m_reserved,0, 6);
    }

    uint8_t m_header;
    uint8_t m_version;
    uint16_t m_number;
    uint32_t m_length;
    uint16_t m_type;
    uint8_t m_reserved[6];
};





class SeerData{
public:
    SeerData(){}
    ~SeerData(){}
    int size(){
        return sizeof(SeerHeader) + m_header.m_length;
    }


    int setData(uint16_t type,
                uint8_t* data = NULL,
                int size = 0,
                uint16_t jsonSize = 0,
                uint16_t number = 0x00,
                uint8_t byte15 = 0){

        m_header.m_header = 0x5A;
        m_header.m_version = 0x01;

        qToBigEndian(type,(uint8_t*)&(m_header.m_type));
        qToBigEndian(number,(uint8_t*)&(m_header.m_number));
        memset(m_header.m_reserved,0,6);

        //把转换后大端uint16_t拆分2个uint8_t.
        uint8_t u0 = (uint8_t)(m_header.m_type & 0xFFu);//取低八位.
        uint8_t u1 = (uint8_t)((m_header.m_type >> 8u) & 0xFFu);
        //保留区把发送的指令，放入0,1位).
        m_header.m_reserved[0] = u0;
        m_header.m_reserved[1] = u1;

        //把json区域长度转换成大端uint6_t,再拆分成两个uint8_t放入保留区2、3位.
        if(jsonSize > 0){
            uint16_t bigJsonSize = 0;
            qToBigEndian(jsonSize,(uint8_t*)&(bigJsonSize));
            uint8_t u2 = (uint8_t)(bigJsonSize & 0xFFu);
            uint8_t u3 = (uint8_t)((bigJsonSize >> 8u) & 0xFFu);
            m_header.m_reserved[2] = u2;
            m_header.m_reserved[3] = u3;
        }
        m_header.m_reserved[4] = byte15;
        if (data != NULL){
            memcpy(m_data, data, size);
        }
        qToBigEndian(size,(uint8_t*)&(m_header.m_length));
        return 16 + size;
    }
private:
    SeerHeader m_header;
    uint8_t m_data[1];
};


class KxHeader{
public:
    KxHeader(){
        m_header = 0x6b;
        m_version = 0x78;
        m_type = 0;
        m_number=0;
    }

    uint8_t m_header;
    uint8_t m_version;
    uint16_t m_number;
    uint16_t m_type;
};

class KxData{
public:
    KxData(){}
    ~KxData(){}
    int size(){
        return sizeof(KxHeader) + m_header.m_number;
    }
    int setData(
            uint16_t type,
            uint8_t* data = NULL,
            int size = 0,
            uint16_t number = 0x00
            )
    {

        m_header.m_header = 0x6b;
        m_header.m_version = 0x78;

        qToBigEndian(number,(uint16_t*)&(m_header.m_number));
        qToBigEndian(type,(uint16_t*)&(m_header.m_type));
        if (data != NULL){
            memcpy(m_data, data, size);
        }
        return number;
    }
private:
    KxHeader m_header;
    uint8_t m_data[];
};


#endif
