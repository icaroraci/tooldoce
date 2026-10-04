# TODO

Estruturas do Exemplo (leiaute 1.00) a implementar, na ordem do schema oficial. Cada item leva ao diagrama da estrutura.

Marque `[x]` quando a estrutura tiver: criação/liberação, setters com validação, geração do XML e testes validando contra o XSD. A lista é gerada por `python3 "$(pkg-config --variable=ferramentas libnfe)/gerar_diagramas.py" --config tests/outro_documento/documento.json --todo`, que preserva os itens marcados.

## Estruturas do Exemplo

- [ ] [**exemplo**](diagramas/exemplo.svg)
  - [ ] [**infEx**](diagramas/exemplo/infEx.svg)
    - [ ] [**ide**](diagramas/exemplo/infEx/ide.svg)
    - [ ] [**veiculo**](diagramas/exemplo/infEx/veiculo.svg)
    - [ ] [**condutor**](diagramas/exemplo/infEx/condutor.svg) `1..10`
    - [ ] [**contratante**](diagramas/exemplo/infEx/contratante.svg) `0..1` _(opcional)_

## Além do leiaute

- [ ] Assinatura, transmissão e eventos
