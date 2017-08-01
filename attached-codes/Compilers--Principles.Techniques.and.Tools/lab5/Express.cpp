/****************************************************************
            ʵ����  �Զ����µ��������

	Ҫ����ʵ����Ļ�����ʵ���Զ����µ��������
  (ע����ʽ��E1�е�1Ϊ�±ꡣ���﷨�����У�E1��EΪͬһ���ս��������������ơ�)
	E->T {L.i=T.s} L {E.s=L.s}
	L->+ T {L1.i=L.i+T.s} L1 {L.s=L1.s}
	L->- T {L1.i=L.i-T.s} L1 {L.s=L1.s}
	L->��  {L.s=L.i}
	T->  F {R.i=F.s} R {T.s=R.s}
	R->* F {R1.i=R.i * F.s} L1 {R.s=R1.s}
	R->/ F {R1.i=R.i / F.s} L1 {R.s=R1.s}
	R->��  {R.s=R.i}
	F->(E) {F.s=E.i} 
	F->$num{F.s=$num.val}

	˼·������ÿ�����ս�����̣��̳�������Ϊ�������룬�ۺ����Է���

	ע�����������к�����ı��ʽ��Ϊ��ͨ���������ʽ��
	��(3.2+6.4)*2=��ĩβһ��Ҫ�Ӹ�=���ٻس���

                                          Yao Hong  2008.4.12
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "token.h"

/***********************************************
*				ȫ�ֱ�������������
***********************************************/
FILE *stream;				//������
char word[40];				//����
int wip;					//����ָ��
char bufChar;				//�ڴʷ������׶Σ�����ʹ���˳�ǰ��������ǰ�����ַ������ڸñ����С�
float curValue;				//��ŵ�ǰ��ֵ
int code;					//��ǰ���ʵı���

float E();					//��Լ������ս�����庯��
float T();
float R(float Ri);
float L(float Li);
float F();

void error(char msg[]);
/***********************************************
	������ɴʷ�����
************************************************/
//�б��Ƿ�Ϊ����
int isDigit(char ch) {
	if(ch>='0' && ch<='9')
		return 1;
	return 0;
}

//��ȡһ���ַ�,����ǰʹ���˳�ǰ��������ǰ�����ַ�������bufChar�С�
char readChar() {
	char ch;
	if(bufChar==' ') 
		ch=fgetc(stream);
	else {
		ch=bufChar;
		bufChar=' ';			//�����ÿ�
	}
	return ch;
}

//����һ������
int readNum(){
	char ch;
	 
	ch=readChar();
	while(isDigit(ch)){
		word[wip++]=ch;
		ch=readChar();
	}
	if(ch=='.'){					//С����
		word[wip++]='.';			//��С����д�봮.
		ch=fgetc(stream);
		while(isDigit(ch)){
			word[wip++]=ch;
			ch=readChar();
		}
	}

	word[wip++]='\0';				//�����ַ�����ֹλ�á�

	bufChar=ch;						//����ǰ�����ַ������ڻ����С�

	sscanf(word,"%f",&curValue);	//��word�е��ַ�ת��Ϊ�����ơ�

	return $num;					//�������ֵı��롣
}

//�Ƿ�Ϊ�ո�
int isBlank(char ch){
	if(ch==' '||ch=='\n'||ch=='\r'||ch=='\t') return 1;
	else
		return 0;
}

//�����ж�ȡһ������
int lexyy() {
	char ch=' ';
	
	if(bufChar==' ') 
		while(isBlank(ch)) ch=fgetc(stream); //�������еĿո�س�
	else {					//ʹ���˳�ǰ���������
		ch=bufChar;
		bufChar=' ';		//�ÿո�������������ַ���
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
		bufChar=ch;			//������������
		wip=0;				//����ָ����0
		return readNum();	//��ȡ�������֣��������ֵĵ��ʱ��룬�����ֵ�ֵ�ݴ���curValue������
	}
	else
		error("�ʷ�����");
	return 0;
}

/********************************************
*			�ݹ��½����﷨�������
*********************************************/

//���������
void error(char msg[])
{
	printf(msg); 
	exit(0);
}

//E->T {L.i=T.s} L {E.s=L.s}
float E() {
	float Ts,Ls;
	/*�벹ȫ����*/;
	return Ls;
}

//L->+ T {L1.i=L.i+T.s} L1 {L.s=L1.s}
//L->- T {L1.i=L.i-T.s} L1 {L.s=L1.s}
//L->��  {L.s=L.i}
float L(float Ri) {
	float Ts,Ls;
	Ls=0;
	if(code==$add) {			//�ӷ� +
		/*�벹ȫ����*/
	}else if(code==$sub) {		//���� -
		/*�벹ȫ����*/
	}else
		/*�벹ȫ����*/
	return Ls;
}

//T->  F {R.i=F.s} R {T.s=L.s}
float T() {
	float Fs,Rs;
	/*�벹ȫ����*/
	return Rs;
}

//R->* F {R1.i=R.i * F.s} R1 {R.s=R1.s}
//R->/ F {R1.i=R.i / F.s} R1 {R.s=R1.s}
//R->��  {R.s=R.i}
float R(float Ri) {
	float Fs,Rs;
	if(code==$mul) {			//�˷� *
		/*�벹ȫ����*/
	}else if(code==$div) {		//���� /
		/*�벹ȫ����*/
	}else
		/*�벹ȫ����*/;
	return Rs;
}

//F->(E) {F.s=E.s} 
//F->$num{F.s=$num.val}
float F() {
	float s;
	if(code==$llbr) {		//�Ƿ�Ϊ'('
		/*�벹ȫ����*/
	}else if(code==$num) {
		/*�벹ȫ����*/
	}else
		error("���ʽ�﷨����");
	return s;
}

//��ʼ��
void init() {
	bufChar=' ';
}

int main() {

	stream=stdin;			//stdin���׼������

	printf("�������������ʽ(ע��ĩβ�ȼ� = �ٻس����� \"3*(2.1+2)=\")��\n");

	init();					//��ʼ��

	code=lexyy();			//ͨ���ʷ���������ȡһ�����š�

	float s;
	s=E();					//�ӿ�ʼ���ſ�ʼ���з���

	printf("%f\n",s);

	getch();
	return 0;
}

