// qry.h
#ifndef QRY_H
#define QRY_H

#include "smutreap.h"
#include "lista.h"
#include <stdio.h>

typedef void* ContextoQry;  // Tipo opaco

// Funções principais
ContextoQry inicializar_contexto(SmuTreap arvore, FILE *txt_file, FILE *svg_file);
void finalizar_contexto(ContextoQry ctx);

// Comandos
void comando_selr(ContextoQry ctx, int n, double x, double y, double w, double h);

void processaQry(void *parametros, SmuTreap arvore);

#endif