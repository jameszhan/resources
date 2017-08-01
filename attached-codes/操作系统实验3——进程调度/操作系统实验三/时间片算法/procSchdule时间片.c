#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <conio.h>
#define getpch(type) (type*)malloc(sizeof(type))
//#define NULL 0
struct pcb{
	char name[10];                 //������
	char state;                    //����״̬��ʾ ������/ִ�У� 
	int ntime;                     //��������ִ��ʱ��
	int rtime;                     //��ʱ��Ƭ��ʱ��                   
	struct pcb* link;              //ָ����һ�����̵�ָ��
}*ready=NULL, *p;
typedef struct pcb PCB;
int start;                                        //�˵����Ʊ�־
void sort()                                       //���̰�ָ���㷨����
{
	PCB *end, *recent;
	int inlist=0;                             //��ʾ�����Ѿ��嵽�������
	if(ready==NULL)
	{
		ready=p; 
	}
	else               
	{
        recent=ready;
		end=recent->link;
        while(end!=NULL)
		{
			  recent=recent->link;
			  end=end->link;	
		}
		if(end==NULL)
          recent->link=p;
	}    //end else
}

void input()                                                    //�������뺯��
{
	int i,num;
	
	printf("\nPlease input the number of processes:");
	scanf("%d",&num);                                          //������̸��� num
	for(i=0;i<num;i++)
	{
		printf("\nProcess NO.%d:\n",i);
		p=getpch(struct pcb);                                 
		                                                       //������Ϣ¼�� 
		printf("\nInput the name of the process:");
		scanf("%s",p->name);
		printf("\nInput the running time of the process:");
		scanf("%d",&p->ntime);
		printf("\n");                                         
		                                                        
		p->rtime=0;          //����ռ��cpu��ʱ��
		p->state='w';        //���̾���״̬  
		p->link=NULL;
	    sort();              //ÿ����һ����������һ��  �������� num ��
	}
}

int space()                                 //ͳ�������еĽ�����
{
	int l=0;
	PCB *pr=ready;
	while(pr!=NULL)
	{
		l++;                    //���ڼ�¼�����еĽ�����
		pr=pr->link;
	}
	return(l);
}

void disp(PCB *pr)                             //��ӡ��ʾ��ǰ����ִ�еĽ��̵������Ϣ
{
	printf("\nqname\tstate\tndtime\truntime\n");
	printf("|%s\t",pr->name);
	printf("|%c\t",pr->state);
	printf("|%d\t",pr->ntime);
	printf("|%d\t",pr->rtime);
	printf("\n");
}

void check()
{
	PCB *pr;
	printf("\n *********The current running process is: %s\n",p->name);
	disp(p);
	pr=ready;                                                  //��ס��ͷ����
	printf("\n *********The state of the Waiting List:\n");
	while(pr!=NULL)                                            //��ӡ��ʣ������н��̵������Ϣ
	{
		disp(pr);
		pr=pr->link;
	}
	if(pr==NULL)
		printf("\nNo waiting process!\n");
}

void destroy()                                        //��ӡ����ִ����Ϣ��ʾ
{
	printf("\nProcess [%s] has finished.\n",p->name);
	free(p);                                          //�ͷ���ʱ����p���ڴ�ռ�   
}

void running()
{
	(p->rtime)++;                                   //??????????????????????????????????????????????
	if(p->rtime==p->ntime)
		destroy();
	else
	{
//		(p->super)--;
		p->state='w';
		sort();                   //���²�������
	}
}
 /*******************************************************************�˵�����********/
void Menu()
{
  /*******************          �˵�ѡ��      *****************************************/   
	char menu;
    printf("     **********************************************************************\n");
    printf("     **                                                                  **\n");
    printf("     **                      ******************                          **\n");
    printf("     **                      *  PRO SCHEDULE  *                          **\n");
    printf("     **                      ******************                          **\n");
	printf("     **                                                                  **\n");
	printf("     **                      MENU  DIRECTION                             **\n");
	printf("     **                                                                  **\n");
	printf("     **                      y:  ENTER SIMULATE                          **\n");
	printf("     **                      e:  PROCESS INTRODUCTION                    **\n");
	printf("     **                                                                  **\n");
	printf("     **                      q:  QUIT PROCESS                            **\n");
	printf("     **                                                                  **\n");
	printf("     **                      author: qhu07 xuzikun                       **\n");
    printf("     **********************************************************************\n\n");

     
	printf("Please input your choice!\n");
    scanf("%c",&menu);
	switch(menu)                                               //�˵�ѡ��
	{
	case 'Y':
	case 'y':
		start=1;
		system("cls");
		printf("\n*** MODE OF SIMILATE! ***\n***ʱ��Ƭ���̵���***\n");
		break;
    case 'e':
	case 'E':
		system("cls");
	    printf("ʱ��Ƭ���̵���ģ��\n");                                //˵���ļ��ļ���;
		break;
	case 'q':
	case 'Q':
		exit(0);
        break;
	default:
		exit(0);
		break;
	}
}
void main()
{
	int len, h=0;
	char ch;
	Menu();
	if(start==1)
	{
	input();
	len=space();
	while((len!=0)&&(ready!=NULL))                 //�����ж��Ƿ�׸�ԣ�������������������
	{
		ch=getchar();
		h++;
		printf("\n The execute number: %d \n",h);
		//ȡ������������Ľ��� �������ͷ����
		p=ready;
		ready=p->link;
		p->link=NULL;
		p->state='R';
		//////////
		check();
		running();
		printf("\nProcess Any Key to Continue...");
		ch=getchar();
	}
	printf("\n\nAll the processes has finished running");
	ch=getchar();
	}
}
