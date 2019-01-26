#ifndef __PRINT_HEX_H__
#define __PRINT_HEX_H__

#define LINE_LEN 16
#define INFO_HEX_SPACE 6
#define INFO_BEGIN_POS (LINE_LEN * 3 + INFO_HEX_SPACE)

#define PRINTF_YELLOW(fmt,args...)\
        printf("\033[33m"fmt"\033[0m\n", ##args);

#define PRINTF_BLUE(fmt,args...)\
        printf("\033[34m"fmt"\033[0m\n", ##args);

#define PRINTF_GREY(fmt,args...)\
        printf("\033[90m"fmt"\033[0m\n", ##args);

#define PRINTF_GREEN(fmt,args...)\
        printf("\033[32m"fmt"\033[0m\n", ##args);

#define PRINTF_RED(fmt,args...)\
        printf("\033[31m"fmt"\033[0m\n", ##args);

#define PRINTF_PINK(fmt,args...)\
        printf("\033[35m"fmt"\033[0m\n", ##args);

//打印空格，直至信息开始的地方　
void goto_infobegin(int line_count)
{
	int i = 0;
	if(line_count == LINE_LEN)
	{
		while (i < INFO_HEX_SPACE)
		{
			i++;
			printf( " " );
		}
	}
	else if(line_count < LINE_LEN)
	{
		while (i < (INFO_HEX_SPACE + (LINE_LEN-line_count) * 3)) 
		{
			i++;
			printf( " " );
		}
	}
}


//将数据包用16进制打印
int show_hex_format( unsigned char *pData, unsigned int len)
{
	PRINTF_PINK("full packet :");
	for(unsigned int i = 0; i < len; ++i )
	{
		if( i % LINE_LEN == 0 )
		{
			goto_infobegin(LINE_LEN);
			for(unsigned int k = i-LINE_LEN; k < i && i != 0 ;k ++)
			{
				if( pData[k] >= 0x20 && pData[k] <= 0x7F)
				{
					printf( "%c",pData[k]);
				}
				else
				{
					printf( "." );
				}
			}
			printf( "\n" );
		}
		printf( "%02X ", pData[i]);
		if(i == len - 1)
		{
			goto_infobegin(len % LINE_LEN);
			for(unsigned int k = LINE_LEN * (len/LINE_LEN); k < len ;k ++)
			{
				if( pData[k] >= 0x20 && pData[k] <= 0x7F)
				{
					printf( "%c",pData[k]);
				}
				else
				{
					printf( "." );
				}
			}
			printf( "\n" );
		}
	}

	printf("\n\n");

	return 0;
}

void show_hex(unsigned char *pData, unsigned int len)
{
	PRINTF_BLUE("appinfo     :");
	for(unsigned int i = 0; i < len; i++ )
        {
		if(i%35==0 && i != 0)
			printf("\n");
		printf( "%02X", pData[i]);
	}

	printf("\n\n");
}
#endif
