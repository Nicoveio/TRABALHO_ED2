#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fila.h"

struct fila {
    Elemento* dados;
    int inicio;
    int fim;
    int capacidade;
    int tamanho;
};

Fila fila_cria(int capacidade_inicial) {
    if (capacidade_inicial <= 0) capacidade_inicial = 10; // capacidade padrão

    struct fila* f = (struct fila*) malloc(sizeof(struct fila));
    if (!f) return NULL;

    f->dados = (Elemento*) malloc(capacidade_inicial * sizeof(Elemento));
    if (!f->dados) {
        free(f);
        return NULL;
    }

    f->inicio = 0;
    f->fim = 0;
    f->capacidade = capacidade_inicial;
    f->tamanho = 0;

    return (Fila) f;
}

void fila_libera(Fila fila) {
    if (!fila) return;

    struct fila* f = (struct fila*) fila;
    free(f->dados);
    free(f);
}

bool fila_vazia(Fila fila) {
    if (!fila) return true;

    struct fila* f = (struct fila*) fila;
    return (f->tamanho == 0);
}

static bool fila_redimensiona(struct fila* f, int nova_capacidade) {
Elemento* novo = (Elemento*) malloc(nova_capacidade * sizeof(Elemento));
if (!novo) return false;

// Copia os elementos antigos para o novo buffer
int tamanho = (f->fim >= f->inicio)
    ? f->fim - f->inicio
    : f->capacidade - f->inicio + f->fim;

for (int i = 0; i < tamanho; i++) {
    novo[i] = f->dados[(f->inicio + i) % f->capacidade];
}

// Atualiza índices e ponteiros
f->inicio = 0;
f->fim = tamanho;
free(f->dados);
f->dados = novo;
f->capacidade = nova_capacidade;
return true;
}

bool fila_insere(Fila fila, Elemento elem) {
    if (!fila) return false;

    struct fila* f = (struct fila*) fila;

    if (f->tamanho == f->capacidade) {
        // dobrar a capacidade
        if (!fila_redimensiona(f, f->capacidade * 2)) {
            return false;
        }
    }

    f->dados[f->fim] = elem;
    f->fim = (f->fim + 1) % f->capacidade;
    f->tamanho++;
    return true;
}

Elemento fila_remove(Fila fila) {
    if (!fila) return NULL;

    struct fila* f = (struct fila*) fila;

    if (f->tamanho == 0) return NULL;

    Elemento elem = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;

    // opcional: reduzir capacidade se fila ficar muito vazia (não obrigatório)
    return elem;
}
