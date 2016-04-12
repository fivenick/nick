#include<iostream>
#include"wifitable.h"
#include"stationtable.h"
#include"analylog.h"
#include"database.h"
using namespace std;
bool initDB(MYSQL* mDB,DataBase mDBName);
DataBase& initDBConf(DataBase& mDBName);
int main(){
    MYSQL mysql;
    DataBase database;
    initDBConf(database);
    bool resDB=initDB(&mysql,database);
    if(!resDB)
        return 0;
    WifiTable wifi;
    StationTable station;
    string wifiFileName="/home/wangyao/wifiData/juhaijian_test_zhengjiang";
    string stationFileName="/home/wangyao/wifiData/juhaijian_test_zhengjiang_station";
//    string wifiFileName="/home/yangxingya/callog/cailog/wififile2";
//    string stationFileName="/home/yangxingya/callog/cailog/stationfile2";
//    string wifiFileName="/home/yangxingya/callog/newanaly/testdata/wifidata";
//    string stationFileName="/home/yangxingya/callog/newanaly/testdata/statdata";
    AnalyLog analyWifi(wifiFileName,&wifi);
    AnalyLog analyStation(stationFileName,&station);
    analyWifi.insert(&mysql);
    analyStation.insert(&mysql);
    return 1;
}
bool initDB(MYSQL* mDB,DataBase mDBName){
    mysql_init(mDB);
        if(mysql_real_connect(mDB,mDBName.host.c_str(),mDBName.user.c_str(),mDBName.pwd.c_str(),mDBName.dbName.c_str(),0,NULL,0)){
            cout<<"connect db success"<<endl;
            return true;
        }else
            cout<<"connect db fail"<<endl;
        return false;
}
DataBase& initDBConf(DataBase& mDBName){
        mDBName.host="localhost";
        mDBName.user="mysql";
        mDBName.pwd="titps4gg";
        mDBName.dbName="location160309";
        return mDBName;
}
