# Calculadora de Rendimento do CDI

![Design](https://i.imgur.com/2Qr9Szd.png)

## Descrição
Este programa calcula o rendimento do CDI em um período especificado e fornece um relatório detalhado sobre os valores acumulados.

## Funcionalidades
- Permite declarar quantos dias de aplicação já se passaram para o cálculo do IOF.
- Define o valor inicial da aplicação.
- Permite especificar a taxa Selic do período na entrada "Taxa Anual".
- Permite definir a porcentagem do CDI em "CDI".
- Estima o rendimento total considerando aplicações mensais constantes.
- Permite escolher o dia do mês em que a aplicação mensal ocorrerá.
- Define o local de salvamento do relatório após pressionar o botão "Calcular".
- O botão "Calcular" executa os cálculos até o último dia do período e libera a opção de salvar o relatório.
- O botão "Relatório" permite salvar o arquivo .csv e perguntar se deseja abri-lo no programa padrão.
- A entrada "Salvar em" define onde o arquivo .csv do relatório será salvo.

## Configurações
- O programa inicia com valores padrões, mas permite configuração personalizada.
- No menu de configurações, é possível definir:
  - % do CDI padrão.
  - Taxa SELIC padrão.
  - Local de salvamento padrão.
  - Opção de gerar relatório automaticamente no local padrão.
  - Opção para definir o que fazer ao gerar relatório
- Ao salvar as configurações, o programa solicitará reinicialização.
- Ao lado dos botões de salvar e cancelar, tem um botão para resetar as configurações para o padrão

## Requisitos Mínimos
### Windows:
- 64 MB de RAM.
- Windows 10 ou superior.
- Processador 64 bits.
- 70 MB de espaço livre.
- Resolução minima de 1024x576

### Linux:
- 128 MB de RAM.
- Qualquer distribuição Linux moderna (atualmente testado somente em distros baseadas em Debian, mas o suporte a outras distros é provável).
- Biblioteca libqt6 instalada na versão 6.8.1.
- Processador 64 bits.
- 70 MB de espaço livre.
- Resolução minima de 1024x576

### Outros sitemas:

Não foi testado a compilação e o programa em outros sistemas, mas deve compilar e rodar sem muitas complicaçãoes.

## Tecnologias Utilizadas
- Linguagem: C++
- Framework: Qt6

## Como compilar

Os binários nas releases foram feitas usando o Qt Creator.

### Como compilar usando o Qt Creator
- Instale o Qt Creator no [site oficial](https://www.qt.io/).
- Faça a instalação, certificando-se que a opão de Qt 6.8.1 para C++ foi escolhida.
    - Foi feito um teste na versão 6.6 do Qt e a compilação foi bem sucedida e o programa rodou como esperado, mas os binários oficiais do repositório foram feitos na versão 6.8.1.
- Importe o projeto e configure o compilador.
- Compile assim mesmo ou modifique como desejar.


## Como acelarar o programa

O programa foi feito para ter um delay de 250ms para cada ação como forma de design. Essa configuração é definida no campo "delay" em .cdicfg/cfg.txt (na pasta do usuário que está rodando o programa). O valor é em milisegundos.

## Licença
Este projeto está licenciado sob a Licença GPLv3.

