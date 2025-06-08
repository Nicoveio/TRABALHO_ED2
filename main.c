#include <stdio.h>
#include <stdlib.h>
#include "geo.h"
#include "utils.h"
#include "smutreap.h"
#include "formas.h"
#include "svg.h"
#include "qry.h"  

int main(int argc, char *argv[]) {
    // 1. Processamento de Argumentos
    void *parametros = criarParametros();
    if (!parametros) {
        fprintf(stderr, "Erro ao alocar parâmetros.\n");
        return 1;
    }

    if (!processarArgumentos(parametros, argc, argv)) {
        destruirParametros(parametros);
        return 2;
    }

    // 2. Criação da Árvore e Processamento do .geo
    SmuTreap arvore = newSmuTreap(getHitCount(parametros), getPromotionRate(parametros), 0.0000000001);
    setPrioridadeMax(arvore, getPrioridadeMax(parametros));

    printf("Processando arquivo .geo...\n");
    processaGeo(parametros, arvore);
    printf("✓ Arquivo GEO processado com sucesso.\n");

    // 3. Geração do SVG Base (antes das consultas)
    char *caminho_svg_base = getCaminhoSvgBase(parametros);
    if (caminho_svg_base) {
        printf("Gerando SVG base em: %s\n", caminho_svg_base);
        if (gerarSVG(arvore, NULL, caminho_svg_base)) {
            printf("✓ Arquivo SVG base gerado com sucesso!\n");
        } else {
            printf("✗ Erro ao gerar arquivo SVG base.\n");
        }
        free(caminho_svg_base);
    }

    // 4. Processamento do .qry (se existir)
    if (temArquivoQry(parametros)) {
        printf("\n=== PROCESSANDO CONSULTAS (.qry) ===\n");

        InformacoesAdicionais infosDaConsulta = processaQry(parametros, arvore);

        if (infosDaConsulta) {
            printf("✓ Arquivo QRY processado com sucesso.\n");

            char *caminho_svg_consulta = getCaminhoSvgConsulta(parametros);
            if (caminho_svg_consulta) {
                printf("Gerando SVG de consulta em: %s\n", caminho_svg_consulta);
                gerarSVG(arvore, infosDaConsulta, caminho_svg_consulta);
                printf("✓ SVG de consulta gerado.\n");
                free(caminho_svg_consulta);
            }

            char *caminho_txt_consulta = getCaminhoTxtConsulta(parametros);
            if (caminho_txt_consulta) {
                printf("✓ Relatório TXT gerado em: %s\n", caminho_txt_consulta);
                free(caminho_txt_consulta);
            }

            // Lembre-se de criar a função para liberar esta memória!
            // destruirInformacoesAdicionais(infosDaConsulta);
            
            printf("=== CONSULTAS CONCLUÍDAS ===\n");
        } else {
            fprintf(stderr, "✗ Erro fatal durante o processamento do arquivo QRY.\n");
        }
    } else {
        printf("\nNenhum arquivo .qry fornecido - processamento concluído.\n");
    }

    // 5. Limpeza Final
    printf("\nFinalizando o programa e liberando memória...\n");
    destruirParametros(parametros);
    killSmuTreap(arvore);

    printf("Programa concluído.\n");
    return 0;
}