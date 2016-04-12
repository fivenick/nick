/*************************************************************************
    > File Name: parsefile.h
    > Author: nick
    > Mail: allenthr@163.com 
    > Created Time: 2016年04月06日 星期三 12时02分49秒
 ************************************************************************/
#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include<iostream>
#include<list>
#include<map>
#include<string>
using namespace std;

typedef list<map<string,string>> (*proc)(string);
typedef void (*outWay)(list<map<string,string>> mConstent,string mFileName);

class ParseFile{
	private:
		string inFileName;
		string outFileName;
		proc   fproc;
		outway  foutway;
	public:
		ParseFile(string mInFileName,proc mproc,outway moutway,string mOutFileName);
		void parse();
};

#endif

