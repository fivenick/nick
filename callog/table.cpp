#include"table.h"
#include<iostream>
using namespace std;


Table::Table(){
    tableName="";
}
string Table::getTableName(){
    return tableName;
}
bool Table::isNull(string mData){
    int start=mData.find_first_not_of("\t");
    int end=mData.find_last_not_of("\t");
    string tmp=mData.substr(start,end-start+1);
    if(tmp == "NULL")
        return true;
    return false;
}
bool Table::judgeZero(string mData){
    if((mData.size() == 1) && (mData[0] == '0'))
        return true;
    return false;
}
string Table::getAttLstStr(){
    if(attLst.size() == 0)
        return "";
    int len=attLst.size();
    string tmpRes;
    int index=0;
    for(;index<len;++index){
        tmpRes.append(attLst[index]);
        tmpRes.append(",");
    }
    tmpRes[tmpRes.size()-1]=' ';
    return tmpRes;
}
string Table::getValLstStr(vector<string> mLst){
    int len=mLst.size();
    if(len == 0)
        return "";
    int attLen=attLst.size();    
    string tmpRes;
    if(len == attLen){
        int index=0;
        for(;index<len;++index){
            string tmpVal=mLst[index];
            if(attTypeLst[index] == 1){
                tmpVal.insert(0,"\'");
                tmpVal.append("\'");
            }
            tmpRes.append(tmpVal);
            tmpRes.append(",");
        }
        tmpRes[tmpRes.size()-1]=' ';
    }else if(len < attLen){
//        tmpRes=getInCompleteValStr(mLst);
//          cout<<mLst.size()<<" : "<<mLst[0]<<endl;
        tmpRes="11111";
    }else{
        cout<<"getValLstStr is fail"<<endl;
        return "";
    }
    return tmpRes;
}

string Table::getInCompleteValStr(vector<string> mLst){
    int realLen=mLst.size();
    if(realLen == 0)
        return "";
    int len=attLst.size();
    int leftCount=len-realLen;
    int index=0;
    int indexVal=0;
    string tmpRes;
    for(;indexVal<realLen;++indexVal){
        string tmpStrVal=mLst[indexVal];
        if(leftCount == 0){
            if(attTypeLst[index]==1){
                tmpStrVal.insert(0,"\'");
                tmpStrVal.append("\'");
            }
            tmpRes.append(tmpStrVal);
        }else{
            if(judgeValAccordAtt(tmpStrVal,index)){
                if(attTypeLst[index]==1){
                    tmpStrVal.insert(0,"\'");
                    tmpStrVal.append("\'");
                }
                tmpRes.append(tmpStrVal);
            }else{
                tmpRes.append("NULL");
                --indexVal;
                --leftCount;
            }
        }
        ++index;
        tmpRes.append(",");
    }
    while(leftCount > 0){
        tmpRes.append("NULL");
        tmpRes.append(",");
        --leftCount;
    }
    tmpRes[tmpRes.size()-1]=' ';
    return tmpRes;
}
bool Table::judgeValAccordAtt(string mData,int mColIndex){
    return false;
}






