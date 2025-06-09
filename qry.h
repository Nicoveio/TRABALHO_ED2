#ifndef QRY_H
#define QRY_H

// qry.h precisa conhecer os tipos SmuTreap e InformacoesAdicionais,
// por isso inclui os headers onde eles são definidos.
#include "smutreap.h"
#include "svg.h"

/**
 * @brief Processa um arquivo de consulta (.qry), executando os comandos
 * sobre a árvore de formas e gerando os resultados.
 * * Esta é a função principal do módulo de consulta. Ela orquestra a leitura
 * do arquivo .qry, o despacho dos comandos para funções internas e a
 * coleta de dados para a geração dos arquivos de saída (.txt e .svg).
 * * @param parametros Um ponteiro opaco contendo as configurações do programa
 * (caminhos de arquivo, etc.), obtido a partir de utils.h.
 * @param arvore A árvore SmuTreap já populada com as formas do arquivo .geo.
 * * @return Um ponteiro opaco do tipo InformacoesAdicionais. Este ponteiro contém
 * todos os dados necessários para que o módulo SVG desenhe os efeitos visuais
 * das consultas (pontos, retângulos, vetores, etc.).
 * O chamador (a função main) é responsável por liberar a memória deste ponteiro
 * quando não for mais necessário. Em caso de erro fatal durante o processamento,
 * a função retorna NULL.
 */
InformacoesAdicionais processaQry(void *parametros, SmuTreap arvore);

/**
 * @brief Libera a memória alocada para uma estrutura InformacoesAdicionais.
 * @param infos O ponteiro para a estrutura a ser liberada.
 */
void destruirInformacoesAdicionais(InformacoesAdicionais infos);

#endif 

