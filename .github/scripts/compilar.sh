#!/bin/sh
# Compila cada fonte de src/libnfe separadamente e gera a biblioteca.
#
# KNOWN_BROKEN lista os fontes (nome do arquivo) que ainda não compilam;
# cada um deve ter uma issue aberta. O script falha se:
#   - um fonte fora da lista não compilar;
#   - um fonte da lista passar a compilar (remova-o da lista).
#
# Uso local: KNOWN_BROKEN="ide.c refNF.c" KNOWN_BROKEN_HEADERS="ide.h" \
#            sh .github/scripts/compilar.sh

set -u

conhecidos=" ${KNOWN_BROKEN:-} "
falhou=0
compilados=""
log=$(mktemp)

for fonte in src/libnfe/*.c; do
	arquivo=$(basename "$fonte")
	objeto="obj/${arquivo%.c}.o"

	if make "$objeto" >"$log" 2>&1; then
		compilados="$compilados $fonte"
		case "$conhecidos" in
		*" $arquivo "*)
			echo "::error file=$fonte::$arquivo agora compila; remova-o de KNOWN_BROKEN"
			falhou=1
			;;
		*)
			echo "ok: $arquivo"
			;;
		esac
	else
		case "$conhecidos" in
		*" $arquivo "*)
			echo "::warning file=$fonte::$arquivo ainda não compila (falha conhecida)"
			echo "::group::Erros de $arquivo"
			cat "$log"
			echo "::endgroup::"
			;;
		*)
			cat "$log"
			echo "::error file=$fonte::$arquivo não compila"
			falhou=1
			;;
		esac
	fi
done

# Cada header público deve compilar sozinho (KNOWN_BROKEN_HEADERS segue a
# mesma regra de KNOWN_BROKEN)
headers_conhecidos=" ${KNOWN_BROKEN_HEADERS:-} "
xml2_cflags=$(${XML2_CONFIG:-xml2-config} --cflags)

for header in include/libnfe/*.h; do
	arquivo=$(basename "$header")

	# shellcheck disable=SC2086
	if printf '#include <libnfe/%s>\n' "$arquivo" |
		${CC:-cc} -std=c99 -Wall -Werror -fsyntax-only -Iinclude $xml2_cflags -x c - >"$log" 2>&1; then
		case "$headers_conhecidos" in
		*" $arquivo "*)
			echo "::error file=$header::$arquivo agora compila sozinho; remova-o de KNOWN_BROKEN_HEADERS"
			falhou=1
			;;
		*)
			echo "ok: $arquivo (sozinho)"
			;;
		esac
	else
		case "$headers_conhecidos" in
		*" $arquivo "*)
			echo "::warning file=$header::$arquivo ainda não compila sozinho (falha conhecida)"
			;;
		*)
			cat "$log"
			echo "::error file=$header::$arquivo não compila sozinho"
			falhou=1
			;;
		esac
	fi
done

rm -f "$log"

if [ "$falhou" -ne 0 ]; then
	exit 1
fi

# Gera a biblioteca com o que compila; sem falhas conhecidas, build completo
if [ -z "${KNOWN_BROKEN:-}" ]; then
	make
else
	make C_SOURCE="$compilados"
fi
