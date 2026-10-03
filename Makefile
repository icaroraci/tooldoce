# Dependências: libxml2 (pacote libxml2-dev / libxml2-devel), xmlsec1 com
# OpenSSL (libxmlsec1-dev / xmlsec1-devel e xmlsec1-openssl-devel) e libcurl
# com OpenSSL (libcurl4-openssl-dev / libcurl-devel)
XML2_CONFIG ?= xml2-config
PKG_CONFIG  ?= pkg-config

ifeq ($(filter clean uninstall formatar verificar-formato,$(MAKECMDGOALS)),)
ifeq ($(shell command -v $(XML2_CONFIG) 2>/dev/null),)
$(error $(XML2_CONFIG) não encontrado. Instale a libxml2 de desenvolvimento (ex.: apt install libxml2-dev ou dnf install libxml2-devel))
endif
ifneq ($(shell $(PKG_CONFIG) --exists xmlsec1-openssl 2>/dev/null && echo ok),ok)
$(error xmlsec1-openssl não encontrado. Instale a xmlsec1 de desenvolvimento (ex.: apt install libxmlsec1-dev ou dnf install xmlsec1-devel xmlsec1-openssl-devel))
endif
ifneq ($(shell $(PKG_CONFIG) --exists libcurl 2>/dev/null && echo ok),ok)
$(error libcurl não encontrada. Instale a libcurl de desenvolvimento com OpenSSL (ex.: apt install libcurl4-openssl-dev ou dnf install libcurl-devel))
endif
endif


# Flags do compilador
# -MMD -MP gera arquivos .d para recompilar quando um header muda
CFLAGS := -Werror -Wall -Wextra -Wwrite-strings -std=c99 -g -fPIC -MMD -MP $(shell $(XML2_CONFIG) --cflags 2>/dev/null) $(shell $(PKG_CONFIG) --cflags xmlsec1-openssl libcurl 2>/dev/null)


# Flags para adicionar libs
LIBS := $(shell $(XML2_CONFIG) --libs 2>/dev/null) $(shell $(PKG_CONFIG) --libs xmlsec1-openssl libcurl 2>/dev/null)


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
SCHEMADIR  ?= $(PREFIX)/share/tooldoce/schemas

#Schemas oficiais instalados para a validação (nfe_validador_new)
SCHEMAS = $(addprefix tests/schemas/nfe/,nfe_v4.00.xsd leiauteNFe_v4.00.xsd tiposBasico_v4.00.xsd DFeTiposBasicos_v1.00.xsd xmldsig-core-schema_v1.01.xsd)
CFLAGS += -DNFE_DIR_SCHEMAS='"$(SCHEMADIR)"'


#Nome de todas os arquivos fontes com path e extensão (*.c)
C_SOURCE = $(wildcard $(SOURCE)/*.c)


#Objetos com path ./obj/ e extensão (*.o)
OBJ = $(addprefix $(LOBJ)/,$(notdir $(C_SOURCE:.c=.o)))


.PHONY: all libnfe install uninstall test exemplos clean formatar verificar-formato

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
	install -d $(DESTDIR)$(SCHEMADIR)
	install -m 644 $(SCHEMAS) $(DESTDIR)$(SCHEMADIR)/


uninstall:
	rm -fv $(DESTDIR)$(LIBDIR)/$(LIBNAME) $(DESTDIR)$(LIBDIR)/$(SONAME) $(DESTDIR)$(LIBDIR)/$(REALNAME)
	rm -rfv $(DESTDIR)$(INCLUDEDIR)/libnfe
	rm -rfv $(DESTDIR)$(SCHEMADIR)
	-rmdir $(DESTDIR)$(PREFIX)/share/tooldoce 2>/dev/null


#Testes: cada tests/test_*.c vira um executável em obj/, compilado junto com
#os fontes da biblioteca e com sanitizers (desative com SANITIZE=)
TESTES = $(addprefix $(LOBJ)/,$(basename $(notdir $(wildcard tests/test_*.c))))
# O AddressSanitizer de compiladores mais antigos (ex.: GCC 12.2 do Debian 12)
# entra em laço ("AddressSanitizer:DEADLYSIGNAL") em kernels com
# vm.mmap_rnd_bits acima de 28, comum na WSL2; sem PIE os testes funcionam
# (#254). Ao trocar SANITIZE, recompile os testes com make -B test.
MMAP_RND_BITS := $(shell cat /proc/sys/vm/mmap_rnd_bits 2>/dev/null)
SANITIZE_PIE := $(shell test "$(MMAP_RND_BITS)" -gt 28 2>/dev/null && echo "-fno-pie -no-pie")
SANITIZE ?= -fsanitize=address,undefined -fno-omit-frame-pointer $(SANITIZE_PIE)
CFLAGS_TESTE = $(filter-out -MMD -MP,$(CFLAGS)) $(SANITIZE)

test: $(TESTES)
	@for t in $(TESTES); do echo "== $$t"; $$t tests || exit 1; done

$(LOBJ)/test_%: tests/test_%.c $(wildcard tests/*.h) $(C_SOURCE) $(wildcard $(INCLUDE)/libnfe/*.h) | $(LOBJ)
	$(CC) $(CFLAGS_TESTE) -I$(INCLUDE) $< $(C_SOURCE) -o $@ $(LIBS)


#Exemplos: ligados à biblioteca compartilhada, como um programa externo
EXEMPLOS = $(addprefix $(LOBJ)/,$(basename $(notdir $(wildcard examples/*.c))))

exemplos: $(EXEMPLOS)

$(LOBJ)/%: examples/%.c $(LIB)/$(LIBNAME) | $(LOBJ)
	$(CC) $(filter-out -MMD -MP -fPIC,$(CFLAGS)) -I$(INCLUDE) $< -L$(LIB) -lnfe -Wl,-rpath,$(abspath $(LIB)) -o $@ $(LIBS)


#Formatação (.clang-format)
CLANG_FORMAT ?= clang-format
FONTES_C = $(wildcard $(SOURCE)/*.c $(INCLUDE)/libnfe/*.h tests/*.c tests/*.h examples/*.c)

formatar:
	$(CLANG_FORMAT) -i $(FONTES_C)

verificar-formato:
	$(CLANG_FORMAT) --dry-run --Werror $(FONTES_C)


clean:
	rm -fv $(LOBJ)/*.o $(LOBJ)/*.d $(LIB)/libnfe.so* $(TESTES) $(EXEMPLOS)


-include $(OBJ:.o=.d)
