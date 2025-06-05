#include "geo.h"
#include <stdlib.h>
#include <string.h>
#include "smutreap.h"
#include "formas.h"
#include "utils.h"
#include "svg.h"

#define MAX_TENTATIVAS 10000
#define INCREMENTO 0.0001




void processaGeo(void *parametros, SmuTreap t) {
    char *caminhoGeo = getCaminhoCompletoGeo(parametros);
    if (!caminhoGeo) {
        fprintf(stderr, "ERRO: Caminho do arquivo GEO é inválido ou inacessível.\n");
        exit(1);
    }

    FILE *geo = fopen(caminhoGeo, "r");
    free(caminhoGeo);
    if (!geo) {
        fprintf(stderr, "ERRO: Falha ao abrir o arquivo GEO.\n");
        exit(1);
    }

    char linha[512], comando[4];
    char corb[64], corp[64], cor[64], txto[1024], a;
    char estiloFontFamily[32] = "sans";
    char estiloFontWeight[8] = "normal";
    int estiloFontSize = 12;

    int linha_numero = 0, formas_inseridas = 0, formas_tentativas = 0;
    int id;
    double x, y, r, w, h, x1, y1, x2, y2;

    printf("DEBUG: Iniciando processamento do arquivo GEO...\n");

    while (fgets(linha, sizeof(linha), geo)) {
        linha_numero++;
        char *ptr = linha;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r' || *ptr == '#') continue;

        sscanf(ptr, "%3s", comando);
        if (strcmp(comando, "ts") == 0) {
            sscanf(ptr, "ts %31s %7s %d", estiloFontFamily, estiloFontWeight, &estiloFontSize);
            printf("DEBUG: Estilo de texto definido (Font: %s, Weight: %s, Size: %d)\n",
                   estiloFontFamily, estiloFontWeight, estiloFontSize);
            continue;
        }

        Forma forma = NULL;
        Node verificacao = NULL;
        int tentativas = 0;

        formas_tentativas++;

        if (strcmp(comando, "c") == 0) {
            if (sscanf(ptr, "c %d %lf %lf %lf %63s %63s", &id, &x, &y, &r, corb, corp) != 6) {
                fprintf(stderr, "ERRO: Falha ao ler círculo na linha %d: %s", linha_numero, linha);
                continue;
            }
            forma = criaForma(CIRCULO);
            setCircle(forma, id, x, y, r, corb, corp);
            while ((verificacao = getNodeSmuT(t, x, y)) && tentativas++ < MAX_TENTATIVAS)
                x += INCREMENTO;
            if (tentativas >= MAX_TENTATIVAS) {
                fprintf(stderr, "ERRO: Falha ao inserir CIRCULO ID=%d após %d tentativas.\n", id, MAX_TENTATIVAS);
                liberaForma(forma);
                continue;
            }
            insertSmuT(t, x, y, forma, CIRCULO, formaCalculaBoundingBox);
            if (getNodeSmuT(t, x, y)) {
                formas_inseridas++;
            }

        } else if (strcmp(comando, "r") == 0) {
            if (sscanf(ptr, "r %d %lf %lf %lf %lf %63s %63s", &id, &x, &y, &w, &h, corb, corp) != 7) {
                fprintf(stderr, "ERRO: Falha ao ler retângulo na linha %d: %s", linha_numero, linha);
                continue;
            }
            forma = criaForma(RETANGULO);
            setRect(forma, id, x, y, w, h, corb, corp);
            while ((verificacao = getNodeSmuT(t, x, y)) && tentativas++ < MAX_TENTATIVAS)
                x += INCREMENTO;
            if (tentativas >= MAX_TENTATIVAS) {
                fprintf(stderr, "ERRO: Falha ao inserir RETANGULO ID=%d após %d tentativas.\n", id, MAX_TENTATIVAS);
                liberaForma(forma);
                continue;
            }
            insertSmuT(t, x, y, forma, RETANGULO, formaCalculaBoundingBox);
            if (getNodeSmuT(t, x, y)) {
                formas_inseridas++;
            }

        } else if (strcmp(comando, "l") == 0) {
            if (sscanf(ptr, "l %d %lf %lf %lf %lf %63s", &id, &x1, &y1, &x2, &y2, cor) != 6) {
                fprintf(stderr, "ERRO: Falha ao ler linha na linha %d: %s", linha_numero, linha);
                continue;
            }
            forma = criaForma(LINHA);
            setLine(forma, id, x1, y1, x2, y2, cor);
            double x_med = (x1 + x2) / 2.0;
            double y_med = (y1 + y2) / 2.0;
            x = x_med; y = y_med;
            while ((verificacao = getNodeSmuT(t, x, y)) && tentativas++ < MAX_TENTATIVAS)
                x += INCREMENTO;
            if (tentativas >= MAX_TENTATIVAS) {
                fprintf(stderr, "ERRO: Falha ao inserir LINHA ID=%d após %d tentativas.\n", id, MAX_TENTATIVAS);
                liberaForma(forma);
                continue;
            }
            insertSmuT(t, x, y, forma, LINHA, formaCalculaBoundingBox);
            if (getNodeSmuT(t, x, y)) {
                formas_inseridas++;
            }

        } else if (strcmp(comando, "t") == 0) {
            if (sscanf(ptr, "t %d %lf %lf %63s %63s %c %1023[^\n]", &id, &x, &y, corb, corp, &a, txto) != 7) {
                fprintf(stderr, "ERRO: Falha ao ler texto na linha %d: %s", linha_numero, linha);
                continue;
            }
            size_t len = strlen(txto);
            if (len >= 2 && txto[0] == '"' && txto[len - 1] == '"') {
                txto[len - 1] = '\0';
                memmove(txto, txto + 1, len - 1);
            }

            forma = criaForma(TEXTO);
            setText(forma, id, x, y, corb, corp, a, txto,
                    estiloFontFamily, estiloFontWeight, estiloFontSize);
            while ((verificacao = getNodeSmuT(t, x, y)) && tentativas++ < MAX_TENTATIVAS)
                x += INCREMENTO;
            if (tentativas >= MAX_TENTATIVAS) {
                fprintf(stderr, "ERRO: Falha ao inserir TEXTO ID=%d após %d tentativas.\n", id, MAX_TENTATIVAS);
                liberaForma(forma);
                continue;
            }
            insertSmuT(t, x, y, forma, TEXTO, formaCalculaBoundingBox);
            if (getNodeSmuT(t, x, y)) {
                formas_inseridas++;
            }

        } else {
            fprintf(stderr, "AVISO: Comando desconhecido '%s' na linha %d: %s", comando, linha_numero, linha);
            continue;
        }
    }

    fclose(geo);

    char *caminho_dot = getCaminhoSvgBase(parametros);
        if (!caminho_dot) {
          fprintf(stderr, "Erro ao criar caminho do arquivo DOT\n");
          return;
            }

        size_t len = strlen(caminho_dot);
        strcpy(caminho_dot + len - 3, "dot");

if (!printDotSmuTreap(t, caminho_dot)) {
    fprintf(stderr, "ERRO: Falha ao criar arquivo DOT.\n");
} else {
    printf("DEBUG: Arquivo DOT gerado com sucesso.\n");
}

free(caminho_dot);
    printf("\n=== RELATÓRIO FINAL ===\n");
    printf("Linhas processadas: %d\n", linha_numero);
    printf("Tentativas de inserção: %d\n", formas_tentativas);
    printf("Formas inseridas com sucesso: %d\n", formas_inseridas);
    printf("Formas rejeitadas: %d\n", formas_tentativas - formas_inseridas);

    if (formas_tentativas > formas_inseridas) {
        printf("AVISO: Algumas formas não foram inseridas corretamente.\n");
        printf("Causas possíveis:\n");
        printf("- Coordenadas duplicadas ou próximas demais\n");
        printf("- Estrutura da árvore pode estar rejeitando inserções\n");
        printf("- Número de tentativas excedido\n");
    }
}
