// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <windows.h>
#include <iostream>
#include "misc.h"
#include "app.h"
#include "network.h"
#include "node.h"
#include "message.h"

int dwRunedApp = 0;
struct Application *appList;
extern struct Node nSelfnode;

BOOL isAppInList(struct Application *node)
{
    struct Application *next = appList;
    while(next != NULL)
    {
        if(node == next)
        {
            return TRUE;
        }
        next = next->next;
    }
    return FALSE;
}

void addApp(struct Application *node)
{
    if(isAppInList(node))
    {
        return;
    }
    node->next = appList;
    appList = node;
    return;
}

void deleteApp(struct Application *node)
{
    struct Application *next = appList;
    if(!isAppInList(node))
    {
        return;
    }
    if(next == node)
    {
        appList = node->next;
        return;
    }
    while(next != NULL)
    { 
        if(next->next == node)
        {
            next->next = node->next;
            return;
        }
        next = next->next;
    }
    return;
}

struct Application *getAppByAppnum(unsigned short sAppnum)
{
    struct Application *next = appList;
    while(next!=0)
    {
        if(next->appnum == sAppnum)
        {
            return next;
        }
        next = next->next;
    }
    return NULL;
}

BOOL LoadMetaApp(char *cAppname)
{
    struct Application app;
    char tmp[30];
    memcpy(tmp, cAppname, strlen(cAppname));
    memcpy(tmp + strlen(cAppname), ".dll", sizeof(".dll"));
    app.dll = LoadLibraryA(tmp);
    if(!app.dll)
    {
        out_in_red("ERROR: Cann't find application " + std::string(cAppname) + " !\n\n");
        return 0;
    }
    GetAppNum numfunc = (GetAppNum) GetMetaDllFunc(&app, "GetAppNum");
    app.appnum = (*numfunc)();
    if(app.appnum == 0)
    {
        out_in_red("ERROR: Application " + std::string(cAppname) + " isn't a heathy DLL!\n\n");
        FreeMetaDll(&app);
        return 0;
    }
    app.ondata = (OnData)GetMetaDllFunc(&app, "OnData");
    if(!app.ondata)
    {
        out_in_red("ERROR: Application " + std::string(cAppname) + " isn't a heathy DLL!\n\n");
        FreeMetaDll(&app);
        return 0;
    }
    app.onnode = (OnNode)GetMetaDllFunc(&app, "OnNode");
    if(!app.onnode)
    {
        out_in_red("ERROR: application " + std::string(cAppname) + " isn't a heathy DLL!\n\n");
        FreeMetaDll(&app);
        return 0;
    }
    app.init = (AppInit)GetMetaDllFunc(&app, "AppInit");
    if(!app.init)
    {
        out_in_red("ERROR: application " + std::string(cAppname) + " isn't a heathy DLL!\n\n");
        FreeMetaDll(&app);
        return 0;
    }
    if(!(*app.init)())
    {
        out_in_yellow("Warning: Application " + std::string(cAppname) + " initial error!\n\n");
        FreeMetaDll(&app);
        return 0;
    }
    app.name = cAppname;
    addApp(&app);
    return 0;
}

void *GetMetaDllFunc(struct Application *app, char *cFuncname)
{
    return (void *)GetProcAddress(app->dll, cFuncname);
}

void FreeMetaDll(struct Application *app)
{
    FreeLibrary(app->dll);
    return;
}

__declspec(dllexport) BOOL SendAppData(int *nodeaddr, unsigned short appnum, int dwLen, char *cData)
{
    struct Node *nTo = getNodeByAddress(nodeaddr);
    struct Message_data mData;

    if(dwLen > META_MAXLEN)
    {
        mData.zero = 0;
        mData.app_num = appnum;
        mData.block_num = 1;
        mData.block_len = dwLen;
        mData.all_block = 1;
        if(!get_bit(nTo->node_type, appnum))
        {
            return FALSE;
        }
        senddatamessage(nTo, cData, dwLen, mData);  
    }else{
        mData.zero = 0;
        mData.app_num = appnum;
        mData.block_num = 1;
        mData.block_len = dwLen;
        mData.all_block = 1;
        if(!get_bit(nTo->node_type, appnum))
        {
            return FALSE;
        }
        senddatamessage(nTo, cData, dwLen, mData);
    }
    
    return TRUE;
}

BOOL LoadApp(char *cAppname)
{
    return TRUE;
}

void app_init(char **cAppNames, int dwAppNum)
{
    int i = 0;
    if(dwAppNum == 0) 
    {
        //out_in_red("ERROR: No application run!");
        //exit(0);
        return;
    }
    while(i < dwAppNum)
    {
        LoadMetaApp(cAppNames[i]);
        i++, dwRunedApp++;
    }
    if(dwRunedApp == 0) 
    {
        //out_in_red("ERROR: No application run!\n\n");
        //exit(0);
    }
    return;
}

