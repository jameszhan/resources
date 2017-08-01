/***********************************************************
*       实验三  LL(1)分析表演示程序模板(教学实验用)
*
*    采用LL(1)表分析法实现表达式文法的语法检验。
*	 (0)E ->TX
*    (1)X ->+TX		(2)X ->-TX		(3)X ->ε
*    (4)T ->FY
*    (5)Y ->*FY		(6)Y ->/FY		(7)F ->ε 
*	 (8)F ->(E)		(9)F ->i
*	 思路：其中i指代数字。先通过词法分析，将实际数字识别
*          用i替代，再进行分析。如
*          3.14*2 =>  i*i
*
*                                 Yao Hong 2008.4.12
************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>


#define MAXPROD			10		//产生式候选数目
#define MAXLLITEM		16		//LL分析表的非空项数

#define STACK_SIZE		200		//堆栈的最大容量
#define MAXEXP_SIZE		100		//表达式的最大长

//存放产生式的结构
typedef struct Production{
	char LeftCode;				//产生式左部
	char rightCodes[10];		//产生式右部
	int rightLen;				//产生式右部长				
}Production;

//分析表项结构
typedef struct LLItem{
   char LeftCode;				//当前非终结符
   char inputCode;				//当前输入
   int Prod;					//产生式编号
}LLItem;

char	stack[STACK_SIZE];	//堆栈体
int		top;					//栈顶指针

LLItem  LLTable[MAXLLITEM];		//LL分析表
Production prods[MAXPROD];		//产生式集

char exp[MAXEXP_SIZE];			//表达式字符串
int	 epos;						//表达式中的当前访问位置	

//错误处理函数
void error(char msg[]) {
	printf(msg);
	getch();
	exit(0);
}

void init() {
	prods[0].LeftCode='E';sprintf(prods[0].rightCodes,"%s","TX"); prods[0].rightLen=2;//E ->TX
	prods[1].LeftCode='X';sprintf(prods[1].rightCodes,"%s","+TX");prods[1].rightLen=3;//X ->+TX
	prods[2].LeftCode='X';sprintf(prods[2].rightCodes,"%s","-TX");prods[2].rightLen=3;
	prods[3].LeftCode='X';sprintf(prods[3].rightCodes,"%s","");   prods[3].rightLen=0;
	prods[4].LeftCode='T';sprintf(prods[4].rightCodes,"%s","FY"); prods[4].rightLen=2;
	prods[5].LeftCode='Y';sprintf(prods[5].rightCodes,"%s","*FY");prods[5].rightLen=3;
	prods[6].LeftCode='Y';sprintf(prods[6].rightCodes,"%s","/FY");prods[6].rightLen=3;
	prods[7].LeftCode='Y';sprintf(prods[7].rightCodes,"%s","");   prods[7].rightLen=0;
	prods[8].LeftCode='F';sprintf(prods[8].rightCodes,"%s","(E)");prods[8].rightLen=3;
	prods[9].LeftCode='F';sprintf(prods[9].rightCodes,"%s","i");  prods[9].rightLen=1;

	LLTable[ 0].LeftCode='E';LLTable[ 0].inputCode='(';LLTable[0].Prod=0;
	LLTable[ 1].LeftCode='E';LLTable[ 1].inputCode='i';LLTable[1].Prod=0;

	LLTable[ 2].LeftCode='X';LLTable[ 2].inputCode='+';LLTable[2].Prod=1;
	LLTable[ 3].LeftCode='X';LLTable[ 3].inputCode='-';LLTable[3].Prod=2;
	LLTable[ 4].LeftCode='X';LLTable[ 4].inputCode=')';LLTable[4].Prod=3;
	LLTable[ 5].LeftCode='X';LLTable[ 5].inputCode='#';LLTable[5].Prod=3;

	LLTable[ 6].LeftCode='T';LLTable[ 6].inputCode='(';LLTable[6].Prod=4;
	LLTable[ 7].LeftCode='T';LLTable[ 7].inputCode='i';LLTable[7].Prod=4;

	LLTable[ 8].LeftCode='Y';LLTable[ 8].inputCode='*';LLTable[8].Prod=5;
	LLTable[ 9].LeftCode='Y';LLTable[ 9].inputCode='/';LLTable[9].Prod=6;
	LLTable[10].LeftCode='Y';LLTable[10].inputCode='+';LLTable[10].Prod=7;
	LLTable[11].LeftCode='Y';LLTable[11].inputCode='-';LLTable[11].Prod=7;
	LLTable[12].LeftCode='Y';LLTable[12].inputCode=')';LLTable[12].Prod=7;
	LLTable[13].LeftCode='Y';LLTable[13].inputCode='#';LLTable[13].Prod=7;

	LLTable[14].LeftCode='F';LLTable[14].inputCode='(';LLTable[14].Prod=8;
	LLTable[15].LeftCode='F';LLTable[15].inputCode='i';LLTable[15].Prod=9;
}

//入栈操作
void StackPush(char code) { 
	if(top<STACK_SIZE)
		stack[++top]=code;
	else
		error("堆栈溢出");
}

//出栈操作
char StackPop() { 
	if(top<=0)
		error("堆栈为空");
	return stack[top--];
}

//取栈顶元素
char GetStackTop(){ 
	if(top<=0)
		error("堆栈为空");
	return stack[top];
}

//获取当前单词（在本文法中只含有单字符的单词，故逻辑上的单词作物理上的字符型处理）
char getCurrentWord() {
	return exp[epos];
}

//定位并获取下一单词
char getNextWord() { 
    epos++;
	//过滤掉空格，跳格，回车
	while(exp[epos]==' ' || exp[epos]=='\t' || exp[epos]=='\n')
		epos++;
	return exp[epos];
}


//查LL分析表，input为输入的终结符，leftCode为待展开的非终结符
//返回产生式编号，若无，返回-1
int getLLItem(char input,char leftCode){ 
	int i;
	for(i=0;i<MAXLLITEM;i++)
		if(LLTable[i].inputCode==input && LLTable[i].LeftCode==leftCode)
			return LLTable[i].Prod;
	return -1;
}

//输入产生式编号，返回产生式结构体指针
Production *nextOperate(int prod){ 
	return &prods[prod];
}

//输入、堆栈操作、输出。
//若为非终结符出栈，右边入栈
//若为终结符，比较，相等则继续，不等则出错。
void main() {
    char c;									//接收当前输入的单词（本文法中为单字母符号）
	int i;									//入栈次数控制变量
	int len;								//当前产生式右部的长
	int proNum;								//当前产生式的编号
	char stTop;								//栈顶元素
	Production *prod;						//当前选中的产生式

	init();

	StackPush('#'); StackPush('E');			//初始化
	//scanf("%s",exp);						//输入待分析的表达式，调试阶段最好用下一句
	sprintf(exp,"%s","i+i#");				//用于调试阶段，避免每次都要输入。

	c=getCurrentWord();
	while((stTop=GetStackTop())!='#') {
		if(stTop>='A' && stTop<='Z') {		//栈顶为非终结符处理过程，大写字母表示非终结符

			proNum=getLLItem(c,StackPop());	//查LL分析表，同时出栈

			if(proNum>=0) {
				prod=nextOperate(proNum);	//(1)根据产生式编号取得产生式结构体指针

				len=prod->rightLen;			//(2)通过结构体获得产生式右部的长len

				for(i=0;i<len;i++)			//(3)构造循环用len控制，将产生式右部入栈。
					StackPush(prod->rightCodes[len-i-1]);	
			}
			else
				error("语法错误：找不到对应的文法");
		}
		else {								//不是非终结符，作为终结符处理，直接进行匹配
			if(c!=StackPop())
				error("语法错误：终结符不匹配");
			else
				c=getNextWord();
		}
	}
	
	printf("语法正确");
	
	getchar();			//暂停，避免立即回到集成环境，以便看输出
}
