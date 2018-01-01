#include <stdio.h>
#include "libad9851.h"
 
int main(void)
{
    puts("This is a shared library test...");
    //setup();
    setupWithParameters(180000000UL, 70000000UL, 25, 28, 29);
		sendFrequency(10000000);

    return 0;
}