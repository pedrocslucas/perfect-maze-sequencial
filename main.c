#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAZE_SIZE 20000

char** maze;

// Estrutura para representar as c�lulas do algoritmo de cria��o do labirinto
typedef struct {
    long long x;
    long long y;
} Cell;

Cell* cells;

// Fun��o para criar as c�lulas
void criar_celulas() {
    cells = (Cell*)malloc((MAZE_SIZE * MAZE_SIZE / 2) * sizeof(Cell));
}

// Fun��o para liberar a mem�ria alocada para as c�lulas
void liberar_memoria_celulas() {
    free(cells);
}

//Fun��o para criar uma inst�ncia da vari�vel do labirinto chamada de 'maze'
void inicializa_maze() {
    maze = (char**)malloc(MAZE_SIZE * sizeof(char*));
    for (long long i = 0; i < MAZE_SIZE; i++) {
        maze[i] = (char*)malloc(MAZE_SIZE * sizeof(char));
    }
}

//Fun��o para liberar a mem�ria da vari�vel 'maze', ap�s o uso
void liberar_memoria() {
    for (long long i = 0; i < MAZE_SIZE; i++) {
        free(maze[i]);
    }
    free(maze);
}

void create_maze() {
    inicializa_maze();

    #pragma omp parallel for collapse(2)
    for (long long i = 0; i < MAZE_SIZE; i++) {
        for (long long j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = '#';
        }
    }

    criar_celulas();

    // Lista para armazenar as células candidatas para expandir
    int num_cells = 1;
    cells[0].x = 1;
    cells[0].y = 1;

    // Marca a célula inicial como parte do labirinto
    maze[1][1] = ' ';

    // Loop for para controlar o processo de expansão das células
    for (int iter = 0; num_cells > 0; iter++) {
        // Escolhe uma célula aleatória da lista
        long long index = rand() % num_cells;
        long long x = cells[index].x;
        long long y = cells[index].y;

        // Verifica as células vizinhas
        int dx[] = {-2, 2, 0, 0};
        int dy[] = {0, 0, -2, 2};
        int neighbors[4];
        int num_neighbors = 0;

        // Encontra as células vizinhas não marcadas
        for (int i = 0; i < 4; i++) {
            long long nx = x + dx[i];
            long long ny = y + dy[i];
            if (nx > 0 && nx < MAZE_SIZE && ny > 0 && ny < MAZE_SIZE && maze[nx][ny] == '#') {
                neighbors[num_neighbors++] = i;
            }
        }

        // Se houver células vizinhas não marcadas
        if (num_neighbors > 0) {
            // Escolhe uma outra célula vizinha aleatória
            int direction = neighbors[rand() % num_neighbors];
            long long nx = x + dx[direction];
            long long ny = y + dy[direction];

            // Marca a célula como parte do labirinto
            maze[nx][ny] = ' ';
            // Abre a parede entre a célula atual e a célula escolhida
            maze[x + dx[direction] / 2][y + dy[direction] / 2] = ' ';

            // Adiciona a célula escolhida à lista de células candidatas
            cells[num_cells].x = nx;
            cells[num_cells].y = ny;
            num_cells++;
        } else {
            // Se não houver células vizinhas não marcadas, remove a célula atual da lista
            cells[index] = cells[num_cells - 1];
            num_cells--;
        }
    }

    liberar_memoria_celulas();

    // Define o ponto de partida e o ponto de chegada
    maze[0][0] = 'E';
    maze[0][MAZE_SIZE - 1] = 'S';

    // Cria contornos entre a entrada e a saída.
    #pragma omp parallel for simd
    for (long long i = 0; i < MAZE_SIZE; i++) {
        maze[i][MAZE_SIZE - 1] = '#';
        maze[MAZE_SIZE - 1][i] = '#';
    }

    maze[0][0] = 'E';
    maze[0][MAZE_SIZE - 1] = 'S';

    maze[0][1] = ' ';
    maze[1][0] = ' ';

    maze[0][MAZE_SIZE - 2] = ' ';
    maze[1][MAZE_SIZE - 2] = ' ';
}

// Fun��o para imprimir o labirinto
void print_maze() {
    for (long long i = 0; i < MAZE_SIZE; i++) {
        for (long long j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

int eh_valido(long long x, long long y) {
    return (x >= 0 && x < MAZE_SIZE) && (y >= 0 && y < MAZE_SIZE);
}

int busca_em_profundidade(long long startX, long long startY) {
    int found = 0; // Variável para indicar se o caminho foi encontrado

    // Vetores para representar as direções possíveis (esquerda, baixo, direita, cima)
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    // Pragma OpenMP para paralelizar o loop externo
    #pragma omp parallel for
    for (long long i = 0; i < MAZE_SIZE; i++) {
        // Loop interno para percorrer as colunas da matriz 'maze'
        for (long long j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == 'S') {
                // Se encontrou a saída, atualiza a variável 'found'
                found = 1;
            }

            // Se a célula atual for vazia ou a saída, marca como visitada
            if (maze[i][j] == ' ' || maze[i][j] == 'S') {
                for (int k = 0; k < 4; k++) {
                    // Calcula as novas coordenadas
                    long newX = j + dx[k];
                    long newY = i + dy[k];

                    // Verifica se as novas coordenadas estão dentro dos limites do labirinto
                    if (eh_valido(newX, newY) && maze[newY][newX] == ' ') {
                        // Marca como visitada
                        maze[newY][newX] = '-';
                    }
                }
            }
        }
    }

    return found; // Retorna 1 se o caminho foi encontrado, 0 caso contrário
}

int main() {
    srand(time(NULL));

    omp_set_num_threads(8); // Define o n�mero de Threads

    double timei = omp_get_wtime();
    create_maze();
    int resp = busca_em_profundidade(0, 0);

    double timef = omp_get_wtime();
    printf("Time: %lf\n", timef-timei);

    if(resp)
        printf("Caminho encontrado! =)\n");
    else
        printf("Caminho NAO encontrado! =(\n");

    liberar_memoria();

    return 0;
}
