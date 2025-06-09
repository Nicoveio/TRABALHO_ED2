#!/bin/bash

# ===================================================================
# SCRIPT DE TESTE AUTOMATIZADO v4.0
# - Lógica de caminho corrigida para compatibilidade com utils.c
# ===================================================================

# --- CONFIGURAÇÃO ---
# Altere estes caminhos para os corretos no seu computador.
# IMPORTANTE: Não coloque uma barra "/" no final dos caminhos.
PASTA_CASOS="/home/nicolas-oliveira/Downloads/t1"
PASTA_SAIDA="/home/nicolas-oliveira/Documentos/EVANDRO_ED2/ted/saida"
EXECUTAVEL="./ted"
# --- FIM DA CONFIGURAÇÃO ---


# ... (seção de cores e compilação continua a mesma) ...

# 3. Executar os testes
echo -e "${YELLOW}Iniciando a execução dos casos de teste...${NC}"

# Itera sobre todos os arquivos .geo na pasta de casos
for ARQUIVO_GEO_COMPLETO in "$PASTA_CASOS"/*.geo; do
    
    NOME_BASE=$(basename "$ARQUIVO_GEO_COMPLETO" .geo)
    SUBPASTA_QRY="$PASTA_CASOS/$NOME_BASE"

    # Verifica se a subpasta de qry correspondente existe
    if [ -d "$SUBPASTA_QRY" ]; then
        
        # Itera sobre todos os .qry dentro da subpasta
        for ARQUIVO_QRY_COMPLETO in "$SUBPASTA_QRY"/*.qry; do
            if [ -f "$ARQUIVO_QRY_COMPLETO" ]; then

                # Extrai apenas os nomes dos arquivos, sem o caminho
                ARQUIVO_GEO=$(basename "$ARQUIVO_GEO_COMPLETO")
                ARQUIVO_QRY=$(basename "$ARQUIVO_QRY_COMPLETO")

                echo -e "\n${YELLOW}Executando teste:${NC}"
                echo -e "  GEO: ${GREEN}$ARQUIVO_GEO${NC} (da pasta $PASTA_CASOS)"
                echo -e "  QRY: ${GREEN}$ARQUIVO_QRY${NC} (da pasta $SUBPASTA_QRY)"
                
                # --- MONTAGEM DO COMANDO CORRIGIDA ---
                # -e aponta para a pasta do QRY
                # -f usa um caminho relativo ("../") para subir um nível e encontrar o GEO
                "$EXECUTAVEL" -e "$SUBPASTA_QRY" -f "../$ARQUIVO_GEO" -o "$PASTA_SAIDA" -q "$ARQUIVO_QRY"
                
                echo -e "${GREEN}Teste concluído.${NC}"
                echo "-----------------------------------------------------"
            fi
        done
    fi
done

echo -e "${GREEN}Todos os testes foram executados! Verifique a pasta de saída.${NC}"
