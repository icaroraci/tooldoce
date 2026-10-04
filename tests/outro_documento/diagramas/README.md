# Diagramas das estruturas do Exemplo (leiaute 1.00)

Gerados automaticamente a partir do schema oficial (`tests/schemas/exemplo/leiauteExemplo_v1.00.xsd`) por `tools/gerar_diagramas.py`. **Não edite os SVGs**: atualize o schema e rode `python3 "$(pkg-config --variable=ferramentas libnfe)/gerar_diagramas.py" --config tests/outro_documento/documento.json`.

Em cada diagrama: caixa tracejada = opcional; `0..1`, `1..∞` = ocorrências; **seq.** = os filhos aparecem nessa ordem; **escolha** = apenas um dos filhos; caixas amarelas (⊞) são estruturas com diagrama próprio, listadas abaixo.

- [exemplo](exemplo.svg)
  - [infEx](exemplo/infEx.svg) — Informações do documento
    - [ide](exemplo/infEx/ide.svg) — Identificação
    - [veiculo](exemplo/infEx/veiculo.svg) — Veículo de tração
      - [prop](exemplo/infEx/veiculo/prop.svg) `0..1` — Proprietário do veículo, quando não é o emitente
    - [condutor](exemplo/infEx/condutor.svg) `1..10` — Condutores
    - [contratante](exemplo/infEx/contratante.svg) `0..1` — Contratante do serviço
