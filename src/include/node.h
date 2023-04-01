// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <windef.h>

#define META_UDPPORT        12452
#define META_MAXLEN         16*1024*1024 //16MB
#define META_NODEADDRLEN    16

struct NodeOption
{
    int RDMustACK:2;
    short RDBlocksize;
    int SDMustACK:2;
};

struct Node
{
    /// @brief Node network address
    int node_addr[4];
    /// @brief Node IP address
    int node_ipaddr;
    /// @brief Node port number
    unsigned short node_port;
    /// @brief Support applictions
    long long node_type[1024];
    /// @brief Is node connected?
    int connected:1;
    /// @brief Node options
    struct NodeOption option;
    /// @brief Next node
    struct Node *next;
};

int get_bit(long long *bitmap, int bit);

void set_bit(long long *bitmap, int bit);

BOOL isNodeInList(struct Node *node);

void addNode(struct Node *node);

void deleteNode(struct Node *node);

struct Node *getNodeByAddress(int *addr);

struct Node *getNodeByIp(int ip);

void node_init();
