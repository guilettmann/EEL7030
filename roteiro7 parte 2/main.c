#include <stdio.h>
#include "driver/gptimer.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "soc/gpio_reg.h"

uint8_t sec = 0, min = 0;
bool apertado = false;
int n = 3;

gptimer_handle_t inicia_temporizador(void );
uint8_t converte_7seg(int dado, int ponto);
void delay(int ms);
void escreve_display(uint8_t display, uint8_t valor_codificado);
bool IRAM_ATTR funcao_tratamento_alarme(gptimer_handle_t temporizador, const gptimer_alarm_event_data_t *edata, void *user_ctx);

static void botao_apertado(void* arg){
	apertado=true;
	n++;
}

void app_main() {
	  gpio_config_t io_conf;
	  io_conf.intr_type = GPIO_INTR_NEGEDGE;
	  io_conf.pin_bit_mask = (1 << 10);
	  io_conf.mode = GPIO_MODE_INPUT;
	  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

	  gpio_config(&io_conf);
	  gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
	  gpio_isr_handler_add(10, botao_apertado, NULL);

    esp_task_wdt_deinit(); // Desativa o watchdog de idle

    // Inicia o temporizador e recebe a estrutura de manipulação
    gptimer_handle_t temporizador = inicia_temporizador();

    gptimer_stop(temporizador);

    for (int i = 0; i <= 9; i++){
    	gpio_reset_pin(i);
    }

    REG_WRITE(GPIO_ENABLE_REG, 0x3FF);
    REG_WRITE(GPIO_OUT_REG, 0x3FF);

    while (1) {
    	if(sec>=60){
    		min++;
    		sec=0;
    	}

   	if(apertado){
   		if(n>5){
   			n=3;
   		}
   		if((n%2)!=0){
   			gptimer_start(temporizador);
   		}
   		else{
   			gptimer_stop(temporizador);
   		}

   		apertado = false;
   	}
       escreve_display(0, converte_7seg(sec%10, 0));
       escreve_display(1, converte_7seg(sec/10, 0));
       escreve_display(2, converte_7seg(min%10, 1));
       escreve_display(3, converte_7seg(min/10, 0));
    }
}

bool funcao_tratamento_alarme(gptimer_handle_t temporizador, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
  // Tratamento da interrupção disparada pelo alarme
	sec++;
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
