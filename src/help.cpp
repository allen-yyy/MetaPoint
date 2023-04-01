// Copyright (c) 2022 Allen He
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <iostream>
#include <string>
#include "misc.h"

using namespace std;

string command[COMMANDS][2]=
{
    {"help","Show this screen"},
    {"console","Open a console"}
};

string opt[OPTS][2]
{
	{"-a,--app app,(app,...)","Node apps"},
	{"-h,--help command","Show command help"},
	{"-v,--version","Print version numbers"},
	{"-n,--node","Add a node"}
};

void
help(char *function)
{
	if(function == NULL)
	{
		cout<<"USAGE:\n\t";
		cout<<"metapoint [options] command\n\n";
		cout<<"VERSION\n\t";
		cout<<"0.01t-not-ready\n\n";
		cout<<"COMMANDS:\n\n";
    	for(int i=0;i<COMMANDS;i++)
		{
			std::cout<<"\t"<<command[i][0]<<"\t\t\t"<<command[i][1]<<endl;
		}
   		cout<<endl;
		cout<<"OPTIONS:"<<endl;
    	for(int i=0;i<3;i++)
		{
			cout.setf(std::ios::left);
			cout.width(0);
			cout<<"\t";
			cout.width(24);
			cout<<opt[i][0]; 
			cout<<opt[i][1]<<endl;
		}
	}else{
		string tmp(function);
		for(int i=0;i<COMMANDS;i++)
		{
			if(command[i][0]==tmp)
			{
				cout<<command[i][1];
				break;
			}
		}
	}
	exit(0);

    return;
}