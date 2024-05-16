#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
    for (int i = 0; ; i++)
    {
        if (i%2==0)
	        printf("Hello, kernel World!\n");
        else
            printf("HELLO AGAIN!\n");
    }
}
