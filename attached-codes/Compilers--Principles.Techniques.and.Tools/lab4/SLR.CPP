/***********************************************************
*       实验三  LL(1)分析表演示程序模板(教学实验用)
*
*   采用SLR分析法实现表达式文法：
*   E->E+T|T
*   T->T*F|F
*   F->(E)|$num
*
*   其中$num代表一个实数，通过词法分析得到. 
*
*  	注：本程序运行后，输入的表达式即为普通的算术表达式。
*	如(3.2+6.4)*2#。末尾一定要加个#，再回车。
*                                         
*                                   Yao Hong 2009.10.20修订
*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "token.h"

#define MAXSIZE 200			//堆栈的最大尺寸
#define MAXROW	11			//分析表的最大行数
#define MAXCOL	9			//分析表的最大列数

#define S 10				//入栈编码
#define G 10				//Goto表中的编码
#define R 100				//归约编码
#define ACC 1000			//接受编码

//产生式候选登记表
//由于产生式右部的内容已经在堆栈顶部，只需出栈，无需比对，故本结构只保存其长度
typedef struct Production {
	char leftName;			//左部名称
	int leftCode;			//左部编码
	int rightLen;			//右部长度，决定出栈数。
} Prod;						

//标记字典
typedef struct C{
	char name;				//名称
	int	code;				//编码
}TokenDict;			

//代表非终结符和终结符(由于符号栈和状态栈均为同步操作。通过结构体的方式实现堆栈合并)
typedef struct T{
	char name;				//名称(便于外部输出)
	int code;				//编码(便于内部处理)
	int state;				//状态
	int value;				//该非终结符的值
}Token;

//堆栈类型
typedef struct S1 {
	Token body[MAXSIZE];	//堆栈体
	int ip;					//栈顶指针
} TokenStack;

//以下定义全局变量
int at[MAXROW][MAXCOL];		//分析表
Prod  p[7];					//产生式候选集
TokenDict dictionary[10];	//单词字典
TokenStack *stack;			//堆栈（集成符号栈，状态栈，也包括后面的语义栈）

FILE *stream;				//输入流
char word[40];				//单词
int wip;					//单词指针
char bufChar=' ';			//在词法分析阶段，由于使用了超前搜索，超前搜索字符保存在该变量中。
int  opState;				//当前分析状态(归约0 或 移进1)。

void error(char msg[]);

/****************************************
*		以下完成堆栈的操作				*
*****************************************/

//初始化堆栈
void initStack(TokenStack *s) {
	if(s==NULL) error("堆栈未初始化");
	s->ip=0;
	s->body[0].code=$sharp;
	s->body[0].name='#';
	s->body[0].state=0;
	s->body[0].value=0;
}

//入栈
void push(TokenStack *s,char name,int code,int state,int value) {
	 /*...请补充完整...*/
}

//出栈
Token *pop(TokenStack *s) {
	Token *t= &s->body[s->ip--];
	return t;
}

//获取栈顶元素
Token *getTop(TokenStack *s) {
	return &s->body[s->ip];	
}

/****************************************
*		以下完成词法分析				*
*****************************************/

//判别是否为数字
int isDigit(char ch) {
	if(ch>='0' && ch<='9')
		return 1;
	return 0;
}

//读取一个字符,若当前使用了超前搜索，超前搜索字符保存在bufChar中。
char readChar() {
	char ch;
	if(bufChar==' ') 
		ch=fgetc(stream);
	else {
		ch=bufChar;
		bufChar=' ';			//缓冲置空
	}
	return ch;
}

//读入一数字串（含小数部分）
int readNum(){
	char ch;

	wip=0;
	ch=readChar();
	while(isDigit(ch)){
		word[wip++]=ch;
		ch=readChar();
	}
	if(ch=='.'){				//小数点
		word[wip++]=ch;
		ch=readChar();
		while(isDigit(ch)){
			word[wip++]=ch;
			ch=readChar();
		}
	}

	word[wip]='\0'; 
	printf("Num:%s",word);
	bufChar=ch;					//将超前搜索字符保存在缓冲中。
	return $num;				//返回数字的编码。
}

//是否为空格
int isBlank(char ch){
	if(ch==' '||ch=='\n'||ch=='\r'||ch=='\t') return 1;
	else
		return 0;
}

//译码表的初始化
void initDictionary() {
	
	dictionary[0].name='+';
	dictionary[0].code=$add;

	dictionary[1].name='-';
	dictionary[1].code=$sub;

	dictionary[2].name='*';
	dictionary[2].code=$mul;

	dictionary[3].name='/';
	dictionary[3].code=$div;

	dictionary[4].name='(';
	dictionary[4].code=$llbr;

	dictionary[5].name=')';
	dictionary[5].code=$lrbr;

	dictionary[6].name='E';
	dictionary[6].code=_E;

	dictionary[7].name='T';
	dictionary[7].code=_T;

	dictionary[8].name='F';
	dictionary[8].code=_F;

	dictionary[9].name='i';		//i代表数字,这与教材一致
	dictionary[9].code=$num;
}

//从流中读取一个单词，专门针对本文法。
int lexyy() {
	char ch=readChar();
	while(isBlank(ch)) ch=readChar(); //跳过所有的空格回车
	
	if(ch=='+')
		return $add;
	else if(ch=='-')
		return $sub;
	else if(ch=='*')
		return $mul;
	else if(ch=='/')
		return $div;
	else if(ch=='(')
		return $llbr;
	else if(ch==')')
		return $lrbr;
	else if(ch=='#')
		return $sharp;
	else if(isDigit(ch)) {
		bufChar=ch;				//相当于超前搜素，返回一字符，让readNum统一处理
		return readNum();		//由于本实验不考虑语义的值，所以不考虑所读的值。
	}
	return 0;
}

/****************************************
*		以下完成语法分析				*
****************************************/
void error(char msg[])
{
	printf(msg); 
	//free(tokenStack);
	//getchar();
	getch();
	exit(0);
}

//初始化单词缓冲
void initWordBuf() {
	bufChar=' ';
	wip=0;
	word[wip]='\0';
}

void initTable() {
	//初始化表头
	at[0][0]=$num;		//数字
	at[0][1]=$add;		//+
	at[0][2]=$mul;		//*
	at[0][3]=$llbr;		//(
	at[0][4]=$lrbr;		//)
	at[0][5]=$sharp;	//#
	at[0][6]=_E;		//<E>
	at[0][7]=_T;		//<T>
	at[0][8]=_F;		//<F>
	//at[0][9]=$sub;		//-	  后面添加，使之更符合习惯
	//at[0][10]=$div;		// /

	//设置表的内容
	at[1][0]=S+5;		//S5
	at[1][3]=S+4;		//S4
	at[1][6]=G+1;		//Goto 1
	at[1][7]=G+2;		//Goto 2
	at[1][8]=G+3;		//Goto 3 (以下类似) 

	//第2行
	at[2][1]=S+6;
	at[2][5]=ACC;		//接受
	
	at[3][1]=R+2;
	at[3][2]=S+7;
	at[3][4]=R+2;
	at[3][5]=R+2;

	at[4][1]=R+4;
	at[4][2]=R+4;
	at[4][4]=R+4;
	at[4][5]=R+4;

	at[5][0]=S+5;
	at[5][3]=S+4;
	at[5][6]=G+8;
	at[5][7]=G+2;
	at[5][8]=G+3;

	at[6][1]=R+6;
	at[6][2]=R+6;
	at[6][4]=R+6;
	at[6][5]=R+6;

	at[7][0]=S+5;
	at[7][3]=S+4;
	at[7][7]=G+9;
	at[7][8]=G+3;

	at[8][0]=S+5;
	at[8][3]=S+4;
	at[8][8]=G+10;
	
	at[9][1]=S+6;
	at[9][4]=S+11;

	at[10][1]=R+1;
	at[10][2]=S+7;
	at[10][4]=R+1;
	at[10][5]=R+1;

	at[11][1]=R+3;
	at[11][2]=R+3;
	at[11][4]=R+3;
	at[11][5]=R+3;

	at[12][1]=R+5;
	at[12][2]=R+5;
	at[12][4]=R+5;
	at[12][5]=R+5;

	//Prod p[7];
	p[1].leftName='E';
	p[1].leftCode=_E;
	p[1].rightLen=3;

	p[2].leftName='E';
	p[2].leftCode=_E;
	p[2].rightLen=1;

	p[3].leftName='T';
	p[3].leftCode=_T;
	p[3].rightLen=3;
	
	p[4].leftName='T';
	p[4].leftCode=_T;
	p[4].rightLen=1;

	p[5].leftName='F';
	p[5].leftCode=_F;
	p[5].rightLen=3;

	p[6].leftName='F';
	p[6].leftCode=_F;
	p[6].rightLen=1;
}


//输出堆栈的值
void outputStacks(TokenStack *s)
{
	int i;

	printf("\n");
	printf("符号栈(状态栈):");
	for(i=0;i<=s->ip;i++) {
			printf("%c(%d) ",s->body[i].name,s->body[i].state);
	 
	}
 
	printf("\n");
}

//根据当前的编号查其名称(输出时用)
char getName(int code) {
	int i=0;
	while(i<9) {
		i++;
		if(dictionary[i].code==code)
			return dictionary[i].name;
	}
	return ' ';
}

//从堆栈取出状态
int getState(TokenStack *s) {
	/*...请补充该过程...*/
}

//根据状态，获取下一步操作
int getOperate(int state,int input) {
	/*...请补充该过程...*/
}



//对一个输入符号执行分析
//说明：由于堆栈集成为一个，所以堆栈操作可以不指出堆栈变量。
int analysis(TokenStack *s,int code) {
		
	int curState,curOp,pi;					//当前状态，操作，产生式编号
	curState=/*...请补充该语句...*/;		//获取当前状态
	curOp=/*...请补充该语句...*/;			//获取当前操作

	if(curOp==-1) error("分析表中未定义");

	if(curOp==ACC) {						//接受过程处理
		printf("\n输入串正确");
		//getchar();
		getch();
		exit(0);							//输入串正确出口
	}
	else if(curOp>R) {						//归约过程处理
		pi=curOp-R;							//去掉R标识，得到产生式编号

		//根据产生式候选右部的长度决定出栈数
		for(int i=0;i<p[pi].rightLen;i++) {	//此时栈顶以下若干元素恰好为产生式右部
			/*...请补充该语句...*/;			//出栈。
		}

		curState=/*...请补充该语句...*/;	//获取当前栈顶状态。
		
		curOp=/*...请补充该语句...*/;		//查Goto表获取跳转的状态。
		
		//符号栈入栈.
		//状态入栈(即查Goto表对应的值，所以G与S必须相等)
		/*...请补充该语句...*/;

		outputStacks(s);						
		opState=0;								//当前为归约状态
	}
	else if(curOp>S) {							//Si入栈
		
		/*...请补充该语句...*/;					//当前符号、状态入栈
		outputStacks(s);
		opState=1;								//当前为移进状态
	}
	else 
		error("语法错误!");						//上述已有的编码空间不存在，则出错。
	return 0;
}

int main() {
	int code,op;
	TokenStack *s;

	stream=stdin;			//将stream重定向到标准输入设备

	initWordBuf();			//初始化单词缓冲区
	initDictionary();		//初始化标记词典
	
	initTable();			//初始化分析表

	s=(TokenStack *)malloc(sizeof(TokenStack));
	
	initStack(s);			//初始化栈

	printf("请输入算术表达式：\n");

	code=lexyy();			//通过词法分析器读取一个符号。

	while(1)				//主程序不考虑退出，分析表的使用过程中自然有出口。
	{    
		analysis(s,code);
		if(opState==1)		//当前为移进动作
		  code=lexyy();		//输入下一个符号
	}

	free(s);				//释放资源
	 
	return 0;
}