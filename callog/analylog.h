#ifndef _ANALYLOG_H
#define _ANALYLOG_H
#include<string>
#include</usr/include/mysql/mysql.h>
#include"table.h"
using namespace std;


class AnalyLog{
    private:
        string fileName;
        Table*  table;
    public:
        AnalyLog(string mFile,Table* mTable);
        void insert(MYSQL* mdb);    
        bool insertLine(MYSQL* mDB,string mLine);
        void getStrLst(string mLine,vector<string>& mLst); //mLine中的字符串按空格分割成一个字符串数组
        string getQuery(vector<string> mLst);
};

#endif
