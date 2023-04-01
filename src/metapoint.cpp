// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <iostream>
#include <windows.h>
#include <algorithm>
#include "misc.h"
#include "network.h"
#include "node.h"
#include "app.h"

using namespace std;

BOOL bIsConsole = 0;
extern int dwNodenums;
extern int dwRunedApp;
extern struct Node nSelfnode;

int hchar2int(char c)
{
	if(c<='9'&&c>='0') return c-'0';
	if(c<='z'&&c>='a') return c-'a'+10;
	if(c<='Z'&&c>='A') return c-'A'+10;
	return 0;
}

int expand_string(const char *cStr, char **cArgs);

int
metapoint_init(int argc, void *argv[], char **cApps)
{
    BOOL bOhelp = FALSE;
    char *cHelpfun = NULL;
    int dwAppNum = 0;
    int i = 0;
    DWORD dwRecvpid;
    cout<<"MetaPoint--The decentralized network.\n\n";
	cout<<"Copyright 2022 Allen He\n\n";

    nSelfnode.node_port = 0;

    while(i<argc)
    {
        if(strncmp((char *)argv[i],"-h",2)==0)
        {
            i++;
            cHelpfun = (char *) argv[i];
            bOhelp = TRUE;
        }else if(strncmp((char *)argv[i],"help",4)==0)
        {
            bOhelp = TRUE;
        }else if(strncmp((char *)argv[i],"--help",6)==0)
        {
            i++;
            cHelpfun = (char *) argv[i];
            bOhelp = TRUE;
        }else if(strncmp((char *)argv[i],"-a",2)==0)
        {
            i++;
            dwAppNum = expand_string((char *)argv[i], cApps);
        }else if(strncmp((char *)argv[i],"console",7)==0)
        {
            bIsConsole = TRUE;
        }else if(strncmp((char *)argv[i],"-n",2)==0)
        {
            i++;
            char nodeaddr[30],nodeip[30];
            short nodeport = 0;
            struct Node nNew;
            sscanf((char *)argv[i],"%s@%s:%d", nodeaddr, nodeip, &nodeport);
            memset(&nNew, 0, sizeof(nNew));
            for(int i=0;i<32;i++)
            {
                nNew.node_addr[i/8] = nNew.node_addr[i/8]*16 + hchar2int(nodeaddr[i]);
            }
            nNew.node_ipaddr = inet_addr(nodeip);
            nNew.node_port = htons(nodeport);
        }else if(strncmp((char *)argv[i],"--node",6)==0)
        {
            i++;
            char nodeaddr[30],nodeip[30];
            short nodeport = 0;
            struct Node nNew;
            sscanf((char *)argv[i],"%s@%s:%d", nodeaddr, nodeip, &nodeport);
            memset(&nNew, 0, sizeof(nNew));
            for(int i=0;i<32;i++)
            {
                nNew.node_addr[i/8] = nNew.node_addr[i/8]*16 + hchar2int(nodeaddr[i]);
            }
            nNew.node_ipaddr = inet_addr(nodeip);
            nNew.node_port = htons(nodeport);
            connect(&nNew);
        }else if(strncmp((char *)argv[i],"--port",6)==0)
        {
            unsigned short port;
            i++;
            port = atoi((const char *)argv[i]);
            nSelfnode.node_port = htons(port);
        }
        i++;
    }
    if(bOhelp == TRUE) help(cHelpfun);
    
    node_init();
    CreateThread(NULL, 256, RecvThread, NULL, 0, &dwRecvpid);
    return dwAppNum;
}

int expand_string(const char *cStr, char **cArgs)
{
    int dwStrlen = strlen(cStr), i=0;
    int len = 0, argc = 0;
    while(i<=dwStrlen)
    {
        if((cStr[i] == ',' && len > 0) || (i == dwStrlen && len>0))
        {
            char *tmp = (char *)malloc(len+1);
            memcpy(tmp, cStr + i - len, len);
            cArgs[argc] = tmp;
            cArgs[argc][len] = 0;
            argc++;
            len = 0;
        }else len++;
        i++;
    }
    return argc;
}

void out_in_red(string str)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    cout<<str;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return;
}

void out_in_yellow(string str)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
    cout<<str;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return;
}

int main(int argc, void *argv[])
{
    char *cApps[65536];
    int dwAppnum = metapoint_init(argc, argv, cApps);
    if(!bIsConsole && !dwRunedApp && !dwNodenums)
    {
        out_in_red("Error: No node, no application and no console!\n");
        exit(0);
    }
    if(!bIsConsole && !dwNodenums)
    {
        out_in_yellow("Warning: No node and Cann't add node!\n");
    }
    app_init(cApps, dwAppnum);

    string cmdline;

    while(1)
    {
        cout<<">";
        cin>>cmdline;

        transform(cmdline.begin(), cmdline.end(), cmdline.begin(), ::tolower); 

        if(cmdline == "addnode")
        {
            char nodeaddr[30]={},nodeip[30]={},str[40]={};
            short nodeport = 0;
            struct Node nNew;
            cin.getline(str, 40);
            int i=2;
            memcpy(nodeaddr, str + i, META_NODEADDRLEN);
            i+=META_NODEADDRLEN+1;
            while(1)
            {
                if(str[i] == ':')
                {
                    i++;
                    break;
                }
                nodeip[i-META_NODEADDRLEN-3] = str[i];
                i++;
            }
            nodeip[i-META_NODEADDRLEN-1] = '\0';
            nodeport = atoi(str + i);
            memset(&nNew, 0, sizeof(nNew));
            memcpy(nNew.node_addr, nodeaddr, META_NODEADDRLEN);
            nNew.node_ipaddr = inet_addr(nodeip);
            nNew.node_port = htons(nodeport);
            connect(&nNew);
        }else if(cmdline == "addapp")
        {
            char cAppname[30];
            cin>>cAppname;
            LoadMetaApp(cAppname);
        }
    }
    return 0;
}