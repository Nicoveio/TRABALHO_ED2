#include "svg.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "formas.h"
#include "smutreap.h"


#define MARGEM_CANVAS 20.0

typedef struct { double x, y; } Coord;

typedef struct {
    double x, y, w, h;
} BoundingBox;

typedef struct {
    Coord origem;
    Coord destino;
} VetorMovimento;

typedef struct InformacoesAdicionais {
    Lista pontos_x_vermelhos;        // tipo Coord*
    Lista pontos_hashtag_vermelhos;  // tipo Coord*
    Lista pontos_ancoras;            // tipo Coord*
    Lista formas_destacadas;
    Lista retangulos_selecao;        // tipo BoundingBox*
    Lista vetores_movimento;         // tipo VetorMovimento*
    Lista formas_clonadas;           // tipo Forma
} InformacoesAdicionaisImp;


typedef struct {
    FILE *arquivo;          
    double largura_canvas;  
    double altura_canvas;   
    int contador_formas;    
    Lista nos_destaque;
    InformacoesAdicionais info_adicional; 
} ContextoSVGInterno;




typedef struct {
    double min_x, min_y, max_x, max_y;
    bool primeira_forma;
} CalculoDimensoes;


typedef struct {
    int id;             // identificador numérico
    double x, y;        // centro
    double r;           // raio
    char corb[64];      // cor da borda
    char corp[64];      // cor de preenchimento
    double largura_borda;
} Circle;

typedef struct {
    int id;
    double x, y;        // âncora (canto superior esquerdo)
    double w, h;        // largura e altura
    char corb[64];
    char corp[64];
    double largura_borda;
} Rect;

typedef struct {
    int id;
    double x1, y1, x2, y2; // coordenadas das extremidades
    char cor[64];
    double largura_borda;
} Line;

typedef struct {
    int id;
    double x, y;
    char corb[64];
    char corp[64];
    char a;              // âncora: 'i', 'm', ou 'f'
    char txto[1024];     // conteúdo do texto
    char fontFamily[32]; // nova: Arial, serif, etc.
    char fontWeight[8];  // nova: n, b, b+, l
    int fontSize;        // nova: em px
    double largura_borda;
} Text;

typedef struct{
    void * forma;
    DescritorTipoInfo tipo;
}forma;

// ======================= FUNÇÕES AUXILIARES PRO SVG ATÉ O MOMENTO / TESTE=======================


static ContextoSVGInterno* criarContextoSVG(FILE *arquivo, double largura, double altura, InformacoesAdicionais infos) {
    ContextoSVGInterno *contexto = malloc(sizeof(ContextoSVGInterno));
    if (!contexto) return NULL;
    
    contexto->arquivo = arquivo;
    contexto->largura_canvas = largura;
    contexto->altura_canvas = altura;
    contexto->contador_formas = 0;
    contexto->info_adicional = infos;
    
    return contexto;
}

static void calcularDimensoesCallback(SmuTreap t, Node n, Info i, double x, double y, void *aux) {
    CalculoDimensoes *calc = (CalculoDimensoes*)aux;
    DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
    
    double forma_x, forma_y, forma_w, forma_h;
    
    formaCalculaBoundingBox(tipo, i, &forma_x, &forma_y, &forma_w, &forma_h);
    
    if (calc->primeira_forma) {
        calc->min_x = forma_x;
        calc->min_y = forma_y;
        calc->max_x = forma_x + forma_w;
        calc->max_y = forma_y + forma_h;
        calc->primeira_forma = false;
    } else {
        if (forma_x < calc->min_x) calc->min_x = forma_x;
        if (forma_y < calc->min_y) calc->min_y = forma_y;
        if (forma_x + forma_w > calc->max_x) calc->max_x = forma_x + forma_w;
        if (forma_y + forma_h > calc->max_y) calc->max_y = forma_y + forma_h;
    }
}


void escreverCabecalhoSVG(FILE *arquivo, double largura, double altura) {
    fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(arquivo, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
    fprintf(arquivo, "width=\"%.2f\" height=\"%.2f\" ", largura, altura);
    fprintf(arquivo, "viewBox=\"0 0 %.2f %.2f\">\n", largura, altura);
    fprintf(arquivo, "  <!-- Gerado automaticamente -->\n");

    // Estilos e definições
    fprintf(arquivo, "  <defs>\n");
    fprintf(arquivo, "    <style>\n");
    fprintf(arquivo, "      .forma-destaque { stroke-width: 3; opacity: 0.8; stroke-dasharray: 5,5; }\n");
    fprintf(arquivo, "      .forma-normal { stroke-width: 1; opacity: 1.0; }\n");
    fprintf(arquivo, "    </style>\n");
    fprintf(arquivo,
        "    <marker id=\"seta\" markerWidth=\"10\" markerHeight=\"10\" refX=\"5\" refY=\"5\" "
        "orient=\"auto\" markerUnits=\"strokeWidth\">\n"
        "      <path d=\"M 0 0 L 10 5 L 0 10 z\" fill=\"blue\" />\n"
        "    </marker>\n");
    fprintf(arquivo, "  </defs>\n");

    // Fundo branco
    fprintf(arquivo, "  <rect width=\"100%%\" height=\"100%%\" fill=\"white\" />\n");

    // Grupo transformado (espelhamento vertical)
    fprintf(arquivo, "  <g transform=\"translate(0,%.2f) scale(1,-1)\">\n", altura);
    fprintf(arquivo, "\n");
}



static void escreverRodapeSVG(FILE *arquivo) {
    fprintf(arquivo, "  </g>\n");
    fprintf(arquivo, "</svg>\n");
}


static double converterY(double y_cartesiano, double altura_canvas) {
    return altura_canvas - y_cartesiano;
}


static const char* validarCor(const char* cor, const char* cor_padrao) {
    if (cor && strlen(cor) > 0) {
        return cor;
    }
    return cor_padrao;
}

static void obterCoresForma(Info i, DescritorTipoInfo tipo, char **corb, char **corp) {
    forma *f = (forma*)i;
    
    switch (tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            *corb = c->corb;
            *corp = c->corp;
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            *corb = r->corb;
            *corp = r->corp;
            break;
        }
        case LINHA: {
            Line *l = (Line*)f->forma;
            *corb = l->cor;
            *corp = NULL; // Linha não tem preenchimento
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            *corb = t->corb;
            *corp = t->corp;
            break;
        }
        default:
            *corb = "black";
            *corp = "none";
    }
}


static const char* obterTextoForma(Info i, DescritorTipoInfo tipo) {
    if (tipo != TEXTO) return "Texto";
    
    forma *f = (forma*)i;
    Text *t = (Text*)f->forma;
    return t->txto;
}

static void obterDadosLinha(Info i, double *x1, double *y1, double *x2, double *y2) {
    forma *f = (forma*)i;
    Line *l = (Line*)f->forma;
    *x1 = l->x1;
    *y1 = l->y1;
    *x2 = l->x2;
    *y2 = l->y2;
}


static double obterRaioCirculo(Info i) {
    forma *f = (forma*)i;
    Circle *c = (Circle*)f->forma;
    return c->r;
}


static char obterAncoraTexto(Info i) {
    forma *f = (forma*)i;
    Text *t = (Text*)f->forma;
    return t->a;
}

void imprimeTextoEscapado(FILE *arquivo, const char *texto) {
    for (const char *p = texto; *p != '\0'; p++) {
        switch (*p) {
            case '&':  fprintf(arquivo, "&amp;");  break;
            case '<':  fprintf(arquivo, "&lt;");   break;
            case '>':  fprintf(arquivo, "&gt;");   break;
            case '"':  fprintf(arquivo, "&quot;"); break;
            case '\'': fprintf(arquivo, "&apos;"); break; // opcional
            default:   fputc(*p, arquivo);          break;
        }
    }
}

static double getRaioEquivalente(Info info_forma) {
    if (!info_forma) return 5.0; 

    DescritorTipoInfo tipo = formaGetTipo(info_forma);
    forma* f = (forma*)info_forma;

    switch(tipo) {
        case CIRCULO: {
         
            Circle* c = (Circle*)f->forma;
            return c->r;
        }
        default: {
           
            return 5.0;
        }
    }
}
void desenharInfosAdicionaisSVG(ContextoSVGInterno *ctx) {
    FILE *arq = ctx->arquivo;
    double altura_canvas = ctx->altura_canvas;
    if(ctx->info_adicional==NULL)return;
    InformacoesAdicionaisImp * infos = (InformacoesAdicionaisImp*)ctx->info_adicional;

    int contador_ancoras_desenhadas = 0;


    // 1. Pontos "x" vermelhos (formas destruídas) - CENTRALIZEI
    if (infos->pontos_x_vermelhos) {
        Iterador it = lista_iterador(infos->pontos_x_vermelhos);
        while (iterador_tem_proximo(it)) {
            Coord *p = (Coord*) iterador_proximo(it);
            double y_svg = altura_canvas - p->y;
            
            // Adicionados text-anchor e dominant-baseline para centralização mais estável.
            fprintf(arq, "  <text x=\"%.2f\" y=\"%.2f\" font-size=\"12\" font-weight=\"bold\" fill=\"firebrick\" text-anchor=\"middle\" fill-opacity=\"1.0\" dominant-baseline=\"central\">x</text>\n",
                    p->x, y_svg);
        }
        iterador_destroi(it); // Lembre-se de destruir o iterador
    }

    // 2. Pontos "#" vermelhos (local da explosão) - CENTRALIZEI
    if (infos->pontos_hashtag_vermelhos) {
        Iterador it = lista_iterador(infos->pontos_hashtag_vermelhos);
        while (iterador_tem_proximo(it)) {
            Coord *p = (Coord*) iterador_proximo(it);
            double y_svg = altura_canvas - p->y;

            // Centralizado e com estilo diferente (maior e azul escuro) para diferenciar da destruição.
            fprintf(arq, "  <text x=\"%.2f\" y=\"%.2f\" font-size=\"14\" font-weight=\"bold\" fill=\"darkblue\" text-anchor=\"middle\" fill-opacity=\"0,4\" dominant-baseline=\"central\">#</text>\n",
                    p->x, y_svg);
        }
        iterador_destroi(it); // Lembre-se de destruir o iterador
    }

    // 3. Retângulos de seleção
    if (infos->retangulos_selecao) {
        Iterador it = lista_iterador(infos->retangulos_selecao);
        while (iterador_tem_proximo(it)) {
            BoundingBox *bb = (BoundingBox*) iterador_proximo(it);
            double rect_y_svg = altura_canvas - (bb->y + bb->h);
            fprintf(arq, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
                    bb->x, rect_y_svg, bb->w, bb->h);
            fprintf(arq, "stroke=\"red\" fill=\"none\" stroke-dasharray=\"4 2\" stroke-opacity = \"0.8\" />\n");
        }
    }

    // 4. Círculos nas âncoras (formas selecionadas)
    if (infos->pontos_ancoras) {

        Iterador it = lista_iterador(infos->pontos_ancoras);
        while (iterador_tem_proximo(it)) {
             contador_ancoras_desenhadas++;
            Coord *p = (Coord*) iterador_proximo(it);
            double y_svg = altura_canvas - p->y;
            fprintf(arq, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\" fill=\"red\" fill-opacity =\"0.5\"  />\n",
                    p->x, y_svg);
        }
    }

    // 5. Vetores de movimento (transp / cln)
    if (infos->vetores_movimento) {
        Iterador it = lista_iterador(infos->vetores_movimento);
        while (iterador_tem_proximo(it)) {
            VetorMovimento *vm = (VetorMovimento*) iterador_proximo(it);
            double y1 = altura_canvas - vm->origem.y;
            double y2 = altura_canvas - vm->destino.y;
            fprintf(arq, "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ",
                    vm->origem.x, y1, vm->destino.x, y2);
            fprintf(arq, "stroke=\"red\" stroke-width=\"1\" marker-end=\"url(#arrow)\" stroke-opacity=\"0.2\" />\n");
        }
    }


    if (infos->formas_destacadas) {
        Iterador it = lista_iterador(infos->formas_destacadas);
        while (iterador_tem_proximo(it)) {
            Info info_forma = iterador_proximo(it);
            
            // Pega o tipo para a nossa lógica condicional
            DescritorTipoInfo tipo = formaGetTipo(info_forma);

            // 1. Pega a âncora. O destaque SEMPRE será centrado nela.
            double anchor_x, anchor_y;
            GetXY(&anchor_x, &anchor_y, info_forma);

       
          double raio_destaque = getRaioEquivalente(info_forma);
      
            double y_svg = altura_canvas - anchor_y;

            fprintf(arq, "  \n");
            fprintf(arq, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" ", anchor_x, y_svg, raio_destaque);
            fprintf(arq, "fill=\"none\" stroke=\"red\" stroke-width=\"1\" />\n");
        }
        iterador_destroi(it);
    }

}


static void escreverFormaCallback(SmuTreap t, Node n, Info i, double x, double y, void *aux) {
    ContextoSVGInterno *contexto = (ContextoSVGInterno*)aux;
    DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
    FILE *arquivo = contexto->arquivo;

    contexto->contador_formas++;


    double y_svg = contexto->altura_canvas - y;
    const char* classe_css = "forma-normal";


  
    char *cor_borda, *cor_preenchimento;
    obterCoresForma(i, tipo, &cor_borda, &cor_preenchimento);
    const char *corb_final = validarCor(cor_borda, "black");
    const char *corp_final = validarCor(cor_preenchimento, "none");

    fprintf(arquivo, "  <!-- Forma %d (Tipo: %d, ID: %d) -->\n",
         contexto->contador_formas, tipo, formaGetId(i));


    switch (tipo) {
        case CIRCULO: {
            Circle* c = (Circle*)((forma*)i)->forma;
            double raio = obterRaioCirculo(i);
            fprintf(arquivo, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" ",
                    x, y_svg, raio);
            fprintf(arquivo, "fill=\"%s\" stroke=\"%s\" fill-opacity= \"0.8\" stroke-opacity=\"0.6\" class=\"%s\" stroke-width=\"%.2f\" />\n",
                    corp_final, corb_final, classe_css, c->largura_borda );
            break;
        }

        case RETANGULO: {
             Rect* r = (Rect*)((forma*)i)->forma;
            double forma_x, forma_y, forma_w, forma_h;
            formaCalculaBoundingBox(tipo, i, &forma_x, &forma_y, &forma_w, &forma_h);
            // Corrige Y do retângulo para o sistema SVG:
            double rect_y_svg = contexto->altura_canvas - (forma_y + forma_h); // deu certo essa orientação
            fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
                    forma_x, rect_y_svg, forma_w, forma_h);
            fprintf(arquivo, "fill=\"%s\" stroke=\"%s\" fill-opacity = \"0.7\" stroke-opacity=\"0.5\" class=\"%s\" stroke-width=\"%.2f\" />\n",
                    corp_final, corb_final, classe_css, r->largura_borda);
            break;
        }

        case LINHA: {
            Line* l = (Line*)((forma*)i)->forma;
            double x1, y1, x2, y2;
            obterDadosLinha(i, &x1, &y1, &x2, &y2);
            double y1_svg = contexto->altura_canvas - y1;
            double y2_svg = contexto->altura_canvas - y2;
            fprintf(arquivo, "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ",
                    x1, y1_svg, x2, y2_svg);
            fprintf(arquivo, "stroke=\"%s\" stroke-opacity=\"1.0\" class=\"%s\" stroke-width=\"%.2f\" />\n", corb_final, classe_css, l->largura_borda);
            break;
        }

        case TEXTO: {
            forma *f = (forma*)i;
            Text *t = (Text*)f->forma;

            const char* texto = t->txto;
            char ancora = t->a;

            const char* text_anchor;
            switch (ancora) {
                case 'i': text_anchor = "start"; break;
                case 'm': text_anchor = "middle"; break;
                case 'f': text_anchor = "end"; break;
                default:  text_anchor = "start"; break;
            }

        
            fprintf(arquivo,
                     "  <text x=\"%.2f\" y=\"%.2f\" transform=\"scale(1,-1)\" ", x, -y_svg);

            fprintf(arquivo, "font-family=\"%s\" font-size=\"%d\" font-weight=\"%s\" ",
                    t->fontFamily, t->fontSize, t->fontWeight);
            fprintf(arquivo, "fill=\"%s\" stroke=\"%s\" fill-opacity=\"1.0\" stroke-opacity=\"1.0\" text-anchor=\"%s\" class=\"%s\" stroke-width=\"%.2f\"  >",
             corp_final, corb_final, text_anchor, classe_css, t->largura_borda);

            imprimeTextoEscapado(arquivo, texto);
            fprintf(arquivo, "</text>\n");
            break;
        }

        default:
            fprintf(arquivo, "  <!-- Tipo de forma desconhecido: %d -->\n", tipo);
            break;
    }

    fprintf(arquivo, "\n");
}


void calcularDimensoesCanvas(SmuTreap arvore, double *largura, double *altura) {
    if (!arvore || !largura || !altura) {
        *largura = 800.0; 
        *altura = 600.0;
        return;
    }
    
    CalculoDimensoes calc = {0, 0, 0, 0, true};
    
    // calculando dimensoes
    visitaProfundidadeSmuT(arvore, calcularDimensoesCallback, &calc);
    
    if (calc.primeira_forma) {

        *largura = 1600.0;
        *altura = 1200.0;
    } else {
        // margemzinha
        *largura = (calc.max_x - calc.min_x) + (4 * MARGEM_CANVAS);
        *altura = (calc.max_y - calc.min_y) + (4 * MARGEM_CANVAS);
        
        // dimensao  minima pra visualizar
        if (*largura < 400.0) *largura = 400.0;
        if (*altura < 300.0) *altura = 300.0;
    }
}

bool gerarSVG(SmuTreap arvore, InformacoesAdicionais infos, const char *caminho) {
    if (!arvore || !caminho) {
        return false;
    }
    
    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo SVG: %s\n", caminho);
        return false;
    }
    
   
    double largura, altura;
    calcularDimensoesCanvas(arvore, &largura, &altura);
    
    // Escrever cabeçalho
    escreverCabecalhoSVG(arquivo, largura, altura);
    
    // Criar contexto interno
    ContextoSVGInterno *contexto = criarContextoSVG(arquivo, largura, altura, infos);
    if (!contexto) {
        fclose(arquivo);
        return false;
    }
    
    fprintf(arquivo, "  <!-- FORMAS GEOMÉTRICAS -->\n");
    visitaProfundidadeSmuT(arvore, escreverFormaCallback, contexto);
    desenharInfosAdicionaisSVG(contexto);
    
    escreverRodapeSVG(arquivo);
    
    fclose(arquivo);
    
    printf("SVG gerado com sucesso: %s\n", caminho);
    printf("Dimensões: %.0fx%.0f pixels\n", largura, altura);
    printf("Formas processadas: %d\n", contexto->contador_formas);
    
   
    free(contexto);
    
    return true;
}
void calcular_ancora(DescritorTipoInfo tipo, Info info, double *ancora_x, double *ancora_y) {
    switch(tipo) {
        case CIRCULO: {
            forma *f = (forma*)info;
            Circle *c = (Circle*)f->forma;
            *ancora_x = c->x; // centro
            *ancora_y = c->y; // centro
            break;
        }
        case RETANGULO: {
             forma *f = (forma*)info;
            Rect *r = (Rect*)f->forma;
            *ancora_x = r->x; // canto superior esquerdo
            *ancora_y = r->y; // canto superior esquerdo
            break;
        }
        case LINHA: {
             forma *f = (forma*)info;
            Line *l = (Line*)f->forma;
            *ancora_x = l->x1; // primeira extremidade
            *ancora_y = l->y1; // primeira extremidade
            break;
        }
        case TEXTO: {
             forma *f = (forma*)info;
            Text *t = (Text*)f->forma;
            // A âncora depende do tipo de âncora do texto
            *ancora_x = t->x;
            *ancora_y = t->y;
            
            // Ajustar baseado na âncora do texto se necessário
            // 'i' = início, 'm' = meio, 'f' = fim
            // Por simplicidade, usar sempre x,y como âncora
            break;
        }
        default:
            *ancora_x = 0;
            *ancora_y = 0;
            break;
    }
}


