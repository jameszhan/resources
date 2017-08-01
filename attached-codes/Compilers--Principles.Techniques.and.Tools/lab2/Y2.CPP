
/***********************************************************
*       实验二  递归下降语法分析演示程序模板(教学实验用)
*
*   为简化起见，将词法分析结果文件一次性读入到一结构体数组中。
*递归分析程序直接从结构体数组中去数。
*
*                                 Yao Hong  2008.4.5
************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "Lex.h"

#define MAXSIZE 500		//缓冲区可存放的单词数

typedef struct {
	int code;			//单词编号
	float attr;			//属性，采用float是因为常数直接作为属性，不考虑存放在符号表中
}Word;

Word w[MAXSIZE];		//作为单词缓冲

FILE *fptr;				//词法分析结果文件指针

int	bip=0;				//缓冲区扫描指针（逻辑指针）

float t;				//定义一个浮点数的目的是防止使用fscanf时，浮点库未启动错误。

int code,attr;			//当前待处理的单词类型及属性。

//函数声明
void PR();
void DSS();
void DS();
void IDS();
void TYPE();

void ESS();
void ES();
void WS();
void IFS();
void AS();
void RE();
void ROP();
void E();
void E1();
void T();
void T1();
void F();

//打开文件
void init() {

	if((fptr=fopen("obj.txt","r"))==NULL) {
		printf("Can't open");
		exit(0);
	}
}

//从文件中依次读单词编码及属性，写入到结构体数组中。
void getStreamToBuf() {
	int i=0;
	
	//依次从文件中读符号代码，写入到w数组。
	while(!feof(fptr) && i<MAXSIZE) {

		fscanf(fptr,"%d %f\n",&w[i].code,&w[i].attr);
		i++;
	}
	printf("%d\n",i);
}

//获取当前符号的类型及属性
void getCurCode(int &code,int &attr) {
	code=w[bip].code;
	attr=w[bip].attr;
	bip++;
}


//回退一个符号
void back() {
	bip--;
}


//错误处理
void error(char *msg) {
	printf("%s\n",msg);
	exit(0);			//凡是出错，均要退出。
}

/****************************************************************
*                      以下为递归下降子程序                     *
****************************************************************/

//开始符号 <PR>→<DSS><ESS>
void PR() {
	DSS();ESS();
}

//定义语句集的分析 <DSS>→<DS><DSS>|ε
void DSS() {
	//从缓冲区中取下一字符
	getCurCode(code,attr);
	
	//ε的情况,表示当前过程不处理。
	if(code!=VAR) 
	{	back();		//回退
		return; }
	else 
	{
		//进入到变量列表分析
		IDS();

		//接下来应该出现分号。
		getCurCode(code,attr);
		
		//SEM为宏定义，在Lex.h中定义，表示分号的编码
		if(code==SEM)	//递归，分析下一var列表。
			DSS();
		else
			//分号不匹配就出错。
			error("分号不匹配。\n");
	}
}

//变量列表分析 <IDS>→,<ID><IDS>|ε
void IDS() {
	//由于词法分析中已经产生了完整的单词，故标识符也为终结符;
	getCurCode(code,attr);

	if(code==COMMA) { //当然可以出现更多的标识符,每出现一个新的标识符，必先出现逗号。
		getCurCode(code,attr); //至少要有一个标识符。	
		
		if(code==ID) {
		
			IDS();
		//由于使用了超前搜索，无论取了多少个变量，最后都多取了一个，所以回退。
		back();
		}
	}
	else
	{
		//由于前面出现了<TYPE>,此处必须出现标识符，若不是，则出错。
		error("缺少标识符");
	}
}



void ESS() {

}

void T2() {

}

void T1() {

	T2();

	getCurCode(code,attr);

	while(code==ADD || code==SUB) 
		T2();
}

void E() {
	getCurCode(code,attr);
	
	//是小括号
	if(code==LLBR) {
		E();

		getCurCode(code,attr);

		if(code!=LRBR) error("括号不匹配\n");
	}
}


void main(int argc,char *args[]) {

	t=0;	//使用浮点库，防止其未启动错误。

	//初始化，打开文件
	init();

	//从文件中读出所有的符号写入缓冲区。
	getStreamToBuf();
 
	//从开始符号开始分析
	PR();

	printf("语法正确");  //能走到这一步，前面肯定没错。

	fclose(fptr);
}
