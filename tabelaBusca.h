#ifndef TABELA_BUSCA_H
#define TABELA_BUSCA_H

#include <stdbool.h>

/**
 * @brief Tipo opaco para a Tabela de Busca.
 * Usada para verificação rápida de existência de coordenadas.
 */
typedef void* TabelaBusca;

/**
 * @brief Cria uma nova tabela de busca com um tamanho base.
 * @param tamanho_base O tamanho inicial do vetor interno da tabela.
 * @return Um ponteiro para a nova TabelaBusca, ou NULL em caso de erro.
 */
TabelaBusca tb_cria(int tamanho_base);

/**
 * @brief Libera toda a memória usada pela tabela de busca.
 * @param tb A tabela de busca a ser liberada.
 */
void tb_libera(TabelaBusca tb);

/**
 * @brief Insere um par de coordenadas (x, y) na tabela de busca.
 * @param tb A tabela de busca.
 * @param x A coordenada x.
 * @param y A coordenada y.
 * @return true se a inserção foi bem-sucedida, false caso contrário.
 */
bool tb_insere(TabelaBusca tb, double x, double y);

/**
 * @brief Busca por um par de coordenadas (x, y) na tabela de busca.
 * @param tb A tabela de busca.
 * @param x A coordenada x a ser buscada.
 * @param y A coordenada y a ser buscada.
 * @return true se as coordenadas já existem na tabela, false caso contrário.
 */
bool tb_busca(TabelaBusca tb, double x, double y);

#endif // TABELA_BUSCA_H