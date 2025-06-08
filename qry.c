
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "qry.h"
#include "utils.h"
#include "formas.h"
#include "lista.h"
#include "smutreap.h"

// =============================================================
// ESTRUTURAS DE DADOS INTERNAS
// =============================================================


typedef struct { double x, y; } Coord;

typedef struct {
    double x, y, w, h;
} BoundingBox;

typedef struct {
    Coord origem;
    Coord destino;
} VetorMovimento;

// A implementação da struct opaca InformacoesAdicionais (vinda de svg.c).
typedef struct InformacoesAdicionaisImp {
    Lista pontos_x_vermelhos;
    Lista pontos_hashtag_vermelhos;
    Lista pontos_ancoras;
    Lista formas_destacadas;
    Lista retangulos_selecao;
    Lista vetores_movimento;
    Lista formas_clonadas;
} InformacoesAdicionaisImp;

// A struct de contexto que definimos.
typedef struct {
    SmuTreap arvore;
    FILE* arquivoTxt;
    InformacoesAdicionais infosSvg;
    Lista selecionados[100];
} ContextoConsulta;


typedef struct { 
    int id_procurado; 
} AuxBuscaId;


// =============================================================
// DECLARAÇÃO DAS FUNÇÕES "TRABALHADORAS" (INTERNAS)
// =============================================================

static bool inicializa_infosAdicionais(InformacoesAdicionaisImp* infos);
static void zera_selecionados(Lista* array_listas);

// Uma função para cada comando do .qry
static void processa_selr(ContextoConsulta* contexto, int n, double x, double y, double w, double h);
static void processa_seli(ContextoConsulta* contexto, int n, double x, double y);
static void processa_disp(ContextoConsulta* contexto, int id, int n);
static void processa_transp(ContextoConsulta* contexto, int id, double x, double y);
static void processa_cln(ContextoConsulta* contexto, int n, double dx, double dy);
static void processa_spy(ContextoConsulta* contexto, int id);
static void processa_cmflg(ContextoConsulta* contexto, int id, const char* corb, const char* corp, const char* w);
static void processa_blow(ContextoConsulta* contexto, int id);


// ======================================================================
// IMPLEMENTAÇÃO DA FUNÇÃO PÚBLICA processaQry
// ======================================================================

InformacoesAdicionais processaQry(void *parametros, SmuTreap arvore) {

    // --- FASE 1: PREPARAÇÃO DO AMBIENTE ---
    char* caminho_qry = getCaminhoCompletoQry(parametros);
    char* caminho_txt = getCaminhoTxtConsulta(parametros);
    FILE* arq_qry = fopen(caminho_qry, "r");
    FILE* arq_txt = fopen(caminho_txt, "w");

    if (!arq_qry || !arq_txt) { /* Tratamento de erro... */ return NULL; }

    ContextoConsulta contexto;
    contexto.arvore = arvore;
    contexto.arquivoTxt = arq_txt;
    contexto.infosSvg = malloc(sizeof(InformacoesAdicionaisImp));

  if (!contexto.infosSvg || !inicializa_infosAdicionais(contexto.infosSvg)) {
    fprintf(stderr, "Erro fatal ao inicializar estruturas para consulta.\n");
  
    if (contexto.infosSvg) {
        free(contexto.infosSvg);
    }

    fclose(arq_qry);
    fclose(arq_txt);

    free(caminho_qry);
    free(caminho_txt);

    return NULL; 
}
    zera_selecionados(contexto.selecionados);


  
    printf("Lendo e processando comandos de '%s'...\n", caminho_qry);

    char linha[1024];
    while (fgets(linha, sizeof(linha), arq_qry)) {
        char comando[32];
        char* ptr = linha;

        // Ignora espaços no início da linha
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        // Ignora linhas vazias ou comentários
        if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r' || *ptr == '#') continue;

        // Lê apenas a primeira palavra para identificar o comando
        sscanf(ptr, "%31s", comando);

        if (strcmp(comando, "selr") == 0) {
            int n; double x, y, w, h;
            sscanf(ptr, "%*s %d %lf %lf %lf %lf", &n, &x, &y, &w, &h);
            processa_selr(&contexto, n, x, y, w, h);

        } else if (strcmp(comando, "seli") == 0) {
            int n; double x, y;
            sscanf(ptr, "%*s %d %lf %lf", &n, &x, &y);
            processa_seli(&contexto, n, x, y);

        } else if (strcmp(comando, "disp") == 0) {
            int id, n;
            sscanf(ptr, "%*s %d %d", &id, &n);
            processa_disp(&contexto, id, n);

        } else if (strcmp(comando, "transp") == 0) {
            int id; double x, y;
            sscanf(ptr, "%*s %d %lf %lf", &id, &x, &y);
            processa_transp(&contexto, id, x, y);

        } else if (strcmp(comando, "cln") == 0) {
            int n; double dx, dy;
            sscanf(ptr, "%*s %d %lf %lf", &n, &dx, &dy);
            processa_cln(&contexto, n, dx, dy);

        } else if (strcmp(comando, "spy") == 0) {
            int id;
            sscanf(ptr, "%*s %d", &id);
            processa_spy(&contexto, id);

        } else if (strcmp(comando, "cmflg") == 0) {
            int id;
            char corb[64], corp[64], w[16];
            sscanf(ptr, "%*s %d %63s %63s %15s", &id, corb, corp, w);
            processa_cmflg(&contexto, id, corb, corp, w);

        } else if (strcmp(comando, "blow") == 0) {
            int id;
            sscanf(ptr, "%*s %d", &id);
            processa_blow(&contexto, id);

        } else {
            fprintf(contexto.arquivoTxt, "AVISO: Comando desconhecido na linha: %s", linha);
        }
    }


    // --- FASE 3: FINALIZAÇÃO ---
    printf("Processamento de consultas concluído.\n");
    fclose(arq_qry);
    fclose(arq_txt);
    free(caminho_qry);
    free(caminho_txt);

    return contexto.infosSvg;
}


// =============================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES "TRABALHADORAS" (ESQUELETOS)
// =============================================================
static bool inicializa_infosAdicionais(InformacoesAdicionaisImp* infos) {
    // Implementação real da inicialização
    infos->pontos_x_vermelhos = lista_cria();
    infos->pontos_hashtag_vermelhos = lista_cria();
    infos->pontos_ancoras = lista_cria();
    infos->retangulos_selecao = lista_cria();
    infos->vetores_movimento = lista_cria();
    infos->formas_clonadas = lista_cria();
    infos->formas_destacadas = lista_cria();
    return true;
}
static void zera_selecionados(Lista* array_listas) {
    memset(array_listas, 0, sizeof(Lista) * 100);
}



static void processa_selr(ContextoConsulta* contexto, int n, double x, double y, double w, double h) {
    // A linha de log do comando permanece a mesma
    fprintf(contexto->arquivoTxt, "selr %d %.1f %.1f %.1f %.1f\n", n, x, y, w, h);

    // Converte o ponteiro genérico para o tipo real da struct.
    InformacoesAdicionaisImp* infos = (InformacoesAdicionaisImp*)contexto->infosSvg;

    // --- PASSO 1: A CONSULTA ESPACIAL ---
    Lista resultados = lista_cria();
    getInfosDentroRegiaoSmuT(contexto->arvore, x, y, x + w, y + h, 
                             &formaDentroDeRegiao, resultados);

    // --- PASSO 2: PROCESSAR OS RESULTADOS (TXT e SVG) ---
    
    // Variável para contar as iterações do loop
    int formas_processadas_no_loop = 0;

    Iterador it = lista_iterador(resultados);
    while (iterador_tem_proximo(it)) {
        formas_processadas_no_loop++; // Incrementa o contador do loop

        Node no_encontrado = iterador_proximo(it);
        Info info_da_forma = getInfoSmuT(contexto->arvore, no_encontrado);

        int id_forma = formaGetId(info_da_forma);
        fprintf(contexto->arquivoTxt, "  > Forma selecionada: ID=%d\n", id_forma);

        double ancora_x, ancora_y;
        GetXY(&ancora_x, &ancora_y, info_da_forma);
        
        Coord* ponto_ancora = malloc(sizeof(Coord));
        ponto_ancora->x = ancora_x;
        ponto_ancora->y = ancora_y;
        
        lista_insere(infos->pontos_ancoras, ponto_ancora);
    }
    iterador_destroi(it);

    // --- PASSO 3: REGISTRAR O RETÂNGULO DE SELEÇÃO PARA O SVG ---
    BoundingBox* bb_selecao = malloc(sizeof(BoundingBox));
    bb_selecao->x = x;
    bb_selecao->y = y;
    bb_selecao->w = w;
    bb_selecao->h = h;
    lista_insere(infos->retangulos_selecao, bb_selecao);

    // --- PASSO 4: ARMAZENAR A SELEÇÃO ---
    if (contexto->selecionados[n] != NULL) {
        lista_libera(contexto->selecionados[n]);
    }
    contexto->selecionados[n] = resultados;


    // =======================================================
    // ======== BLOCO DE DEPURAÇÃO E CONTAGEM AUTOMÁTICA =======
    // =======================================================
    
    int total_encontrado_na_lista = lista_tamanho(resultados);
    int total_ancoras_para_svg = lista_tamanho(infos->pontos_ancoras);
    
    fprintf(contexto->arquivoTxt, "\n  [RELATÓRIO DE DEPURAÇÃO - selr n=%d]\n", n);
    fprintf(contexto->arquivoTxt, "  - Formas encontradas pela consulta...: %d\n", total_encontrado_na_lista);
    fprintf(contexto->arquivoTxt, "  - Formas processadas no loop........: %d\n", formas_processadas_no_loop);
    fprintf(contexto->arquivoTxt, "  - Âncoras preparadas para o SVG.....: %d\n", total_ancoras_para_svg);

    // Verifica se todos os totais batem
    if (total_encontrado_na_lista == formas_processadas_no_loop && total_encontrado_na_lista == total_ancoras_para_svg) {
        fprintf(contexto->arquivoTxt, "  - VEREDITO: SUCESSO! As contagens são consistentes.\n\n");
    } else {
        fprintf(contexto->arquivoTxt, "  - VEREDITO: FALHA! As contagens estão inconsistentes. Revisar lógica.\n\n");
    }
    // =======================================================
    // =================== FIM DO BLOCO ======================
    // =======================================================
}
static void processa_seli(ContextoConsulta* contexto, int n, double x, double y) {
  
    fprintf(contexto->arquivoTxt, "seli %d %.1f %.1f\n", n, x, y);

    InformacoesAdicionaisImp* infos = (InformacoesAdicionaisImp*)contexto->infosSvg;

   
    Node no_encontrado = getNodeSmuT(contexto->arvore, x, y); //


    if (no_encontrado) {

        Info info_da_forma = getInfoSmuT(contexto->arvore, no_encontrado); //
        int id_forma = formaGetId(info_da_forma); //

        fprintf(contexto->arquivoTxt, "  > Forma selecionada: ID=%d\n", id_forma);

        Coord* ponto_ancora = malloc(sizeof(Coord));
        ponto_ancora->x = x; 
        ponto_ancora->y = y;
        lista_insere(infos->pontos_ancoras, ponto_ancora); 

        Lista nova_selecao = lista_cria(); //
        lista_insere(nova_selecao, no_encontrado); //

        if (contexto->selecionados[n] != NULL) {
            lista_libera(contexto->selecionados[n]); //
        }
        contexto->selecionados[n] = nova_selecao; //
        
        fprintf(contexto->arquivoTxt, "  > Seleção salva no grupo %d.\n\n", n);

    } else {
        // Se getNodeSmuT retornou NULL, nenhuma forma foi encontrada.
        fprintf(contexto->arquivoTxt, "  > Nenhuma forma encontrada na coordenada (%.1f, %.1f).\n\n", x, y);
    }
}

static void processa_disp(ContextoConsulta* contexto, int id, int n) {
    fprintf(contexto->arquivoTxt, "disp %d %d\n", id, n);
    fprintf(contexto->arquivoTxt, "  -> Função 'processa_disp' chamada. Lógica a ser implementada.\n\n");
}

static bool procuraNoPorId_callback(SmuTreap t, Node n, Info i, double x, double y, void *aux) {
    // Converte o ponteiro genérico 'aux' para o nosso tipo de busca.
    AuxBuscaId* busca = (AuxBuscaId*) aux;
    
    // Compara o ID da forma atual com o ID que estamos procurando.
    if (formaGetId(i) == busca->id_procurado) {
        return true; // Encontrou!
    }
    
    return false; // Não é este, continue procurando.
}

static void processa_transp(ContextoConsulta* contexto, int id, double x_novo, double y_novo) {
    fprintf(contexto->arquivoTxt, "transp %d %.1f %.1f\n", id, x_novo, y_novo);

    // --- PASSO 1: ENCONTRAR O NÓ A SER MOVIDO ---
    AuxBuscaId busca = { .id_procurado = id };
    Node no_a_mover = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_a_mover) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma com ID=%d não encontrada para mover.\n\n", id);
        return;
    }

    // --- PASSO 2: GUARDAR DADOS ORIGINAIS E REPORTAR NO TXT ---
    Info info_movida = getInfoSmuT(contexto->arvore, no_a_mover);
    DescritorTipoInfo tipo = getTypeInfoSrbT(contexto->arvore, no_a_mover);
    
    double x_orig, y_orig;
    GetXY(&x_orig, &y_orig, info_movida);

    fprintf(contexto->arquivoTxt, "  > Movendo forma ID=%d (Tipo=%d) da posição (%.1f, %.1f) para (%.1f, %.1f).\n\n",
            id, tipo, x_orig, y_orig, x_novo, y_novo);


    // --- PASSO 3: PREPARAR O EFEITO VISUAL PARA O SVG ---
    // Cria o vetor de movimento e o adiciona na lista para o SVG.
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    VetorMovimento* vm = malloc(sizeof(VetorMovimento));
    vm->origem.x = x_orig;
    vm->origem.y = y_orig;
    vm->destino.x = x_novo;
    vm->destino.y = y_novo;
    lista_insere(infos_svg->vetores_movimento, vm);


    // --- PASSO 4: ATUALIZAR A ÁRVORE (REMOVER E REINSERIR) ---
    // Este é o passo mais crítico.

    // 4.1 Remove o nó da sua posição antiga. A 'info_movida' agora está "solta".
    removeNoSmuT(contexto->arvore, no_a_mover);

    // 4.2 Atualiza as coordenadas DENTRO da estrutura de informação.
    formaMovePara(info_movida, x_novo, y_novo);

    // 4.3 Reinsere a informação atualizada na árvore. Isso criará um NOVO nó
    //     na posição correta da árvore, de acordo com a nova âncora.
    insertSmuT(contexto->arvore, x_novo, y_novo, info_movida, tipo, &formaCalculaBoundingBox);
}


static void processa_cln(ContextoConsulta* contexto, int n, double dx, double dy) {
    fprintf(contexto->arquivoTxt, "cln %d %.1f %.1f\n", n, dx, dy);


    if (n < 0 || n >= 100 || contexto->selecionados[n] == NULL) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Grupo de seleção %d está vazio ou é inválido.\n\n", n);
        return;
    }
    Lista lista_selecionada = contexto->selecionados[n];
    InformacoesAdicionaisImp* infos = (InformacoesAdicionaisImp*)contexto->infosSvg;


    Iterador it = lista_iterador(lista_selecionada);
    while (iterador_tem_proximo(it)) {
        Node no_original = iterador_proximo(it);
        Info info_original = getInfoSmuT(contexto->arvore, no_original);
        DescritorTipoInfo tipo_original = getTypeInfoSrbT(contexto->arvore, no_original);

        // 1. CLONAR a forma
        Info clone_info = formaClona(info_original); //
        if (!clone_info) {
            fprintf(stderr, "AVISO: Falha ao clonar forma ID=%d. Pulando.\n", formaGetId(info_original));
            continue; 
        }


        formaInverteCores(clone_info); //


        double x_orig, y_orig;
        GetXY(&x_orig, &y_orig, info_original); 

        double x_novo = x_orig + dx;
        double y_novo = y_orig + dy;
        formaMovePara(clone_info, x_novo, y_novo); //

       
        insertSmuT(contexto->arvore, x_novo, y_novo, clone_info, tipo_original, &formaCalculaBoundingBox); //
        
        fprintf(contexto->arquivoTxt, "  > Forma ID=%d clonada para (%.1f, %.1f).\n", formaGetId(info_original), x_novo, y_novo);
    }
    iterador_destroi(it);
    fprintf(contexto->arquivoTxt, "\n");
}

// se elas não estiverem já definidas em qry.c




// Em qry.c

// Em qry.c
/*static void processa_spy(ContextoConsulta* contexto, int id) {
    fprintf(contexto->arquivoTxt, "spy %d\n", id);

    // PASSO 1 e 2 (Encontrar espião e preparar SVG)
    AuxBuscaId busca = { .id_procurado = id };
    Node no_espiao = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_espiao) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'espiã' com ID=%d não encontrada.\n\n", id);
        return;
    }
    
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Info info_espiao = getInfoSmuT(contexto->arvore, no_espiao);
    lista_insere(infos_svg->formas_destacadas, info_espiao);

    // PASSO 3: BUSCA SECUNDÁRIA E RELATÓRIO INTELIGENTE
    DescritorTipoInfo tipo_espiao = getTypeInfoSrbT(contexto->arvore, no_espiao);
    
    // O switch agora só decide COMO buscar. O relatório é padronizado.
    Lista resultados_busca = lista_cria();

    switch (tipo_espiao) {
        case RETANGULO: {
            fprintf(contexto->arquivoTxt, "  > Espionando com o retângulo ID=%d...\n", id);
            double rx, ry, rw, rh;
            formaCalculaBoundingBox(tipo_espiao, info_espiao, &rx, &ry, &rw, &rh);
            getInfosDentroRegiaoSmuT(contexto->arvore, rx, ry, rx + rw, ry + rh, &formaDentroDeRegiao, resultados_busca);
            break;
        }
        default: { // Círculo, Linha, Texto, etc.
            fprintf(contexto->arquivoTxt, "  > Espionando com a âncora da forma ID=%d...\n", id);
            double anchor_x, anchor_y;
            GetXY(&anchor_x, &anchor_y, info_espiao);
            getInfosDentroRegiaoSmuT(contexto->arvore, anchor_x, anchor_y, anchor_x, anchor_y, &formaDentroDeRegiao, resultados_busca);
            break;
        }
    }

    // --- LÓGICA DE RELATÓRIO CONDICIONAL ---
    int total_resultados = lista_tamanho(resultados_busca);
    int contador_outras_formas = 0;
    
    Iterador it = lista_iterador(resultados_busca);
    while (iterador_tem_proximo(it)) {
        Node no_achado = iterador_proximo(it);
        if (no_achado != no_espiao) {
            // Se for a primeira "outra forma", imprime o cabeçalho
            if (contador_outras_formas == 0) {
                fprintf(contexto->arquivoTxt, "  > Dados das formas encontradas:\n");
            }
            // Imprime os detalhes da forma encontrada
            fprintf(contexto->arquivoTxt, "  >   - ");
            formaFprintfResumo(contexto->arquivoTxt, getInfoSmuT(contexto->arvore, no_achado));
            fprintf(contexto->arquivoTxt, "\n");
            contador_outras_formas++;
        }
    }
    iterador_destroi(it);

    // Agora, decide qual mensagem final imprimir com base nos contadores
    if (contador_outras_formas == 0) {
        if (total_resultados > 0) {
            // Encontrou resultados, mas nenhum era "outro", então só encontrou a si mesmo.
            fprintf(contexto->arquivoTxt, "  > Somente a própria forma espiã foi encontrada na área. Dados:\n");
            fprintf(contexto->arquivoTxt, "  >   - ");
            formaFprintfResumo(contexto->arquivoTxt, info_espiao);
            fprintf(contexto->arquivoTxt, "\n");
        } else {
            // Não encontrou absolutamente nada na área de busca.
            fprintf(contexto->arquivoTxt, "  > Nenhuma forma encontrada na área de espionagem.\n");
        }
    }
    
    lista_libera(resultados_busca);
    fprintf(contexto->arquivoTxt, "\n");
}*/
// Em qry.c
// Em qry.c
static void processa_spy(ContextoConsulta* contexto, int id) {
    fprintf(contexto->arquivoTxt, "spy %d\n", id);

    // --- PASSO 1: ENCONTRAR A FORMA "ESPIÃ" ---
    AuxBuscaId busca = { .id_procurado = id };
    Node no_espiao = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_espiao) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'espiã' com ID=%d não encontrada.\n\n", id);
        return;
    }

    // --- PASSO 2: PREPARAR O EFEITO VISUAL PARA O SVG ---
    // Adiciona a forma espiã na lista de destaque. Esta parte não muda.
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Info info_espiao = getInfoSmuT(contexto->arvore, no_espiao);
    lista_insere(infos_svg->formas_destacadas, info_espiao);


    // --- PASSO 3: LÓGICA UNIVERSAL DE BUSCA E RELATÓRIO TXT ---
    fprintf(contexto->arquivoTxt, "  > Espionando com a forma ID=%d. Formas encontradas em sua área:\n", id);

    // 3a. Calcula o bounding box da forma espiã.
    DescritorTipoInfo tipo_espiao = getTypeInfoSrbT(contexto->arvore, no_espiao);
    double sx, sy, sw, sh; // Coordenadas do bounding box do espião
    formaCalculaBoundingBox(tipo_espiao, info_espiao, &sx, &sy, &sw, &sh);

    // 3b. Usa o bounding box do espião como a nova área de busca em região.
    Lista resultados_busca = lista_cria();
    getInfosDentroRegiaoSmuT(contexto->arvore, sx, sy, sx + sw, sy + sh, &formaDentroDeRegiao, resultados_busca);

    // 3c. Reporta TODOS os resultados encontrados nessa área.
    //    Isso incluirá a própria forma espiã e qualquer outra que esteja dentro.
    int total_encontrado = lista_tamanho(resultados_busca);
    if (total_encontrado == 0) {
        // Isso seria estranho, pois deveria encontrar pelo menos a si mesmo.
        fprintf(contexto->arquivoTxt, "  >   (Nenhuma forma encontrada, nem a própria espiã - verifique a lógica de contenção)\n");
    } else {
        Iterador it = lista_iterador(resultados_busca);
        while (iterador_tem_proximo(it)) {
            Node no_achado = iterador_proximo(it);
            Info info_achada = getInfoSmuT(contexto->arvore, no_achado);
            
            fprintf(contexto->arquivoTxt, "  >   - ");
            formaFprintfResumo(contexto->arquivoTxt, info_achada);
            fprintf(contexto->arquivoTxt, "\n");
        }
        iterador_destroi(it);
    }
    
    lista_libera(resultados_busca);
    fprintf(contexto->arquivoTxt, "\n");
}
// Em qry.c
/*static void processa_cmflg(ContextoConsulta* contexto, int id, const char* corb, const char* corp, const char* w) {
    fprintf(contexto->arquivoTxt, "cmflg %d %s %s %s\n", id, corb, corp, w);

    // PASSO 1: Encontrar a forma espiã (continua igual)
    AuxBuscaId busca = { .id_procurado = id };
    Node no_espiao = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_espiao) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'espiã' com ID=%d não encontrada.\n\n", id);
        return;
    }

    // PASSO 2: Obter a lista de alvos (continua igual)
    Info info_espiao = getInfoSmuT(contexto->arvore, no_espiao);
    DescritorTipoInfo tipo_espiao = getTypeInfoSrbT(contexto->arvore, no_espiao);
    Lista alvos = lista_cria();

    switch (tipo_espiao) {
        case RETANGULO: {
            double rx, ry, rw, rh;
            formaCalculaBoundingBox(tipo_espiao, info_espiao, &rx, &ry, &rw, &rh);
            getInfosDentroRegiaoSmuT(contexto->arvore, rx, ry, rx + rw, ry + rh, &formaDentroDeRegiao, alvos);
            break;
        }
        default: {
            double anchor_x, anchor_y;
            GetXY(&anchor_x, &anchor_y, info_espiao);
            getInfosAtingidoPontoSmuT(contexto->arvore, anchor_x, anchor_y, &formaPontoInternoAInfo, alvos);
            break;
        }
    }

    // PASSO 3: CAMUFLAGEM E PREPARAÇÃO DO DESTAQUE VISUAL
    fprintf(contexto->arquivoTxt, "  > Camuflando formas na área do espião ID=%d com borda='%s' e preenchimento='%s':\n", id, corb, corp);
    
    int contador = 0;
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Iterador it = lista_iterador(alvos);

    while (iterador_tem_proximo(it)) {
        Node no_alvo = iterador_proximo(it);
        Info info_alvo = getInfoSmuT(contexto->arvore, no_alvo);
        
        // Ação 1: Aplica as novas cores
        formaSetCores(info_alvo, corb, corp);

        // ==========================================================
        // AÇÃO VISUAL: Adiciona a Info na lista de destaque do SVG
        // ==========================================================
        lista_insere(infos_svg->formas_destacadas, info_alvo);
        // ==========================================================
        
        // Ação 2: Reporta no TXT
        fprintf(contexto->arquivoTxt, "  >   - Forma ID=%d camuflada.\n", formaGetId(info_alvo));
        contador++;
    }
    
    if (contador == 0) {
        fprintf(contexto->arquivoTxt, "  >   (Nenhuma forma encontrada para camuflar)\n");
    }

    iterador_destroi(it);
    lista_libera(alvos);
    fprintf(contexto->arquivoTxt, "\n");
}*/

// Em qry.c
static void processa_cmflg(ContextoConsulta* contexto, int id, const char* corb, const char* corp, const char* w) {
    fprintf(contexto->arquivoTxt, "cmflg %d %s %s %s\n", id, corb, corp, w);

    // --- PASSO 1: ENCONTRAR A FORMA "ESPIÃ" ---
    AuxBuscaId busca = { .id_procurado = id };
    Node no_espiao = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_espiao) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'espiã' com ID=%d não encontrada.\n\n", id);
        return;
    }

    // --- PASSO 2: BUSCA UNIVERSAL USANDO O BOUNDING BOX DO ESPIÃO ---
    // Esta lógica agora funciona para QUALQUER tipo de forma.
    
    Info info_espiao = getInfoSmuT(contexto->arvore, no_espiao);
    DescritorTipoInfo tipo_espiao = getTypeInfoSrbT(contexto->arvore, no_espiao);

    fprintf(contexto->arquivoTxt, "  > Usando o bounding box da forma ID=%d como área de busca.\n", id);

    // 2a. Calcula o bounding box da forma espiã, não importa o tipo.
    double sx, sy, sw, sh; // Bounding box do espião
    formaCalculaBoundingBox(tipo_espiao, info_espiao, &sx, &sy, &sw, &sh);
      if (tipo_espiao == RETANGULO) {
        printf("[DEBUG cmflg] Retângulo espião ID=%d. Área de busca: x=%.2f, y=%.2f, w=%.2f, h=%.2f\n", 
               id, sx, sy, sw, sh);
        fflush(stdout);
    }

    // 2b. Usa esse bounding box como a região da busca.
    Lista alvos = lista_cria();
    getInfosDentroRegiaoSmuT(contexto->arvore, sx, sy, sx + sw, sy + sh, &formaDentroDeRegiao, alvos);

    // --- PASSO 3: "CAMUFLAGEM" E DESTAQUE DE TODOS OS ALVOS ENCONTRADOS ---
    fprintf(contexto->arquivoTxt, "  > Camuflando formas encontradas com borda='%s' e preenchimento='%s':\n", corb, corp);
    
    int contador = 0;
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Iterador it = lista_iterador(alvos);

    while (iterador_tem_proximo(it)) {
        Node no_alvo = iterador_proximo(it);
        Info info_alvo = getInfoSmuT(contexto->arvore, no_alvo);
        
        // Ação 1: Aplica as novas cores
        formaSetCores(info_alvo, corb, corp);

        // Ação 2 (Debug Visual): Adiciona a info na lista de destaque do SVG
        lista_insere(infos_svg->formas_destacadas, info_alvo);

        // Ação 3: Reporta no TXT
        fprintf(contexto->arquivoTxt, "  >   - Forma ID=%d camuflada.\n", formaGetId(info_alvo));
        contador++;
    }
    
    if (contador == 0) {
        fprintf(contexto->arquivoTxt, "  >   (Nenhuma forma encontrada para camuflar)\n");
    }

    iterador_destroi(it);
    lista_libera(alvos);
    fprintf(contexto->arquivoTxt, "\n");
}
static void processa_blow(ContextoConsulta* contexto, int id) {
    fprintf(contexto->arquivoTxt, "blow %d\n", id);
    fprintf(contexto->arquivoTxt, "  -> Função 'processa_blow' chamada. Lógica a ser implementada.\n\n");
}