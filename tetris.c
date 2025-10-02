#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 

#define TAM_FILA 5
#define TAM_PILHA 3

typedef struct {
    char nome; // 'I', 'O', 'T', 'L'
    int id;    // id único
} Peca;

typedef struct {
    Peca itens[TAM_FILA];
    int frente; // índice do elemento na frente
    int tras;   // índice do último elemento
    int tamanho;
} Fila;

typedef struct {
    Peca itens[TAM_PILHA];
    int topo; // -1 se vazia, else índice do topo
} Pilha;

// Estrutura para snapshot (undo)
typedef struct {
    Fila fila;
    Pilha pilha;
    int valido; // 1 se há snapshot válido
} Snapshot;

// =========================
// Protótipos
// =========================
Peca gerarPeca();
void inicializarFila(Fila *f);
void inicializarPilha(Pilha *p);

int filaCheia(const Fila *f);
int filaVazia(const Fila *f);
int pilhaCheia(const Pilha *p);
int pilhaVazia(const Pilha *p);

void enfileirar(Fila *f, Peca x);
Peca desenfileirar(Fila *f);

void empilhar(Pilha *p, Peca x);
Peca desempilhar(Pilha *p);

void mostrarFila(const Fila *f);
void mostrarPilha(const Pilha *p);
void mostrarEstado(const Fila *f, const Pilha *p);

void salvarSnapshot(Snapshot *s, const Fila *f, const Pilha *p);
void restaurarSnapshot(Snapshot *s, Fila *f, Pilha *p);
void invalidarSnapshot(Snapshot *s);

// Operações de jogo (Novato / Aventureiro / Mestre)
void jogarPeca(Fila *f, Snapshot *snap);
void reservarPeca(Fila *f, Pilha *p, Snapshot *snap);
void usarReservada(Pilha *p, Snapshot *snap);
void trocarUma(Fila *f, Pilha *p, Snapshot *snap);
void trocarMultiplas(Fila *f, Pilha *p, Snapshot *snap);
void inverterFilaPilha(Fila *f, Pilha *p, Snapshot *snap);

// Utilitários
void limparBufferStdin();

// =========================
// Implementações
// =========================

Peca gerarPeca() {
    static int contador = 0;
    char tipos[] = {'I','O','T','L'};
    Peca nova;
    nova.nome = tipos[rand() % 4];
    nova.id = contador++;
    return nova;
}

/* ---------- Fila ---------- */
void inicializarFila(Fila *f) {
    f->frente = 0;
    f->tras = -1;
    f->tamanho = 0;
    // Preencher com TAM_FILA peças geradas automaticamente
    for (int i = 0; i < TAM_FILA; i++) {
        enfileirar(f, gerarPeca());
    }
}

int filaCheia(const Fila *f) {
    return f->tamanho == TAM_FILA;
}

int filaVazia(const Fila *f) {
    return f->tamanho == 0;
}

void enfileirar(Fila *f, Peca x) {
    if (filaCheia(f)) return;
    f->tras = (f->tras + 1) % TAM_FILA;
    f->itens[f->tras] = x;
    f->tamanho++;
}

Peca desenfileirar(Fila *f) {
    Peca vazio = {'-', -1};
    if (filaVazia(f)) return vazio;
    Peca ret = f->itens[f->frente];
    f->frente = (f->frente + 1) % TAM_FILA;
    f->tamanho--;
    return ret;
}

/* ---------- Pilha ---------- */
void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

int pilhaCheia(const Pilha *p) {
    return p->topo == TAM_PILHA - 1;
}

int pilhaVazia(const Pilha *p) {
    return p->topo == -1;
}

void empilhar(Pilha *p, Peca x) {
    if (pilhaCheia(p)) return;
    p->itens[++(p->topo)] = x;
}

Peca desempilhar(Pilha *p) {
    Peca vazio = {'-', -1};
    if (pilhaVazia(p)) return vazio;
    return p->itens[(p->topo)--];
}

/* ---------- Exibição ---------- */
void mostrarFila(const Fila *f) {
    printf("Fila de pecas\t");
    if (filaVazia(f)) {
        printf("(vazia)");
    } else {
        for (int i = 0; i < f->tamanho; i++) {
            int idx = (f->frente + i) % TAM_FILA;
            printf("[%c %d] ", f->itens[idx].nome, f->itens[idx].id);
        }
    }
    printf("\n");
}

void mostrarPilha(const Pilha *p) {
    printf("Pilha de reserva\t(Topo -> base): ");
    if (pilhaVazia(p)) {
        printf("(vazia)");
    } else {
        for (int i = p->topo; i >= 0; i--) {
            printf("[%c %d] ", p->itens[i].nome, p->itens[i].id);
        }
    }
    printf("\n");
}

void mostrarEstado(const Fila *f, const Pilha *p) {
    printf("\n==================== ESTADO ATUAL ====================\n");
    mostrarFila(f);
    mostrarPilha(p);
    printf("======================================================\n");
}

/* ---------- Snapshot (undo) ---------- */
void salvarSnapshot(Snapshot *s, const Fila *f, const Pilha *p) {
    // copia bit-a-bit
    s->fila = *f;
    s->pilha = *p;
    s->valido = 1;
}

void restaurarSnapshot(Snapshot *s, Fila *f, Pilha *p) {
    if (s->valido) {
        *f = s->fila;
        *p = s->pilha;
        s->valido = 0; // consumido
        printf("Desfeito: estado anterior restaurado.\n");
    } else {
        printf("Nao ha acao para desfazer.\n");
    }
}

void invalidarSnapshot(Snapshot *s) {
    s->valido = 0;
}

/* ---------- Operações ---------- */

/* Jogar peca: desenfileira a da frente e gera nova p/ manter a fila cheia.
   Salva snapshot antes da ação (apenas para nivel Mestre). */
void jogarPeca(Fila *f, Snapshot *snap) {
    if (snap) salvarSnapshot(snap, f, NULL); // Note: When only fila muda, pilha não é passada; we'll handle in caller.
    if (filaVazia(f)) {
        printf("Fila vazia. Nada a jogar.\n");
        invalidarSnapshot(snap); // não válido se nada aconteceu
        return;
    }
    Peca p = desenfileirar(f);
    printf("Peca jogada: [%c %d]\n", p.nome, p.id);
    // gera nova peça e enfileira para manter o tamanho
    enfileirar(f, gerarPeca());
}

/* Reservar peça: move da frente da fila para o topo da pilha (se houver espaço).
   Gera nova peça para enfileirar.
   Salva snapshot antes da ação. */
void reservarPeca(Fila *f, Pilha *p, Snapshot *snap) {
    if (filaVazia(f)) {
        printf("Fila vazia. Nao ha peca para reservar.\n");
        invalidarSnapshot(snap);
        return;
    }
    if (pilhaCheia(p)) {
        printf("Pilha cheia. Nao e possivel reservar.\n");
        invalidarSnapshot(snap);
        return;
    }
    if (snap) salvarSnapshot(snap, f, p);
    Peca mov = desenfileirar(f);
    empilhar(p, mov);
    printf("Peca [%c %d] movida para reserva.\n", mov.nome, mov.id);
    enfileirar(f, gerarPeca());
}

/* Usar reservada: desempilha (remove) do topo */
void usarReservada(Pilha *p, Snapshot *snap) {
    if (pilhaVazia(p)) {
        printf("Pilha vazia. Nao ha peca para usar.\n");
        invalidarSnapshot(snap);
        return;
    }
    if (snap) salvarSnapshot(snap, NULL, p);
    Peca u = desempilhar(p);
    printf("Peca usada da reserva: [%c %d]\n", u.nome, u.id);
}

/* Trocar uma: troca a peça da frente da fila com o topo da pilha */
void trocarUma(Fila *f, Pilha *p, Snapshot *snap) {
    if (filaVazia(f) || pilhaVazia(p)) {
        printf("Troca impossivel: verifique se fila e pilha possuem elementos.\n");
        invalidarSnapshot(snap);
        return;
    }
    if (snap) salvarSnapshot(snap, f, p);
    int idx = f->frente;
    Peca temp = f->itens[idx];
    f->itens[idx] = p->itens[p->topo];
    p->itens[p->topo] = temp;
    printf("Troca realizada entre frente da fila e topo da pilha.\n");
}

/* Troca múltipla: alterna os 3 primeiros da fila com as 3 pecas da pilha
   somente se a fila tiver >=3 e pilha tiver >=3 */
void trocarMultiplas(Fila *f, Pilha *p, Snapshot *snap) {
    if (f->tamanho < 3 || p->topo < 2) {
        printf("Troca multipla impossivel: verifique se existem pelo menos 3 pecas em ambas as estruturas.\n");
        invalidarSnapshot(snap);
        return;
    }
    if (snap) salvarSnapshot(snap, f, p);
    for (int i = 0; i < 3; i++) {
        int idx = (f->frente + i) % TAM_FILA;
        Peca temp = f->itens[idx];
        // pilha: topo .. topo-2 é ordem; queremos que o primeiro da fila troque com topo,
        // segundo com topo-1, terceiro com topo-2 -> isso iguala a ideia de "os 3 primeiros"
        f->itens[idx] = p->itens[p->topo - i];
        p->itens[p->topo - i] = temp;
    }
    printf("Troca multipla entre os 3 primeiros da fila e as 3 pecas da pilha realizada.\n");
}

/* Inverter fila com pilha:
   - Faz uma troca segura entre os conteúdos, respeitando capacidades.
   - Estratégia:
       copia temporária da fila e pilha,
       limpa ambas,
       enfileira na fila os elementos da pilha (do fundo para o topo) até encher a fila,
       empilha na pilha os primeiros elementos da fila-copia (na ordem de frente) até encher a pilha.
   (Essa definição garante comport. deterministico e preserva ordem)
*/
void inverterFilaPilha(Fila *f, Pilha *p, Snapshot *snap) {
    if (snap) salvarSnapshot(snap, f, p);

    // copia
    Fila fcopy = *f;
    Pilha pcopy = *p;

    // limpa originais
    f->frente = 0; f->tras = -1; f->tamanho = 0;
    p->topo = -1;

    // 1) Colocar elementos da pilha (do base para o topo) na fila: queremos manter ordem "de base->topo"
    // Mas pilha tem indices 0..topo (base..topo). Vamos enfileirar do indice 0 até topo, na mesma ordem.
    for (int i = 0; i <= pcopy.topo && f->tamanho < TAM_FILA; i++) {
        enfileirar(f, pcopy.itens[i]);
    }

    // 2) Preencher fila com elementos restantes da fila copia (apenas se houver espaço)
    for (int i = 0; i < fcopy.tamanho && f->tamanho < TAM_FILA; i++) {
        int idx = (fcopy.frente + i) % TAM_FILA;
        enfileirar(f, fcopy.itens[idx]);
    }

    // 3) Empilhar na pilha os primeiros elementos da fila-copia (em ordem de frente),
    //    até encher a pilha.
    for (int i = 0; i < fcopy.tamanho && p->topo < TAM_PILHA - 1; i++) {
        int idx = (fcopy.frente + i) % TAM_FILA;
        empilhar(p, fcopy.itens[idx]);
    }

    printf("Inversao/transferencia entre fila e pilha realizada (respeitando capacidades).\n");
}

/* ---------- Utilitários ---------- */
void limparBufferStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Desafio Tetris Stack
// Tema 3 - Integração de Fila e Pilha
// Este código inicial serve como base para o desenvolvimento do sistema de controle de peças.
// Use as instruções de cada nível para desenvolver o desafio.

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
    srand((unsigned)time(NULL));
    Fila fila;
    Pilha pilha;
    Snapshot snapshot;
    snapshot.valido = 0;

    int nivel = -1;

    printf("Bem-vindo ao desafio \"Tetris Stack\"!\n");
    printf("Escolha o nivel:\n");
    printf("1 - Novato (fila somente)\n");
    printf("2 - Aventureiro (fila + pilha)\n");
    printf("3 - Mestre (todas operacoes)\n");
    printf("Opcao: ");
    if (scanf("%d", &nivel) != 1) {
        printf("Entrada invalida. Encerrando.\n");
        return 0;
    }
    limparBufferStdin();

    // Inicializa estruturas
    inicializarFila(&fila);
    inicializarPilha(&pilha);
    snapshot.valido = 0;

    int opcao = -1;
    do {
        mostrarEstado(&fila, &pilha);

        if (nivel == 1) { // Novato
            printf("\n-- Novato --\n");
            printf("1 - Jogar peca (dequeue)\n");
            printf("2 - Inserir nova peca manualmente (gera e enfileira se houver espaco)\n");
            printf("0 - Sair\n");
            printf("Escolha: ");
            if (scanf("%d", &opcao) != 1) { limparBufferStdin(); opcao = -1; }
            limparBufferStdin();

            switch (opcao) {
                case 1:
                    // Jogar: desenfileira e gera nova
                    if (filaVazia(&fila)) {
                        printf("Fila vazia. Nao ha peca para jogar.\n");
                    } else {
                        Peca p = desenfileirar(&fila);
                        printf("Peca jogada: [%c %d]\n", p.nome, p.id);
                        enfileirar(&fila, gerarPeca());
                    }
                    break;
                case 2:
                    if (filaCheia(&fila)) {
                        printf("Fila cheia. Nao e possivel enfileirar.\n");
                    } else {
                        enfileirar(&fila, gerarPeca());
                        printf("Nova peca gerada e enfileirada.\n");
                    }
                    break;
                case 0:
                    printf("Encerrando (Novato)...\n");
                    break;
                default:
                    printf("Opcao invalida.\n");
            }
        }
        else if (nivel == 2) { // Aventureiro
            printf("\n-- Aventureiro --\n");
            printf("1 - Jogar peca (dequeue)\n");
            printf("2 - Reservar peca (fila -> pilha)\n");
            printf("3 - Usar peca reservada (pop)\n");
            printf("0 - Sair\n");
            printf("Escolha: ");
            if (scanf("%d", &opcao) != 1) { limparBufferStdin(); opcao = -1; }
            limparBufferStdin();

            switch (opcao) {
                case 1:
                    if (filaVazia(&fila)) {
                        printf("Fila vazia. Nada a jogar.\n");
                    } else {
                        Peca p = desenfileirar(&fila);
                        printf("Peca jogada: [%c %d]\n", p.nome, p.id);
                        enfileirar(&fila, gerarPeca());
                    }
                    break;
                case 2:
                    if (filaVazia(&fila)) {
                        printf("Fila vazia. Nada para reservar.\n");
                    } else if (pilhaCheia(&pilha)) {
                        printf("Pilha cheia. Nao e possivel reservar.\n");
                    } else {
                        Peca mov = desenfileirar(&fila);
                        empilhar(&pilha, mov);
                        printf("Peca [%c %d] reservada.\n", mov.nome, mov.id);
                        enfileirar(&fila, gerarPeca());
                    }
                    break;
                case 3:
                    if (pilhaVazia(&pilha)) {
                        printf("Pilha vazia. Nada para usar.\n");
                    } else {
                        Peca u = desempilhar(&pilha);
                        printf("Peca usada da reserva: [%c %d]\n", u.nome, u.id);
                    }
                    break;
                case 0:
                    printf("Encerrando (Aventureiro)...\n");
                    break;
                default:
                    printf("Opcao invalida.\n");
            }
        }
        else if (nivel == 3) { // Mestre
            printf("\n-- Mestre --\n");
            printf("1 - Jogar peca (dequeue)\n");
            printf("2 - Reservar peca (fila -> pilha)\n");
            printf("3 - Usar peca reservada (pop)\n");
            printf("4 - Trocar frente da fila com topo da pilha\n");
            printf("5 - Trocar os 3 primeiros da fila com as 3 da pilha\n");
            printf("6 - Desfazer ultima acao (undo)\n");
            printf("7 - Inverter/transferir fila <-> pilha (respeita capacidades)\n");
            printf("0 - Sair\n");
            printf("Escolha: ");
            if (scanf("%d", &opcao) != 1) { limparBufferStdin(); opcao = -1; }
            limparBufferStdin();

            switch (opcao) {
                case 1:
                    // salvar estado antes e executar
                    salvarSnapshot(&snapshot, &fila, &pilha);
                    if (filaVazia(&fila)) {
                        printf("Fila vazia. Nada a jogar.\n");
                        invalidarSnapshot(&snapshot);
                    } else {
                        Peca p = desenfileirar(&fila);
                        printf("Peca jogada: [%c %d]\n", p.nome, p.id);
                        enfileirar(&fila, gerarPeca());
                    }
                    break;
                case 2:
                    if (filaVazia(&fila)) {
                        printf("Fila vazia. Nada para reservar.\n");
                    } else if (pilhaCheia(&pilha)) {
                        printf("Pilha cheia. Nao e possivel reservar.\n");
                    } else {
                        salvarSnapshot(&snapshot, &fila, &pilha);
                        Peca mov = desenfileirar(&fila);
                        empilhar(&pilha, mov);
                        enfileirar(&fila, gerarPeca());
                        printf("Peca [%c %d] reservada.\n", mov.nome, mov.id);
                    }
                    break;
                case 3:
                    if (pilhaVazia(&pilha)) {
                        printf("Pilha vazia. Nada para usar.\n");
                    } else {
                        salvarSnapshot(&snapshot, &fila, &pilha);
                        Peca u = desempilhar(&pilha);
                        printf("Peca usada da reserva: [%c %d]\n", u.nome, u.id);
                    }
                    break;
                case 4:
                    if (filaVazia(&fila) || pilhaVazia(&pilha)) {
                        printf("Nao e possivel trocar: verifique se fila e pilha contem pecas.\n");
                    } else {
                        salvarSnapshot(&snapshot, &fila, &pilha);
                        trocarUma(&fila, &pilha, NULL); // trocarUma ja imprime mensagem
                    }
                    break;
                case 5:
                    if (fila.tamanho < 3 || pilha.topo < 2) {
                        printf("Troca multipla impossivel: ambas precisam ter ao menos 3 pecas.\n");
                    } else {
                        salvarSnapshot(&snapshot, &fila, &pilha);
                        trocarMultiplas(&fila, &pilha, NULL);
                    }
                    break;
                case 6:
                    // desfazer
                    if (snapshot.valido) {
                        restaurarSnapshot(&snapshot, &fila, &pilha);
                    } else {
                        printf("Nenhuma acao disponivel para desfazer.\n");
                    }
                    break;
                case 7:
                    salvarSnapshot(&snapshot, &fila, &pilha);
                    inverterFilaPilha(&fila, &pilha, NULL);
                    break;
                case 0:
                    printf("Encerrando (Mestre)...\n");
                    break;
                default:
                    printf("Opcao invalida.\n");
            }
        } else {
            printf("Nivel desconhecido. Encerrando.\n");
            break;
        }

    } while (opcao != 0);

    printf("Obrigado por jogar Tetris Stack (simulador)!\n");
    return 0;
}


