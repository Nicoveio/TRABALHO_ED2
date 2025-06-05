#ifndef SVG_H
#define SVG_H

#include "smutreap.h"
#include "formas.h"
#include <stdio.h>

// Tipo opaco para contexto de geração SVG
typedef void* ContextoSVG;
typedef void* InformacoesAdicionais;

/**
 * Gera um arquivo SVG a partir das formas armazenadas na SmuTreap.
 * 
 * @param arvore    SmuTreap contendo as formas geométricas
 * @param caminho   Caminho completo do arquivo SVG a ser gerado
 * @return          true se o arquivo foi gerado com sucesso, false caso contrário
 */
bool gerarSVG(SmuTreap arvore, InformacoesAdicionais infos, const char *caminho);

/**
 * Gera um arquivo SVG destacando formas específicas (para consultas).
 * 
 * @param arvore        SmuTreap contendo todas as formas
 * @param nos_destaque  Lista de nós que devem ser destacados
 * @param caminho       Caminho completo do arquivo SVG a ser gerado
 * @return              true se o arquivo foi gerado com sucesso, false caso contrário
 */
bool gerarSVGComDestaque(SmuTreap arvore, Lista nos_destaque, const char *caminho);
void contarNosCallback(SmuTreap t, Node n, Info i, double x, double y, void *aux);
/**
 * Calcula as dimensões do canvas necessárias para todas as formas.
 * 
 * @param arvore    SmuTreap contendo as formas
 * @param largura   Saída: largura mínima do canvas
 * @param altura    Saída: altura mínima do canvas
 */
void calcularDimensoesCanvas(SmuTreap arvore, double *largura, double *altura);

#endif // SVG_H