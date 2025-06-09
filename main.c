#include <stdio.h>
#include <stdlib.h>
#include "geo.h"
#include "utils.h"
#include "smutreap.h"
#include "formas.h"
#include "svg.h"
#include "qry.h"
#include <math.h>

int main(int argc, char *argv[]) {
    void *parametros = criarParametros();
    processarArgumentos(parametros, argc, argv); // Assumindo tratamento de erro interno
    SmuTreap arvore = newSmuTreap(getHitCount(parametros), getPromotionRate(parametros), 0.000001);
    setPrioridadeMax(arvore, getPrioridadeMax(parametros));
    processaGeo(parametros, arvore);
    printf("✓ Arquivo GEO processado com sucesso.\n");

    char* caminho_dot_base = getCaminhoDotBase(parametros);
    if (caminho_dot_base) {
        printf("Gerando .dot da árvore base em: %s\n", caminho_dot_base);
        printDotSmuTreap(arvore, caminho_dot_base);
        free(caminho_dot_base);
    }

    char *caminho_svg_base = getCaminhoSvgBase(parametros);
    if (caminho_svg_base) {
        printf("Gerando SVG base em: %s\n", caminho_svg_base);
        gerarSVG(arvore, NULL, caminho_svg_base);
        free(caminho_svg_base);
    }


    if (temArquivoQry(parametros)) {
        printf("\n=== PROCESSANDO CONSULTAS (.qry) ===\n");
        InformacoesAdicionais infosDaConsulta = processaQry(parametros, arvore);

        if (infosDaConsulta) {
            printf("✓ Arquivo QRY processado com sucesso.\n");
            

            char* caminho_dot_consulta = getCaminhoDotConsulta(parametros);
            if (caminho_dot_consulta) {
                printf("Gerando .dot da árvore final em: %s\n", caminho_dot_consulta);
                printDotSmuTreap(arvore, caminho_dot_consulta);
                free(caminho_dot_consulta);
            }
   

            char *caminho_svg_consulta = getCaminhoSvgConsulta(parametros);
            if (caminho_svg_consulta) {
                printf("Gerando SVG de consulta em: %s\n", caminho_svg_consulta);
                gerarSVG(arvore, infosDaConsulta, caminho_svg_consulta);
                free(caminho_svg_consulta);
            }

        
        } else {
            fprintf(stderr, "✗ Erro fatal durante o processamento do arquivo QRY.\n");
        }
        destruirInformacoesAdicionais(infosDaConsulta);
    }

    // 5. Limpeza Final
    printf("\nFinalizando o programa...\n");
    destruirParametros(parametros);
    killSmuTreap(arvore);


    printf("Programa concluído.\n");
    return 0;
}