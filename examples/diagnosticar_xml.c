/* Exibe diagnósticos automáticos de um XML contra o XSD informado.
 * Uso: diagnosticar_xml schema.xsd documento.xml
 * Não acrescenta assinatura nem executa as regras locais da NF-e. */
#include <stdio.h>
#include <stdlib.h>
#include <libnfe/erros.h>
#include <libnfe/validar.h>

/* Escapa quebras de linha, aspas e controles para a exibição em terminal.
 * Uma interface HTML deve usar textContent ou escapar seu próprio formato. */
static void mostra(const char *texto)
{
	const unsigned char *p = (const unsigned char *)texto;

	if (!texto) {
		fputs("indisponível", stdout);
		return;
	}
	putchar('"');
	for (; *p; p++) {
		switch (*p) {
		case '\n':
			fputs("\\n", stdout);
			break;
		case '\r':
			fputs("\\r", stdout);
			break;
		case '\t':
			fputs("\\t", stdout);
			break;
		case '\\':
			fputs("\\\\", stdout);
			break;
		case '"':
			fputs("\\\"", stdout);
			break;
		default:
			if (*p < 32 || *p == 127)
				printf("\\x%02x", *p);
			else
				putchar(*p);
		}
	}
	putchar('"');
}

static char *le(const char *caminho, size_t *tam)
{
	FILE *f = fopen(caminho, "rb");
	char *xml = NULL;
	long n;

	if (!f)
		return NULL;
	if (fseek(f, 0, SEEK_END) != 0 || (n = ftell(f)) < 0 ||
	    n > 0x7fffffff || fseek(f, 0, SEEK_SET) != 0)
		goto fim;
	xml = malloc((size_t)n + 1);
	if (!xml)
		goto fim;
	*tam = fread(xml, 1, (size_t)n, f);
	if (*tam != (size_t)n || ferror(f)) {
		free(xml);
		xml = NULL;
		goto fim;
	}
	xml[*tam] = '\0';
fim:
	fclose(f);
	return xml;
}

int main(int argc, char **argv)
{
	nfe_validador *v;
	nfe_erros *erros;
	char *xml;
	size_t tam = 0;
	int rc, i;

	if (argc != 3) {
		fprintf(stderr, "uso: %s schema.xsd documento.xml\n", argv[0]);
		return 2;
	}
	v = nfe_validador_xsd(argv[1]);
	erros = nfe_erros_new();
	xml = le(argv[2], &tam);
	if (!v || !erros || !xml) {
		fputs("Não foi possível carregar XSD/XML ou alocar memória.\n",
		      stderr);
		rc = 2;
		goto fim;
	}
	rc = nfe_validar_xsd(v, xml, tam, 0, erros);
	printf("Retorno local: %d · %d problema(s)\n", rc,
	       nfe_erros_qtd(erros));
	for (i = 0; i < nfe_erros_qtd(erros); i++) {
		fputs("\nTag: ", stdout);
		mostra(nfe_erros_campo(erros, i));
		printf(" · linha %d\nCaminho: ", nfe_erros_linha(erros, i));
		mostra(nfe_erros_caminho(erros, i));
		fputs("\nRecebido: ", stdout);
		mostra(nfe_erros_valor(erros, i));
		if (nfe_erros_restricao(erros, i)) {
			fputs("\nRestrição XSD: ", stdout);
			mostra(nfe_erros_restricao(erros, i));
			fputs("\nEsperado: ", stdout);
			mostra(nfe_erros_esperado(erros, i));
		}
		fputs("\nDiagnóstico original: ", stdout);
		mostra(nfe_erros_msg(erros, i));
		putchar('\n');
	}
	if (rc == 0)
		puts("XML válido contra o XSD; isso não indica autorização "
		     "fiscal.");
	rc = rc == 0 ? 0 : 1;
fim:
	free(xml);
	nfe_erros_free(erros);
	nfe_validador_free(v);
	return rc;
}
