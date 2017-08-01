/***********************************************************
*       ʵ��һ  �ʷ�������ʾ����ģ��(��ѧʵ����)
*
*   ��ģ��ֻ����һ����ܣ�������Ҫ�����ݽṹ���������ݻ���Ҫ
*�����ơ�
*   Ϊ���������Դ����ֱ�Ӷ����ڴ滺�壬�����������̫��
*�������д��һ�ı��ļ���
*   ��ʵ����ʱ�����Ƿ��ű������������з���������ʱ����-1��
*   ���ѶȽϴ󣬿��ȿ�����Ļ�����
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
//״̬ת����ṹ
typedef struct StateTable {
	int state;
	int nextState;
	char input;
}STTable;

//�����ֱ�ṹ
typedef struct Preserve {
	char name[100];				//������
	int code;					//����������
}PreServe;

STTable States[STATESIZE];		//״̬ת����
PreServe ps[PRESERVESIZE];		//�����ֱ�

char srcBuf[BUFSIZE];			//��ʾ������ַ�������1000
char inputStr[INPUTSIZE];		//ÿ�в�����100�ַ�
int srcIp;						//���������߼���ָ��

FILE *fptr;						//�����ļ�ָ��
FILE *fpobj;					//����ļ�ָ��

int state;						//��ǰ״̬
char word[50];					//��ǰ����ʶ��ĵ���
int wip;						//���ڴ���ǰ����ʶ��ĵ���ָ��

//���ļ������ζ����ʱ��뼰���ԣ�д�뵽�ṹ�������С�
void getStreamToBuf() {
	int i;
	while(!feof(fptr) && srcIp<BUFSIZE) {
		fscanf(fptr,"%s",inputStr);			//����һ�ַ������ո�س����ɳ�Ϊ�ֽ�

		i=0;
		while(inputStr[i]!='\0')
			srcBuf[srcIp++]=inputStr[i++];
	}
	srcIp=0;					//���������ݾ����룬������ָ�븴λ��
	
	printf("%s\n",srcBuf);		//���Ի������Ƿ��Ѷ����ļ����ݡ�
}

//��ʼ��
void init() {
	//��Դ�����ļ������뻺����
	if((fptr=fopen("D:\\src.txt","r"))==NULL) {
		printf("Can't open the file");
		exit(0);
	}
    
	//�˴�ʵ�ִ�Ŀ���ļ���Ϊ��һ�������׼��
	//......��������Ĵ�������ʵ��֮

	getStreamToBuf();

	//����Ϊ״̬ת�����ʼ��
	
	States[0].state=0; States[0].input='+'; States[0].nextState=1;
    //�����ֵĵ�һ���ַ�������ȫ������
	States[1].state=0; States[1].input='0'; States[1].nextState=3; 
	States[2].state=0; States[2].input='='; States[2].nextState=1;
	//......״̬ת������Ԫ�飩�ĳ�ʼ�����벹������


	//����Ϊ�����ֳ�ʼ��
	sprintf(ps[0].name,"%s","if");		ps[0].code=$if;
	sprintf(ps[1].name,"%s","else");	ps[1].code=$else;
	//......�벹������
	printf("%s",ps[0].name);
}

//�˳�����
void quit(){
    fclose(fptr);
}

/*************************
*     ����Ϊ�����Ժ���
**************************/

//��ȡ��Ч�ַ������˵��ո񡢻س���
char  getEffChar(){
	while(srcBuf[srcIp]==' '|| srcBuf[srcIp]=='\t'
		  ||srcBuf[srcIp]=='\n')
		srcIp++;
	return srcBuf[srcIp];
}

//�б��Ƿ�����ĸ
bool isLetter(char c){
	bool result=false;
	//......������ʵ��
	return result;
}

//�б��Ƿ��������ַ�
bool isDigit(char c){
	bool result=false;
	//......������ʵ��
	return result;
}

//����ϵͳ�����֣����ر����ֱ���,���򷵻�-1
int getPerserve(char token[]) {
	//��ps��(��ǰ�涨��)�����������;
	//......������ʵ��
	return false;
}

/****************************************************
*          ����Ϊ�Զ�����ʵ��
*
*   ��������Ԫ��Ĵ洢�ṹ,�Բ��ķ�ʽʵ��
****************************************************/

//��ȡ��һ״̬�����뵱ǰ״̬�������ַ���������һ״̬���
int getNextState(int state, char input) {
	int result;
	//......��ʾ����States��ͬʱ����������ά��������ָ��srcIp�ı仯��
	return result;
}

//���ˣ���Գ�ǰ���٣�
void back(){ srcIp--; }

//��ȡ��һ��������
int getToken() {
	char c;							//��ŵ�ǰ�������ַ�
	state=0;						//׼�������µ��ַ�����ǰ״̬��0

	wip=0;							//׼�������µ��ʣ����ʲ���ָ����0

	while(state<100) {				//Ϊ���������Լ������̬״̬��<100��
		c=getEffChar();

		if(c=='\0') return -1;		//��Ϊ�����������ĳ��ڣ�Լ������������Ϊ������־

		word[wip++]=c;				//ÿ��һ���ַ�����д��word

		state=getNextState(state,c);
	}
	word[wip]='\0'					//һ�������������

	if(state==$id) {  //���ڱ����ֺͱ�ʶ������ͬ���Ĺ��ɽṹ����Ҫ��һ���ж�
		int pcode=getPerserve(word);
		
		if(pcode>0) 
			return pcode;
	}
    //����̬״̬��ת�������롣Ϊ���㣬������ֱ����Ƴ�״̬�ŵ����Ժ�����
	//ע������ǳ�����ƹ����е�һ�������Դ���
	return state-100;		
}

//���
void outputCode(int code, int attr) {
	//......������ļ����в�����ÿ����һ�α���������������������д��

}

void main() {
	init();
	
	int code=0;
	while(code>=0) {
		code=getToken();

		//�ϵ�Ҫ��:����������ʱд��-1,�����溯���ĵڶ�������
		//�ϸ�Ҫ��:�ɸ��ݱ�ʶ��������˳��д����
		outputCode(code,-1);  
	}
	fclose(fptr);		//�ر�ԭ�ļ�
	fclose(fpobj);		//�ر�Ŀ���ļ�
	quit();
}