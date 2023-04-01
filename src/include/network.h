// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <windef.h>

/// @brief send DATA message
/// @param to_node the node send to
/// @param data data
/// @param dwDlen data length
/// @param mData Message_data, options
void senddatamessage(struct Node *to_node, char *data, int dwDlen, struct Message_data mData);

/// @brief Disconnect a node
/// @param nDisconnect the node
BOOL disconnect(struct Node *nDisconnect);

/// @brief Connect a node
/// @param nConnect the node want to connect
BOOL connect(struct Node *nConnect);

DWORD WINAPI RecvThread(LPVOID lpParam);