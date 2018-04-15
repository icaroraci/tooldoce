CFLAGS = -Werror -Wall -std=c99 -g `xml2-config --cflags` 
LIBS = `xml2-config --libs`
INCLUDE = ./include
SOURCE = ./src/libnfe
LOBJ = ./OBJ

C_SOURCE=$(wildcard ./src/libnfe/*.c)
OBJ=$(C_SOURCE:.c=.o)
PURE =$(notdir $(OBJ))

libnfe:$(LOBJ)/$(PURE)
	@echo "$^: Os pré-requisitos."
	@echo "$(PURE) "	



%.o:$(SOURCE)/%.c
	gcc $(CFLAGS) -c -I$(INCLUDE) $< -o $(LOBJ)/$@

$(LOBJ)/%.o:$(SOURCE)/%.c $(SOURCE)/%.h
	gcc  $(CFLAGS) -c -I$(INCLUDE) $< -o $@
		@echo "$@: O nome do alvo."
		@echo "$*: O nome do alvo sem a extensão."
		@echo "$<: O primeiro dos pré-requisitos."
		@echo "$^: Os nomes dos arquivos de todos os pré-requisitos, separados por espaços, descarta duplicatas."
		@echo "$+: Similar ao $^, mas inclui duplicatas."
		@echo "$?: Os nomes de todos os pré-requisitos que são mais novos do que o alvo, separados por espaços."
	
clean:
	@rm $(LOBJ)/*.o -fv
