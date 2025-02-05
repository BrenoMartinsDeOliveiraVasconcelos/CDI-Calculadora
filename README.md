# Calculadora de CDI

Projeto experimental para simulação da renda pela taxa Selic.

![Design](https://i.imgur.com/2Qr9Szd.png)

## Como usar

A **Calculadora de CDI** é uma ferramenta desenvolvida para calcular o rendimento do Certificado de Depósito Interbancário (CDI) em um período específico, considerando variáveis como a Taxa Selic, porcentagem do CDI, aplicações mensais e outros fatores. Abaixo estão descritos os principais recursos e funcionalidades do programa:

### Funcionalidades Principais:
1. **Cálculo do Rendimento do CDI:**
   - O programa calcula o rendimento do CDI com base em um período especificado pelo usuário.
   - É possível declarar quantos dias de aplicação já se passaram para o cálculo do IOF (Imposto sobre Operações Financeiras).

2. **Entrada de Dados:**
   - **Valor Inicial:** Insira o valor inicial da aplicação no campo "Valor".
   - **Taxa Anual:** Determine a taxa Selic do período no campo "Taxa Anual".
   - **Porcentagem do CDI:** Especifique a porcentagem do CDI que será aplicada no cálculo.
   - **Aplicações Mensais:** Estime o rendimento total considerando aplicações mensais constantes. É possível escolher o dia do mês em que a aplicação mensal ocorrerá.

3. **Relatório Personalizável:**
   - Após clicar no botão "Calcular", o programa processará os dados até o último dia do período selecionado.
   - O botão "Relatório" será liberado, permitindo ao usuário gerar um arquivo `.csv` com os resultados.
   - Ao gerar o relatório, o programa perguntará se deseja abrir o arquivo `.csv` no programa padrão configurado no sistema.
   - A entrada "Salvar em" define onde o arquivo `.csv` será salvo.

4. **Configurações Iniciais:**
   - Ao iniciar o programa, ele carrega valores padrões pré-definidos, mas o usuário pode personalizar essas configurações.
   - No menu de **Configurações**, é possível definir:
     - A porcentagem padrão do CDI.
     - A taxa Selic padrão.
     - O local de salvamento padrão para os relatórios.
     - Uma opção para gerar relatórios automaticamente e salvar no local padrão pré-definido.

5. **Reinicialização após Configurações:**
   - Após salvar as configurações, o programa solicitará uma reinicialização para aplicar as alterações.

---

## Tecnologias Usadas

- **Linguagem de Programação:** C++
- **Framework:** Qt6 (versão 6.8.1 ou superior)
- **Interface Gráfica:** Desenvolvida com Qt6, proporcionando uma experiência moderna e responsiva.

---

## Requisitos Mínimos

### Para Windows:
- **RAM:** 64 MB
- **Sistema Operacional:** Windows 10 ou superior
- **Processador:** Qualquer processador 64 bits
- **Espaço em Disco:** 1 MB de espaço livre

### Para Linux:
- **RAM:** 100 MB
- **Sistema Operacional:** Qualquer distribuição suportada atualmente
- **Bibliotecas Necessárias:** `libqt6` (versão 6.8.1 ou superior)
- **Processador:** Qualquer processador 64 bits
- **Espaço em Disco:** 1 MB de espaço livre

---

## Como Rodar

1. **Instalação:**
   - Certifique-se de que seu sistema atende aos requisitos mínimos.
   - Para sistemas Linux, instale a biblioteca `libqt6` na versão 6.8.1 ou superior.

2. **Execução:**
   - Execute o programa a partir do executável fornecido.
   - Ao iniciar, o programa carregará os valores padrões. Caso deseje personalizar as configurações, acesse o menu **Configurações**.

3. **Uso Básico:**
   - Insira os valores iniciais, como o valor da aplicação, taxa Selic, porcentagem do CDI e outras variáveis necessárias.
   - Configure o período desejado e clique em **Calcular**.
   - Após o cálculo, gere o relatório clicando em **Relatório** e escolha onde salvar o arquivo `.csv`.

4. **Gerar Relatórios Automaticamente:**
   - Na aba de **Configurações**, habilite a opção para gerar relatórios automaticamente. O programa salvará o arquivo `.csv` no local padrão pré-definido.

---

## Observações Finais

Este projeto foi desenvolvido com foco em simplicidade e eficiência, permitindo ao usuário realizar simulações financeiras com facilidade. Caso encontre algum problema ou tenha sugestões para melhorias, sinta-se à vontade para abrir uma issue no repositório oficial.

---