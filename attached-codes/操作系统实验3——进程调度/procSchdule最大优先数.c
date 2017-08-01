#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <conio.h>
#define getpch(type) (type*)malloc(sizeof(type))
//#define NULL 0
struct pcb{
	char name[10];                 //进程名
	char state;                    //进程状态标示 （就绪/执行）
	int super;                     //进程优先数  
	int ntime;
	int rtime;                     //进程所需执行时间
	struct pcb* link;              //指向下一个进程的指针
}*ready=NULL, *p;
typedef struct pcb PCB;

void sort()                                       //进程按指定算法链接
{
	PCB *first, *second;
	int insert=0;
	if((ready==NULL)||((p->super)>(ready->super)))    //第一个进程录入入口 ready==NULL ready为当前活动最大优先数进程
	{
		p->link=ready;     //让当前优先数大最大的进程排当前进程链表的首位
		ready=p;            
	}
	else               
	{
		first=ready;
		second=first->link;
		while(second!=NULL)
		{
			if((p->super)>(second->super))        //后续进程大于标记的第二个进程的优先数  插入链表
			{
				p->link=second;               
				first->link=p;
				second=NULL;         //当前while循环退出标记  （p 进程已经成功地插入链表） 
				insert=1;            //p 进程成功插入链表的标记
			}
			else                     //移动first 和 second 表示的链表下一个进程  继续查找 插入 
			{
				first=first->link;
				second=second->link;
			}
		}//end while
		if(insert==0) first->link=p;  //second 当前记录的进程为空时 用second 记录进程p
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
		p=getpch(struct pcb);                                   //?????????
		                                                       //进程信息录入 
		printf("\nInput the name of the process:");
		scanf("%s",p->name);
		printf("\nInput the priority of the process:");
		scanf("%d",&p->super);
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
	pr=ready;                                                  //记住表头进程
	printf("\n *********The state of the Waiting List:\n");
	while(pr!=NULL)                                            //打印出剩余的所有进程的相关信息
	{
		disp(pr);
		pr=pr->link;
	}
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
		(p->super)--;
		p->state='w';
		sort();                   //重新插入链表
	}
}

void main()
{
	int len, h=0;
	char ch;
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
