/*
 * Created Date: Saturday, July 19th 2025, 10:15:09 am
 * Author: Hoang Kiet
 */
#include "7_SEG.h"

// Segment code: {1, o, S, g, n, o, L}
unsigned char code Code7Seg[] = {0x00, 0x06, 0x5C, 0x6D, 0x6F, 0x23, 0x63, 0x38};
unsigned int sseg;

void show_7seg(unsigned int b)
{
    switch (b)
    {
        case 0:
            LSA = 0;
            LSB = 0;
            LSC = 0;
            break;
        case 1:
            LSA = 1;
            LSB = 0;
            LSC = 0;
            break;
        case 2:
            LSA = 0;
            LSB = 1;
            LSC = 0;
            break;
        case 3:
            LSA = 1;
            LSB = 1;
            LSC = 0;
            break;
        case 4:
            LSA = 0;
            LSB = 0;
            LSC = 1;
            break;
        case 5:
            LSA = 1;
            LSB = 0;
            LSC = 1;
            break;
        case 6:
            LSA = 0;
            LSB = 1;
            LSC = 1;
            break;
        default:
            LSA = 1;
            LSB = 1;
            LSC = 1;
            break;
    }
}

int decode_7seg(unsigned int a)
{
    sseg = Code7Seg[a];
    return sseg;
}
