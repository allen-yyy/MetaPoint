# Copyright (c) 2022 Allen He
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
G++ = g++ -I../include -I../ -I./include/ -I./ -Os -Wall
OBJS = help.obj crypto/sha-256.obj network/node.obj app/app.obj network/sdrv.obj crypto/message_sum.obj

ALL: metapoint

%.obj: %.cpp Makefile
	$(G++) -c $*.cpp -o $*.obj

metapoint: Makefile
#make -C crypto
#make -C network
	make metapoint.exe

metapoint.exe: metapoint.obj $(OBJS) Makefile
	g++ -g $(OBJS) metapoint.obj -o metapoint.exe -l ws2_32 -Wl,--stack=536870912
