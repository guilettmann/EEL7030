#define GPIO_ENABLE_REG 0x60004020
#define GPIO_OUT_REG 0x60004004 // define diretamente os valores de saída dos pinos correspondentes, sem necessidade de set e clear.
#define GPIO_OUTPUT_W1TS_REG 0x60004008
#define GPIO_OUTPUT_W1TC_REG 0x6000400C
#define IO_MUX_GPIOn_REG(n) (0x60009004 + 4 * n) // Como iremos realizar diversas manipulações de registradores, e essas seguem a mesma estrutura (definição de um ponteiro para o endereço desejado, seguida da atualização do valor apontado por esse endereço), é vantajoso definir uma função que realize esses passos.
#define GPIO_FUNCn_OUT_SEL_CFG_REG(n) (0x60004554 + 4 * n)
#define valor 15

void REG_WRITE(unsigned int address, unsigned int value);
void IO_MUX_set_as_GPIO(int ports[], int length);
void delay(int ms);
unsigned char converte_7seg(int dado, int ponto);

void app_main(void) {
    int portas_io[] = {0, 1, 2, 3, 4, 5, 6, 7};
    int tamanho = sizeof(portas_io) / sizeof(portas_io[0]); // tamanho de array
    IO_MUX_set_as_GPIO(portas_io, tamanho);

    REG_WRITE(GPIO_ENABLE_REG, 0xFF);
    REG_WRITE(GPIO_OUT_REG, 0xFF);

    int n = 0;

    while (1) {
        REG_WRITE(GPIO_OUT_REG, converte_7seg(n, n % 2));
        delay(10000);

        if (n >= 15) {
            n = 0;
        } else {
            n++;
        }
    }
}

unsigned char converte_7seg(int dado, int ponto) {
    switch (dado) {
        case 0: dado = 0x40; break;
        case 1: dado = 0x79; break;
        case 2: dado = 0x24; break;
        case 3: dado = 0x30; break;
        case 4: dado = 0x19; break;
        case 5: dado = 0x12; break;
        case 6: dado = 0x02; break;
        case 7: dado = 0x78; break;
        case 8: dado = 0x00; break;
        case 9: dado = 0x10; break;
        case 10: dado = 0x08; break;
        case 11: dado = 0x03; break;
        case 12: dado = 0x46; break;
        case 13: dado = 0x21; break;
        case 14: dado = 0x06; break;
        case 15: dado = 0x0E; break;
        default: dado = 0x00;
    }

    if (ponto == 0) {
        dado += 0x80;
    }

    return dado;
}

void delay(int ms) {
    for (volatile int i = 0; i < ms * 100; i++);
}

void REG_WRITE(unsigned int address, unsigned int value) {
    unsigned int *ptr_address = (unsigned int *)address;
    *ptr_address = value;
}

void IO_MUX_set_as_GPIO(int ports[], int length) { // multiplexador para setar pinos que não são GPIO diretamente
    for (int i = 0; i < length; i++) {
        int n = ports[i];
        REG_WRITE(IO_MUX_GPIOn_REG(n), 0x1800); // function 1 20 mA
        REG_WRITE(GPIO_FUNCn_OUT_SEL_CFG_REG(n), 0x280); // internos desligados
    }
}
