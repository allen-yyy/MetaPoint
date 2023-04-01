# Copyright (c) 2022 Allen He
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
G++ = g++ -I../include -I../ -Os -Wall -quiet
OBJS = message_sum.cpp sha-256.cpp

ALL: crypto.obj

*.obj: $(OBJS) Makefile
	$(G++) -c $^ -o $@
