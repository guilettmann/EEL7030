#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_task_wdt.h"
#include "driver/gptimer.h"

char rx_buffer_uart[256] = {0};
char tx_buffer_uart[] = "BRUNO OTARIO";

void inicia_uart(void );
gptimer_handle_t inicia_temporizador(void );
bool IRAM_ATTR funcao_tratamento_alarme(gptimer_handle_t temporizador, const gptimer_alarm_event_data_t *edata, void *user_ctx);

void app_main(void ){
	esp_task_wdt_deinit();
	inicia_uart();

	// Inicia o temporizador e recebe a estrutura de manipulação
	gptimer_handle_t temporizador = inicia_temporizador();

	while(1){
		// Complemente o código com os exercícios propostos
		int tamanho_recebido = uart_read_bytes(UART_NUM_1, rx_buffer_uart, 256, 20/portTICK_PERIOD_MS);

		if (tamanho_recebido > 0) {
			rx_buffer_uart[tamanho_recebido] = '\0'; // Termina a string recebida com o caractere de parada

			printf ("Mensagem recebida: ");
			printf (rx_buffer_uart); // Escreve no console a mensagem recebida
			printf ("\n"); // Quebra de linha (enter)

			memset(rx_buffer_uart, 0, 256); // Limpa o buffer
		}
	}
}

bool funcao_tratamento_alarme(gptimer_handle_t temporizador, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
	// Tratamento da interrupção disparada pelo alarme
	uart_write_bytes(UART_NUM_1, tx_buffer_uart, strlen(tx_buffer_uart)); // envia a cada 1 seg
	return true; // Indica que o evento foi tratado
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

    gptimer_alarm_config_t config_alarme = {
        .alarm_count = 1000000,	// 1 segundo (em número de ticks)
        .reload_count = 0, // Reinicia do zero
        .flags.auto_reload_on_alarm = true, // Reinicia o temporizador automaticamente após o alarme
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(temporizador, &config_alarme));

    gptimer_event_callbacks_t config_callback = {
        .on_alarm = funcao_tratamento_alarme,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(temporizador, &config_callback, NULL));

    // Habilita e inicia o temporizador
    ESP_ERROR_CHECK(gptimer_enable(temporizador)); // Usamos a estrutura de retorno como parâmetros de entrada nas funções de manipulação
    ESP_ERROR_CHECK(gptimer_start(temporizador));

    return temporizador; // Retorna a estrutura para manipulação do temporizador
}

void inicia_uart(void ){
// Cria a estrutura para configuração do UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_NUM_1, &uart_config); // Atribui as configurações
    uart_set_pin(UART_NUM_1, 8, 10, -1, -1); //Configura os pinos que serão utilizados pelo UART
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0); // Inicializa o driver UART
}




