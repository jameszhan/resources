/****************************************************************
            实验五  自顶向下的语义分析

	要求：在实验二的基础上实现自顶向下的语义分析
  (注：下式中E1中的1为下标。在语法分析中，E1与E为同一非终结符。其余符号类似。)
	E->T {L.i=T.s} L {E.s=L.s}
	L->+ T {L1.i=L.i+T.s} L1 {L.s=L1.s}
	L->- T {L1.i=L.i-T.s} L1 {L.s=L1.s}
	L->ε  {L.s=L.i}
	T->  F {R.i=F.s} R {T.s=R.s}
	R->* F {R1.i=R.i * F.s} L1 {R.s=R1.s}
	R->/ F {R1.i=R.i / F.s} L1 {R.s=R1.s}
	R->ε  {R.s=R.i}
	F->(E) {F.s=E.i} 
	F->$num{F.s=$num.val}

	思路：对于每个非终结符过程，继承属性作为参数传入，综合属性返回

	注：本程序运行后，输入的表达式即为普通的算术表达式。
	如(3.2+6.4)*2=。末尾一定要加个=，再回车。

                                          Yao Hong  2008.4.12
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "token.h"

/***********************************************
*				全局变量及函数声明
***********************************************/
FILE *stream;				//输入流
char word[40];				//单词
int wip;					//单词指针
char bufChar;				//在词法分析阶段，由于使用了超前搜索，超前搜索字符保存在该变量中。
float curValue;				//存放当前数值
int code;					//当前单词的编码

float E();					//针对几个非终结符定义函数
float T();
float R(float Ri);
float L(float Li);
float F();

void error(char msg[]);
/***********************************************
	以下完成词法分析
************************************************/
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

//读入一个数字
int readNum(){
	char ch;
	 
	ch=readChar();
	while(isDigit(ch)){
		word[wip++]=ch;
		ch=readChar();
	}
	if(ch=='.'){					//小数点
		word[wip++]='.';			//将小数点写入串.
		ch=fgetc(stream);
		while(isDigit(ch)){
			word[wip++]=ch;
			ch=readChar();
		}
	}

	word[wip++]='\0';				//设置字符串终止位置。

	bufChar=ch;						//将超前搜索字符保存在缓冲中。

	sscanf(word,"%f",&curValue);	//将word中的字符转换为二进制。

	return $num;					//返回数字的编码。
}

//是否为空格
int isBlank(char ch){
	if(ch==' '||ch=='\n'||ch=='\r'||ch=='\t') return 1;
	else
		return 0;
}

//从流中读取一个单词
int lexyy() {
	char ch=' ';
	
	if(bufChar==' ') 
		while(isBlank(ch)) ch=fgetc(stream); //跳过所有的空格回车
	else {					//使用了超前搜索的情况
		ch=bufChar;
		bufChar=' ';		//用空格清掉超期搜索字符。
	}
		
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
	else if(ch=='=')
		return $sharp;
	else if(isDigit(ch))  {
		//printf("OK");
		bufChar=ch;			//超期搜索缓冲
		wip=0;				//单词指针清0
		return readNum();	//读取整个数字，返回数字的单词编码，而数字的值暂存于curValue变量。
	}
	else
		error("词法错误");
	return 0;
}

/********************************************
*			递归下降的语法语义代码
*********************************************/

//错误处理代码
void error(char msg[])
{
	printf(msg); 
	exit(0);
}

//E->T {L.i=T.s} L {E.s=L.s}
float E() {
	float Ts,Ls;
	Ts=T();
	Ls=L(Ts);
	return Ls;
}

//L->+ T {L1.i=L.i+T.s} L1 {L.s=L1.s}
//L->- T {L1.i=L.i-T.s} L1 {L.s=L1.s}
//L->ε  {L.s=L.i}
float L(float Ri) {
	float Ts,Ls;
	Ls=0;
	if(code==$add) {			//加法 +
		code=lexyy();
		Ts=T();
		Ls=L(Ri+Ts);
	}else if(code==$sub) {		//减法 -
		code=lexyy();
		Ts=T();
		Ls=L(Ri-Ts);
	}else
		Ls=Ri;
	return Ls;
}

//T->  F {R.i=F.s} R {T.s=L.s}
float T() {
	float Fs,Rs;
	Fs=F();
	Rs=R(Fs);
	return Rs;
}

//R->* F {R1.i=R.i * F.s} R1 {R.s=R1.s}
//R->/ F {R1.i=R.i / F.s} R1 {R.s=R1.s}
//R->ε  {R.s=R.i}
float R(float Ri) {
	float Fs,Rs;
	if(code==$mul) {			//乘法 *
		code=lexyy();
		Fs=F();
		Rs=R(Ri*Fs);
	}else if(code==$div) {		//除法 /
		code=lexyy();
		Fs=F();
		Rs=R(Ri/Fs);
	}else
		Rs=Ri;
	return Rs;
}

//F->(E) {F.s=E.s} 
//F->$num{F.s=$num.val}
float F() {
	float s;
	if(code==$llbr) {		//是否为'('
		code=lexyy();		//再读一个单词
		s=E();
		if(code!=$lrbr)		//是否为')'
			error("括号不匹配");
		code=lexyy();

	}else if(code==$num) {
		s=curValue;			//取出数值
		code=lexyy();		//读下一个单词
	}else
		error("表达式语法错误");
	return s;
}

//初始化
void init() {
	bufChar=' ';
}

int main() {

	stream=stdin;			//stdin表标准输入流

	printf("请输入算术表达式(注意末尾先加 = 再回车，如 \"3*(2.1+2)=\")：\n");

	init();					//初始化

	code=lexyy();			//通过词法分析器读取一个符号。

	float s;
	s=E();					//从开始符号开始进行分析

	printf("%f\n",s);

	getch();
	return 0;
}

