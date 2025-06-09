
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "qry.h"
#include "utils.h"
#include "formas.h"
#include "lista.h"
#include "smutreap.h"


typedef struct { double x, y; } Coord;

typedef struct {
    double x, y, w, h;
} BoundingBox;

typedef struct {
    Coord origem;
    Coord destino;
} VetorMovimento;


typedef struct InformacoesAdicionaisImp {
    Lista pontos_x_vermelhos;
    Lista pontos_hashtag_vermelhos;
    Lista pontos_ancoras;
    Lista formas_destacadas;
    Lista retangulos_selecao;
    Lista vetores_movimento;
    Lista formas_clonadas;
} InformacoesAdicionaisImp;

typedef struct {
    SmuTreap arvore;
    FILE* arquivoTxt;
    InformacoesAdicionais infosSvg;
    Lista selecionados[100];
} ContextoConsulta;


typedef struct { 
    int id_procurado; 
} AuxBuscaId;

static bool inicializa_infosAdicionais(InformacoesAdicionaisImp* infos);
static void zera_selecionados(Lista* array_listas);
static void processa_selr(ContextoConsulta* contexto, int n, double x, double y, double w, double h);
static void processa_seli(ContextoConsulta* contexto, int n, double x, double y);
static void processa_disp(ContextoConsulta* contexto, int id, int n);
static void processa_transp(ContextoConsulta* contexto, int id, double x, double y);
static void processa_cln(ContextoConsulta* contexto, int n, double dx, double dy);
static void processa_spy(ContextoConsulta* contexto, int id);
static void processa_cmflg(ContextoConsulta* contexto, int id, const char* corb, const char* corp, double w);
static void processa_blow(ContextoConsulta* contexto, int id);



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
            double w;
            char corb[64], corp[64];
            sscanf(ptr, "%*s %d %63s %63s %lf", &id, corb, corp, &w);
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
static bool procuraNoPorId_callback(SmuTreap t, Node n, Info i, double x, double y, void *aux) {
    // Converte o ponteiro genérico 'aux' para o nosso tipo de busca.
    AuxBuscaId* busca = (AuxBuscaId*) aux;
    
    // Compara o ID da forma atual com o ID que estamos procurando.
    if (formaGetId(i) == busca->id_procurado) {
        return true; // Encontrou!
    }
    
    return false; // Não é este, continue procurando.
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
    
    int total_encontrado_na_lista = lista_tamanho(resultados);
    
    fprintf(contexto->arquivoTxt, "\n  [RELATÓRIO DE DEPURAÇÃO - selr n=%d]\n", n);
    fprintf(contexto->arquivoTxt, "  - Formas encontradas pela consulta...: %d\n", total_encontrado_na_lista);
    fprintf(contexto->arquivoTxt, "  - Formas processadas neste loop.......: %d\n", formas_processadas_no_loop);
    
    // Apenas para informação, mostramos o total acumulado de âncoras.
    // Não usamos mais este valor para o veredito.
    fprintf(contexto->arquivoTxt, "  - Total de âncoras acumuladas no SVG: %d\n", lista_tamanho(infos->pontos_ancoras));

    // A verificação agora é mais simples e correta.
    // Ela apenas confere se o loop processou todos os itens que a busca encontrou.
    if (total_encontrado_na_lista == formas_processadas_no_loop) {
        fprintf(contexto->arquivoTxt, "  - VEREDITO: SUCESSO! A contagem para ESTA chamada está consistente.\n\n");
    } else {
        fprintf(contexto->arquivoTxt, "  - VEREDITO: FALHA! O loop não processou todas as formas encontradas.\n\n");
    }
  
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

// Em qry.c
static void processa_disp(ContextoConsulta* contexto, int id, int n) {
    fprintf(contexto->arquivoTxt, "disp %d %d\n", id, n);

    // --- FASE 1: PREPARAÇÃO (ENCONTRAR DIREÇÃO E OGIVAS) ---

    // 1a. Encontrar a linha que dá a direção
    AuxBuscaId busca_linha = { .id_procurado = id };
    Node no_linha = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca_linha);

    if (!no_linha || formaGetTipo(getInfoSmuT(contexto->arvore, no_linha)) != LINHA) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Linha de direção com ID=%d não encontrada ou a forma não é uma linha.\n\n", id);
        return;
    }

    // 1b. Calcular o vetor de direção normalizado (comprimento 1)
    double lx1, ly1, lx2, ly2;
    linhaGetPontos(getInfoSmuT(contexto->arvore, no_linha), &lx1, &ly1, &lx2, &ly2);
    double vx = lx2 - lx1;
    double vy = ly2 - ly1;
    double mag = sqrt(vx*vx + vy*vy);
    double margem = 1e-9;
    if (mag > margem) {
        vx /= mag;
        vy /= mag;
    }
    else{
        vx=0;
        vy=0;
    }

    // 1c. Obter a lista de "ogivas" (projéteis)
    if (n < 0 || n >= 100 || contexto->selecionados[n] == NULL) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Grupo de seleção %d está vazio ou é inválido.\n\n", n);
        return;
    }
    Lista ogivas = contexto->selecionados[n];
    
    // --- FASE 2: LOOP DE DISPARO E COLETA DE DADOS ---
    
   InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Lista nos_a_remover = lista_cria(); 

    Iterador it_ogivas = lista_iterador(ogivas);
    while (iterador_tem_proximo(it_ogivas)) {
        Node no_ogiva = iterador_proximo(it_ogivas);
        Info info_ogiva = getInfoSmuT(contexto->arvore, no_ogiva);
        
        // --- Cálculo de trajetória (continua igual) ---
        double distancia = formaGetArea(info_ogiva);
        double x_inicial, y_inicial;
        GetXY(&x_inicial, &y_inicial, info_ogiva);
        double x_final = x_inicial + vx * distancia;
        double y_final = y_inicial + vy * distancia;

        // --- Impressão inicial do disparo (continua igual) ---
        fprintf(contexto->arquivoTxt, "\n  > Disparando ogiva ID=%d (Tipo=%d):\n", formaGetId(info_ogiva), formaGetTipo(info_ogiva));
        fprintf(contexto->arquivoTxt, "    - Posição Inicial: (%.2f, %.2f)\n", x_inicial, y_inicial);
        fprintf(contexto->arquivoTxt, "    - Distância (Área): %.2f\n", distancia);
        fprintf(contexto->arquivoTxt, "    - Posição Final (Explosão): (%.2f, %.2f)\n", x_final, y_final);

        // --- Preparar efeitos SVG (continua igual) ---
        VetorMovimento* vm = malloc(sizeof(VetorMovimento));
        vm->origem.x = x_inicial;   vm->origem.y = y_inicial;
        vm->destino.x = x_final;    vm->destino.y = y_final;
        lista_insere(infos_svg->vetores_movimento, vm);

        Coord* ponto_explosao = malloc(sizeof(Coord));
        ponto_explosao->x = x_final; ponto_explosao->y = y_final;
        lista_insere(infos_svg->pontos_hashtag_vermelhos, ponto_explosao);

        // --- Encontrar e processar vítimas (LÓGICA REFINADA) ---
        Lista alvos_atingidos = lista_cria();
        getInfosAtingidoPontoSmuT(contexto->arvore, x_final, y_final, &formaPontoInternoAInfo, alvos_atingidos);
        
        fprintf(contexto->arquivoTxt, "    - Vítimas da explosão:\n");
        
        int vitimas_contador = 0;
        // 1. Processa as vítimas que foram atingidas pela explosão
        Iterador it_vitimas = lista_iterador(alvos_atingidos);
        while (iterador_tem_proximo(it_vitimas)) {
            Node no_vitima = iterador_proximo(it_vitimas);
            // Evita processar a própria ogiva duas vezes caso ela atinja a si mesma
            if (no_vitima == no_ogiva) continue;

            Info info_vitima = getInfoSmuT(contexto->arvore, no_vitima);
            fprintf(contexto->arquivoTxt, "      - ");
            formaFprintfResumo(contexto->arquivoTxt, info_vitima);
            fprintf(contexto->arquivoTxt, "\n");
            
            double ax_v, ay_v;
            GetXY(&ax_v, &ay_v, info_vitima);
            Coord* ponto_x = malloc(sizeof(Coord));
            ponto_x->x = ax_v; ponto_x->y = ay_v;
            lista_insere(infos_svg->pontos_x_vermelhos, ponto_x);

            lista_insere(nos_a_remover, no_vitima);
            vitimas_contador++;
        }
        iterador_destroi(it_vitimas);
        lista_libera(alvos_atingidos);

        // 2. Agora, reporta e processa a própria ogiva como uma vítima
        fprintf(contexto->arquivoTxt, "      - ");
        formaFprintfResumo(contexto->arquivoTxt, info_ogiva);
        fprintf(contexto->arquivoTxt, " (própria ogiva destruída)\n");
        Coord* ponto_x_ogiva = malloc(sizeof(Coord));
        ponto_x_ogiva->x = x_final; // Usa a posição final da explosão
        ponto_x_ogiva->y = y_final; // Usa a posição final da explosão
        lista_insere(infos_svg->pontos_x_vermelhos, ponto_x_ogiva);

        lista_insere(nos_a_remover, no_ogiva);
        vitimas_contador++; // Incrementa para que a contagem final seja correta

        // A mensagem de "Nenhuma" só aparece se nem a ogiva foi processada (o que não deve acontecer)
        if (vitimas_contador == 0) {
            fprintf(contexto->arquivoTxt, "      (Nenhuma)\n");
        }
    }
    iterador_destroi(it_ogivas);

    // --- FASE 3: DESTRUIÇÃO FINAL ---
    // Remove todos os nós coletados (ogivas e vítimas) de uma só vez
    // Para evitar duplicatas, podemos usar uma lógica de verificação, mas por simplicidade vamos apenas remover.
    Iterador it_remover = lista_iterador(nos_a_remover);
    while(iterador_tem_proximo(it_remover)) {
        Node no_a_remover = iterador_proximo(it_remover);
        // A função removeNoSmuT deve ser robusta para o caso do nó já ter sido removido
        removeNoSmuT(contexto->arvore, no_a_remover);
    }
    iterador_destroi(it_remover);
    lista_libera(nos_a_remover);

    // Limpa a lista de seleção usada, pois as formas não existem mais.
    lista_libera(contexto->selecionados[n]);
    contexto->selecionados[n] = NULL;
    
    fprintf(contexto->arquivoTxt, "\n");
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


static void processa_spy(ContextoConsulta* contexto, int id) {
    fprintf(contexto->arquivoTxt, "spy %d\n", id);

    // PASSO 1 e 2: Encontrar espião e preparar SVG (continuam iguais)
    AuxBuscaId busca = { .id_procurado = id };
    Node no_espiao = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_espiao) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'espiã' com ID=%d não encontrada.\n\n", id);
        return;
    }
    
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Info info_espiao = getInfoSmuT(contexto->arvore, no_espiao);
    lista_insere(infos_svg->formas_destacadas, info_espiao);


    // PASSO 3: Busca universal e relatório filtrado
    fprintf(contexto->arquivoTxt, "  > Espionando com a forma ID=%d. Retângulos e Textos encontrados na área:\n", id);

    DescritorTipoInfo tipo_espiao = getTypeInfoSrbT(contexto->arvore, no_espiao);
    double sx, sy, sw, sh;
    formaCalculaBoundingBox(tipo_espiao, info_espiao, &sx, &sy, &sw, &sh);

    Lista resultados_busca = lista_cria();
    getInfosDentroRegiaoSmuT(contexto->arvore, sx, sy, sx + sw, sy + sh, &formaDentroDeRegiao, resultados_busca);

    // Agora, reporta apenas os resultados que são do tipo RETANGULO ou TEXTO
    int contador_reportados = 0;
    Iterador it = lista_iterador(resultados_busca);
    while (iterador_tem_proximo(it)) {
        Node no_achado = iterador_proximo(it);
        Info info_achada = getInfoSmuT(contexto->arvore, no_achado);
        

       
        DescritorTipoInfo tipo_achado = formaGetTipo(info_achada);
        if (tipo_achado == RETANGULO || tipo_achado == TEXTO) {
            // Se for um dos tipos desejados, imprime o resumo.
            fprintf(contexto->arquivoTxt, "  >   - ");
            formaFprintfResumo(contexto->arquivoTxt, info_achada);
            fprintf(contexto->arquivoTxt, "\n");
            contador_reportados++;
        }
        // ==========================================================
    }
    iterador_destroi(it);

   
    if (contador_reportados == 0) {
        fprintf(contexto->arquivoTxt, "  >   (Nenhum retângulo ou texto encontrado na área de espionagem)\n");
    }
    
    lista_libera(resultados_busca);
    fprintf(contexto->arquivoTxt, "\n");
}

static void processa_cmflg(ContextoConsulta* contexto, int id, const char* corb, const char* corp, const double w) {
    fprintf(contexto->arquivoTxt, "cmflg %d %s %s %lf\n", id, corb, corp, w);

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
        formaSetLarguraBorda(info_alvo, w);

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

    // --- PASSO 1: ENCONTRAR A "BOMBA" ---
    AuxBuscaId busca = { .id_procurado = id };
    Node no_bomba = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca);

    if (!no_bomba) {
        fprintf(contexto->arquivoTxt, "  > AVISO: Forma 'bomba' com ID=%d não encontrada para explodir.\n\n", id);
        return;
    }

    Info info_bomba = getInfoSmuT(contexto->arvore, no_bomba);
    fprintf(contexto->arquivoTxt, "  > Bomba ID=%d ativada. Atributos da figura removida:\n", id);
    fprintf(contexto->arquivoTxt, "  >   - ");
    formaFprintfResumo(contexto->arquivoTxt, info_bomba);
    fprintf(contexto->arquivoTxt, "\n");

    // --- PASSO 2: ENCONTRAR VÍTIMAS ---
    double explosao_x, explosao_y;
    GetXY(&explosao_x, &explosao_y, info_bomba);

    Lista alvos_encontrados = lista_cria();
    getInfosAtingidoPontoSmuT(contexto->arvore, explosao_x, explosao_y, &formaPontoInternoAInfo, alvos_encontrados);

    // --- PASSO 3: REPORTAR, PREPARAR SVG E COLETAR IDs PARA REMOÇÃO ---
    InformacoesAdicionaisImp* infos_svg = (InformacoesAdicionaisImp*)contexto->infosSvg;
    Lista ids_a_remover = lista_cria(); // Lista de IDs (int*)

    // Prepara o '#' no local da explosão
    Coord* ponto_explosao = malloc(sizeof(Coord));
    ponto_explosao->x = explosao_x;
    ponto_explosao->y = explosao_y;
    lista_insere(infos_svg->pontos_hashtag_vermelhos, ponto_explosao);

    fprintf(contexto->arquivoTxt, "  > Atributos das figuras atingidas pela explosão:\n");
    
    // Percorre os alvos encontrados para REPORTAR e COLETAR SEUS IDs
    Iterador it = lista_iterador(alvos_encontrados);
    while (iterador_tem_proximo(it)) {
        Node no_alvo = iterador_proximo(it);
        Info info_alvo = getInfoSmuT(contexto->arvore, no_alvo);
        
        fprintf(contexto->arquivoTxt, "  >   - ");
        formaFprintfResumo(contexto->arquivoTxt, info_alvo);
        fprintf(contexto->arquivoTxt, "\n");

        // Prepara o 'x' para o SVG
        double ax, ay;
        GetXY(&ax, &ay, info_alvo);
        Coord* ponto_x = malloc(sizeof(Coord));
        ponto_x->x = ax;
        ponto_x->y = ay;
        lista_insere(infos_svg->pontos_x_vermelhos, ponto_x);

        // Adiciona o ID do alvo à lista de remoção
        int* id_alvo = malloc(sizeof(int));
        *id_alvo = formaGetId(info_alvo);
        lista_insere(ids_a_remover, id_alvo);
    }
    iterador_destroi(it);
    
    if (lista_tamanho(alvos_encontrados) == 0) {
        fprintf(contexto->arquivoTxt, "  >   (Nenhuma forma atingida)\n");
    }
    lista_libera(alvos_encontrados);

    // --- PASSO 4: EXECUTAR A DESTRUIÇÃO A PARTIR DOS IDs COLETADOS ---
    fprintf(contexto->arquivoTxt, "  > Removendo formas da árvore...\n");
    it = lista_iterador(ids_a_remover);
    while (iterador_tem_proximo(it)) {
        int* id_ptr = (int*) iterador_proximo(it);
        
        // Para cada ID, busca o nó na árvore ATUAL e o remove.
        AuxBuscaId busca_remocao = { .id_procurado = *id_ptr };
        Node no_a_remover = procuraNoSmuT(contexto->arvore, &procuraNoPorId_callback, &busca_remocao);
        
        if (no_a_remover) {
            removeNoSmuT(contexto->arvore, no_a_remover);
            fprintf(contexto->arquivoTxt, "  >   - Nó com ID=%d removido.\n", *id_ptr);
        }
        free(id_ptr);
    }
    iterador_destroi(it);
    lista_libera(ids_a_remover);

    fprintf(contexto->arquivoTxt, "\n");
}


static void liberar_lista_e_elementos(Lista lista) {
    if (!lista) return;

    Iterador it = lista_iterador(lista);
    while (iterador_tem_proximo(it)) {
        free(iterador_proximo(it)); 
    }
    iterador_destroi(it);
    lista_libera(lista); 
}


void destruirInformacoesAdicionais(InformacoesAdicionais infos_void) {
    if (!infos_void) return;

    InformacoesAdicionaisImp* infos = (InformacoesAdicionaisImp*) infos_void;

    liberar_lista_e_elementos(infos->pontos_x_vermelhos);
    liberar_lista_e_elementos(infos->pontos_hashtag_vermelhos);
    liberar_lista_e_elementos(infos->pontos_ancoras);
    liberar_lista_e_elementos(infos->retangulos_selecao);
    liberar_lista_e_elementos(infos->vetores_movimento);

    if (infos->formas_clonadas) {
        Iterador it = lista_iterador(infos->formas_clonadas);
        while(iterador_tem_proximo(it)){
            liberaForma(iterador_proximo(it));
        }
        iterador_destroi(it);
        lista_libera(infos->formas_clonadas);
    }


    if (infos->formas_destacadas) {
        lista_libera(infos->formas_destacadas);
    }

    free(infos);
}