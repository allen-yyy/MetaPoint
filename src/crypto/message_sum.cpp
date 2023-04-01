// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sha-256.h"

#include <iostream>

/// @brief message check sum
/// @param len data length
/// @param data data
/// @return sum
char *
message_sum(int len,char *data)
{
    char m[32]={(char)0x23,(char)0x21,(char)0x09,(char)0x72,(char)0x7b,(char)0x3c,(char)0x66,(char)0x2e,(char)0x37,(char)0x5d,(char)0x1a,(char)0x32,(char)0x48,
                (char)0x6f,(char)0xa1,(char)0xc9,(char)0x25,(char)0x47,(char)0xf7,(char)0x01,(char)0x49,(char)0x2d,(char)0x7a,(char)0x39,(char)0xd4,(char)0xad,
                (char)0x66,(char)0x07,(char)0x5d,(char)0xef,(char)0xae,(char)0x21};
    int point=0;
    for(int i=0;i<len;i++)
    {
        if(point==32) point=0;
        m[point]+=data[i];
        m[point]+=data[i]&i;
        point++;
    }
    char *sum = sha256(m,32);
    return sum;
}