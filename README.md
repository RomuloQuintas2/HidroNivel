# HidroNível

Sistema embarcado de monitoramento de nível de reservatório de água, desenvolvido com Arduino para o projeto da disciplina de Sistemas Embarcados (IFPE Campus Garanhuns).

## Problema

Em locais sem rede de abastecimento monitorada — como a comunidade isolada onde o projeto se aplica, sem energia elétrica convencional nem saneamento — o nível de água dos reservatórios só pode ser verificado visualmente, subindo na caixa d'água. Isso causa desperdício, risco de transbordamento e situações de falta de água sem aviso prévio.

## Funcionamento

Um sensor ultrassônico **HC-SR04** é fixado na tampa do reservatório e mede a distância até a superfície da água. O Arduino converte essa distância em um **percentual de nível** (0% a 100%) e:

- exibe o percentual em tempo real em um **display LCD 16x2 (I2C)**;
- acende um **LED verde**, **amarelo** ou **vermelho** conforme a faixa de nível;
- aciona um **buzzer** quando o nível está crítico (≤ 20%).

| Nível | Indicação |
|---|---|
| > 70% | LED verde — reservatório OK |
| 21% – 70% | LED amarelo — nível médio |
| ≤ 20% | LED vermelho + buzzer — alerta crítico |

## Componentes

| Componente | Função |
|---|---|
| Arduino Uno | Controlador central |
| Sensor ultrassônico HC-SR04 | Mede a distância até a água |
| Display LCD 16x2 (módulo I2C) | Exibe o percentual de nível |
| LED verde / amarelo / vermelho | Indicam a faixa de nível |
| Buzzer ativo | Alerta sonoro em nível crítico |
| Regulador 12V→5V + resistores | Alimentação a partir da bateria/placa solar já existente |

## Ligações (pinagem)

| Componente | Pino no Arduino |
|---|---|
| HC-SR04 — TRIG | D9 |
| HC-SR04 — ECHO | D10 |
| LCD I2C — SDA | A4 |
| LCD I2C — SCL | A5 |
| LED verde | D2 |
| LED amarelo | D3 |
| LED vermelho | D4 |
| Buzzer | D8 |

## Código

O código principal está em [`HidroNivel.ino`](./HidroNivel.ino), comentado identificando:

- **A.** configuração dos pinos
- **B.** inicialização dos sensores/periféricos
- **C.** variáveis utilizadas (calibração, limites de faixa, estado)
- **D.** comandos responsáveis pela leitura do sensor ultrassônico
- **E.** estruturas condicionais de decisão por faixa de nível
- **F.** acionamento dos atuadores (LEDs e buzzer)

## Simulação

O projeto foi simulado no [Wokwi](https://wokwi.com) para validação do circuito e do comportamento do sistema antes da montagem física.

## Status do projeto

Em desenvolvimento — etapa de programação e leitura dos sensores (R5). Próximas etapas: aquisição dos componentes físicos, montagem, integração e testes do protótipo real.

## Autor

Rômulo Fernando Ferreira Quintas — 5º Período ADS, IFPE Campus Garanhuns
