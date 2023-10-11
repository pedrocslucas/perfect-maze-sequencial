#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAZE_SIZE 20

char maze[MAZE_SIZE][MAZE_SIZE];

// Função para imprimir o labirinto
void print_maze() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

// Função para criar um labirinto com caminhos internos e alternativos
void create_maze() {
    // Inicialize o labirinto com todas as células preenchidas com paredes
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = '#';
        }
    }

    // Defina o ponto de partida e o ponto de chegada
    maze[0][0] = 'E';
    maze[0][1] = ' ';
    maze[MAZE_SIZE - 1][MAZE_SIZE - 1] = 'S';
    maze[MAZE_SIZE -1][MAZE_SIZE-2] = ' ';

    // Caminho principal para a saída:
    int x = 1;
    int y = 0;
    while (x < MAZE_SIZE - 1 || y < MAZE_SIZE - 2) {
        maze[x][y] = ' ';
        if (x < MAZE_SIZE - 1 && rand() % 2 == 0) {
            x++;
        } else if (y < MAZE_SIZE - 2) {
            y++;
        }
    }

    // Caminhos alternativos:
    for (int i = 1; i < MAZE_SIZE - 1; i += 2) {
        for (int j = 1; j < MAZE_SIZE - 2; j++) {
            if (rand() % 2 == 0) {
                maze[i][j] = ' ';
            }
        }
    }

    for (int i = 2; i < MAZE_SIZE - 1; i += 2) {
        for (int j = 2; j < MAZE_SIZE - 2; j++) {
            if (rand() % 2 == 0) {
                maze[i][j] = ' ';
            }
        }
    }
}

typedef struct {
    int x, y;
} Coordenada;

typedef struct {
    Coordenada item[MAZE_SIZE*MAZE_SIZE];
    int topo;
} Pilha;

void inicializa_pilha(Pilha *p){
    p->topo = -1;
}

void push(Pilha *p, Coordenada coord){
    p->topo++;
    p->item[p->topo] = coord;
}

Coordenada pop(Pilha *p){
    Coordenada coord = p->item[p->topo];
    p->topo--;
    return coord;
}

int eh_valido(int x, int y){
    return (x >= 0 && x < MAZE_SIZE) && (y >= 0 && y < MAZE_SIZE);
}

int busca_em_profundidade(int inicio_x, int inicio_y) {
    Pilha caminho;
    inicializa_pilha(&caminho);
    int encontrou_caminho = 0; // Variável de controle

    Coordenada inicio = {inicio_x, inicio_y};
    push(&caminho, inicio);

    while(caminho.topo >= 0 && !encontrou_caminho) {
        Coordenada coord = pop(&caminho);
        int x = coord.x;
        int y = coord.y;

        if(maze[x][y] == 'S'){
            printf("Caminho encontrado:\n");
            encontrou_caminho = 1; // Define a variável como verdadeira para interromper o loop
        }

        // Todos os movimentos possíveis
        int movimentosX[] = {0, 0, -1, 1};
        int movimentosY[] = {-1, 1, 0, 0};

        for(int i = 0; i < 4; i++){
            int novoX = x + movimentosX[i];
            int novoY = y + movimentosY[i];

            if((eh_valido(novoX, novoY)) && (maze[novoX][novoY] == ' ')){
                Coordenada novoCoord = {novoX, novoY};
                push(&caminho, novoCoord);
                maze[novoX][novoY] = 'V'; // Marque a célula como visitada quando ela é adicionada à pilha
            }
        }
    }

    if(!encontrou_caminho) {
        printf("Caminho não encontrado... =(\n");
    }

    return encontrou_caminho;
}


int main() {
    srand(time(NULL));

    // Crie o labirinto e inicialize a matriz de caminho
    create_maze();

    printf("Labirinto Criado:\n");
    print_maze();

    busca_em_profundidade(0, 0);
    print_maze();

    return 0;
}
