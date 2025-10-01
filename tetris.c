#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Desafio Tetris Stack
// Tema 3 - Integração de Fila e Pilha
// Este código inicial serve como base para o desenvolvimento do sistema de controle de peças.
// Use as instruções de cada nível para desenvolver o desafio.

#define TAMANHO_FILA 5   // número fixo de peças na fila
#define TAMANHO_PILHA 3 // número fico de peças na pilha

typedef struct {
    char nome;  // tipo da peça (I, O, T, L)
    int id;     // identificador único da peça
} Peca;



Peca fila[TAMANHO_FILA];
int frente = 0, tras = 0, qtdFila = 0;
Peca pilha[TAMANHO_PILHA];
int topo = -1;

int contadorId = 0; 


// Gera uma peça aleatória
Peca gerarPeca() {
    char tipos[] = {'I', 'O', 'T', 'L'};
    Peca nova;
    nova.nome = tipos[rand() % 4]; 
    nova.id = contadorId++;            
    return nova;
}

// Inicializa a fila circular com peças
void inicializarFila() {
   while (qtdFila < TAMANHO_FILA) {
    fila[tras] = gerarPeca();
    tras = (tras + 1) % TAMANHO_FILA;
    qtdFila++;
   }
}

// Exibe o estado atual da fila e pilha
void exibirEstado(){
printf("==================== ESTADO ATUAL ==================\n");

    //fila
    printf("Fila de peças\t");
    int i, idx = frente;
    for (i = 0; i < qtdFila; i++) {
        printf("[%c %d] ", fila[idx].nome, fila[idx].id);
        idx = (idx + 1) % TAMANHO_FILA;
    }
    printf("\n");

    // Pilha
    printf("Pilha de reserva \t(Topo -> Base): ");
    for (i = topo; i >= 0; i--) {
        printf("[%c %d] ", pilha[i].nome, pilha[i].id);
    }
    printf("\n====================================\n");
}

// Remover peça da frente da fila circular
Peca remove() {
    if (qtdFila == 0) {
        printf("Erro: Fila vazia!\n");
        Peca vazio = {'-', -1};
        return vazio;
    }
    Peca removida = fila[frente];
    frente = (frente + 1) % TAMANHO_FILA;
    qtdFila--;
    return removida;
}

// Insere peça no final da fila circular
void inserir(Peca p) {
    if (qtdFila == TAMANHO_FILA) {
        printf("Erro: fila cheia!\n");
        return;
    }
    fila[tras] = p;
    tras = (tras + 1) % TAMANHO_FILA;
    qtdFila++;
}

// Empilha uma peça
void push(Peca p) {
    if (topo == TAMANHO_PILHA - 1){
        printf("Erro: pilha cheia!\n");
        return;
    }
    pilha[++topo] = p;
}

// Desempilhando uma peça
Peca pop() {
    if (topo == -1) {
        printf("Erro: pilha vazia!\n");
        Peca vazio = {'-', -1};
        return vazio;
    }
    return pilha[topo--];
}

// Ações do jogo
void jogarPeca(){
    Peca jogada = remove();
    printf("Jogando peça: [%c %d]\n", jogada.nome, jogada.id);
    inserir(gerarPeca());
}

void reservaPeca() {
    if (topo == TAMANHO_PILHA - 1) {
        printf("A pilha está cheia, não é possível reservar\n");
        return;
    }
    Peca reservada = remove();
    push(reservada);
    printf("Reservando peça: [%c %d]\n", reservada.nome, reservada.id);
    inserir(gerarPeca());
}

void usarReservada() {
    if (topo == -1) {
        printf("A pilha está vazia, não há peças reservadas!\n");
        return;
    }
    Peca usada = pop();
    printf("Usando peça reservada: [%c %d]\n", usada.nome, usada.id);
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
    srand(time(NULL));
    inicializarFila();

    int opcao;
    do {
        exibirEstado();
        printf("\nOpções para Ação:\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar uma peça\n");
        printf("3 - Usar peça reservada\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: jogarPeca(); break;
            case 2: reservaPeca(); break;
            case 3: usarReservada(); break;
            case 0: printf("Saindo do jogo... gratidão!\n"); break;
            default: printf("Opção inválida!\n");
        }           
    }while (opcao != 0);

    return 0;
}

