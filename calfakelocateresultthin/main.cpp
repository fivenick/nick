/*************************************************************************
    > File Name: main.cpp
    > Author: nick
    > Mail: allenthr@163.com 
    > Created Time: 2016年04月06日 星期三 11时51分15秒
 ************************************************************************/

#include<iostream>
#include<string>
#include"parsefile.h"
using namespace std;

list<map<string,string>> parProc(string);
void outWay(list<map<string,string>> mConstent,string mFileName);
int main(){
	string inFileName="/home/allen/workspace_c++/calfakelocateresultthin/test";
	string outFileName="/home/allen/workspace_c++/calfakelocateresultthin/result";
    ParseFile parFile(inFileName,parProc,outWay,outFileName);
	parFile.parse();
	return 0;
}
string getDoubleStr(Double mDouble){
	stringstream ss;
	ss<<mDouble;
	string tmpRes=NULL;
	tmpRes<<ss;
	ss.clear();
	return tmpRes;
}
map<string,string> parProc(string mLine,list<map<string,string>*> mLst){
	map<string,string> res;
	int startGG=mLine.find("gglr=");
	int endGG=mLine.find("]",startGG);
	int startTK=mLine.find("tklr=");
	int endTK=mLine.find("]",startTK);
	if((startGG == string::npos)||(endGG == string::npos)||
			(startTK == string::npos)||(endTK == string::npos))
	    return li;
	Point ggPoint=getPoint(mLine.substr(endGG).substr(startGG));
	Point tkPoint=getPoint(mLine.substr(endTK).substr(startTK));
	double distance=getDistance(ggPoint,tkPoint);
	string strDis=getDoubleStr(distance);
	map<string,string>* tmpMap=(map<string,string>*)malloc(sizeof(map<string,string>));
	tmpMap->insert("distance",strDis);
	mList.insert(tmpMap);
	return tmpMap;
}
void outWay(list<map<string,string>> mConstent,string mFileName){
	FILE* fp=NULL;
	if((fp=fopen(mFileName,"w")) == NULL){
		printf("write to file %s fail!!",mFileName);
		return;
	}
	
}









