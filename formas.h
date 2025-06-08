#ifndef FORMAS_H
#define FORMAS_H

// Inclusão do módulo SmuTreap, onde estão definidas as estruturas Node, Info, etc.
#include "smutreap.h"  

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // Necessário para o tipo bool

// =====================
// Constantes e Tipos
// =====================

// Identificadores para os tipos de formas geométricas
#define CIRCULO 1
#define RETANGULO 2
#define LINHA 3
#define TEXTO 4

// Tipo opaco Forma, representa uma forma geométrica genérica.
// Internamente, encapsula uma estrutura específica (círculo, retângulo, etc.).
typedef void* Forma;

// ===========================================
// Funções de criação e destruição de formas
// ===========================================

/**
 * Cria uma forma genérica do tipo especificado (CIRCULO, RETANGULO, LINHA, TEXTO).
 * Aloca o wrapper Forma e a estrutura interna correspondente.
 * 
 * @param tipo  Descritor do tipo da forma (CIRCULO, RETANGULO, etc.).
 * @return      Ponteiro para a Forma criada.
 */
Forma criaForma(DescritorTipoInfo tipo);

/**
 * Libera a memória associada a uma forma, incluindo a estrutura interna.
 * 
 * @param f     Ponteiro para a Forma a ser destruída.
 */
void liberaForma(Forma f);

// ===========================
// Funções set (preenchimento)
// ===========================

/**
 * Inicializa os dados de um círculo na forma especificada.
 */
void setCircle(Forma f, int id, double x, double y, double r, const char *corb, const char *corp);

/**
 * Inicializa os dados de um retângulo na forma especificada.
 */
void setRect(Forma f, int id, double x, double y, double w, double h, const char *corb, const char *corp);

/**
 * Inicializa os dados de uma linha na forma especificada.
 */
void setLine(Forma f, int id, double x1, double y1, double x2, double y2, const char *cor);

/**
 * Inicializa os dados de um texto na forma especificada.
 */
void setText(Forma f, int id, double x, double y, const char* corb, const char* corp, char a, const char* txto,
             const char* fontFamily, const char* fontWeight, int fontSize);
// ==========================================================
// Funções de interação com SmuTreap usando Info (void*)
// ==========================================================

/**
 * Verifica se a forma está completamente contida dentro do retângulo definido
 * pelos pontos (x1, y1) e (x2, y2).
 */
bool formaDentroDeRegiao(SmuTreap t, Node n, Info i, double x1, double y1, double x2, double y2);

/**
 * Verifica se o ponto (x, y) está dentro da forma representada por Info.
 */
bool formaPontoInternoAInfo(SmuTreap t, Node n, Info i, double x, double y);

/**
 * Visita um nó da árvore que contém a forma, com base em sua posição.
 * Permite execução de operações personalizadas com auxílio do ponteiro aux.
 */
void formaVisitaNo(SmuTreap t, Node n, Info i, double x, double y, void *aux);

/**
 * Calcula a bounding box (retângulo mínimo que envolve a forma).
 * 
 * @param tp    Descritor do tipo da forma.
 * @param i     Ponteiro para a forma (Info).
 * @param x     Saída: coordenada x da âncora da bounding box.
 * @param y     Saída: coordenada y da âncora da bounding box.
 * @param w     Saída: largura da bounding box.
 * @param h     Saída: altura da bounding box.
 */
void formaCalculaBoundingBox(DescritorTipoInfo tp, Info i, double *x, double *y, double *w, double *h);

/**
 * Exibe os dados internos da forma no terminal. Útil para depuração.
 */
void printForma(Forma f);

// ============================
// Funções adicionais utilitárias
// ============================

/**
 * Verifica se a âncora da forma coincide com o ponto (x, y), com margem epsilon.
 * 
 * @param i     Ponteiro para a forma (Info).
 * @param x     Coordenada x a comparar.
 * @param y     Coordenada y a comparar.
 * @return      true se as âncoras forem praticamente iguais, false caso contrário.
 */
bool formaAncoraIgual(Info i, double x, double y);

/**
 * Obtém o identificador da forma.
 * 
 * @param i     Ponteiro para a forma (Info).
 * @return      ID da forma.
 */
int formaGetId(Info i);

/**
 * Move a âncora da forma para as coordenadas (x, y).
 * 
 * @param i     Ponteiro para a forma (Info).
 */
void formaMovePara(Info i, double x, double y);

/**
 * Cria uma cópia independente da forma (clonagem profunda).
 * 
 * @param i     Ponteiro para a forma a ser clonada (Info).
 * @return      Forma clonada (como Forma, pode ser convertida para Info).
 */
Forma formaClona(Info i);

/**
 * Inverte as cores da forma (borda <-> preenchimento).
 * 
 * @param i     Ponteiro para a forma (Info).
 */
void formaInverteCores(Info i);

/**
 * Define as cores de borda e preenchimento da forma.
 * 
 * @param i     Ponteiro para a forma (Info).
 * @param corb  Nova cor da borda.
 * @param corp  Nova cor do preenchimento.
 */
void formaSetCores(Info i, const char* corb, const char* corp);

/**
 * Exibe um resumo da forma no terminal: tipo, ID, posição, cores, etc.
 * 
 * @param i     Ponteiro para a forma (Info).
 */
void formaPrintResumo(Info i);

// Adicione estas declarações ao seu arquivo formas.h

// Função para obter o tipo de uma forma
DescritorTipoInfo formaGetTipo(Info i);

// Função para obter o raio de um círculo
double circuloGetRaio(Info i);

// Função para obter dimensões de um retângulo
double retanguloGetLargura(Info i);
double retanguloGetAltura(Info i);

// Função para obter os pontos de uma linha
void linhaGetPontos(Info i, double *x1, double *y1, double *x2, double *y2);

// Funções para obter conteúdo de texto
char* textoGetConteudo(Info i);
int textoGetConteudoCopia(Info i, char *buffer, int tamanho_buffer);
void GetXY(double *x, double *y, Forma f);
int getFormaId(Forma f);
int getDescritorForma(Forma f);
// ...
void formaPrintResumo(Info i);

// Adicione esta declaração
void formaFprintfResumo(FILE* stream, Info i);

// ...


#endif // FORMAS_H
