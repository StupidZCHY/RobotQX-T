
#ifndef __CRC_H__
#define __CRC_H__
#include <stdint.h>

#include <QByteArray>

uint16_t IapCRC16_cal(QByteArray ptr, uint32_t start, uint16_t nLength);
uint8_t     CRC8_cal    (uint8_t *ptr, uint32_t len, uint16_t crc_init);
uint16_t CRC16_cal(QByteArray ptr, uint32_t start, uint16_t nLength);
uint32_t    CRC32_cal   (uint8_t *ptr, uint32_t len, uint16_t crc_init);
uint16_t cal_crc16_none_table(unsigned char *ptr, int32_t count, uint16_t crc_init);


#endif //__CRC_H__
