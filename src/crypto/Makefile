# Copyright (c) 2022 Allen He
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
G++ = g++ -I../include -I../ -I./ -Os -Wall
OBJS = message_sum.obj sha-256.obj

ALL: crypto.obj

%.obj: %.cpp
	$(G++) -c $*.cpp -o $*.obj

crypto.obj: $(OBJS)
	

