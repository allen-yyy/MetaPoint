// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <winsock.h>
#include <string>
#include <iostream>
#include "node.h"
#include "message.h"
#include "misc.h"
#include "app.h"

extern struct Node nSelfnode;

long long can_load_type[1024];

/// @brief send DATA message
/// @param to_node the node send to
/// @param data data
/// @param dwDlen data length
/// @param mData Message_data, options
void
senddatamessage(struct Node *to_node, char *data, int dwDlen, struct Message_data mData)
{
    char *pPack;
    WSADATA wsaData;                                    
    SOCKET sockSender;                                
    SOCKADDR_IN saSocket, saAddrLocal;                              
    BOOL fBroadcast = TRUE;                          
    char *cSendBuff = (char *)malloc(sizeof(struct Message_base) + sizeof(struct Message_data) + strlen(data));                                 
    struct Message_base mBase;

    mBase.ver = 0x1;
    mBase.dc = DC_DATA;
    mBase.zeros = 0;
    mBase.ih = IH_NOTHELLO;
    mBase.message_len = sizeof(struct Message_base) + sizeof(struct Message_data) + dwDlen;
    memcpy(mBase.from_addr, nSelfnode.node_addr, sizeof(nSelfnode.node_addr));
    memcpy(mBase.to_addr, to_node->node_addr, sizeof(nSelfnode.node_addr));
    memset(mBase.cksum, 0, sizeof(mBase.cksum));
    memcpy(cSendBuff, &mBase, sizeof(struct Message_base));
    memcpy(cSendBuff + sizeof(struct Message_base), &mData, sizeof(mData));
    memcpy(cSendBuff + sizeof(struct Message_base) + sizeof(mData), data, dwDlen);

    memcpy(mBase.cksum, message_sum(sizeof(struct Message_base) + sizeof(struct Message_data) + strlen(data), cSendBuff), sizeof(mBase.cksum));
    memcpy(cSendBuff, &mBase, sizeof(struct Message_base));

    if(WSAStartup(MAKEWORD(1, 1), &wsaData)!=0)   
    {
        return;
    }

    sockSender = socket(PF_INET, SOCK_DGRAM, 0);

    setsockopt(sockSender, SOL_SOCKET, SO_BROADCAST, (CHAR *)&fBroadcast, sizeof(BOOL));

    saAddrLocal.sin_family = AF_INET;
    saAddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    saAddrLocal.sin_port = nSelfnode.node_port;
    bind(sockSender, (sockaddr*)&saAddrLocal, sizeof(saAddrLocal));

    saSocket.sin_family = AF_INET;
    saSocket.sin_addr.s_addr = to_node->node_ipaddr;
    saSocket.sin_port = to_node->node_port;         
 
    sendto(sockSender, cSendBuff, strlen(cSendBuff), 0, (SOCKADDR *)&saSocket, sizeof(SOCKADDR_IN));
 
    closesocket(sockSender);     
    WSACleanup();
    return;
}

/// @brief Disconnect a node
/// @param nDisconnect the node
BOOL
disconnect(struct Node *nDisconnect)
{
    struct Message_base mBase;
    struct Message_control mcDisc;
    char cBuf[sizeof(struct Message_base) + sizeof(struct Message_control)];
    SOCKET sockSender;
    WSADATA wsaData;
    SOCKADDR_IN saSocket, saAddrLocal;
    BOOL fBroadcast = TRUE;

    if(nDisconnect->connected == 0)
    {
        return FALSE;
    }

    mcDisc.all_options = 0;
    mcDisc.ACK = 0;
    mcDisc.FIN = 1;
    mcDisc.MACK = 0;
    mcDisc.zero = 0;
    mcDisc.NACK = 1;
    mcDisc.NODE = 0;
    mcDisc.RST = 0;

    mBase.ver = 0x1;
    mBase.dc = DC_CONTROL;
    mBase.zeros = 0;
    mBase.ih = IH_NOTHELLO;
    memcpy(mBase.from_addr, nSelfnode.node_addr, META_NODEADDRLEN);
    if(nDisconnect->node_addr) 
    {
        memcpy(mBase.to_addr, nDisconnect->node_addr, META_NODEADDRLEN);
    }else{
        memset(mBase.to_addr, 0, META_NODEADDRLEN);
    }
    mBase.message_len = sizeof(struct Message_base) + sizeof(struct Message_control);

    memcpy(cBuf, &mBase, sizeof(struct Message_base));
    memcpy(cBuf + sizeof(struct Message_base), &mcDisc, sizeof(struct Message_control));
    memcpy(mBase.cksum, message_sum(sizeof(struct Message_base) + sizeof(struct Message_control), cBuf), sizeof(mBase.cksum));
    memcpy(cBuf, &mBase, sizeof(struct Message_base));

    if(WSAStartup(MAKEWORD(1, 1), &wsaData)!=0)   
    {
        return FALSE;
    }

    sockSender = socket(PF_INET, SOCK_DGRAM, 0);

    setsockopt(sockSender, SOL_SOCKET, SO_BROADCAST, (CHAR *)&fBroadcast, sizeof(BOOL));
    
    saAddrLocal.sin_family = AF_INET;
    saAddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    saAddrLocal.sin_port = nSelfnode.node_port;
    bind(sockSender, (sockaddr*)&saAddrLocal, sizeof(saAddrLocal));

    saSocket.sin_family = AF_INET;
    saSocket.sin_addr.s_addr = nDisconnect->node_ipaddr;
    saSocket.sin_port = nDisconnect->node_port;     
 
    sendto(sockSender, cBuf, strlen(cBuf), 0, (SOCKADDR *)&saSocket, sizeof(SOCKADDR_IN));

    deleteNode(nDisconnect);
 
    closesocket(sockSender);     
    WSACleanup();
    return TRUE;
}

/// @brief Connect a node
/// @param nConnect the node want to connect
BOOL
connect(struct Node *nConnect)
{
    struct Message_base mBase;
    struct Message_control mcDisc;
    char cBuf[sizeof(struct Message_base) + sizeof(struct Message_control)];
    SOCKET sockSender;
    WSADATA wsaData;
    SOCKADDR_IN saSocket, saAddrLocal;
    BOOL fBroadcast = TRUE;
    int dwSocklen = sizeof(struct sockaddr_in);
    fd_set fdRecv;
    struct timeval timeOut;

    if(nConnect->connected == 1)
    {
        return FALSE;
    }
    memset(&mcDisc, 0, sizeof(mcDisc));
    memset(&mBase, 0, sizeof(mBase));
    memset(cBuf, 0, sizeof(struct Message_base) + sizeof(struct Message_control));
    mcDisc.all_options = 0;
    mcDisc.ACK = 0;
    mcDisc.FIN = 1;
    mcDisc.MACK = 0;
    mcDisc.zero = 0;
    mcDisc.NACK = 0;
    mcDisc.NODE = 0;
    mcDisc.RST = 0;
    mcDisc.APP = 1;

    mBase.ver = 0x1;
    mBase.dc = DC_CONTROL;
    mBase.zeros = 0;
    mBase.ih = IH_HELLO;
    mBase.message_len = sizeof(struct Message_base) + sizeof(struct Message_control);

    memcpy(mBase.from_addr, nSelfnode.node_addr, META_NODEADDRLEN);
    memcpy(mBase.to_addr, nConnect->node_addr, META_NODEADDRLEN);

    memcpy(cBuf, &mBase, sizeof(struct Message_base));
    memcpy(cBuf + sizeof(struct Message_base), &mcDisc, sizeof(struct Message_control));
    memcpy(mBase.cksum, message_sum(sizeof(struct Message_base) + sizeof(struct Message_control), cBuf), sizeof(mBase.cksum));
    memcpy(cBuf, &mBase, sizeof(struct Message_base));

    if(WSAStartup(MAKEWORD(1, 1), &wsaData)!=0)   
    {
        return FALSE;
    }
    sockSender = socket(PF_INET, SOCK_DGRAM, 0);

    setsockopt(sockSender, SOL_SOCKET, SO_BROADCAST, (CHAR *)&fBroadcast, sizeof(BOOL));

    saAddrLocal.sin_family = AF_INET;
    saAddrLocal.sin_addr.s_addr = nSelfnode.node_ipaddr;
    saAddrLocal.sin_port = nSelfnode.node_port;
    bind(sockSender, (sockaddr*)&saAddrLocal, sizeof(saAddrLocal));

    saSocket.sin_family = AF_INET;
    saSocket.sin_addr.s_addr = nConnect->node_ipaddr;
    saSocket.sin_port = nConnect->node_port;
 
    sendto(sockSender, cBuf, sizeof(struct Message_base) + sizeof(struct Message_control), 0, (SOCKADDR *)&saSocket, sizeof(SOCKADDR_IN));
    addNode(nConnect);

    closesocket(sockSender);
    WSACleanup();
    return TRUE;
}

void dumpmbase(struct Message_base *mBase)
{
    std::cout<<int(mBase->ver)<<std::endl;
    std::cout<<int(mBase->ih)<<std::endl;
    std::cout<<int(mBase->dc)<<std::endl;
    std::cout<<int(mBase->message_len)<<std::endl;
    for(int i=0;i<META_NODEADDRLEN;i++)
    {
        char *tmp=(char *)mBase->from_addr;
        std::cout<<tmp[i];
    }
    std::cout<<std::endl;
    for(int i=0;i<META_NODEADDRLEN;i++)
    {
        char *tmp=(char *)mBase->to_addr;
        std::cout<<tmp[i];
    }
    std::cout<<std::endl;
    return;
}

DWORD WINAPI RecvThread(LPVOID lpParam)
{
    SOCKET sockSender;
    WSADATA wsaData;
    SOCKADDR_IN saSocket, saAddrLocal;
    BOOL fBroadcast = TRUE;
    int dwSocklen = sizeof(struct sockaddr_in), dwCksum[8], ret = 0;
    char cBuf[META_MAXLEN];
    struct Message_base *mBase;
    struct Node *node = NULL;

    sockSender = socket(PF_INET, SOCK_DGRAM, 0);

    saAddrLocal.sin_family = AF_INET;
    saAddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    saAddrLocal.sin_port = nSelfnode.node_port;
    bind(sockSender, (sockaddr*)&saAddrLocal, sizeof(saAddrLocal));

    for(;;)
    {
        ret = recvfrom(sockSender, cBuf, META_MAXLEN, 0, (SOCKADDR *)&saSocket, &dwSocklen);

        if(ret == -1)
        {
            continue;
        }

        mBase = (struct Message_base *)cBuf;

        if(memcmp(mBase->to_addr, nSelfnode.node_addr, META_NODEADDRLEN) != 0)
        {
            continue;
        }
        
        if(ret != mBase->message_len)
        {
            continue;
        }
        memcpy(dwCksum, mBase->cksum, sizeof(mBase->cksum));
        memset(mBase->cksum, 0, sizeof(mBase->cksum));
        if(memcmp(message_sum(mBase->message_len, cBuf), dwCksum, sizeof(mBase->cksum)) != 0)
        {
            continue;
        }

        node = getNodeByIp(saSocket.sin_addr.s_addr);

        if(mBase->dc == DC_DATA)
        {
            if(mBase->ih == IH_HELLO) 
            {
                continue;
            }
            struct Message_data *mData;
            mData = (struct Message_data *)cBuf + sizeof(struct Message_base);
            struct Application *aApp = getAppByAppnum(mData->app_num);
            if(!aApp)
            {
                continue;
            }
            if(mData->all_block != mData->block_num)
            {
                memcpy(aApp->databuf + aApp->datalen, mData + sizeof(struct Message_data), mData->block_len);
                aApp->datalen += mData->block_len;
            }else{
                aApp->ondata(aApp->datalen, aApp->databuf);
                memset(aApp->databuf, 0, aApp->datalen);
            }
        }else if(mBase->dc == DC_CONTROL)
        {
            if(mBase->ih == IH_HELLO && node == NULL)
            {
                struct Node nNew;
                memset(&nNew, 0, sizeof(nNew));
                nNew.connected = 1;
                nNew.node_ipaddr = saSocket.sin_addr.s_addr;
                nNew.node_port = saSocket.sin_port;
                memcpy(nNew.node_addr, mBase->from_addr, META_NODEADDRLEN);
                node = &nNew;
                addNode(&nNew);
            }
            struct Message_control *mcMessage;
            mcMessage = (struct Message_control *)cBuf + sizeof(struct Message_base);
            if(mcMessage->FIN || mcMessage->RST)
            {
                deleteNode(node);
                continue;
            }
            int tmp = 0;
            char *point = (char *)cBuf + sizeof(struct Message_base) + sizeof(struct Message_control);
            while(tmp <= mcMessage->all_options)
            {
                if(point[0] == 0)
                {
                    break;
                }else if(point[0] == 1)
                {
                    point++;
                    continue;
                }else if(point[0] == 2)
                {
                    unsigned short number = *((unsigned short *)&point[1]);
                    set_bit(node->node_type, number);
                    tmp++;
                    point += 3;
                }else if(point[0] == 3)
                {
                    unsigned short size = *((unsigned short *)&point[1]);
                    node->option.RDBlocksize = size;
                    tmp++;
                    point += 3;
                }else if(point[0] == 4)
                {
                    tmp++;
                    point += 5;
                }else if(point[0] == 5)
                {
                    struct Node nNew;
                    memset(&nNew, 0, sizeof(nNew));
                    nNew.node_ipaddr = *((int *)point + 1);
                    memcpy(nNew.node_addr,point + 5 , META_NODEADDRLEN);
                    connect(&nNew);
                    tmp++;
                    point += 21;
                }else if(point[0] == 6)
                {
                    tmp++;
                    point += 3;
                }
            
            }
        }
    }
    return 0;
}