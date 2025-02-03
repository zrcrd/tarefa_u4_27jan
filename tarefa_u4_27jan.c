//Bibliotecas utilizadas
#include <stdio.h> // Padrão da linguagem C
#include "pico/stdlib.h" //Padrão do raspberry pi pico
#include "hardware/pio.h"
#include "ws2818b.pio.h" //Utilizada para configuração da matriz de leds

// Definindo os pinos dos componentes
#define MATRIZ_WS2818 7  // Pino de NP_CTLRL da matriz de LEDs
#define LEDS_TOTAL 25    // Número total de LEDs na matriz
#define BOTAO_A 5      // Pino do Botão A
#define BOTAO_B 6      // Pino do Botão B
#define PIN_BLUE 12        // Pino do LED azul
#define PIN_GREEN 11         // Pino do LED verde
#define PIN_RED 13         // Pino do LED vermelho

//Variável para controlar o 50 em porcentagem
//uint8_t (8 bits <=> 256 valores)

int EVENTOS = 10; // Número de eventos para alternar entre os botões
int estado_atual = 0; // Estado atual do sistema que será incrementado ou decrementado.

// Tempo mínimo entre interrupções para debouncing
#define DEBOUNCE_DELAY 250 // Milissegundos

// Variáveis para controlar o debouncing
volatile uint32_t last_irq_time_A = 0;
volatile uint32_t last_irq_time_B = 0;

/* Função para converter as posições (x, y) da matriz que será utilizada 
para desenhar os números um índice do vetor de LEDs */
int getIndex(int x, int y) {
    if (x % 2 == 0) {
        return 24 - (x * 5 + y);
    } else {
        return 24 - (x * 5 + (4 - y));
    }
}

//Inicializar os pinos do led RGB
void RGB_INIT() {
  //Inicializa
  gpio_init(PIN_RED);
  gpio_init(PIN_GREEN);
  gpio_init(PIN_BLUE);
  //configura como saída
  gpio_set_dir(PIN_RED, GPIO_OUT);
  gpio_set_dir(PIN_GREEN, GPIO_OUT);
  gpio_set_dir(PIN_BLUE, GPIO_OUT);
}

// Função para acionar leds RGB e configurar o tempo ligado
void LEDS_ESTATIC(bool r, bool g, bool b, int tempo) {
 RGB_INIT(); //Chamando a função RGB_INIT
 gpio_put(PIN_RED, r);
 gpio_put(PIN_GREEN, g);
 gpio_put(PIN_BLUE, b);
 sleep_ms(tempo);
 /* podemos acionar os leds com: LEDS_ESTATIC(r, g, b, tempo);
    Onde r, g, b são números binários que representam nível lógico alto ou baixo
    tempo é o intervalo em milissegundos que os leds ficaram acesos. */
} 

//CONFIGURANDO A MATRIZ DE LEDS com a biblioteca ws2818b equivalente a ws2812b

// Definição da estrutura de np_color para cada LED da matriz
struct pixel_t {
    uint8_t R, G, B;
};
typedef struct pixel_t npLED_t;
npLED_t leds[LEDS_TOTAL];

// PIO e LEDS_ESTATIC machine para NP_CTLRL dos LEDs 
PIO np_pio;
uint sm;

// Função para atualizar os LEDs da matriz
void np_write() {
    for (uint i = 0; i < LEDS_TOTAL; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

// Função de NP_CTLRL inicial da matriz de LEDs com a ws2818b.
void NP_CTLRL(uint pino) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pino, 800000.f);

    for (uint i = 0; i < LEDS_TOTAL; ++i) {
        leds[i].R = leds[i].G = leds[i].B = 0;
    }
    np_write();
}

// Função para configurar a np_color de um LED específico
void np_color(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[indice].R = r;
    leds[indice].G = g;
    leds[indice].B = b;
}

//DESENHANDO CADA NÚMERO
void DESENHO0() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0},{0, 0, 0}},
        {{0, 0, 0}, {50, 0, 0}, {0, 0, 0}, {50, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 0}, {0, 0, 0}, {50, 0, 0},  { 0, 0, 0}},
        {{0, 0, 0}, {50, 0, 0}, {0, 0, 0}, {50, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0},{0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO1() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO2() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO3() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {100, 50, 0}, {100, 50, 0}, {100, 50, 0}, {0, 0, 0  }},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, 50, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, 50, 0}, {100, 50, 0}, {100, 50, 0}, {0, 0,   0}},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, 50, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, 50, 0}, {100, 50, 0}, {100, 50, 0}, {0, 0,   0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO4() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO5() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 50, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 50, 0}, {0, 50, 0}, {0, 50, 0}, {0, 0, 0}}
    
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO6() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 0}, {0, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 50}, {0, 0, 50}, {0, 0, 50}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO7() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 50, 50}, {0, 50, 50}, {0, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 50, 50}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO8() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {50, 0, 50}, {50, 0, 50}, {50, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 50}, {0, 0, 0}, {50, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 50}, {50, 0, 50}, {50, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 50}, {0, 0, 0}, {50, 0, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 0, 50}, {50, 0, 50}, {50, 0, 50}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

void DESENHO9() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {50, 50, 50}, {50, 50, 50}, {50, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 50, 50}, {0, 0, 0}, {50, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 50, 50}, {50, 50, 50}, {50, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {50, 50, 50}, {0, 0, 0}},
        {{0, 0, 0}, {50, 50, 50}, {50, 50, 50}, {50, 50, 50}, {0, 0, 0}}

    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            np_color(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    np_write();
}

// switch para atualizar o estado de cada número
void atualizar_estado() {
    switch (estado_atual) {
        case 0:
            DESENHO0();
            printf("0\n");
            break;
        case 1:
            DESENHO1();
            printf("1\n");
            break;
        case 2:
            DESENHO2();
            printf("2\n");
            break;
        case 3:
            DESENHO3();
            printf("3\n");
            break;
        case 4:
            DESENHO4();
            printf("4\n");
            break;
        case 5:
            DESENHO5();
            printf("5\n");
            break;
        case 6:
            DESENHO6();
            printf("6\n");
            break;
        case 7:
            DESENHO7();
            printf("7\n");
            break;
        case 8:
            DESENHO8();
            printf("8\n");
            break;
        case 9:
            DESENHO9();
            printf("9\n");
            break;
    }
}

// Aplicando as rotinhas de interrupção IRQ para np_colorrigir o debounce.
/* Aproveitando a função para criar uma estrutura condicional para incrementar e
   decrementar um número da função ATUALIZAR_ESTADO lendo o estado atual sempre que
   um dos botões forem pressionados. */
void debounce_botao(uint pino, volatile uint32_t *last_irq_time, int direcao) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - *last_irq_time > DEBOUNCE_DELAY) {
        *last_irq_time = tempo_atual;
        estado_atual += direcao;
        if (estado_atual >= EVENTOS) estado_atual = 0;
        if (estado_atual < 0) estado_atual = EVENTOS - 1;
        atualizar_estado();
    }
}

// Função para inicializar os pinos dos botões como entrada IN;
void BUTTON_INIT() {
    gpio_init(BOTAO_A);
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_pull_up(BOTAO_B);
}
// Função principal
int main() {
    stdio_init_all();
    NP_CTLRL(MATRIZ_WS2818); //NP_CTLRL do pino 7
    BUTTON_INIT(); // Permite a interação com os botões
    printf("0\n");
    DESENHO0(); //Define o estado inicial da matriz de leds como o número 0

    while (1) {

        // Ativa a interrupção do botão B e decrementa um estado.
        if (!gpio_get(BOTAO_B)) debounce_botao(BOTAO_B, &last_irq_time_B, -1);
        //Ativa a interrupção do botão A e incrementa um estado.
        if (!gpio_get(BOTAO_A)) debounce_botao(BOTAO_A, &last_irq_time_A, +1);
        sleep_ms(10); // Aguarda 10 ms para a próxima leitura.

        /* O próximo ciclo será sempre depois de 210 ms devido ao led ficar 180 ms ligado
        10 ms desligado e o aguardo de 10 ms para o próximo ciclo*/

        LEDS_ESTATIC(1, 0, 0, 100); // O led vermelho vermelho tipo RGB fica 100 ms ligado
        LEDS_ESTATIC(0, 0, 0, 100);  // Todos os leds ficam 100 ms desligados
    }
    return (0); //Este ponto nunca será alcançado.
}