#include "formas.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    int id;             // identificador numérico
    double x, y;        // centro
    double r;           // raio
    char corb[64];      // cor da borda
    char corp[64];      // cor de preenchimento
} Circle;

typedef struct {
    int id;
    double x, y;        // âncora (canto superior esquerdo)
    double w, h;        // largura e altura
    char corb[64];
    char corp[64];
} Rect;

typedef struct {
    int id;
    double x1, y1, x2, y2; // coordenadas das extremidades
    char cor[64];
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
} Text;

typedef struct{
    void * forma;
    DescritorTipoInfo tipo;
}forma;




Forma criaForma(DescritorTipoInfo tipo) {
    forma *f = malloc(sizeof(forma));
    if (!f) return NULL;

    f->tipo = tipo;

    switch(tipo) {
        case CIRCULO:
            f->forma = malloc(sizeof(Circle));
            if (f->forma) memset(f->forma, 0, sizeof(Circle));
            break;
        case RETANGULO:
            f->forma = malloc(sizeof(Rect));
            if (f->forma) memset(f->forma, 0, sizeof(Rect));
            break;
        case LINHA:
            f->forma = malloc(sizeof(Line));
            if (f->forma) memset(f->forma, 0, sizeof(Line));
            break;
        case TEXTO:
            f->forma = malloc(sizeof(Text));
            if (f->forma) memset(f->forma, 0, sizeof(Text));
            break;
        default:
            free(f);
            return NULL;
    }

    if (!f->forma) {
        free(f);
        return NULL;
    }

    return (Forma) f;
}

void liberaForma(Forma f) {
    if (!f) return;
    forma *form = (forma*)f;
    if (form->forma) free(form->forma);
    free(form);
}

void setCircle(Forma f, int id, double x, double y, double r, const char* corb, const char* corp) {
    if (!f || ((forma*)f)->tipo != CIRCULO) return;
    Circle* c = (Circle*)((forma*)f)->forma;
    c->id = id;
    c->x = x;
    c->y = y;
    c->r = r;
    strncpy(c->corb, corb, 63);
    c->corb[63] = '\0';
    strncpy(c->corp, corp, 63);
    c->corp[63] = '\0';
}

void setRect(Forma f, int id, double x, double y, double w, double h, const char* corb, const char* corp) {
    if (!f || ((forma*)f)->tipo != RETANGULO) return;
    Rect* r = (Rect*)((forma*)f)->forma;
    r->id = id;
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    strncpy(r->corb, corb, 63);
    r->corb[63] = '\0';
    strncpy(r->corp, corp, 63);
    r->corp[63] = '\0';
}

void setLine(Forma f, int id, double x1, double y1, double x2, double y2, const char* cor) {
    if (!f || ((forma*)f)->tipo != LINHA) return;
    Line* l = (Line*)((forma*)f)->forma;
    l->id = id;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;
    strncpy(l->cor, cor, 63);
    l->cor[63] = '\0';
}

void setText(Forma f, int id, double x, double y, const char* corb, const char* corp, char a, const char* txto,
             const char* fontFamily, const char* fontWeight, int fontSize) {
    if (!f || ((forma*)f)->tipo != TEXTO) return;
    Text* t = (Text*)((forma*)f)->forma;

    t->id = id;
    t->x = x;
    t->y = y;
    t->a = a;
    strncpy(t->corb, corb, 63);
    t->corb[63] = '\0';
    strncpy(t->corp, corp, 63);
    t->corp[63] = '\0';
    strncpy(t->txto, txto, 1023);
    t->txto[1023] = '\0';

    strncpy(t->fontFamily, fontFamily, 31);
    t->fontFamily[31] = '\0';
    strncpy(t->fontWeight, fontWeight, 7);
    t->fontWeight[7] = '\0';
    t->fontSize = fontSize;
}



void boundingBoxCirculo(Circle *c, double *x, double *y, double *w, double *h) {

    if (c== NULL) {
    printf("CIRCULO VAZIO");
    return;
}

    *x = c->x - c->r;
    *y = c->y - c->r;
    *w = 2 * c->r;
    *h = 2 * c->r;
}

void boundingBoxRetangulo(Rect *r, double *x, double *y, double *w, double *h) {
    *x = r->x;
    *y = r->y;
    *w = r->w;
    *h = r->h;
}

void boundingBoxLinha(Line *l, double *x, double *y, double *w, double *h) {
    double x_min = l->x1 < l->x2 ? l->x1 : l->x2;
    double y_min = l->y1 < l->y2 ? l->y1 : l->y2;
    double x_max = l->x1 > l->x2 ? l->x1 : l->x2;
    double y_max = l->y1 > l->y2 ? l->y1 : l->y2;

    *x = x_min;
    *y = y_min;
    *w = x_max - x_min;
    *h = y_max - y_min;
}

void boundingBoxTexto(Text *t, double *x, double *y, double *w, double *h) {
   
    double largura_char = 8.0;
    double altura_char = 12.0;
    int len = strlen(t->txto);

    double largura_total = len * largura_char;

   
    switch (t->a) {
        case 'i':
            *x = t->x;
            break;
        case 'm': 
            *x = t->x - largura_total / 2.0;
            break;
        case 'f': 
            *x = t->x - largura_total;
            break;
        default: 
            *x = t->x;
    }

    *y = t->y;
    *w = largura_total;
    *h = altura_char;
}
void formaCalculaBoundingBox(DescritorTipoInfo tipo, Info i, double *x, double *y, double *w, double *h) {
    forma *f = (forma*) i;

    switch (tipo) {
        case CIRCULO:
            boundingBoxCirculo((Circle*)f->forma, x, y, w, h);
            break;
        case RETANGULO:
            boundingBoxRetangulo((Rect*)f->forma, x, y, w, h);
            break;
        case LINHA:
            boundingBoxLinha((Line*)f->forma, x, y, w, h);
            break;
        case TEXTO:
            boundingBoxTexto((Text*)f->forma, x, y, w, h);
            break;
        default:
            *x = *y = *w = *h = 0;
    }
}



// Nova assinatura, agora aceitando o parâmetro 'tolerancia'
bool boundingBoxDentroDeRegiao(double bx, double by, double bw, double bh, 
                               double x1, double y1, double x2, double y2){

   const double TOLERANCIA_GEOMETRICA = 0.01;

    // Normalização das coordenadas da região (seu código já faz isso, está ótimo)
    double rx1 = x1 < x2 ? x1 : x2;
    double rx2 = x1 > x2 ? x1 : x2;
    double ry1 = y1 < y2 ? y1 : y2;
    double ry2 = y1 > y2 ? y1 : y2;

    // A verificação, agora usando a nossa tolerância geométrica local e previsível.
    return (bx >= rx1 - TOLERANCIA_GEOMETRICA) &&
           (by >= ry1 - TOLERANCIA_GEOMETRICA) &&
           ((bx + bw) <= rx2 + TOLERANCIA_GEOMETRICA) &&
           ((by + bh) <= ry2 + TOLERANCIA_GEOMETRICA);
}




static bool circuloDentroDeRegiao(Circle *c, double x1, double y1, double x2, double y2, double tolerancia) {
    return (c->x - c->r >= x1 - tolerancia) && (c->x + c->r <= x2 + tolerancia) &&
           (c->y - c->r >= y1 - tolerancia) && (c->y + c->r <= y2 + tolerancia);
}

static bool retanguloDentroDeRegiao(Rect *r, double x1, double y1, double x2, double y2, double tolerancia) {
    return (r->x >= x1 - tolerancia) && (r->x + r->w <= x2 + tolerancia) &&
           (r->y >= y1 - tolerancia) && (r->y + r->h <= y2 + tolerancia);
}

static bool linhaDentroDeRegiao(Line *l, double x1, double y1, double x2, double y2, double tolerancia) {
    // Para uma linha, verificamos se ambos os pontos estão dentro da região com tolerância.
    return (l->x1 >= x1 - tolerancia) && (l->x1 <= x2 + tolerancia) &&
           (l->y1 >= y1 - tolerancia) && (l->y1 <= y2 + tolerancia) &&
           (l->x2 >= x1 - tolerancia) && (l->x2 <= x2 + tolerancia) &&
           (l->y2 >= y1 - tolerancia) && (l->y2 <= y2 + tolerancia);
}

static bool textoDentroDeRegiao(Text *t, double x1, double y1, double x2, double y2, double tolerancia) {
    // Sua lógica para calcular o bounding box do texto é uma boa aproximação.
    // Vamos usá-la aqui.
    double largura_char = 8.0; // Valores aproximados
    double altura_char = 12.0;
    double largura_total = strlen(t->txto) * largura_char;
    double bx;

    switch (t->a) {
        case 'i': bx = t->x; break;
        case 'm': bx = t->x - largura_total / 2.0; break;
        case 'f': bx = t->x - largura_total; break;
        default:  bx = t->x;
    }

    double by = t->y - altura_char; // Y da âncora geralmente é a base do texto.

    return (bx >= x1 - tolerancia) && (bx + largura_total <= x2 + tolerancia) &&
           (by >= y1 - tolerancia) && (by + altura_char <= y2 + tolerancia);
}





bool formaDentroDeRegiao(SmuTreap t, Node n, Info i, double x1, double y1, double x2, double y2) {
    if (!i) return false;

    double forma_x, forma_y, forma_w, forma_h;
    DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
    

    formaCalculaBoundingBox(tipo, i, &forma_x, &forma_y, &forma_w, &forma_h);


   
    return boundingBoxDentroDeRegiao(forma_x, forma_y, forma_w, forma_h,
                                     x1, y1, x2, y2);
}


void printForma(Forma f) {
    if (!f) return;
    forma *fo = (forma*) f;

    switch (fo->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*) fo->forma;
            printf("Circulo id=%d, x=%.2f, y=%.2f, r=%.2f, corb=%s, corp=%s\n",
                   c->id, c->x, c->y, c->r, c->corb, c->corp);
            break;
        }
        default:
            printf("Tipo desconhecido\n");
    }
}

bool formaPontoInternoAInfo(SmuTreap t, Node n, Info i, double x, double y) {
    if (!i) return false;
    forma *f = (forma*)i;

    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            double dx = x - c->x;
            double dy = y - c->y;
            return dx * dx + dy * dy <= c->r * c->r;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            return x >= r->x && x <= r->x + r->w && y >= r->y && y <= r->y + r->h;
        }
        case LINHA:
            return false; 
        case TEXTO: {
            Text *t = (Text*)f->forma;
            return fabs(t->x - x) < 1e-6 && fabs(t->y - y) < 1e-6;
        }
    }
    return false;
}

bool formaAncoraIgual(Info i, double x, double y) {
    if (!i) return false;
    forma *f = (forma*)i;

    double fx = 0, fy = 0;
    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            fx = c->x; fy = c->y;
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            fx = r->x; fy = r->y;
            break;
        }
        case LINHA: {
            Line *l = (Line*)f->forma;
            fx = l->x1; fy = l->y1;
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            fx = t->x; fy = t->y;
            break;
        }
    }

    return fabs(fx - x) < 1e-6 && fabs(fy - y) < 1e-6;
}

int formaGetId(Info i) {
    if (!i) return -1;
    forma *f = (forma*)i;

    switch (f->tipo) {
        case CIRCULO: return ((Circle*)f->forma)->id;
        case RETANGULO: return ((Rect*)f->forma)->id;
        case LINHA: return ((Line*)f->forma)->id;
        case TEXTO: return ((Text*)f->forma)->id;
    }
    return -1;
}

void formaMovePara(Info i, double x, double y) {
    if (!i) return;
    forma *f = (forma*)i;

    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            c->x = x; c->y = y;
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            r->x = x; r->y = y;
            break;
        }
        case LINHA: {
            Line *l = (Line*)f->forma;
            double dx = x - l->x1;
            double dy = y - l->y1;
            l->x1 = x; l->y1 = y;
            l->x2 += dx; l->y2 += dy;
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            t->x = x; t->y = y;
            break;
        }
    }
}


Forma formaClona(Forma i) {
    if (!i) return NULL;
    forma *f = (forma*)i;

    Forma nova = criaForma(f->tipo);
    if (!nova) return NULL;

    switch (f->tipo) {
        case CIRCULO: {
            Circle *src = (Circle*)f->forma;
            setCircle(nova, src->id, src->x, src->y, src->r, src->corb, src->corp);
            break;
        }
        case RETANGULO: {
            Rect *src = (Rect*)f->forma;
            setRect(nova, src->id, src->x, src->y, src->w, src->h, src->corb, src->corp);
            break;
        }
        case LINHA: {
            Line *src = (Line*)f->forma;
            setLine(nova, src->id, src->x1, src->y1, src->x2, src->y2, src->cor);
            break;
        }
        case TEXTO: {
            Text *src = (Text*)f->forma;
            setText(nova, src->id, src->x, src->y, src->corb, src->corp, src->a, src->txto, src->fontFamily, src->fontWeight, src->fontSize);
            break;
        }
    }

    return nova;
}


void formaInverteCores(Info i) {
    if (!i) return;
    forma *f = (forma*)i;

    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            char tmp[64];
            strcpy(tmp, c->corb);
            strcpy(c->corb, c->corp);
            strcpy(c->corp, tmp);
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            char tmp[64];
            strcpy(tmp, r->corb);
            strcpy(r->corb, r->corp);
            strcpy(r->corp, tmp);
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            char tmp[64];
            strcpy(tmp, t->corb);
            strcpy(t->corb, t->corp);
            strcpy(t->corp, tmp);
            break;
        }
        case LINHA:
            break;
    }
}
void formaSetCores(Info i, const char* corb, const char* corp) {
    if (!i) return;
    forma *f = (forma*)i;

    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            strncpy(c->corb, corb, 63); c->corb[63] = '\0';
            strncpy(c->corp, corp, 63); c->corp[63] = '\0';
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            strncpy(r->corb, corb, 63); r->corb[63] = '\0';
            strncpy(r->corp, corp, 63); r->corp[63] = '\0';
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            strncpy(t->corb, corb, 63); t->corb[63] = '\0';
            strncpy(t->corp, corp, 63); t->corp[63] = '\0';
            break;
        }
        case LINHA:
                Line *l = (Line*)f->forma;
            strncpy(l->cor, corb, 63);
            l->cor[63] = '\0';
            break; 
    }
}

DescritorTipoInfo formaGetTipo(Info i) {
    if (!i) return -1;
    forma *f = (forma*)i;
    return f->tipo;
}


double circuloGetRaio(Info i) {
    if (!i) return 0.0;
    forma *f = (forma*)i;
    if (f->tipo != CIRCULO) return 0.0;
    
    Circle *c = (Circle*)f->forma;
    return c->r;
}


double retanguloGetLargura(Info i) {
    if (!i) return 0.0;
    forma *f = (forma*)i;
    if (f->tipo != RETANGULO) return 0.0;
    
    Rect *r = (Rect*)f->forma;
    return r->w;
}

double retanguloGetAltura(Info i) {
    if (!i) return 0.0;
    forma *f = (forma*)i;
    if (f->tipo != RETANGULO) return 0.0;
    
    Rect *r = (Rect*)f->forma;
    return r->h;
}

void linhaGetPontos(Info i, double *x1, double *y1, double *x2, double *y2) {
    if (!i || !x1 || !y1 || !x2 || !y2) {
        if (x1) *x1 = 0.0;
        if (y1) *y1 = 0.0;
        if (x2) *x2 = 0.0;
        if (y2) *y2 = 0.0;
        return;
    }
    
    forma *f = (forma*)i;
    if (f->tipo != LINHA) {
        *x1 = *y1 = *x2 = *y2 = 0.0;
        return;
    }
    
    Line *l = (Line*)f->forma;
    *x1 = l->x1;
    *y1 = l->y1;
    *x2 = l->x2;
    *y2 = l->y2;
}


char* textoGetConteudo(Info i) {
    if (!i) return NULL;
    forma *f = (forma*)i;
    if (f->tipo != TEXTO) return NULL;
    
    Text *t = (Text*)f->forma;
    return t->txto;
}


int textoGetConteudoCopia(Info i, char *buffer, int tamanho_buffer) {
    if (!i || !buffer || tamanho_buffer <= 0) return 0;
    
    forma *f = (forma*)i;
    if (f->tipo != TEXTO) return 0;
    
    Text *t = (Text*)f->forma;
    strncpy(buffer, t->txto, tamanho_buffer - 1);
    buffer[tamanho_buffer - 1] = '\0';
    
    return strlen(buffer);
}


// Coloque esta nova função em formas.c
void formaFprintfResumo(FILE* stream, Info i) {
    if (!i || !stream) return; // Verifica se o stream e a info são válidos
    
    forma *f = (forma*)i;

    // A lógica do switch é a mesma, apenas trocando printf por fprintf
    switch (f->tipo) {
        case CIRCULO: {
            Circle *c = (Circle*)f->forma;
            fprintf(stream, "Círculo #%d: centro=(%.2lf, %.2lf), raio=%.2lf, corb=%s, corp=%s",
                c->id, c->x, c->y, c->r, c->corb, c->corp);
            break;
        }
        case RETANGULO: {
            Rect *r = (Rect*)f->forma;
            fprintf(stream, "Retângulo #%d: x=%.2lf, y=%.2lf, w=%.2lf, h=%.2lf, corb=%s, corp=%s",
                r->id, r->x, r->y, r->w, r->h, r->corb, r->corp);
            break;
        }
        case LINHA: {
            Line *l = (Line*)f->forma;
            fprintf(stream, "Linha #%d: (%.2lf, %.2lf) até (%.2lf, %.2lf), cor=%s",
                l->id, l->x1, l->y1, l->x2, l->y2, l->cor);
            break;
        }
        case TEXTO: {
            Text *t = (Text*)f->forma;
            fprintf(stream, "Texto #%d: pos=(%.2lf, %.2lf), âncora=%c, texto=\"%s\", corb=%s, corp=%s",
                t->id, t->x, t->y, t->a, t->txto, t->corb, t->corp);
            break;
        }
    }
}

// Opcional, mas recomendado: atualize sua função antiga para reutilizar código
void formaPrintResumo(Info i) {
    formaFprintfResumo(stdout, i); // Chama a nova função, mandando para o terminal (stdout)
    printf("\n"); // Adiciona o newline que o fprintf não tem
}

typedef struct {
    int modo;           
    int contador;      
    FILE *arquivo;      
    void *dados_busca;  
} ContextoVisita;


void formaVisitaNo(SmuTreap t, Node n, Info i, double x, double y, void *aux) {
    if (!t || !n || !i) return;
    
  
    if (!aux) {
        DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
        int id = formaGetId(i);
        
        const char* nome_tipo;
        switch(tipo) {
            case CIRCULO: nome_tipo = "CÍRCULO"; break;
            case RETANGULO: nome_tipo = "RETÂNGULO"; break;
            case LINHA: nome_tipo = "LINHA"; break;
            case TEXTO: nome_tipo = "TEXTO"; break;
            default: nome_tipo = "DESCONHECIDO"; break;
        }
        
        printf("  -> %s (ID=%d) em (%.2f, %.2f)\n", nome_tipo, id, x, y);
        return;
    }
    
  
    ContextoVisita *ctx = (ContextoVisita*)aux;
    
    switch(ctx->modo) {
        case 0: // Debug detalhado
            {
                DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
                int id = formaGetId(i);
                double bb_x, bb_y, bb_w, bb_h;
                getBoundingBoxSmuT(t, n, &bb_x, &bb_y, &bb_w, &bb_h);
                
                printf("  [%d] Tipo=%d ID=%d Âncora=(%.2f,%.2f) BB=[%.1f,%.1f,%.1fx%.1f]\n",
                       ++ctx->contador, tipo, id, x, y, bb_x, bb_y, bb_w, bb_h);
            }
            break;
            
        case 1: // Apenas contagem
            ctx->contador++;
            break;
            
        case 2: // Escrever em arquivo
            if (ctx->arquivo) {
                DescritorTipoInfo tipo = getTypeInfoSrbT(t, n);
                int id = formaGetId(i);
                fprintf(ctx->arquivo, "%d,%d,%.2f,%.2f\n", tipo, id, x, y);
                ctx->contador++;
            }
            break;
            
        case 3: // Busca específica (implementar conforme necessidade)
            // Usar ctx->dados_busca para critérios específicos
            ctx->contador++;
            break;
            
        default:
            // Se aux for apenas um contador simples (int*)
            if (sizeof(int*) == sizeof(void*)) {
                int *contador = (int*)aux;
                (*contador)++;
                printf("  [%d] Forma em (%.2f, %.2f)\n", *contador, x, y);
            }
            break;
    }
}

void percorrerParaDebug(SmuTreap arvore) {
    printf("=== PERCURSO DEBUG ===\n");
    visitaProfundidadeSmuT(arvore, formaVisitaNo, NULL);
}


int contarFormas(SmuTreap arvore) {
    ContextoVisita ctx = {1, 0, NULL, NULL}; // modo=1 (contagem)
    visitaProfundidadeSmuT(arvore, formaVisitaNo, &ctx);
    return ctx.contador;
}

void salvarFormasEmArquivo(SmuTreap arvore, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) return;
    
    fprintf(f, "tipo,id,x,y\n"); // Cabeçalho CSV
    
    ContextoVisita ctx = {2, 0, f, NULL}; // modo=2 (arquivo)
    visitaProfundidadeSmuT(arvore, formaVisitaNo, &ctx);
    
    fclose(f);
    printf("Salvas %d formas em %s\n", ctx.contador, caminho);
}


void GetXY(double *x, double *y, Forma f){
    if(!f){
        printf("forma vazia");
        exit(1);
    }
    forma *f1 = (forma*)f;
    if(f1->tipo==CIRCULO){
        Circle *c= ( Circle*)f1->forma;
        *x = c->x;
        *y = c->y;
    }else if(f1->tipo ==RETANGULO){
        Rect *r = (Rect*)f1->forma;
        *x = r->x;
        *y = r->y;
    }else if(f1->tipo==LINHA){
        Line *l = (Line*)f1->forma;
        *x=l->x1;
        *y=l->y1;
    }else if(f1->tipo ==TEXTO){
        Text *t = (Text*)f1->forma;
        *x=t->x;
        *y = t->y;
    }else printf("Não chegou forma.");

}

int getFormaId(Forma f){
      forma *f1 = (forma*)f;

    if(f1->tipo==CIRCULO){
        Circle *c= ( Circle*)f1->forma;
        return c->id;
    }else if(f1->tipo ==RETANGULO){
        Rect *r = (Rect*)f1->forma;
        return r->id;
    }else if(f1->tipo==LINHA){
        Line *l = (Line*)f1->forma;
        return l->id;
    }else if(f1->tipo ==TEXTO){
        Text *t = (Text*)f1->forma;
        return t->id;
    }else printf("Não chegou forma.");
}