// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/** 
 * @brief The MetaPoint message struct
*/
struct Message_base
{
    /// @brief Metapoint version
    char ver;
    #define DC_DATA 1
    #define DC_CONTROL 0
    /// @brief Data or control
    char dc:2;
    char zeros:4;
    #define IH_HELLO 1
    #define IH_NOTHELLO 0
    /// @brief Is HELLO message or not 
    char ih:2;
    /// @brief The message length
    short message_len;
    /// @brief From address(Nework ID)
    int from_addr[4];
    /// @brief To address(Nework ID)
    int to_addr[4];
    /// @brief Message check summary
    int cksum[8];
};


/**
 * @brief The MetaPoint data message struct
 * 
 */
struct Message_data
{
    /// @brief Block number
    char block_num;
    /// @brief All blocks
    char all_block;
    /// @brief Application number
    short app_num;
    /// @brief Push the message fast
    int PSH:2;
    /// @brief Send a YACK to sender
    int ACK:2;
    /// @brief Received ACK
    int YACK:2;
    /// @brief All block sended
    int FIN:2;
    int zero:8;
    /// @brief Block length
    short block_len;
};

struct Message_control
{
    /// @brief Options number
    char all_options;

    // Flags

    /// @brief Connection close
    int FIN:2;
    /// @brief Connection terminated abnormally
    int RST:2;
    /// @brief If received a DATA message, must send a YACK to sender
    int MACK:2;
    /// @brief This CONTROL message don't need ACK
    int NACK:2;
    /// @brief Sender needs node
    int NODE:2;
    /// @brief If received this message, must send a ACK to sender (Except with NACK)
    int ACK:2;
    /// @brief Sender needs receiver's support application
    int APP:2;

    int zero:10;
};