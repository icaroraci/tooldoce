# Dependência: libxml2 (pacote libxml2-dev / libxml2-devel)
XML2_CONFIG ?= xml2-config

ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifeq ($(shell command -v $(XML2_CONFIG) 2>/dev/null),)
$(error $(XML2_CONFIG) não encontrado. Instale a libxml2 de desenvolvimento (ex.: apt install libxml2-dev ou dnf install libxml2-devel))
endif
endif


# Flags do compilador
# -MMD -MP gera arquivos .d para recompilar quando um header muda
CFLAGS := -Werror -Wall -std=c99 -g -fPIC -MMD -MP $(shell $(XML2_CONFIG) --cflags)


# Flags para adicionar libs
LIBS := $(shell $(XML2_CONFIG) --libs)


#-I includes
INCLUDE = ./include


#Paths do código fonte
SOURCE = ./src/libnfe


#Objetos compilados para Library
LOBJ = ./OBJ


#Path da lib
LIB = ./lib


#Nome de todas os arquivos fontes com path e extensão (*.c)
C_SOURCE = $(wildcard $(SOURCE)/*.c)


#Objetos com path ./OBJ/ e extensão (*.o)
OBJ = $(addprefix $(LOBJ)/,$(notdir $(C_SOURCE:.c=.o)))


.PHONY: all libnfe clean

all: libnfe

libnfe: $(LIB)/libnfe.so.0.0

$(LIB)/libnfe.so.0.0: $(OBJ) | $(LIB)
	$(CC) -shared $^ -o $@ $(LIBS)


#Compila se não existir, ou recompila, se houve alteracao no fonte
$(LOBJ)/%.o: $(SOURCE)/%.c | $(LOBJ)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@


#Cria os diretórios de saída
$(LOBJ) $(LIB):
	mkdir -p $@


clean:
	rm -fv $(LOBJ)/*.o $(LOBJ)/*.d $(LIB)/libnfe.so*


-include $(OBJ:.o=.d)
