#ifndef SVG_H
#define SVG_H

#include "smutreap.h"
#include "formas.h"
#include <stdio.h>

// Tipo opaco para contexto de geração SVG e Informações adicionais relacionado a manipulação das informações
typedef void* ContextoSVG;
typedef void* InformacoesAdicionais;

/**
 * @brief Gera um arquivo SVG a partir das formas na árvore e de informações adicionais.
 * * Esta função desenha todas as formas contidas na SmuTreap e, opcionalmente,
 * desenha efeitos visuais extras (pontos, vetores, destaques) que são
 * passados através da struct InformacoesAdicionais.
 * * @param arvore    A SmuTreap contendo as formas geométricas a serem desenhadas.
 * @param infos     Ponteiro para a struct de InformacoesAdicionais, contendo dados
 * para efeitos visuais (pontos, destaques, etc.). Pode ser NULL se
 * nenhum efeito adicional for necessário (como no SVG base).
 * @param caminho   Caminho completo do arquivo SVG a ser criado/sobrescrito.
 * @return          true se o arquivo foi gerado com sucesso, false caso contrário.
 */
bool gerarSVG(SmuTreap arvore, InformacoesAdicionais infos, const char *caminho);


/**
 * Calcula as dimensões do canvas necessárias para todas as formas.
 * 
 * @param arvore    SmuTreap contendo as formas
 * @param largura   Saída: largura mínima do canvas
 * @param altura    Saída: altura mínima do canvas
 */
void calcularDimensoesCanvas(SmuTreap arvore, double *largura, double *altura);

#endif // SVG_H