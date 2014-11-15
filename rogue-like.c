#include <avr/io.h>
#include <avr/wdt.h>

int main(void){


    for(;;){
        wdt_reset();
    }

    return 0;
}

