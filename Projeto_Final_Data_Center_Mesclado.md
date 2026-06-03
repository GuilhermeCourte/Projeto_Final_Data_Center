# Monitoramento Inteligente de Data Center com IoT e Cloud Computing

**Disciplina:** Cloud Computing e Internet das Coisas (IoT)
**Tipo:** Trabalho Final de Curso

---

## 1. Contexto do Problema

A empresa **CloudTech Solutions** instalou um rack de servidores em uma unidade operacional remota que não dispõe de monitoramento presencial ou suporte técnico 24 horas por dia.

Recentemente, a organização enfrentou um incidente crítico: o sistema de climatização (ar-condicionado) falhou e a temperatura interna do ambiente elevou-se gradativamente sem emitir alertas. Como consequência, o servidor principal sofreu um desligamento automático por superaquecimento (*thermal shutdown*), resultando em **4 horas de downtime** (tempo de inatividade) e severos prejuízos financeiros para o negócio.

O objetivo deste projeto é evitar novos incidentes por meio de monitoramento automatizado com IoT, Edge Computing e controle remoto em tempo real.

---

## 2. Arquitetura da Solução

Para mitigar o problema e evitar reincidências, será desenvolvida uma solução de IoT baseada nas seguintes camadas:

| Camada | Componente | Função |
|--------|-----------|--------|
| **Edge Device** | ESP32 + Sensor DHT22 | Leitura de temperatura e umidade |
| **Atuador** | Relé + LED (simula exaustor/ventilador) | Ventilação automática do ambiente |
| **Comunicação** | Protocolo MQTT com autenticação | Troca de mensagens entre dispositivo e nuvem |
| **Gestão** | Dashboard no Node-RED | Visualização de dados e controle remoto |

> **Nota:** O ESP32 pode ser implementado via simulador **Wokwi** ou com hardware físico correspondente.

---

## 3. Requisitos Funcionais da Solução

### 3.1. Camada de Telemetria (Dispositivo → Nuvem)

- O ESP32 deve realizar a leitura de temperatura e umidade a cada **5 segundos**.
- Os dados devem ser encapsulados em um único objeto **JSON** e publicados no tópico MQTT:
  ```
  cloudtech/datacenter/monitor
  ```
- **Regra de Edge Computing:** Se a temperatura monitorada ultrapassar **28 °C**, o ESP32 deve acionar o relé (LED) de forma automática e imediata, operando de maneira autônoma e independente de qualquer comando vindo da nuvem.

### 3.2. Camada de Controle (Nuvem → Dispositivo)

- O Dashboard deve conter um botão de **"Ativação Manual do Exaustor"**.
- O ESP32 deve manter uma assinatura ativa no tópico MQTT:
  ```
  cloudtech/datacenter/cmd
  ```
- Comandos esperados: `ON` / `OFF`

### 3.3. Camada de Visualização (Dashboard)

- **Indicadores em tempo real:**
  - Temperatura atual
  - Umidade atual
  - Status do exaustor

- **Indicador Analógico (Gauge):** Mostrador visual da temperatura em tempo real com mapeamento cromático de criticidade:
  - 🟢 **Verde:** Temperatura < 24 °C — Operação Ideal
  - 🟡 **Amarelo:** Temperatura entre 24 °C e 27 °C — Alerta
  - 🔴 **Vermelho:** Temperatura > 28 °C — Crítico / Acionamento Automático

- **Gráfico Histórico (Chart):** Gráfico de linha exibindo o histórico de temperatura e umidade das **últimas 24 horas**, com atualização em tempo real.

- **Log de Eventos:** Exibição de mensagens textuais informativas sempre que o exaustor for ativado, sejam por gatilho automático local ou por comando manual. Exemplos:
  - *"Exaustor ligado automaticamente"*
  - *"Exaustor ligado manualmente"*

- **Controle Manual:** Botão para ligar/desligar o exaustor remotamente.

---

## 4. Diretrizes de UI/UX (Design de Interface)

A interface não deve se limitar às cores e disposições padrão da ferramenta Node-RED. É fundamental aplicar boas práticas de design:

- **Identidade coerente:** O design deve remeter a um ambiente de infraestrutura de TI/Data Center — recomenda-se uso de temas escuros, fontes limpas e contrastes adequados.
- **Simbologia:** Incorporação de ícones representativos (ex.: *Font Awesome*) para ilustrar sensores, fluxo de ar e status dos atuadores.
- **Modularidade e organização:** Disposição lógica dos blocos de dados — indicadores de telemetria devem ser agrupados de forma distinta e visualmente separada dos botões de controle.

---

## 5. Requisitos de Entrega

### 5.1. Requisitos Obrigatórios

- **Hardware:** Uso obrigatório do simulador online **Wokwi** ou de componentes físicos correspondentes.
- **Segurança:** A conexão com o Broker MQTT exige, obrigatoriamente, validação por meio de **usuário (*username*)** e **senha (*password*)**.
- **Qualidade de código:** O código-fonte do microcontrolador deve estar devidamente estruturado, limpo e amplamente comentado, explicando a função de cada bloco lógico e biblioteca utilizada.

### 5.2. Itens de Entrega

O projeto deverá conter:

- Código-fonte comentado do ESP32
- Dashboard funcional no Node-RED
- Comunicação MQTT configurada e operacional
- Automação local (Edge Computing) implementada
- Documentação organizada

### 5.3. Estrutura de Diretórios Recomendada

```
/projeto
  /esp32          → Código-fonte do microcontrolador
  /dashboard      → Fluxo exportado do Node-RED
  /documentacao   → Diagramas, relatório e demais documentos
```

---

## 6. Critérios de Avaliação

Os projetos serão submetidos aos seguintes testes práticos para aferição da nota:

| # | Critério | Descrição do Teste |
|---|----------|--------------------|
| 1 | **Conectividade** | O ESP32 consegue estabelecer conexão estável com a rede Wi-Fi e com o Broker MQTT em menos de **10 segundos**? |
| 2 | **Integridade** | O pacote JSON transmitido é estruturalmente válido e suas chaves correspondem exatamente ao que o fluxo do Node-RED espera? |
| 3 | **Resiliência** | Ao acionar o controle manual no Dashboard, o LED no Wokwi/dispositivo responde de forma **instantânea**? |
| 4 | **Autonomia (Edge Continuity)** | Na hipótese de queda de conexão com o Node-RED, o ESP32 mantém a capacidade de acionar o LED localmente ao detectar temperatura > 28 °C? |

---

## 7. Dinâmica de Apresentação (Pitch)

Os grupos deverão realizar uma defesa técnica da solução por meio de uma apresentação concisa (*pitch*), abordando obrigatoriamente os seguintes tópicos:

1. **Problema:** Contextualização do desafio operacional e os impactos gerados pela ausência de monitoramento.
2. **Arquitetura:** Explicação detalhada do fluxo de dados ponta a ponta (Edge → Nuvem), amparada por diagramas arquiteturais.
3. **Stack Tecnológica:** Justificativa técnica para a escolha de cada ferramenta, protocolo e biblioteca aplicados.
4. **Demonstração Prática:** Validação do ecossistema em tempo real, demonstrando a alteração de estados no simulador e a respectiva plotagem nos gráficos do Dashboard.
5. **Segurança e Escalabilidade:** Explanação do modelo de segurança implementado e proposições de evolução da arquitetura para cenários industriais reais.

---

## 8. Stack Tecnológica

| Tecnologia | Finalidade |
|-----------|-----------|
| **ESP32** | Microcontrolador (Edge Device) |
| **DHT22** | Sensor de temperatura e umidade |
| **MQTT** | Protocolo de comunicação |
| **Node-RED** | Dashboard e automação em nuvem |
| **Wokwi** | Simulação do circuito |
| **Arduino IDE** | Desenvolvimento e upload do firmware |
| **HiveMQ / Mosquitto** | Broker MQTT |

---

*Documento consolidado a partir das versões GPT, Gemini e PDF (slides). Revisado e mesclado para uso oficial na disciplina.*
