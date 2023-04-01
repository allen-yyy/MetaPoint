// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include <iostream>
#include <windows.h>
#include <windns.h>
#include <time.h>
#include "crypto.h"
#include "misc.h"
#include "node.h"
#include "network.h"

using namespace std;
string dns[3]
{
    "dns.meta-cdn1.allen-he.eu.org",
    "dns.meta-cdn2.allen-he.eu.org",
    "dns.meta-cdn3.allen-he.eu.org",
};

struct Node *naList = NULL;
struct Node nSelfnode;
int dwNodenums = 0;

int get_bit(long long *bitmap, int bit)
{
    int dwIndex = bit / 64, dwBit = bit % 64;

    return bitmap[dwIndex] & (1<<bit);
}

void set_bit(long long *bitmap, int bit)
{
    int dwIndex = bit / 64, dwBit = bit % 64;

    bitmap[dwIndex] |= (1<<bit);
    return;
}

BOOL isNodeInList(struct Node *node)
{
    struct Node *next = naList;
    while(next != NULL)
    {
        if(node == next)
        {
            return TRUE;
        }
        next = next->next;
    }
    return getNodeByAddress(node->node_addr) != NULL ? TRUE : FALSE;
}

void addNode(struct Node *node)
{
    if(isNodeInList(node))
    {
        return;
    }
    node->next = naList;
    naList = node;
    dwNodenums++;
    return;
}

void deleteNode(struct Node *node)
{
    struct Node *next = naList;
    if(!isNodeInList(node))
    {
        return;
    }
    if(next == node)
    {
        naList = node->next;
        dwNodenums--;
        return;
    }
    while(next != NULL)
    { 
        if(next->next == node)
        {
            next->next = node->next;
            dwNodenums--;
            return;
        }
        next = next->next;
    }
    return;
}

struct Node *getNodeByAddress(int *addr)
{
    struct Node *next = naList;
    while(next!=0)
    {
        if(memcmp(next->node_addr, addr, sizeof(next->node_addr)) == 0)
        {
            return next;
        }
        next = next->next;
    }
    return NULL;
}

struct Node *getNodeByIp(int ip)
{
    struct Node *next = naList;
    while(next!=0)
    {
        if(next->node_ipaddr == ip)
        {
            return next;
        }
        next = next->next;
    }
    return NULL;
}

void node_init()
{
    int i;
    FILE *fNodeaddr = fopen("./nodeaddr.meta","r");
    int ndaddr[4];
    WSADATA wsaData;
    char cHostname[256];
    
    if(fNodeaddr == NULL)
    {
        fNodeaddr = fopen("./nodeaddr.meta","w");
        string str;                 
        char c;                     
        int idx;                    
        srand(time(NULL));
        for(idx = 0;idx < 255;idx++)
        {
           c = 'a' + rand()%26;
           str.push_back(c);      
        }
        char tmp[32+2];
        memcpy(tmp,str.c_str(),str.size());
        memcpy(ndaddr, message_sum(str.size(), tmp), META_NODEADDRLEN);
        fwrite(ndaddr, 1, 16,fNodeaddr);
        fclose(fNodeaddr);
    }else{
        fread(ndaddr, META_NODEADDRLEN, 1, fNodeaddr);
        fclose(fNodeaddr);
    }
    nSelfnode.connected = 1;
    memcpy(nSelfnode.node_addr, ndaddr, META_NODEADDRLEN);
    if (WSAStartup(MAKEWORD(2, 2),&wsaData)!=0)
    {
        cout<<"ERROR: WSA startup error!";
        exit(0);
    }
    gethostname(cHostname,256);
    HOSTENT *host = gethostbyname(cHostname);
    if (NULL==host)
    {
        out_in_red("gethostbyname() Failed!\n");
    }
    in_addr PcAddr;
    for (int i=0;;i++)
    {
        char *p = host->h_addr_list[i];
        if (p == NULL)
        {
            break;
        }
        memcpy(&(PcAddr.S_un.S_addr), p, host->h_length);
    }
    nSelfnode.node_ipaddr = htonl(PcAddr.S_un.S_addr);

    if(nSelfnode.node_port == 0)
    {
        nSelfnode.node_port = htons(META_UDPPORT);
    }
    /*for(i=0;i<3;i++)
    {
        PDNS_RECORD dnsRecords;
        TCHAR *cDnsname;
        struct Node node;

        node.connected = 0;
        node.node_ipaddr = 0;
        memset(node.node_addr, 0, sizeof(node.node_addr));
        MultiByteToWideChar(CP_ACP, 0, dns[i].c_str() , -1, cDnsname, dns[i].size());
        int status = DnsQuery(cDnsname, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &dnsRecords, NULL);
        node.node_ipaddr = dnsRecords[0].Data.A.IpAddress;
        DnsRecordListFree(dnsRecords, DnsFreeRecordListDeep);
        connect(&node);
    }*/
    return;
}