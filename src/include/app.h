// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <windef.h>

typedef unsigned short (*GetAppNum)();
typedef BOOL (*OnData)(DWORD, BYTE *);
typedef BOOL (*AppInit)();
typedef BOOL (*OnNode)(int *);

struct Application
{
    char *name;
    unsigned short appnum;
    HINSTANCE dll;
    OnData ondata;
    AppInit init;
    OnNode onnode;
    int datalen;
    BYTE *databuf;
    struct Application *next;
};

struct Application *getAppByAppnum(unsigned short sAppnum);

void deleteApp(struct Application *node);

void addApp(struct Application *node);

BOOL isAppInList(struct Application *node);

void app_init(char **cAppNames, int dwAppNum);

BOOL LoadMetaApp(char *cAppname);

void *GetMetaDllFunc(struct Application *app, char *cFuncname);

void FreeMetaDll(struct Application *app);