#ifndef _TABLE_H
#define _TABLE_H
#include<string>
#include<vector>
#include"boost/regex.hpp"
using namespace std;
using namespace boost;

class Table{
    protected:
        string tableName;
        vector<string> attLst;
        vector<int>    attTypeLst; //1代表字符串varchar，2代表整型int，3代表bigint,4代表double
    public:
        Table();
        string getTableName();
        string getAttLstStr();//得到中间用逗号分隔的字符串
        string getValLstStr(vector<string> mLst);//得到中间用逗号分隔的字符串
        string getInCompleteValStr(vector<string> mLst);
        virtual bool   judgeValAccordAtt(string mData,int mColIndex);
        bool isNull(string mData);
        bool judgeZero(string mData);
};


#endif
