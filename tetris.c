#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Desafio Tetris Stack
// Tema 3 - Integração de Fila e Pilha
// Este código inicial serve como base para o desenvolvimento do sistema de controle de peças.
// Use as instruções de cada nível para desenvolver o desafio.

#define TAMANHO_FILA 5   // número fixo de peças na fila

// -------------------- Definição de tipos -------------------- //
typedef struct {
    char nome;  // tipo da peça (I, O, T, L)
    int id;     // identificador único da peça
} Peca;

typedef struct {
    Peca itens[TAMANHO_FILA];
    int frente;   // índice do primeiro elemento
    int tras;     // índice do último elemento
    int qtd;      // quantidade de elementos na fila
} Fila;

// -------------------- Variáveis globais -------------------- //
int contadorId = 0; // controla o id único das peças

// -------------------- Funções auxiliares -------------------- //
char tiposPecas[] = {'I', 'O', 'T', 'L'};

// Gera automaticamente uma nova peça com tipo aleatório e id único
Peca gerarPeca() {
    Peca nova;
    nova.nome = tiposPecas[rand() % 4]; // seleciona tipo aleatório
    nova.id = contadorId++;             // atribui id único
    return nova;
}

// Inicializa a fila com TAMANHO_FILA peças geradas automaticamente
void inicializarFila(Fila *f) {
    f->frente = 0;
    f->tras = -1;
    f->qtd = 0;
    for (int i = 0; i < TAMANHO_FILA; i++) {
        Peca p = gerarPeca();
        f->tras = (f->tras + 1) % TAMANHO_FILA;
        f->itens[f->tras] = p;
        f->qtd++;
    }
}

// Verifica se a fila está cheia
int filaCheia(Fila *f) {
    return f->qtd == TAMANHO_FILA;
}

// Verifica se a fila está vazia
int filaVazia(Fila *f) {
    return f->qtd == 0;
}

// Insere uma peça no final da fila
void inserirFila(Fila *f, Peca p) {
    if (filaCheia(f)) {
        printf("A fila está cheia! Não é possível adicionar nova peça.\n");
        return;
    }
    f->tras = (f->tras + 1) % TAMANHO_FILA;
    f->itens[f->tras] = p;
    f->qtd++;
    printf("Peça [%c %d] adicionada à fila.\n", p.nome, p.id);
}

// Remove uma peça da frente da fila
void removerFila(Fila *f) {
    if (filaVazia(f)) {
        printf("A fila está vazia! Não há peças para jogar.\n");
        return;
    }
    Peca removida = f->itens[f->frente];
    f->frente = (f->frente + 1) % TAMANHO_FILA;
    f->qtd--;
    printf("Jogando peça [%c %d]...\n", removida.nome, removida.id);
}

// Exibe o estado atual da fila
void exibirFila(Fila *f) {
    printf("\nFila de peças:\n");
    if (filaVazia(f)) {
        printf("[vazia]\n");
        return;
    }
    int i = f->frente;
    for (int count = 0; count < f->qtd; count++) {
        Peca p = f->itens[i];
        printf("[%c %d] ", p.nome, p.id);
        i = (i + 1) % TAMANHO_FILA;
    }
    printf("\n");
}
int main() {

    // 🧩 Nível Novato: Fila de Peças Futuras
    //
    // - Crie uma struct Peca com os campos: tipo (char) e id (int).
    // - Implemente uma fila circular com capacidade para 5 peças.
    // - Crie funções como inicializarFila(), enqueue(), dequeue(), filaCheia(), filaVazia().
    // - Cada peça deve ser gerada automaticamente com um tipo aleatório e id sequencial.
    // - Exiba a fila após cada ação com uma função mostrarFila().
    // - Use um menu com opções como:
    //      1 - Jogar peça (remover da frente)
    //      0 - Sair
    // - A cada remoção, insira uma nova peça ao final da fila.

    srand(time(NULL)); // inicializa gerador aleatório
    Fila fila;
    inicializarFila(&fila);

    int opcao;
    do {
        exibirFila(&fila);
        printf("\nOpções de ação:\n");
        printf("1 - Jogar peça (Remover)\n");
        printf("2 - Inserir nova peça (Inserir)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                removerFila(&fila);
                break;
            case 2:
                inserirFila(&fila, gerarPeca());
                break;
            case 0:
                printf("👋 Encerrando o jogo...\n");
                break;
            default:
                printf("⚠️ Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);

    




    // 🧠 Nível Aventureiro: Adição da Pilha de Reserva
    //
    // - Implemente uma pilha linear com capacidade para 3 peças.
    // - Crie funções como inicializarPilha(), push(), pop(), pilhaCheia(), pilhaVazia().
    // - Permita enviar uma peça da fila para a pilha (reserva).
    // - Crie um menu com opção:
    //      2 - Enviar peça da fila para a reserva (pilha)
    //      3 - Usar peça da reserva (remover do topo da pilha)
    // - Exiba a pilha junto com a fila após cada ação com mostrarPilha().
    // - Mantenha a fila sempre com 5 peças (repondo com gerarPeca()).


    // 🔄 Nível Mestre: Integração Estratégica entre Fila e Pilha
    //
    // - Implemente interações avançadas entre as estruturas:
    //      4 - Trocar a peça da frente da fila com o topo da pilha
    //      5 - Trocar os 3 primeiros da fila com as 3 peças da pilha
    // - Para a opção 4:
    //      Verifique se a fila não está vazia e a pilha tem ao menos 1 peça.
    //      Troque os elementos diretamente nos arrays.
    // - Para a opção 5:
    //      Verifique se a pilha tem exatamente 3 peças e a fila ao menos 3.
    //      Use a lógica de índice circular para acessar os primeiros da fila.
    // - Sempre valide as condições antes da troca e informe mensagens claras ao usuário.
    // - Use funções auxiliares, se quiser, para modularizar a lógica de troca.
    // - O menu deve ficar assim:
    //      4 - Trocar peça da frente com topo da pilha
    //      5 - Trocar 3 primeiros da fila com os 3 da pilha


    return 0;
}

