#!/bin/sh
# Compila cada fonte de src/libnfe separadamente e gera a biblioteca.
#
# KNOWN_BROKEN lista os fontes (nome do arquivo) que ainda não compilam;
# cada um deve ter uma issue aberta. O script falha se:
#   - um fonte fora da lista não compilar;
#   - um fonte da lista passar a compilar (remova-o da lista).
#
# Uso local: KNOWN_BROKEN="ide.c refNF.c" .github/scripts/compilar.sh

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
