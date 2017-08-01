#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <conio.h>
#define getpch(type) (type*)malloc(sizeof(type))
//#define NULL 0
struct pcb{
	char name[10];                 //������
	char state;                    //����״̬��ʾ ������/ִ�У�
	int super;                     //����������  
	int ntime;
	int rtime;                     //��������ִ��ʱ��
	struct pcb* link;              //ָ����һ�����̵�ָ��
}*ready=NULL, *p;
typedef struct pcb PCB;

void sort()                                       //���̰�ָ���㷨����
{
	PCB *first, *second;
	int insert=0;
	if((ready==NULL)||((p->super)>(ready->super)))    //��һ������¼����� ready==NULL readyΪ��ǰ��������������
	{
		p->link=ready;     //�õ�ǰ�����������Ľ����ŵ�ǰ�����������λ
		ready=p;            
	}
	else               
	{
		first=ready;
		second=first->link;
		while(second!=NULL)
		{
			if((p->super)>(second->super))        //�������̴��ڱ�ǵĵڶ������̵�������  ��������
			{
				p->link=second;               
				first->link=p;
				second=NULL;         //��ǰwhileѭ���˳����  ��p �����Ѿ��ɹ��ز������� 
				insert=1;            //p ���̳ɹ���������ı��
			}
			else                     //�ƶ�first �� second ��ʾ��������һ������  �������� ���� 
			{
				first=first->link;
				second=second->link;
			}
		}//end while
		if(insert==0) first->link=p;  //second ��ǰ��¼�Ľ���Ϊ��ʱ ��second ��¼����p
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
		p=getpch(struct pcb);                                   //?????????
		                                                       //������Ϣ¼�� 
		printf("\nInput the name of the process:");
		scanf("%s",p->name);
		printf("\nInput the priority of the process:");
		scanf("%d",&p->super);
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
	printf("\nqname\tstate\tpriority\tndtime\truntime\n");
	printf("|%s\t",pr->name);
	printf("|%c\t",pr->state);
	printf("|%d\t",pr->super);
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
		(p->super)--;
		p->state='w';
		sort();                   //���²�������
	}
}

void main()
{
	int len, h=0;
	char ch;
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
