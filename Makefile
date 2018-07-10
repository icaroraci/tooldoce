# Flags do comnpilador
CFLAGS = -Werror -Wall -std=c99 -g -fPIC `xml2-config --cflags`


# Flags para adicionar libs
LIBS = `xml2-config --libs`


#-I includes
INCLUDE = ./include


#Paths do código fonte
SOURCE = ./src/libnfe


#Objetos compilados para Library
LOBJ = ./OBJ


#Path da lib
LIB = ./lib


#Nome de todas os arquivos fontes com path e extensão (*.c) 
C_SOURCE=$(wildcard ./src/libnfe/*.c)


#Nome de todas os arquivos fontes com path e extensão (*.o) 
# Substitui a extenssao de (*.c) por (*.o)
OBJ=$(C_SOURCE:.c=.o)


#Nome de todas os arquivos fontes com extensão (*.o) e sem path
PURE =$(notdir $(OBJ))


#Adiciona o path ".OBJ/" aos arquivos fontes com extensão (*.o)
ALL  = $(addprefix $(LOBJ)/,$(PURE))


#Todos os objetos de PURE
all:$(PURE)


#Todos os objetos de OBJ/*.o
libnfe:$(ALL)
	gcc -shared $^ -o $(LIB)/libnfe.so.0.0


#Compila se não existir, ou recompila, se houve alteracao no objeto
#Nome da regra é um path do objeto
$(LOBJ)/%.o:$(SOURCE)/%.c
	gcc $(CFLAGS) -I$(INCLUDE) -c $^ -o $@
	@echo Compila só se não existir ou desatualizado



%.o:$(SOURCE)/%.c
	gcc $(CFLAGS) -I$(INCLUDE) -c $^ -o $(LOBJ)/$@
	@echo Compilação incondicional
	


var:
	@echo "$@: $$@ O nome do alvo."
	@echo "$*: $$* O nome do alvo sem a extensão."
	@echo "$<: $$< O primeiro dos pré-requisitos."
	@echo "$^: $$^ Os nomes dos arquivos de todos os pré-requisitos, separados por espaços, descarta duplicatas."
	@echo "$+: $$+ Similar ao $^, mas inclui duplicatas."
	@echo "$?: $$? Os nomes de todos os pré-requisitos que são mais novos do que o alvo, separados por espaços."

lobj:
	@echo $(LOBJ)
	
csource:
	@echo $(C_SOURCE)

obj:
	@echo $(OBJ)

pure:
	@echo $(PURE)

aul:
	@echo $(ALL)

clean:
	@rm $(LOBJ)/*.o -fv


