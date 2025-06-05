PROJ_NAME = ted
ALUNO = nicolashlo

# Compilador e flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0
LIBS = -lm  # math library, importante para pow, sqrt...

# Fontes
SRCS = main.c geo.c qry.c fila.c lista.c formas.c smutreap.c svg.c utils.c

# Objetos correspondentes
OBJETOS = $(SRCS:.c=.o)

# Regra principal: gerar executável
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(OBJETOS) $(LIBS) $(LDFLAGS)

# Regra genérica para .o a partir de .c, com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Dependências manuais (exemplo com as que você falou)
# fila e lista não dependem de nada extra
fila.o: fila.h
lista.o: lista.h

# smutreap depende de fila e lista (além do seu header)
smutreap.o: smutreap.h fila.h lista.h

# formas depende de smutreap
formas.o: formas.h smutreap.h

# main depende de todos que usar (exemplo)
main.o: main.c formas.h smutreap.h fila.h lista.h geo.h qry.h svg.h utils.h

# geo, qry, svg, utils só dependem deles mesmos (headers correspondentes)
geo.o: geo.h
qry.o: qry.h
svg.o: svg.h
utils.o: utils.h

# Limpeza dos arquivos objeto e executável
clean:
	rm -f $(OBJETOS) $(PROJ_NAME)

# Empacotamento para entrega
pack: $(PROJ_NAME)
	rm -f ../$(ALUNO).zip
	echo $(ALUNO)
	date >> .entrega
	cd ..; zip $(ALUNO).zip -r src/*.c src/*.h src/Makefile LEIA-ME.txt .entrega


