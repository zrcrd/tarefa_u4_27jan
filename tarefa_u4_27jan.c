
//Bibliotecas utilizadas
#include <stdio.h> // Padrão da linguagem C
#include "pico/stdlib.h" //Padrão do raspberry pi pico
#include "hardware/pio.h"
#include "ws2818b.pio.h" //Utilizada para configuração da matriz de leds

// Definindo os pinos dos componentes
#define PINO_MATRIZ 7  // Pino de controle da matriz de LEDs
#define NUM_LEDS 25    // Número total de LEDs na matriz
#define BOTAO_A 5      // Pino do Botão A
#define BOTAO_B 6      // Pino do Botão B
#define BLUE 12        // Pino do LED azul
#define GREEN 11         // Pino do LED verde
#define RED 13         // Pino do LED vermelho


//Variável para controlar o brilho em porcentagem
//uint8_t (8 bits <=> 256 valores)
int BRILHO = 255;

int num_eventos = 10; // Número de eventos para alternar entre os botões
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
void iniciar_rgb() {
  //Inicializa
  gpio_init(RED);
  gpio_init(GREEN);
  gpio_init(BLUE);
  //configura como saída
  gpio_set_dir(RED, GPIO_OUT);
  gpio_set_dir(GREEN, GPIO_OUT);
  gpio_set_dir(BLUE, GPIO_OUT);
}

// Função para acionar leds RGB e configurar o tempo ligado
void state(bool r, bool g, bool b, int tempo) {
 iniciar_rgb(); //Chamando a função iniciar_RGB
 gpio_put(RED, r);
 gpio_put(GREEN, g);
 gpio_put(BLUE, b);
 sleep_ms(tempo);
 /* podemos acionar os leds com: state(r, g, b, tempo);
    Onde r, g, b são números binários que representam nível lógico alto ou baixo
    tempo é o intervalo em milissegundos que os leds ficaram acesos. */
} 

//CONFIGURANDO A MATRIZ DE LEDS com a biblioteca ws2818b equivalente a ws2812b

// Definição da estrutura de cor para cada LED da matriz
struct pixel_t {
    uint8_t R, G, B;
};
typedef struct pixel_t npLED_t;
npLED_t leds[NUM_LEDS];

// PIO e state machine para controle dos LEDs 
PIO np_pio;
uint sm;

// Função para atualizar os LEDs da matriz
void bf() {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

// Função de controle inicial da matriz de LEDs com a ws2818b.
void controle(uint pino) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pino, 800000.f);

    for (uint i = 0; i < NUM_LEDS; ++i) {
        leds[i].R = leds[i].G = leds[i].B = 0;
    }
    bf();
}

// Função para configurar a cor de um LED específico
void cor(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[indice].R = r;
    leds[indice].G = g;
    leds[indice].B = b;
}

//DESENHANDO CADA NÚMERO
void number0() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0},{0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  { 0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0},{0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number1() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number2() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number3() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0, 0  }},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number4() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number5() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}}
    
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number6() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number7() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number8() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

void number9() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}}

    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

// switch para atualizar o estado de cada número
void atualizar_estado() {
    switch (estado_atual) {
        case 0:
            number0();
            printf("0\n");
            break;
        case 1:
            number1();
            printf("1\n");
            break;
        case 2:
            number2();
            printf("2\n");
            break;
        case 3:
            number3();
            printf("3\n");
            break;
        case 4:
            number4();
            printf("4\n");
            break;
        case 5:
            number5();
            printf("5\n");
            break;
        case 6:
            number6();
            printf("6\n");
            break;
        case 7:
            number7();
            printf("7\n");
            break;
        case 8:
            number8();
            printf("8\n");
            break;
        case 9:
            number9();
            printf("9\n");
            break;
    }
}

// Aplicando as rotinhas de interrupção IRQ para corrigir o debounce.
/* Aproveitando a função para criar uma estrutura condicional para incrementar e
   decrementar um número da função ATUALIZAR_ESTADO lendo o estado atual sempre que
   um dos botões forem pressionados. */
void debounce_botao(uint pino, volatile uint32_t *last_irq_time, int direcao) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - *last_irq_time > DEBOUNCE_DELAY) {
        *last_irq_time = tempo_atual;
        estado_atual += direcao;
        if (estado_atual >= num_eventos) estado_atual = 0;
        if (estado_atual < 0) estado_atual = num_eventos - 1;
        atualizar_estado();
    }
}

// Função para inicializar os pinos dos botões como entrada IN;
void iniciar_botoes() {
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
    controle(PINO_MATRIZ); //Controle do pino 7
    iniciar_botoes(); // Permite a interação com os botões
    printf("0\n");
    number0(); //Define o estado inicial da matriz de leds como o número 0

    while (1) {

        // Ativa a interrupção do botão B e decrementa um estado.
        if (!gpio_get(BOTAO_B)) debounce_botao(BOTAO_B, &last_irq_time_B, -1);
        //Ativa a interrupção do botão A e incrementa um estado.
        if (!gpio_get(BOTAO_A)) debounce_botao(BOTAO_A, &last_irq_time_A, +1);
        sleep_ms(10); // Aguarda 10 ms para a próxima leitura.

        /* O próximo ciclo será sempre depois de 210 ms devido ao led ficar 180 ms ligado
        10 ms desligado e o aguardo de 10 ms para o próximo ciclo*/

        state(1, 0, 0, 100); // O led vermelho vermelho tipo RGB fica 100 ms ligado
        state(0, 0, 0, 100);  // Todos os leds ficam 100 ms desligados
    }
    return (0); //Este ponto nunca será alcançado.
}
