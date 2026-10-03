# Dependência: libxml2 (pacote libxml2-dev / libxml2-devel)
XML2_CONFIG ?= xml2-config

ifeq ($(filter clean uninstall,$(MAKECMDGOALS)),)
ifeq ($(shell command -v $(XML2_CONFIG) 2>/dev/null),)
$(error $(XML2_CONFIG) não encontrado. Instale a libxml2 de desenvolvimento (ex.: apt install libxml2-dev ou dnf install libxml2-devel))
endif
endif


# Flags do compilador
# -MMD -MP gera arquivos .d para recompilar quando um header muda
CFLAGS := -Werror -Wall -Wextra -Wwrite-strings -std=c99 -g -fPIC -MMD -MP $(shell $(XML2_CONFIG) --cflags 2>/dev/null)


# Flags para adicionar libs
LIBS := $(shell $(XML2_CONFIG) --libs 2>/dev/null)


#-I includes
INCLUDE = ./include


#Paths do código fonte
SOURCE = ./src/libnfe


#Objetos compilados para Library
LOBJ = ./obj


#Path da lib
LIB = ./lib


#Nomes da biblioteca compartilhada
LIBNAME  = libnfe.so
SONAME   = $(LIBNAME).0
REALNAME = $(LIBNAME).0.0


#Destino do `make install` (DESTDIR permite instalar em diretório temporário)
PREFIX     ?= /usr/local
LIBDIR     ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include


#Nome de todas os arquivos fontes com path e extensão (*.c)
C_SOURCE = $(wildcard $(SOURCE)/*.c)


#Objetos com path ./obj/ e extensão (*.o)
OBJ = $(addprefix $(LOBJ)/,$(notdir $(C_SOURCE:.c=.o)))


.PHONY: all libnfe install uninstall test clean

all: libnfe

libnfe: $(LIB)/$(REALNAME) $(LIB)/$(SONAME) $(LIB)/$(LIBNAME)

$(LIB)/$(REALNAME): $(OBJ) | $(LIB)
	$(CC) -shared -Wl,-soname,$(SONAME) $^ -o $@ $(LIBS)

#Links simbólicos: libnfe.so -> libnfe.so.0 -> libnfe.so.0.0
$(LIB)/$(SONAME): $(LIB)/$(REALNAME)
	ln -sf $(REALNAME) $@

$(LIB)/$(LIBNAME): $(LIB)/$(SONAME)
	ln -sf $(SONAME) $@


#Compila se não existir, ou recompila, se houve alteracao no fonte
$(LOBJ)/%.o: $(SOURCE)/%.c | $(LOBJ)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@


#Cria os diretórios de saída
$(LOBJ) $(LIB):
	mkdir -p $@


install: libnfe
	install -d $(DESTDIR)$(LIBDIR) $(DESTDIR)$(INCLUDEDIR)/libnfe
	install -m 755 $(LIB)/$(REALNAME) $(DESTDIR)$(LIBDIR)/
	ln -sf $(REALNAME) $(DESTDIR)$(LIBDIR)/$(SONAME)
	ln -sf $(SONAME) $(DESTDIR)$(LIBDIR)/$(LIBNAME)
	install -m 644 $(INCLUDE)/libnfe/*.h $(DESTDIR)$(INCLUDEDIR)/libnfe/


uninstall:
	rm -fv $(DESTDIR)$(LIBDIR)/$(LIBNAME) $(DESTDIR)$(LIBDIR)/$(SONAME) $(DESTDIR)$(LIBDIR)/$(REALNAME)
	rm -rfv $(DESTDIR)$(INCLUDEDIR)/libnfe


#Testes: cada tests/test_*.c vira um executável em obj/, compilado junto com
#os fontes da biblioteca e com sanitizers (desative com SANITIZE=)
TESTES = $(addprefix $(LOBJ)/,$(basename $(notdir $(wildcard tests/test_*.c))))
SANITIZE ?= -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS_TESTE = $(filter-out -MMD -MP,$(CFLAGS)) $(SANITIZE)

test: $(TESTES)
	@for t in $(TESTES); do echo "== $$t"; $$t tests || exit 1; done

$(LOBJ)/test_%: tests/test_%.c tests/teste.h $(C_SOURCE) $(wildcard $(INCLUDE)/libnfe/*.h) | $(LOBJ)
	$(CC) $(CFLAGS_TESTE) -I$(INCLUDE) $< $(C_SOURCE) -o $@ $(LIBS)


clean:
	rm -fv $(LOBJ)/*.o $(LOBJ)/*.d $(LIB)/libnfe.so* $(TESTES)


-include $(OBJ:.o=.d)
