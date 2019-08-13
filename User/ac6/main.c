/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : minilzo压缩算法的移植
*              实验目的：
*                1. 学习minilzo压缩算法的移植。
*              实验内容：
*                1. 数据压缩和解压测试，请下载程序并重新给板子上电，上电后自动打印。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-08-12   Eric2013     1. CMSIS软包版本 V5.6.0
*                                         2. HAL库版本 V1.3.0
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"			/* 底层硬件驱动 */
#include "minilzo.h"


/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V7-MiniLZO无损压缩算法移植和使用"
#define EXAMPLE_DATE	"2019-08-12"
#define DEMO_VER		"1.0"


static void PrintfLogo(void);
static void PrintfHelp(void);
static uint8_t TestMiniLZO(uint32_t _Len);


/* 定义测试用的输入输出缓冲和大小 */
#define IN_LEN      (20*1024ul)
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)
static unsigned char __LZO_MMODEL in  [ IN_LEN ];
static unsigned char __LZO_MMODEL out [ OUT_LEN ];

/* 压缩需要内存大小 */
#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);
	
uint8_t  InTempData[IN_LEN]__attribute__((section(".ARM.__at_0x24000000")));;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参: 无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;		/* 按键代码 */

	
	bsp_Init();		/* 硬件初始化 */
	
	PrintfLogo();	/* 打印例程名称和版本等信息 */
	PrintfHelp();	/* 打印操作提示 */

	bsp_StartAutoTimer(0, 100);	/* 启动1个100ms的自动重装的定时器 */
	
	
	/* 初始化 LZO */
	if (lzo_init() != LZO_E_OK)
	{
		printf("LZO初始化失败 \r\n");
		printf("一般是编译造成的操作，可以重新编译，并设置不做任何优化，同时使能-DLZO_DEBUG进行调试\r\n");
	}

	/* 测试数据的压缩和解压 */
	TestMiniLZO(1024*1);
	TestMiniLZO(1024*2); 
	TestMiniLZO(1024*3);
	TestMiniLZO(1024*4);  
	TestMiniLZO(1024*5);
	TestMiniLZO(1024*6);  
	TestMiniLZO(1024*7);
	TestMiniLZO(1024*8);
	TestMiniLZO(1024*9);
	TestMiniLZO(1024*10); 
	TestMiniLZO(1024*11);
	TestMiniLZO(1024*12); 
	TestMiniLZO(1024*13);
	TestMiniLZO(1024*14);  
	TestMiniLZO(1024*15);
	TestMiniLZO(1024*16);  
	TestMiniLZO(1024*17);
	TestMiniLZO(1024*18);
	TestMiniLZO(1024*19);
	TestMiniLZO(1024*20); 	
	
	/* 主程序大循环 */
	while (1)
	{
		/* CPU空闲时执行的函数，在 bsp.c */
		bsp_Idle();		
		
		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			/* 每隔100ms 进来一次 */
			/* 翻转LED2的状态 */
			bsp_LedToggle(2);	
		}

		/* 处理按键事件 */
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode > 0)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:			   /* K1键按下 */
					break;

				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: TestMiniLZO
*	功能说明: _Len 要压缩的数据量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t TestMiniLZO(uint32_t _Len)
{
	uint32_t i;
	int r;
	lzo_uint in_len;
	lzo_uint out_len;
	lzo_uint new_len;
	uint32_t start, end, cnt;

	if(_Len > IN_LEN)
	{
		printf("函数参数_Len超出允许的最大值%ld\r\n", IN_LEN);
	}
	
	/* 准备要压缩的数据 */
	in_len = _Len;
	
	
	lzo_memset(in,0,in_len);
	
	for(i=0; i< in_len; i++)
	{
		in[i] = rand()%256;
		InTempData[i] = in[i];
	}

	printf("-----------------------------------\r\n");
	printf("-----------------------------------\r\n");
	
	/* 将in数组中的数据压缩到out数组 */
	start = DWT_CYCCNT;
	r = lzo1x_1_compress(in,in_len,out,&out_len,wrkmem);
	end = DWT_CYCCNT;
	cnt = end - start;
	
	if (r == LZO_E_OK)
		printf("压缩 %lu 字节为 %lu 字节，耗时%dus\r\n",
		(unsigned long) in_len, (unsigned long) out_len, cnt/400);
	else
	{
		printf("内部错误 - 压缩失败: %d\r\n", r);
		return 2;
	}
	
	/* 检查是否有位解压的数据 */
	if (out_len >= in_len)
	{
		printf(" 有未解压的数据 \r\n");
		return 0;
	}

	/* 将压缩后的数组out，解压到in数组 */
	start = DWT_CYCCNT;
	r = lzo1x_decompress(out,out_len,in,&new_len,NULL);
	end = DWT_CYCCNT;
	cnt = end - start;
	if (r == LZO_E_OK && new_len == in_len)
		printf("解压 %lu 字节为 %lu 字节，耗时%dus\r\n",
		(unsigned long) out_len, (unsigned long) in_len, cnt/400);
	else
	{
		printf("内部错误 - 压缩失败: %d\r\n", r);
		return 1;
	}
	
	/* 校验压缩是否成功 */
	for(i = 0; i < in_len; i++)
	{
		if(InTempData[i] != in[i])
		{
			printf("测试压缩失败\r\n");
		}
	}
	
	return 0;
}


/*
*********************************************************************************************************
*	函 数 名: PrintfHelp
*	功能说明: 打印操作提示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
	printf("操作提示:\r\n");
	printf("1. 数据压缩和解压测试，请下载程序并重新给板子上电，上电后自动打印\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	
	/* 检测CPU ID */
	{
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;
		
		CPU_Sn0 = *(__IO uint32_t*)(0x1FF1E800);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FF1E800 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FF1E800 + 8);

		printf("\r\nCPU : STM32H743XIH6, BGA240, 主频: %dMHz\r\n", SystemCoreClock / 1000000);
		printf("UID = %08X %08X %08X\n\r", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST的HAL库版本 */
	printf("* HAL库版本  : V1.3.0 (STM32H7xx HAL Driver)\r\n");
	printf("* \r\n");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* 旺旺  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* 微信公众号: armfly_com \r\n");
	printf("* 淘宝店: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
