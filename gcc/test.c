#include "uart.h"

int main(void)
{
	char c;
	int num = 3;
	char d = "Hi";
	while (1)
	{
		c = uart_read_blocking();
		uart_write_blocking(c);
		uart_write_blocking(d);
	}
	return 0;
}
