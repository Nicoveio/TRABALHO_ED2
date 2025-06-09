#ifndef FORMAS_H
#define FORMAS_H

#include <stdbool.h>
#include <stdio.h>
#include "smutreap.h" 


//-------------------------------------
// Constantes e Tipos de Dados Públicos
//-------------------------------------

// Identificadores para os tipos de formas geométricas
#define CIRCULO   1
#define RETANGULO 2
#define LINHA     3
#define TEXTO     4

/**
 * @brief Tipo opaco para uma forma geométrica genérica.
 * Internamente, é um ponteiro para uma struct 'forma' que encapsula
 * o tipo e um ponteiro para a struct da forma específica (Circle, Rect, etc.).
 */
typedef void* Forma;


//-------------------------------------------------
// Funções de Ciclo de Vida (Criação e Destruição)
//-------------------------------------------------

/**
 * @brief Cria uma instância de uma forma genérica com base no tipo.
 * @param tipo O tipo da forma a ser criada (ex: CIRCULO, RETANGULO).
 * @return Um ponteiro para a nova forma (Info) ou NULL em caso de erro.
 */
Info criaForma(DescritorTipoInfo tipo);

/**
 * @brief Libera toda a memória associada a uma forma.
 * @param i Ponteiro para a forma (Info) a ser destruída.
 */
void liberaForma(Info i);


//-------------------------------------------------------
// Funções de Configuração de Atributos (Setters)
//-------------------------------------------------------

/**
 * @brief Configura os atributos de um Círculo.
 */
void setCircle(Info i, int id, double x, double y, double r, const char* corb, const char* corp);

/**
 * @brief Configura os atributos de um Retângulo.
 */
void setRect(Info i, int id, double x, double y, double w, double h, const char* corb, const char* corp);

/**
 * @brief Configura os atributos de uma Linha.
 */
void setLine(Info i, int id, double x1, double y1, double x2, double y2, const char* cor);

/**
 * @brief Configura os atributos de um Texto.
 */
void setText(Info i, int id, double x, double y, const char* corb, const char* corp, char a, const char* txto,
             const char* fontFamily, const char* fontWeight, int fontSize);

/**
 * @brief Define as cores de borda e preenchimento de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @param corb Nova cor da borda.
 * @param corp Nova cor do preenchimento.
 */
void formaSetCores(Info i, const char* corb, const char* corp);

/**
 * @brief Define a largura da borda de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @param nova_largura A nova espessura da borda.
 */
void formaSetLarguraBorda(Info i, double nova_largura);

/**
 * @brief Inverte as cores de borda e preenchimento de uma forma.
 * @param i Ponteiro para a forma (Info).
 */
void formaInverteCores(Info i);

/**
 * @brief Move a âncora de uma forma para uma nova coordenada.
 * @param i Ponteiro para a forma (Info).
 * @param x Nova coordenada x da âncora.
 * @param y Nova coordenada y da âncora.
 */
void formaMovePara(Info i, double x, double y);


//--------------------------------------------------
// Funções de Consulta de Atributos (Getters)
//--------------------------------------------------

/**
 * @brief Obtém o tipo de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @return O descritor do tipo da forma (CIRCULO, RETANGULO, etc.).
 */
DescritorTipoInfo formaGetTipo(Info i);

/**
 * @brief Obtém o ID de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @return O ID numérico da forma.
 */
int formaGetId(Info i);

/**
 * @brief Obtém as coordenadas da âncora de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @param x Ponteiro de saída para a coordenada x da âncora.
 * @param y Ponteiro de saída para a coordenada y da âncora.
 */
void GetXY(double *x, double *y, Forma f);


//----------------------------------------
// Funções de Geometria e Colisão
//----------------------------------------

/**
 * @brief Calcula a "área" de uma forma para o comando 'disp'.
 * @param i Ponteiro para a forma (Info).
 * @return O valor da área calculada conforme as regras da especificação.
 */
double formaGetArea(Info i);

/**
 * @brief Calcula o retângulo mínimo que envolve a forma (bounding box).
 * @param tipo Descritor do tipo da forma.
 * @param i Ponteiro para a forma (Info).
 * @param x Saída: coordenada x do canto inferior esquerdo do bounding box.
 * @param y Saída: coordenada y do canto inferior esquerdo do bounding box.
 * @param w Saída: largura do bounding box.
 * @param h Saída: altura do bounding box.
 */
void formaCalculaBoundingBox(DescritorTipoInfo tipo, Info i, double *x, double *y, double *w, double *h);

/**
 * @brief Verifica se uma forma está inteiramente contida em uma região retangular.
 * Usa uma tolerância geométrica interna para lidar com imprecisões.
 * @param i Ponteiro para a forma (Info).
 * @param x1, y1, x2, y2 Coordenadas que definem a região de busca.
 * @return true se a forma está contida, false caso contrário.
 */
bool formaDentroDeRegiao(SmuTreap t, Node n, Info i, double x1, double y1, double x2, double y2);

/**
 * @brief Verifica se um ponto (x,y) está contido na geometria de uma forma.
 * @param i Ponteiro para a forma (Info).
 * @param x Coordenada x do ponto.
 * @param y Coordenada y do ponto.
 * @return true se o ponto é interno à forma, false caso contrário.
 */
bool formaPontoInternoAInfo(SmuTreap t, Node n, Info i, double x, double y);

/**
 * @brief Obtém as coordenadas das duas extremidades de uma forma do tipo Linha.
 * * @param i Ponteiro para a forma (Info) do tipo Linha.
 * @param x1 Ponteiro de saída para a coordenada x do primeiro ponto.
 * @param y1 Ponteiro de saída para a coordenada y do primeiro ponto.
 * @param x2 Ponteiro de saída para a coordenada x do segundo ponto.
 * @param y2 Ponteiro de saída para a coordenada y do segundo ponto.
 */
void linhaGetPontos(Info i, double *x1, double *y1, double *x2, double *y2);


//-------------------------------------------
// Funções de Utilitários e Relatórios
//-------------------------------------------

/**
 * @brief Cria uma cópia profunda (independente) de uma forma.
 * @param i Ponteiro para a forma (Info) a ser clonada.
 * @return Um ponteiro para a nova forma clonada (Info).
 */
Info formaClona(Info i);

/**
 * @brief Imprime um resumo formatado dos dados de uma forma em um arquivo/stream.
 * @param stream O stream de saída (ex: stdout para o terminal ou um FILE* para um arquivo).
 * @param i Ponteiro para a forma (Info) a ser impressa.
 */
void formaFprintfResumo(FILE* stream, Info i);

/**
 * @brief Imprime um resumo formatado dos dados de uma forma no terminal (stdout).
 * @param i Ponteiro para a forma (Info) a ser impressa.
 */
void formaPrintResumo(Info i);


#endif 