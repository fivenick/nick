/*************************************************************************
    > File Name: analyinfile.h
    > Author: nick
    > Mail: allenthr@163.com 
    > Created Time: 2016年03月21日 星期一 15时27分14秒
 ************************************************************************/
#ifndef _ANALYINFILE_H
#define _ANALYINFILE_H

#include<stdio.h>
#include<string.h>
extern void dealInFile(char* mInFileName,char* mOutFileName);

struct FileLine{
	char* id;     //每个基站/wifi的标识符
	double  space;
	char** coordLst;//该字符数组中的每个元素都类似于“经度,纬度”
	int len;   //数组coordLst的长度
};

#endif
