#include <stdio.h>

#pragma import(__use_no_semihosting)


//In arch_serial.c
extern void sendchar(char *ch);
extern int getkey(void);


FILE __stdout;
FILE __stdin;


int fputc(int ch, FILE *f)
{

	sendchar((char *)&ch);
	return ch;
}

int fgetc(FILE *f)
{
	int ch;
	
	ch = getkey();
	sendchar((char *)&ch);
	return ch;
}


int ferror(FILE *f)
{

	/* Your implementation of ferror */
	return EOF;
}


void _ttywrch(int ch)
{

	sendchar((char *)&ch);
}


void _sys_exit(int return_code)
{

	while (1);    /* endless loop */
}



