#include <stdio.h>
#include "driver/gptimer.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "soc/gpio_reg.h"

gptimer_handle_t inicia_temporizador(void );
uint8_t converte_7seg(int dado, int ponto);
void delay(int ms);
void escreve_display(uint8_t display, uint8_t valor_codificado);

void app_main() {
    esp_task_wdt_deinit(); // Desativa o watchdog de idle

    // Inicia o temporizador e recebe a estrutura de manipulação
    gptimer_handle_t temporizador = inicia_temporizador();

    uint64_t contagem_atual = 0; // Variável para armazenar a última leitura do contador
    uint64_t n = 1;
    uint8_t frac = 0, sec = 0, min = 0;

    for (int i = 0; i <= 10; i++){
    	gpio_reset_pin(i);
    }

    REG_WRITE(GPIO_ENABLE_REG, 0x7FF);
    REG_WRITE(GPIO_OUT_REG, 0x7FF);

    while (1) {
        gptimer_get_raw_count(temporizador, &contagem_atual); // Captura o valor atual do contador e salva na variável contagem_atual

        if(contagem_atual >= n*10000){
        	frac++;
        	n++;
        }
        if (frac >= 100){
        	sec++;
        	frac = 0;
        }
       if (sec >= 60){
    	   min++;
    	   sec = 0;
       }

       escreve_display(0, converte_7seg(frac%10, 0));
       escreve_display(1, converte_7seg(frac/10, 0));
       escreve_display(2, converte_7seg(sec%10, 1));
       escreve_display(3, converte_7seg(sec/10, 0));
       escreve_display(4, converte_7seg(min%10, 1));
       escreve_display(5, converte_7seg(min/10, 0));
    }
}


gptimer_handle_t inicia_temporizador(void ) {
    gptimer_handle_t temporizador = NULL; // Inicializa a estrutura de retorno

	// Inicializa a estrutura de configuração
    gptimer_config_t config_temporizador= {
        .clk_src = GPTIMER_CLK_SRC_APB, // Fonte de clock APB
        .direction = GPTIMER_COUNT_UP, // Contagem crescente
        .resolution_hz = 1000000, // Resolução em 1 MHz
	};

  	// Cria e inicializa o temporizador
    ESP_ERROR_CHECK(gptimer_new_timer(&config_temporizador, &temporizador));

    // Habilita e inicia o temporizador
    ESP_ERROR_CHECK(gptimer_enable(temporizador)); // Usamos a estrutura de retorno como parâmetros de entrada nas funções de manipulação
    ESP_ERROR_CHECK(gptimer_start(temporizador));

    return temporizador; // Retorna a estrutura para manipulação do temporizador
}

uint8_t converte_7seg(int dado, int ponto) {
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

void escreve_display(uint8_t display, uint8_t valor_codificado){
	REG_WRITE(GPIO_OUT_REG, (display << 8 | valor_codificado));
	delay(2);
}
