PROJ_NAME = ted
ALUNO = nicolashlo

# Compilador e flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0
LIBS = -lm  # math library, importante para pow, sqrt...


SRCS = main.c geo.c qry.c fila.c lista.c formas.c smutreap.c svg.c utils.c tabelaBusca.c


OBJETOS = $(SRCS:.c=.o)


$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(OBJETOS) $(LIBS) $(LDFLAGS)


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Arquivos mais básicos, sem muitas dependências
fila.o: fila.c fila.h
lista.o: lista.c lista.h
utils.o: utils.c utils.h

# Tabela de Busca usa apenas seu próprio header
tabelaBusca.o: tabelaBusca.c tabelaBusca.h

# SmuTreap depende de fila e lista
smutreap.o: smutreap.c smutreap.h fila.h lista.h

# Formas depende da SmuTreap (para os tipos Info, Node, etc.)
formas.o: formas.c formas.h smutreap.h

# SVG desenha formas, então depende de formas.h e indiretamente de smutreap.h
svg.o: svg.c svg.h formas.h smutreap.h lista.h

# GEO processa formas e usa a tabela de busca e a árvore
geo.o: geo.c geo.h formas.h smutreap.h tabelaBusca.h utils.h

# QRY é o mais complexo, usa quase tudo
qry.o: qry.c qry.h formas.h smutreap.h lista.h svg.h utils.h

# MAIN é o orquestrador, então inclui (quase) todos os headers públicos
main.o: main.c geo.h utils.h smutreap.h formas.h svg.h qry.h

clean:
	rm -f $(OBJETOS) $(PROJ_NAME)

pack:
	@echo "--- Criando arquivo LEIA-ME.txt ---"
	@echo "Nicolas Henrique Lima de Olivera" > ../LEIA-ME.txt
	@echo "202400560214" >> ../LEIA-ME.txt

	@echo "--- Compilando o projeto antes de empacotar ---"
	@$(MAKE) 

	@echo "--- Criando o arquivo .zip ---"
	@rm -f ../$(ALUNO).zip
	
	
	@cd ..; zip $(ALUNO).zip -r src -x "src/$(PROJ_NAME)" -x "src/*.o" -x "src/*~" -x "src/.*"


	@cd ..; zip $(ALUNO).zip LEIA-ME.txt
	
	@echo "--- Limpando arquivos temporários ---"

	@rm -f ../LEIA-ME.txt
	
	
	@echo "Arquivo $(ALUNO).zip criado com sucesso no diretório superior!"
	

