#ifndef GEO_H
#define GEO_H

#include "smutreap.h" // Necessário para o tipo SmuTreap


// =                      MÓDULO DE LEITURA GEO                      =
// =      Responsável por ler o arquivo de entrada .geo e            =
// =        popular a estrutura de dados principal (árvore).         =



/**
 * @brief Processa um arquivo de entrada .geo, lendo as formas e inserindo-as na árvore.
 * * Esta função abre o arquivo .geo especificado nos parâmetros, lê cada linha,
 * interpreta os comandos de criação de formas (círculo, retângulo, linha, texto),
 * e utiliza as funções dos módulos 'formas' e 'smutreap' para criar e inserir
 * cada forma na árvore espacial.
 * * @param parametros Um ponteiro opaco para a struct de parâmetros (criada por utils.c),
 * que contém o caminho para o arquivo .geo.
 * @param t A SmuTreap (árvore) onde as formas lidas serão inseridas. A árvore
 * deve ter sido previamente criada e estar vazia.
 */
void processaGeo(void *parametros, SmuTreap t);


#endif // GEO_H