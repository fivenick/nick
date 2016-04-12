/*************************************************************************
    > File Name: parsefile.cpp
    > Author: nick
    > Mail: allenthr@163.com 
    > Created Time: 2016年04月06日 星期三 12时33分44秒
 ************************************************************************/

#include<iostream>
#include<stdio.h>
#include"parsefile.h"

#define LINELEN   1000
using namespace std;
ParseFile::ParseFile(string mInFileName,proc mproc,outway moutway,string mOutFileName){
	inFileName=mInFileName;
	fproc=mproc;
	foutway=moutway;
	outFileName=mOutFileName;
}
void ParseFile::parse(){
	FILE* fp;
	if((fp = fopen(inFileName.c_str(),"r"))==NULL){
		printf("open file %s fail!!",inFileName.c_str());
		return;
	}
	char lineBuf[LINELEN]={0};
	while(1){
		fgets(lineBuf,LINELEN,fp);
		if(feof(fp))
			break;
		list<map<string,string>> res=fproc(lineBuf);
		foutway(res,outFileName);
	}
	fclose(fp);
}

