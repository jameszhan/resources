
#ifndef TOKEN
#define TOKEN
/* �ս������  */
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
#define $num	38		/*	����	*/
#define $id		39		/*	����	*/
#define $eps	40		/*  ��		*/
#define $sharp	41		/*  #		*/

/* ���ս������*/		/*	�߼����	˵��		*/
#define _PR		100		/*	<PR>		��������	*/
#define _DSS	101		/*	<DSS>		˵����伯	*/
#define _DS		102		/*	<DS>		˵�����	*/
#define _ESS	103		/*	<ESS>		��ִ����伯*/
#define	_ES		104		/*	<ES>		��ִ�����	*/
#define _IDL	105		/*	<IDL>		�����б�	*/
#define	_IFS	106		/*	<IFS>		IF�ṹ		*/
#define	_WS		107		/*	<WS>		While�ṹ	*/
#define _AS		108		/*	<AS>		��ֵ���	*/
#define _IS		109		/*	<IS>		�������	*/
#define _OS		110		/*	<OS>		������	*/

#define _RE		111		/*	<RE>		��ϵ���ʽ	*/
#define _ROp	112		/*	<RE>		��ϵ�����	*/

#define _E		113		/*	<E>			�������ʽ	*/
#define _E1		114		/*	<E'>		�����ӱ��ʽ*/
#define _T		115		/*	<T>			��������	*/
#define _T1		116		/*	<T'>		�������ӱ�	*/
#define _F		117		/*	<F>			����������	*/

/*���±�ʾ���ֲ�ͬ���ȼ������*/
#define _Op1	118		/*	<Op1>		���������1	*/
#define _Op2	119		/*	<Op2>		���������2	*/

/*���ֺͱ�ʶ���ڴʷ������׶��Ѿ����壬�����Ϊ�ս������*/

/*����Ϊȫ�ֱ�������*/
#define MAXTOKEN  100
int tokens[MAXTOKEN];	/*���ʣ�������100��		*/
int attribs[MAXTOKEN];	/*��������				*/
int ip=0;					/*��ǰ����(����)ָ��	*/

#endif