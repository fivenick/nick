#ifndef _STATIONTABLE_H
#define _STATIONTABLE_H
#include"table.h"

class StationTable:public Table{
    public:
        StationTable();
        bool judgeValAccordAtt(string mData,int mColIndex);
        bool judgeValAtt0(string mData);
        bool judgeValAtt1(string mData);
        bool judgeValAtt2(string mData);
        bool judgeValAtt3(string mData);
        bool judgeValAtt4(string mData);
        bool judgeValAtt5(string mData);
        bool judgeValAtt6(string mData);
        bool judgeValAtt7(string mData);
        bool judgeValAtt8(string mData);
        bool judgeValAtt9(string mData);
        bool judgeValAtt10(string mData);
        bool judgeValAtt11(string mData);
        bool judgeValAtt12(string mData);
};

#endif

