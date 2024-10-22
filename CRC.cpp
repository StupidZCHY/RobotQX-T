#include "CRC.h"



/*
crc 计算都遵循MSB First 的规则
CRC8:31
CRC-16:1021
*/
const uint8_t crc8_tab[256]=
{
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

const static uint16_t crc16_tab[256]= {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
                                0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
                                0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
                                0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
                                0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
                                0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
                                0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
                                0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
                                0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
                                0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
                                0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
                                0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
                                0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
                                0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
                                0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
                                0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
                                0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
                                0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
                                0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
                                0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
                                0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
                                0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
                                0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
                                0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
                                0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
                                0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
                                0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
                                0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
                                0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
                                0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
                                0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
                                0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
                               };


const uint32_t crc32_tab[256]= {0x0,0x4c11db7,0x9823b6e,0xd4326d9,0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
                                0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
                                0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
                                0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,
                                0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
                                0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,
                                0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
                                0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,
                                0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
                                0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,
                                0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
                                0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,
                                0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
                                0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,
                                0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
                                0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,
                                0x18aeb13,0x54bf6a4,0x808d07d,0xcc9cdca,
                                0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,
                                0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
                                0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,
                                0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
                                0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,
                                0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
                                0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,
                                0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
                                0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,
                                0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
                                0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,
                                0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
                                0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,
                                0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
                                0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,
                                0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
                                0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,
                                0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
                                0x315d626,0x7d4cb91,0xa97ed48,0xe56f0ff,
                                0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
                                0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,
                                0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
                                0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,
                                0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
                                0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,
                                0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
                                0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,
                                0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
                                0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,
                                0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
                                0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,
                                0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
                                0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,
                                0x29f3d35,0x65e2082,0xb1d065b,0xfdc1bec,
                                0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
                                0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
                                0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
                                0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
                                0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
                                0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
                                0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
                                0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
                                0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,
                                0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
                               };

/***********************************************************
CRC8 Coding & Decoding G(X) = x^8+x^5+x^4+x^0 (31)
***********************************************************/
uint8_t CRC8_cal(uint8_t *ptr, uint32_t len, uint16_t crc_init)
{
    uint8_t  crc;
    crc = crc_init;


    while (len--!=0)
    {
        crc=crc8_tab[*ptr^crc];
        ptr++;
    }
    return(crc);
}


      static uint8_t auchCRCHi[] = {
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
       0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
       0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
       0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
       0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
       0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
       0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
       0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
       0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
       0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
       0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
       0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
       };
       /// <summary>
       /// crc校验数组1
       /// </summary>
       static uint8_t auchCRCLo[] = {
       0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
       0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
       0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
       0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
       0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
       0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
       0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
       0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
       0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
       0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
       0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
       0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
       0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
       0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
       0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
       0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
       0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
       0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
       0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
       0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
       0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
       0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
       0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
       0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
       0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
       0x43, 0x83, 0x41, 0x81, 0x80, 0x40
       };



/***********************************************************
CRC16 Coding & Decoding G(X) = X^16+X^12+X^5+1
***********************************************************/
uint16_t CRC16_cal(QByteArray ptr, uint32_t start, uint16_t nLength)
{

    uint8_t uchCRCHi = 0xFF;         /* 高CRC字节初始化  */
    uint8_t uchCRCLo = 0xFF;          /* 低CRC 字节初始化 */
    uint16_t uIndex;                          /* CRC循环中的索引  */
    uint16_t r_crc;
    for (uint32_t i = start; i < (start + nLength); i++)
    {
        uIndex = (uint16_t)(uchCRCHi ^ ptr[i]); /* 计算CRC */
        uchCRCHi = (uint8_t)(uchCRCLo ^ auchCRCHi[uIndex]);
            uchCRCLo = auchCRCLo[uIndex];
    }
    r_crc = (uint16_t)(uchCRCHi * 256);
    r_crc += uchCRCLo;
    return r_crc;
}

/***********************************************************
CRC16 Coding & Decoding G(X) = X^16+X^12+X^5+1
***********************************************************/
uint16_t IapCRC16_cal(QByteArray ptr, uint32_t start, uint16_t nLength)
{


//    const ushort polynomial = 0x1021; // CCITT polynomial
//    quint16 crc = 0x0000;

//    for (int i = start; i < nLength; i++) {
//        crc ^= (ptr.at(i) << 8);
//        for (int j = 0; j < 8; j++) {
//            if (crc & 0x8000)
//                crc = (crc << 1) ^ polynomial;
//            else
//                crc = crc << 1;
//        }
//    }

    uint16_t crc = 0;
    uint8_t crctemp;
    int i = start;
    while (nLength > 0)
    {
        nLength -= 1;
        crctemp = crc >> 8;
        crc = (crc << 8) & 0xFFFF;
        uint8_t index = 0;
        index = crctemp ^ ptr[i];
        crc = (crc16_tab[index] ^ crc) & 0xFFFF;
        i += 1;
    }
    return crc;
}


/***********************************************************
CRC32 Coding & Decoding G(X) = X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X+1
***********************************************************/
uint32_t CRC32_cal(uint8_t *ptr, uint32_t len, uint16_t crc_init)
{
    uint32_t crc,
             oldcrc32;
    uint8_t  temp;
    crc = crc_init;
    while (len--!=0)
    {
        temp=(crc>>24)&0xff;
        oldcrc32=crc32_tab[*ptr^temp];
        crc=(crc<<8)^oldcrc32;
        ptr++;


    }
    return(crc);
}




/*
 * Notes  这个不需要crc16tab
 */
uint16_t cal_crc16_none_table(unsigned char *ptr, int32_t count, uint16_t crc_init)
{
    int crc;
    char i;


    crc = crc_init;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        }
        while(--i);
    }
    return (crc&0xFFFF);
}





