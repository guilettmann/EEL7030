#define GPIOEnableOut 0x60004020
#define GPIOSet 0x60004008
#define GPIOReset 0x6000400C

#define pin0 (0x1)
#define pin1 (0x1 << 1)
#define pin2 (0x1 << 2)
#define pin3 (0x1 << 3)

void delay(int ms)
{
   for (volatile int i = 0; i < ms * 1000; i++);
}

struct RegsGPIO 
{
    unsigned long int *outEnable; 
    unsigned long int *set;        
    unsigned long int *reset;      
};

struct RegsGPIO GPIO = // inicialização da struct
{
    (unsigned long int *)GPIOEnableOut, // (unsigned long int *)GPIOEnableOut: Este é o endereço do registrador que habilita os pinos GPIO. O cast (unsigned long int *) garante que o compilador trate o endereço como um ponteiro para unsigned long int.
    (unsigned long int *)GPIOSet,
    (unsigned long int *)GPIOReset
};

void app_main()
{
    int n = 0;
    *GPIO.outEnable = 0xF; // asterisco muda o valor de onde o ponteiro aponta
    *GPIO.set = 0xF;

    while (1) 
    {
        *GPIO.set = 0xF;
        delay(1000);
        *GPIO.reset = n;
        delay(1000);

        if (n < 15) 
        {
            n++;
        } else 
        {
            n = 0;
        }
    }
}