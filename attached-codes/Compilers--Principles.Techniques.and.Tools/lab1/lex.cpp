/***********************************************************
*       实验一  词法分析演示程序模板(教学实验用)
*
*   本模板只构造一个框架，定义主要的数据结构。具体内容还需要
*大家设计。
*   为简化起见，将源程序直接读入内存缓冲，因此容量不能太大。
*分析结果写入一文笔文件。
*   本实验暂时不考虑符号表操作，因此所有符号属性暂时均填-1。
*   如难度较大，可先考虑屏幕输出。
*
*                                 Yao Hong  2008.4.5
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Lex.h"

#define BUFSIZE			2000
#define INPUTSIZE		100
#define STATESIZE		100
#define PRESERVESIZE	100
//状态转换表结构
typedef struct StateTable {
	int state;
	int nextState;
	char input;
}STTable;

//保留字表结构
typedef struct Preserve {
	char name[100];				//保留字
	int code;					//保留字内码
}PreServe;

STTable States[STATESIZE];		//状态转换表
PreServe ps[PRESERVESIZE];		//保留字表

char srcBuf[BUFSIZE];			//演示程序的字符不超过1000
char inputStr[INPUTSIZE];		//每行不超过100字符
int srcIp;						//缓冲区（逻辑）指针

FILE *fptr;						//输入文件指针
FILE *fpobj;					//输出文件指针

int state;						//当前状态
char word[50];					//当前正被识别的单词
int wip;						//用于处理当前正被识别的单词指针

//从文件中依次读单词编码及属性，写入到结构体数组中。
void getStreamToBuf() {
	int i;
	while(!feof(fptr) && srcIp<BUFSIZE) {
		fscanf(fptr,"%s",inputStr);			//读入一字符串，空格回车均可成为分界

		i=0;
		while(inputStr[i]!='\0')
			srcBuf[srcIp++]=inputStr[i++];
	}
	srcIp=0;					//当所有内容均读入，缓冲区指针复位。
	
	printf("%s\n",srcBuf);		//测试缓冲区是否已读入文件数据。
}

//初始化
void init() {
	//打开源程序文件，读入缓冲区
	if((fptr=fopen("D:\\src.txt","r"))==NULL) {
		printf("Can't open the file");
		exit(0);
	}
    
	//此处实现打开目标文件，为下一步输出作准备
	//......仿照上面的代码自行实现之

	getStreamToBuf();

	//以下为状态转换表初始化
	
	States[0].state=0; States[0].input='+'; States[0].nextState=1;
    //用数字的第一个字符来代表全部数字
	States[1].state=0; States[1].input='0'; States[1].nextState=3; 
	States[2].state=0; States[2].input='='; States[2].nextState=1;
	//......状态转换表（三元组）的初始化，请补充完整


	//以下为保留字初始化
	sprintf(ps[0].name,"%s","if");		ps[0].code=$if;
	sprintf(ps[1].name,"%s","else");	ps[1].code=$else;
	//......请补充完整
	printf("%s",ps[0].name);
}

//退出后处理
void quit(){
    fclose(fptr);
}

/*************************
*     以下为辅助性函数
**************************/

//获取有效字符（过滤掉空格、回车）
char  getEffChar(){
	while(srcBuf[srcIp]==' '|| srcBuf[srcIp]=='\t'
		  ||srcBuf[srcIp]=='\n')
		srcIp++;
	return srcBuf[srcIp];
}

//判别是否是字母
bool isLetter(char c){
	bool result=false;
	//......请自行实现
	return result;
}

//判别是否是数字字符
bool isDigit(char c){
	bool result=false;
	//......请自行实现
	return result;
}

//如是系统保留字，返回保留字编码,否则返回-1
int getPerserve(char token[]) {
	//查ps表(见前面定义)，返回其编码;
	//......请自行实现
	return false;
}

/****************************************************
*          以下为自动机的实现
*
*   建议以三元组的存储结构,以查表的方式实现
****************************************************/

//获取下一状态，输入当前状态，输入字符，返回下一状态编号
int getNextState(int state, char input) {
	int result;
	//......提示：查States表，同时本函数负责维护缓冲区指针srcIp的变化。
	return result;
}

//回退（针对超前搜速）
void back(){ srcIp--; }

//获取下一单词内码
int getToken() {
	char c;							//存放当前读到的字符
	state=0;						//准备读入新的字符，当前状态置0

	wip=0;							//准备读入新单词，单词操作指针清0

	while(state<100) {				//为方便起见，约定非终态状态号<100，
		c=getEffChar();

		if(c=='\0') return -1;		//作为整个输入流的出口，约定负数编码作为结束标志

		word[wip++]=c;				//每读一个字符则将其写入word

		state=getNextState(state,c);
	}
	word[wip]='\0'					//一个单词输入完毕

	if(state==$id) {  //由于保留字和标识符具有同样的构成结构，需要进一步判断
		int pcode=getPerserve(word);
		
		if(pcode>0) 
			return pcode;
	}
    //将终态状态号转换成内码。为方便，将内码直接设计成状态号的线性函数。
	//注：这仅是程序设计过程中的一个技巧性处理。
	return state-100;		
}

//输出
void outputCode(int code, int attr) {
	//......对输出文件进行操作，每调用一次本函数，将上面两个参数写入

}

void main() {
	init();
	
	int code=0;
	while(code>=0) {
		code=getToken();

		//较低要求:所有属性暂时写入-1,即下面函数的第二个参数
		//较高要求:可根据标识符产生的顺序，写入编号
		outputCode(code,-1);  
	}
	fclose(fptr);		//关闭原文件
	fclose(fpobj);		//关闭目标文件
	quit();
}