#include"wifitable.h"
#include<math.h>
#include<iostream>
#include"boost/regex.hpp"
using namespace std;
using namespace boost;

string attLstWifi[]={"aprefid","imei","mac","ts2","ssid","bssid","mapsp2","la","lo","rssi"};
int attTypeLstWifi[]={1,1,1,3,1,1,1,4,4,2};

WifiTable::WifiTable(){
    tableName="wifi_160314";
    int len=sizeof(attLstWifi)/sizeof(attLstWifi[0]);
    int index=0;
    for(;index<len;++index){
        attLst.push_back(attLstWifi[index]);
        attTypeLst.push_back(attTypeLstWifi[index]);
    }
}

bool WifiTable::judgeValAccordAtt(string mData,int mColIndex){
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
    }
}
bool WifiTable::judgeValAtt0(string mData){
    if(isNull(mData))
        return true;
//    string pat="(^[0-9a-zA-Z][0-9a-zA-Z]{14}$)|(^[0-9a-zA-Z][0-9a-zA-Z]{13})";
    string pat="^[0-9a-zA-Z]{32}$";
    regex pattern(pat,regex_constants::extended);
    if(regex_match(mData.c_str(),pattern))
        return true;
    return false;
}
bool WifiTable::judgeValAtt1(string mData){
    return true;
}
bool WifiTable::judgeValAtt2(string mData){
    return true;
}
bool WifiTable::judgeValAtt3(string mData){
    return true;
}
bool WifiTable::judgeValAtt4(string mData){
    return true;
}
bool WifiTable::judgeValAtt5(string mData){
    return true;
}
bool WifiTable::judgeValAtt6(string mData){
    return true;
}
bool WifiTable::judgeValAtt7(string mData){
    return true;
}
bool WifiTable::judgeValAtt8(string mData){
    return true;
}
bool WifiTable::judgeValAtt9(string mData){
    return true;
}

