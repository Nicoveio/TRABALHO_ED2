

		case TEXTO: {
 		   forma *f = (forma*)i;
   		   Text *t = (Text*)f->forma;

    const char* texto = t->txto;
    char ancora = t->a;

    double texto_x = x;
    double texto_y_svg = converterY(y, altura);

    // Alinhamento via âncora
    const char* text_anchor;
    switch (ancora) {
        case 'i': text_anchor = "start"; break;
        case 'm': text_anchor = "middle"; break;
        case 'f': text_anchor = "end"; break;
        default:  text_anchor = "start"; break;
    }

    fprintf(arquivo, "  <text x=\"%.2f\" y=\"%.2f\" ", texto_x, texto_y_svg);
    fprintf(arquivo, "font-family=\"%s\" font-size=\"%d\" font-weight=\"%s\" ",
        t->fontFamily, t->fontSize, t->fontWeight);
    fprintf(arquivo, "fill=\"%s\" stroke=\"%s\" text-anchor=\"%s\" class=\"%s\">",
        corp_final, corb_final, text_anchor, classe_css);

    imprimeTextoEscapado(arquivo, texto);  // <- texto seguro
    fprintf(arquivo, "</text>\n");
    break;
}
