#include"stationtable.h"
#include"boost/regex.hpp"
#include<iostream>
#include<math.h>
using namespace std;
using namespace boost;

string attLstStat[]={"imei","mac","capssid","capbssid","capaprefid","la","lo","mapsp","mccmnc","lac","cid","ctype","cts"};
int attTypeLstStat[]={1,1,1,1,1,4,4,1,2,2,2,1,3};

StationTable::StationTable(){
    tableName="station_160314";
    int len=sizeof(attLstStat)/sizeof(attLstStat[0]);
    int index=0;
    for(;index<len;++index){
        attLst.push_back(attLstStat[index]);
        attTypeLst.push_back(attTypeLstStat[index]);
    }
}
bool StationTable::judgeValAccordAtt(string mData,int mColIndex){
    switch(mColIndex){
        case 0:
            return judgeValAtt0(mData);
        case 1:
            return judgeValAtt1(mData);
        case 2:
            return judgeValAtt2(mData);
        case 3:
            return judgeValAtt3(mData);
        case 4:
            return judgeValAtt4(mData);
        case 5:
            return judgeValAtt5(mData);
        case 6:
            return judgeValAtt6(mData);
        case 7:
            return judgeValAtt7(mData);
        case 8:
            return judgeValAtt8(mData);
        case 9:
            return judgeValAtt9(mData);
        case 10:
            return judgeValAtt10(mData);
        case 11:
            return judgeValAtt11(mData);
        case 12:
            return judgeValAtt12(mData);
    }
}
bool StationTable::judgeValAtt0(string mData){
    if(isNull(mData))
        return true;
    string pat="(^[0-9a-zA-Z][0-9a-zA-Z]{14}$)|(^[0-9a-zA-Z][0-9a-zA-Z]{13})";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))    
        return true;
    return false;
}

bool StationTable::judgeValAtt1(string mData){
    if(isNull(mData))
        return true;
    string pat="^[0-9a-zA-Z]{2}:[0-9a-zA-Z]{2}:[0-9a-zA-Z]{2}:[0-9a-zA-Z]{2}:[0-9a-zA-Z]{2}:[0-9a-zA-Z]{2}$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt2(string mData){
    if(isNull(mData))
        return true;
    if(!judgeValAtt3(mData) && !judgeValAtt4(mData) && !judgeValAtt5(mData) && !judgeValAtt6(mData) && !judgeValAtt7(mData) && !judgeValAtt8(mData) && !judgeValAtt9(mData) && !judgeValAtt10(mData) && !judgeValAtt12(mData))
        return true;
    return false;
}

bool StationTable::judgeValAtt3(string mData){
    return judgeValAtt1(mData);
}
bool StationTable::judgeValAtt4(string mData){
    if(isNull(mData))
        return true;
    string pat="^[0-9a-zA-Z][0-9a-zA-Z]{31}$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt5(string mData){
    if(isNull(mData))
        return true;
    string pat="^\\d+(\\.\\d+)?$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt6(string mData){
    return judgeValAtt5(mData);
}
bool StationTable::judgeValAtt7(string mData){
    if(isNull(mData))
        return true;
    string pat="^[tgb]$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;

}
bool StationTable::judgeValAtt8(string mData){
    if(isNull(mData))
        return true;
    string pat="^[0-9]?\\d+$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt9(string mData){
     if(isNull(mData))
        return true;
    string pat="^[0-9]?\\d+$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt10(string mData){
     if(isNull(mData))
        return true;
    string pat="^[0-9]?\\d+$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt11(string mData){
     if(isNull(mData))
        return true;
    string pat="^[0-9a-zA-Z]+$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool StationTable::judgeValAtt12(string mData){
     if(isNull(mData))
        return true;
    string pat="^145";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}

