#include<iostream>
#include<fstream>
#include"analylog.h"
using namespace std;

AnalyLog::AnalyLog(string mFile,Table* mTable){
    fileName=mFile;
    table=mTable;
}
void AnalyLog::insert(MYSQL* mDB){
    ifstream fin;
    string lineData;
    fin.open(fileName.c_str());
    if(!fin.is_open()){
       cout<<"文件 "<<fileName<<" 打开失败"<<endl;
       return ;
    }
    while(getline(fin,lineData)){
       if(!insertLine(mDB,lineData.c_str())){
              cout<<"insert line failed: "<<lineData<<endl;
              return ;
       }
    }
    fin.close();
    return ;
    
}
void AnalyLog::getStrLst(string mLine,vector<string>& mLst){
    int size=mLine.size();
    if(size == 0)
        return ;
    int index=0;
    string tmpVal;
    for(;index<size;++index){
         if(mLine[index] == '\t'){
              if(tmpVal.empty())
                  tmpVal="NULL";
              mLst.push_back(tmpVal);
              tmpVal.clear();
              continue;
         }
         char tmpChar=mLine[index];
         if(tmpChar=='\'')
               tmpVal+='\\';
         tmpVal+=tmpChar;
    }
    if(tmpVal.empty())
        tmpVal="NULL";
    mLst.push_back(tmpVal);
}
string AnalyLog::getQuery(vector<string> mLst){
    string queryFront("insert into ");
    queryFront+=table->getTableName();
    queryFront+="( ";
    queryFront+=table->getAttLstStr();
    queryFront+=" )";
    string queryBack(" values( ");
    queryBack+=table->getValLstStr(mLst);
    queryBack+=" )";
    return queryFront+queryBack;
}
bool AnalyLog::insertLine(MYSQL* mDB,string mLine){
        int size=mLine.size();
        if(size == 0)
             return true;
        vector<string> strLst;
        getStrLst(mLine,strLst);
        string query=getQuery(strLst);
        if(mysql_real_query(mDB,query.c_str(),query.length()) == 0){
       //      cout<<"插入字符串成功: "<<query<<endl;
             return true;
        }
        cout<<query<<endl;
  //      cout<<"插入字符串失败： "<<query<<endl;
  //      return false;
        return true;
}

