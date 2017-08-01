
#ifndef TOKEN
#define TOKEN
/* 终结符定义  */
#define $if		1		/*	if		*/
#define $else	2		/*	else	*/
#define $while	3		/*	while	*/
#define $cout	4		/*	cout	*/
#define $cin	5		/*	cin		*/
#define	$input	6		/*	>>		*/
#define $output 7		/*	<<		*/
#define $var	8		/*  var		*/

#define $add	20		/*	+		*/
#define $sub	21		/*	-		*/
#define $mul	22		/*	*		*/
#define $div	23		/*	/		*/
#define $pwd	24		/*	**		*/
#define $eq		25		/*	==		*/
#define $lt		26		/*	<		*/
#define	$gt		27		/*	>		*/
#define $le		28		/*	<=		*/
#define $ge		29		/*	>=		*/
#define $neq	30		/*	!=		*/
#define $llbr	31		/*	(		*/
#define $lrbr	32		/*	)		*/
#define $blbr	33		/*	{		*/
#define $brbr	34		/*	}		*/

#define $sem	35		/*	;		*/
#define $comma	36		/*	,		*/
#define $assign	37		/*	=		*/
#define $num	38		/*	常数	*/
#define $id		39		/*	变量	*/
#define $eps	40		/*  ε		*/
#define $sharp	41		/*  #		*/

/* 非终结符定义*/		/*	逻辑表达	说明		*/
#define _PR		100		/*	<PR>		整个程序	*/
#define _DSS	101		/*	<DSS>		说明语句集	*/
#define _DS		102		/*	<DS>		说明语句	*/
#define _ESS	103		/*	<ESS>		可执行语句集*/
#define	_ES		104		/*	<ES>		可执行语句	*/
#define _IDL	105		/*	<IDL>		变量列表	*/
#define	_IFS	106		/*	<IFS>		IF结构		*/
#define	_WS		107		/*	<WS>		While结构	*/
#define _AS		108		/*	<AS>		赋值语句	*/
#define _IS		109		/*	<IS>		输入语句	*/
#define _OS		110		/*	<OS>		输出语句	*/

#define _RE		111		/*	<RE>		关系表达式	*/
#define _ROp	112		/*	<RE>		关系运算符	*/

#define _E		113		/*	<E>			算术表达式	*/
#define _E1		114		/*	<E'>		算术子表达式*/
#define _T		115		/*	<T>			算术因子	*/
#define _T1		116		/*	<T'>		算术因子表	*/
#define _F		117		/*	<F>			算术项因子	*/

/*以下表示两种不同优先级的算符*/
#define _Op1	118		/*	<Op1>		算术运算符1	*/
#define _Op2	119		/*	<Op2>		算术运算符2	*/

/*数字和标识符在词法分析阶段已经定义，因而作为终结符处理*/

/*以下为全局变量定义*/
#define MAXTOKEN  100
int tokens[MAXTOKEN];	/*单词，可容纳100个		*/
int attribs[MAXTOKEN];	/*单词属性				*/
int ip=0;					/*当前符号(单词)指针	*/

#endif