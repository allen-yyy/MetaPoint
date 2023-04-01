# Copyright (c) 2022 Allen He
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
G++ = g++ -I../include -I../ -I../crypto -Os -Wall -l ws2_32
OBJS = node.obj sdrv.obj

ALL: network.obj

%.obj: %.cpp
	$(G++) -c $*.cpp -o $*.obj

network.obj: $(OBJS)