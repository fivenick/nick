#include"analyinfile.h"
#include"weiszfeld.h"
#include"sample.h"

extern void convertCoord(char** mCoord,double* mRes);
extern void getLineData(char* mLineBuf,struct FileLine* mLineRes);
extern void fillFileLine(char** mVec,struct FileLine* mFileLine);
extern bool checkLine(char* mLineBuf);
extern void outResCoord(struct coordinate* mCoordRes , struct FileLine mfileLine,FILE* mOutFp);
extern struct coordinate* getResCoord(char** argv,int mLen);

char tmpvec[600000]={0};
char lineBuf[600000]={0};
int main(){
	char* inFileName="/home/allen/workspace_c++/forest-weiszfeld2/testdata/station.out";
   // char* inFileName="/home/allen/workspace_c++/forest-weiszfeld2/testdata/data";
	char* outFileName="/home/allen/workspace_c++/forest-weiszfeld2/testdata/station_result.out";
	dealInFile(inFileName,outFileName);
	return 0;
}
void  dealInFile(char* mInFileName,char* mOutFileName){
	FILE* inFp;
	FILE* outFp;
	if((inFp = fopen(mInFileName,"r")) == NULL){
		printf("open file error!! :%s\n",mInFileName);
		return;
	}
	if((outFp = fopen(mOutFileName,"w")) == NULL){
		printf("open file error!! :%s\n",mOutFileName);
		return;
	}
    while(1){
        memset(lineBuf,0,600000);
		memset(tmpvec,0,600000);
		fgets(lineBuf,600000,inFp);
		if(feof(inFp))
			break;
		if(!checkLine(lineBuf)){
			printf("the error line:%s\n ",lineBuf);
			continue;
		}
        struct FileLine lineRes;
		getLineData(lineBuf,&lineRes);
		struct coordinate* coordRes=getResCoord(lineRes.coordLst,lineRes.len);
        outResCoord(coordRes,lineRes,outFp); 
		char** tmpCoordLst=lineRes.coordLst;
		int tmpIndex=0;
		for(;tmpIndex<50000;++tmpIndex)
            free(tmpCoordLst[tmpIndex]);
		free(tmpCoordLst);
	}
	fclose(inFp);
	fclose(outFp);
	return ;
}
bool checkLine(char* mLineBuf){
//	if(strlen(lineBuf)>14000)
//		return false;
//	char* res[4]={NULL};
/*	strcpy(tmpvec,mLineBuf);
	int index=0;
	res[0]=strtok(tmpvec," ");
	while(res[index] != NULL)
		res[++index]=strtok(NULL," ");
*/
//	if(index != 3)
//		return false;
	return true;
}

void getLineData(char* mLineBuf,struct FileLine* mLineRes){
	char* res[4]={NULL};
	printf("getLineData\n");
    strcpy(tmpvec,mLineBuf);
	int index=0;
	res[0]=strtok(tmpvec," ");
	while(res[index] != NULL)
		res[++index]=strtok(NULL," ");
	printf("%s\n",res[0]);
	printf("%s\n",res[1]);
	printf("%s\n",res[2]);
	fillFileLine(res,mLineRes);
    return;
}
void fillFileLine(char** mVec,struct FileLine* mFileLine){
	if(mFileLine == NULL)
		return;
    mFileLine->id=mVec[0];
	mFileLine->space=atof(mVec[1]);
	char* tmp=mVec[2];
	char** res;
	res=(char**)my_malloc(50000*sizeof(char*));
	int kindex=0;
	for(;kindex<50000;++kindex){
		res[kindex]=(char*)my_malloc(50*sizeof(char));
	}
	int index=0;
	char* tmpBuf=strtok(tmp,"|");
	while(tmpBuf != NULL){
		printf("fillFileLine\n");
		strcpy(res[index++],tmpBuf);
		tmpBuf=strtok(NULL,"|");
	}
    mFileLine->coordLst=res;
	mFileLine->len=index;
}

struct coordinate* getResCoord(char** argv,int mLen){
	struct samples* ss=samples_create();
	char tmpStr[30]={0};
	int index=0;
	for(;index<mLen;++index){
		memset(tmpStr,0,30);
		char* tmpEle[2];
		printf("getResCoord\n");
		printf("%dkk%s\n",index,argv[index]);
        strcpy(tmpStr,argv[index]);
		printf("ll%s\n",tmpStr);
//		char* tmpStr=argv[index];
		char* result=NULL;
		tmpEle[0]=strtok(tmpStr,",");
		tmpEle[1]=strtok(NULL,",");
		double tmpCoord[2];
		convertCoord(tmpEle,tmpCoord);
	//	printf("x=%f,y=%f\n",tmpCoord[0],tmpCoord[1]);
		struct coordinate* cc=coordinate_create('s',tmpCoord[0],tmpCoord[1]);
		struct sample* sa=sample_create(cc,0,0,0);
		samples_add(ss,sa);
	}
	struct coordinate* result=weiszfeld(ss,1.0);
	return result;
}
void convertCoord(char** mCoord,double* mRes){//mCoord形式是“经度,纬度”
	mRes[0]=atof(mCoord[0]);
	mRes[1]=atof(mCoord[1]);
}

void outResCoord(struct coordinate* mCoordRes , struct FileLine mFileLine,FILE* mOutFp){
//	printf("the resutl coordinate is %f,%f\n",mCoordRes->x,mCoordRes->y);
    printf("out1\n");
    fprintf(mOutFp,"%s,%f,%f\n",mFileLine.id,mCoordRes->x,mCoordRes->y);
    printf("out2\n");
}

