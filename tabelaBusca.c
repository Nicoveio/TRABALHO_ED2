#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tabelaBusca.h"

// Nó da lista encadeada (para colisões de hash)
typedef struct no_busca {
    double x;
    double y;
    struct no_busca* prox;
} NoBusca;

// Estrutura interna da Tabela de Busca
typedef struct tabela_busca_imp {
    int tamanho_tabela;
    NoBusca** tabela; // Um vetor de ponteiros para nós
} TabelaBuscaImp;

// Função de Hash: transforma (x,y) em um índice do vetor
static int hash_coord(double x, double y, int tamanho_tabela) {
    unsigned long long hx = 0;
    memcpy(&hx, &x, sizeof(double));

    unsigned long long hy = 0;
    memcpy(&hy, &y, sizeof(double));

    unsigned long long hash_final = hx ^ (hy << 1);
    return hash_final % tamanho_tabela;
}

// Implementação das funções públicas
TabelaBusca tb_cria(int tamanho_base) {
    if (tamanho_base <= 0) return NULL;

    TabelaBuscaImp* tb = malloc(sizeof(TabelaBuscaImp));
    if (!tb) return NULL;

    tb->tamanho_tabela = tamanho_base;
    tb->tabela = calloc(tamanho_base, sizeof(NoBusca*));
    if (!tb->tabela) {
        free(tb);
        return NULL;
    }
    return tb;
}

void tb_libera(TabelaBusca t) {
    if (!t) return;
    TabelaBuscaImp* tb = (TabelaBuscaImp*)t;

    for (int i = 0; i < tb->tamanho_tabela; i++) {
        NoBusca* atual = tb->tabela[i];
        while (atual != NULL) {
            NoBusca* proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(tb->tabela);
    free(tb);
}

bool tb_insere(TabelaBusca t, double x, double y) {
    if (!t) return false;
    TabelaBuscaImp* tb = (TabelaBuscaImp*)t;
    
    int indice = hash_coord(x, y, tb->tamanho_tabela);

    NoBusca* novo_no = malloc(sizeof(NoBusca));
    if (!novo_no) return false;

    novo_no->x = x;
    novo_no->y = y;
    novo_no->prox = tb->tabela[indice];
    tb->tabela[indice] = novo_no;

    return true;
}

bool tb_busca(TabelaBusca t, double x, double y) {
    if (!t) return false;
    TabelaBuscaImp* tb = (TabelaBuscaImp*)t;

    int indice = hash_coord(x, y, tb->tamanho_tabela);

    NoBusca* atual = tb->tabela[indice];
    while (atual != NULL) {
        if (fabs(atual->x - x) < 1e-9 && fabs(atual->y - y) < 1e-9) {
            return true; // Encontrou
        }
        atual = atual->prox;
    }
    return false; // Não encontrou
}