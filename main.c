#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define ROWS 4
#define COLS 4

// ----- CONFIGURAÇÃO DO KEYPAD -----
int row_pins[ROWS] = {18,19,21,22};
int col_pins[COLS] = {23,5,17,16};

char keys[ROWS][COLS] = {
    {'1','2','3','+'},
    {'4','5','6','-'},
    {'7','8','9','*'},
    {'%','0','=','/'}
};

// Inicializa o keypad
void keypad_init() {
    for(int i=0;i<ROWS;i++){
        gpio_set_direction(row_pins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(row_pins[i], GPIO_PULLUP_ONLY);
    }
    for(int i=0;i<COLS;i++){
        gpio_set_direction(col_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(col_pins[i], 1);
    }
}

// Lê qual tecla foi pressionada no keypad
char keypad_getkey() {
    for(int c=0;c<COLS;c++){
        gpio_set_level(col_pins[c], 0);
        for(int r=0;r<ROWS;r++){
            if(gpio_get_level(row_pins[r]) == 0){
                vTaskDelay(pdMS_TO_TICKS(200)); // debounce
                gpio_set_level(col_pins[c], 1);
                return keys[r][c];
            }
        }
        gpio_set_level(col_pins[c], 1);
    }
    return 0;
}

// ----- FUNÇÃO DE CALCULO -----
float calcular(float num1, float num2, char operador, int *erro) {
    *erro = 0;
    switch(operador) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': 
            if(num2 == 0) {
                *erro = 1;
                return 0;
            }
            return num1 / num2;
        
        case '%': 
            return (num1 * num2) / 100.0;
        
        default:
            *erro = 1;
            return 0;
    }
}

void app_main() {

    keypad_init();

    char input[20] = "";
    float num1=0, num2=0;
    char operador=0;

    printf("Feito por Felipe Moreira ツ\n");
    printf("\n=== CALCULADORA ESP32 HIBRIDA ===\n");
    printf("Use o teclado numérico e digite no AQUI ↓.\n");
    printf("Use 'Enter' para calcular \n");
    printf("Utilize +,-,/ e * \n");


    while(1) {
        // ----- LEITURA DO KEYPAD -----
    char key = keypad_getkey();
    if(key) {
     printf("Tecla: %c\n", key);

    if(key >= '0' && key <= '9') {
    strncat(input, &key, 1);
    }
    else if(key == '+' || key == '-' || key == '*' || key == '/') {
    num1 = atof(input);
    operador = key;
    memset(input,0,sizeof(input));
    }
    else if(key == '=') {
    num2 = atof(input);
      int erro;
        float resultado = calcular(num1, num2, operador, &erro);
          if(erro) {
          printf("Erro na operacao!\n");
          } else {
          printf("Resultado: %.2f\n", resultado);
                }
          memset(input,0,sizeof(input));
            }
            else if(key == 'C') {
                memset(input,0,sizeof(input));
                printf("Limpo\n");
            }
        }

        // ----- LEITURA SERIAL (Enter) -----
        char serialBuffer[50];
        if(fgets(serialBuffer, sizeof(serialBuffer), stdin) != NULL) {
        float sn1, sn2;
         char sop;
          if(sscanf(serialBuffer, "%f %c %f", &sn1, &sop, &sn2) == 3) {
         int erro;
        float resultado = calcular(sn1, sn2, sop, &erro);
        if(erro) {
    printf("Erro na operacao!\n");
  } else {
                    printf("Resultado: %.2f\n", resultado);
                }
            } else {
                printf("Entrada Serial invalida! Use: numero operador numero\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
