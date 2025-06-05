
#include <stdio.h>
#include <stdlib.h>
#include "geo.h"
#include "utils.h"
#include "smutreap.h"
#include "formas.h"
#include "svg.h"
#include "qry.h"  

int main(int argc, char *argv[]) {
    void *parametros = criarParametros();
    if (!parametros) {
        fprintf(stderr, "Erro ao alocar parâmetros.\n");
        return 1;
    }

    if (!processarArgumentos(parametros, argc, argv)) {
        fprintf(stderr, "Erro ao processar argumentos.\n");
        destruirParametros(parametros);
        return 2;
    }


    SmuTreap arvore = newSmuTreap(getHitCount(parametros), getPromotionRate(parametros), 10e-10);
    setPrioridadeMax(arvore, getPrioridadeMax(parametros));

    printf("Processando arquivo .geo...\n");
    processaGeo(parametros, arvore);
    printf("✓ Arquivo GEO processado com sucesso.\n");

    char *caminho_svg_base = getCaminhoSvgBase(parametros);
    if (!caminho_svg_base) {
        fprintf(stderr, "Erro ao obter caminho do SVG base\n");
        killSmuTreap(arvore);
        destruirParametros(parametros);
        return 1;
    }
    
    printf("Gerando SVG base em: %s\n", caminho_svg_base);


    freopen("log_debug_formas.txt", "w", stdout); // Redireciona tudo do printf
    if (gerarSVG(arvore, NULL, caminho_svg_base)) {
      
        printf("✓ Arquivo SVG base gerado com sucesso!\n");
    } else {
        printf("✗ Erro ao gerar arquivo SVG base\n");
    }
    free(caminho_svg_base);
    
   
    if (temArquivoQry(parametros)) {
        printf("\n=== PROCESSANDO CONSULTAS (.qry) ===\n");
 
        printf("Processando arquivo .qry...\n");
        /*processaQry(parametros, arvore);
        printf("✓ Arquivo QRY processado com sucesso.\n");

        char *caminho_svg_consulta = getCaminhoSvgConsulta(parametros);
        char *caminho_txt_consulta = getCaminhoTxtConsulta(parametros);
        
        if (caminho_svg_consulta) {
            printf("✓ SVG de consulta gerado: %s\n", caminho_svg_consulta);
            free(caminho_svg_consulta);
        }
        
        if (caminho_txt_consulta) {
            printf("✓ Relatório TXT gerado: %s\n", caminho_txt_consulta);
            free(caminho_txt_consulta);
        }
        
        printf("=== CONSULTAS CONCLUÍDAS ===\n");
    } else {
        printf("Nenhum arquivo .qry fornecido - processamento concluído.\n");
    }*/
    }

    // Limpeza final
    destruirParametros(parametros);
    killSmuTreap(arvore);
    fclose(stdout); // Encerra a gravação no arquivo

    return 0;
}