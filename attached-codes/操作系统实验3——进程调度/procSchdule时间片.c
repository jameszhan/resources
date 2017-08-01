#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <conio.h>
#define getpch(type) (type*)malloc(sizeof(type))
//#define NULL 0
struct pcb{
	char name[10];                 //进程名
	char state;                    //进程状态标示 （就绪/执行） 
	int ntime;                     //进程所需执行时间
	int rtime;                     //用时间片总时间                   
	struct pcb* link;              //指向下一个进程的指针
}*ready=NULL, *p;
typedef struct pcb PCB;
int start;                                        //菜单控制标志
void sort()                                       //进程按指定算法链接
{
	PCB *end, *recent;
	int inlist=0;                             //表示进程已经插到链表最后
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

void input()                                                    //进程输入函数
{
	int i,num;
	
	printf("\nPlease input the number of processes:");
	scanf("%d",&num);                                          //定义进程个数 num
	for(i=0;i<num;i++)
	{
		printf("\nProcess NO.%d:\n",i);
		p=getpch(struct pcb);                                 
		                                                       //进程信息录入 
		printf("\nInput the name of the process:");
		scanf("%s",p->name);
		printf("\nInput the running time of the process:");
		scanf("%d",&p->ntime);
		printf("\n");                                         
		                                                        
		p->rtime=0;          //进程占有cpu的时间
		p->state='w';        //进程就绪状态  
		p->link=NULL;
	    sort();              //每输入一个进程链接一次  共调用了 num 次
	}
}

int space()                                 //统计链表中的进程数
{
	int l=0;
	PCB *pr=ready;
	while(pr!=NULL)
	{
		l++;                    //用于记录链表中的进程数
		pr=pr->link;
	}
	return(l);
}

void disp(PCB *pr)                             //打印显示当前正在执行的进程的相关信息
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
	pr=ready;                                                  //记住表头进程
	printf("\n *********The state of the Waiting List:\n");
	while(pr!=NULL)                                            //打印出剩余的所有进程的相关信息
	{
		disp(pr);
		pr=pr->link;
	}
	if(pr==NULL)
		printf("\nNo waiting process!\n");
}

void destroy()                                        //打印进程执行信息提示
{
	printf("\nProcess [%s] has finished.\n",p->name);
	free(p);                                          //释放临时变量p的内存空间   
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
		sort();                   //重新插入链表
	}
}
 /*******************************************************************菜单函数********/
void Menu()
{
  /*******************          菜单选项      *****************************************/   
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
	switch(menu)                                               //菜单选项
	{
	case 'Y':
	case 'y':
		start=1;
		system("cls");
		printf("\n*** MODE OF SIMILATE! ***\n***时间片进程调度***\n");
		break;
    case 'e':
	case 'E':
		system("cls");
	    printf("时间片进程调度模拟\n");                                //说明文件的加载;
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
	while((len!=0)&&(ready!=NULL))                 //条件判断是否赘言？？？？？？？？？？
	{
		ch=getchar();
		h++;
		printf("\n The execute number: %d \n",h);
		//取出最大优先数的进程 即链表表头进程
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
