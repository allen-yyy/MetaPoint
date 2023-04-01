// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define COMMANDS 2
#define OPTS 4

/// @brief help function
void help(char *function);

/// @brief message check sum
char *message_sum(int len,char *data);

void out_in_red(std::string str);

void out_in_yellow(std::string str);