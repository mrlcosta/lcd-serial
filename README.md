# Projeto de Comunicação Serial e Controle de Hardware com RP2040

## Enunciado

Este projeto tem como objetivo consolidar os conceitos de comunicação serial, utilizando a placa de desenvolvimento BitDogLab com o microcontrolador RP2040. Durante o desenvolvimento, serão exploradas funcionalidades de controle de LEDs, display, botões, interrupções e debouncing.

## Objetivos

- **Compreensão da Comunicação Serial**: Explorar o uso de UART e I2C na comunicação entre o microcontrolador e os periféricos.
- **Controle de LEDs**: Manipular LEDs comuns e LEDs endereçáveis WS2812, demonstrando o domínio do controle de diferentes tipos de LEDs.
- **Botões e Interrupções**: Trabalhar com botões de acionamento, implementando interrupções e debouncing via software.
- **Integração de Hardware e Software**: Desenvolver um projeto funcional que combine componentes físicos com código, utilizando conceitos adquiridos.

## Descrição do Projeto

Este projeto é desenvolvido para a placa de desenvolvimento BitDogLab e utiliza diversos periféricos conectados ao microcontrolador RP2040:

- **Matriz de LEDs 5x5 (WS2812)** conectada à GPIO 7.
- **LED RGB** com pinos conectados às GPIOs 11, 12 e 13.
- **Botão A** conectado à GPIO 5.
- **Botão B** conectado à GPIO 6.
- **Display SSD1306 (128x64)** conectado via I2C (GPIO 14 e GPIO 15).

## Funcionalidades do Projeto

### 1. Modificação da Biblioteca `font.h`
- Adicionar caracteres minúsculos à biblioteca `font.h`, utilizando criatividade para criar novos caracteres.

### 2. Entrada de Caracteres via PC
- Utilizar o **Serial Monitor** do VS Code para receber caracteres digitados.
- Cada caractere digitado será exibido no **display SSD1306**.
- Para números de 0 a 9, um símbolo correspondente será exibido na **matriz de LEDs WS2812**.

### 3. Interação com o Botão A
- Pressionar o **Botão A** alterna o estado do **LED RGB Verde** (ligado/desligado).
- A operação será registrada:
  - Uma mensagem sobre o estado do LED será exibida no **display SSD1306**.
  - Um texto informativo será enviado ao **Serial Monitor**.

### 4. Interação com o Botão B
- Pressionar o **Botão B** alterna o estado do **LED RGB Azul** (ligado/desligado).
- A operação será registrada:
  - Uma mensagem sobre o estado do LED será exibida no **display SSD1306**.
  - Um texto informativo será enviado ao **Serial Monitor**.

## Requisitos do Projeto

### 1. Uso de Interrupções
- As funcionalidades dos botões devem ser implementadas utilizando **interrupções (IRQ)**.

### 2. Debouncing de Botões
- O tratamento de **debouncing** dos botões deve ser feito via software, evitando múltiplos acionamentos por um único clique.

### 3. Controle de LEDs
- O projeto deve demonstrar o controle de **LEDs comuns** e **LEDs WS2812**.

### 4. Utilização do Display 128x64
- O **display SSD1306** deve ser utilizado para mostrar caracteres em **maiúsculas** e **minúsculas**.
- O protocolo **I2C** deve ser utilizado para a comunicação com o display.

### 5. Envio de Informações via UART
- O código deve permitir o envio de informações ao **Serial Monitor** via comunicação **UART**.

### 6. Organização do Código
- O código deve ser bem estruturado e comentado, visando a clareza e a fácil compreensão.

---

## video demonstrativo


[![Assista ao vídeo demonstrativo](https://i.ytimg.com/vi/m4CvZlHNlw0/hq720_2.jpg?sqp=-oaymwEoCJUDENAFSFryq4qpAxoIARUAAIhC0AEB2AEB4gEKCBgQAhgGOAFAAQ==&rs=AOn4CLDEAiMRf0h5bnH_93G-STIvsmkGAQ)](https://youtu.be/uzgP26FSORw?si=cq8t9pHX9QuGYKlS)


## Conclusão

O projeto tem como propósito integrar diversos conceitos de programação embarcada com o uso de periféricos e comunicação serial. A implementação bem-sucedida permite consolidar o entendimento sobre o controle de hardware e a comunicação entre dispositivos usando o microcontrolador RP2040.
