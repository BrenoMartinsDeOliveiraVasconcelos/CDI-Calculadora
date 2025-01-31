/*   Calculadora de rendimento do CDI.
*    Copyright (C) 2025  Breno Martins
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "calculadora.h"
#include "optionmenu.h"
#include "ui_calculadora.h"

// Headers pessoais
#include <temp_management.h>
#include <qcsv.h>
#include <qtdecimalconversion.h>
#include <qstringutils.h>
#include <calcfunctions.h>
#include <configmanager.h>
#include <applicationclass.h>

// Bibliotecas C++
#include <iostream>
#include <vector>
#include <map>

// Bibliotecas qt16777215
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QSysInfo>
#include <QString>
#include <QIODevice>
#include <QTextStream>
#include <QFileDialog>
#include <QErrorMessage>
#include <QMessageBox>

using namespace std;

class nomesTemporario{
public:
    QString taxas = "taxas.txt";
    QString pastaRelatorio = "CDI_Calc";
};

Calculadora::Calculadora(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculadora)
    {
        ui->setupUi(this);

        configuration config;

        if (!config.generateConfigFolder()){
            cout << "Não foi possível gerar o diretório de configuração.\n";
            exit(1);
        }

        map<QString, QString> mapaConfig = config.getConfig();

        // Inicializar valores
        ui->taxaInput->setText(mapaConfig["selic"]);
        ui->cdiInput->setText(mapaConfig["cdi"]);
        ui->aplicacaoMes->setText("0,00");
        ui->diaInput->setText("1");

        // Configurar data
        QDate dataInicial = QDate::currentDate();
        QDate inicioAno;
        QDate fimAno;

        inicioAno.setDate(dataInicial.year(), 1, 1);
        fimAno.setDate(dataInicial.year(), 12, 31);

        ui->dataInicial->setMinimumDate(inicioAno);
        ui->dataInicial->setMaximumDate(fimAno.addDays(-1));

        ui->dataLimite->setMinimumDate(inicioAno.addDays(1));
        ui->dataLimite->setMaximumDate(fimAno);

        ui->dataInicial->setDate(QDate::currentDate());
        ui->dataLimite->setDate(fimAno);

        // Inicializar a tabela
        ui->relatorio->setColumnCount(1024);
        ui->relatorio->setRowCount(1024);

        ui->relatorio->horizontalHeader()->setMinimumSectionSize(200);

        QMessageBox::warning(this, "Aviso", "Esse programa realiza uma estimativa considerando que as taxas de juros definidas continuem as mesmas durante o ano e ignora feriados durante o ano, o que pode causar valores diferentes da realidade. Também, é importante considerar o imposto de renda e as taxas de inflação para estimar o rendimento real já que isso não é calculado pelo programa. Os valores finais não devem ser levados como regra e sim como base.", QMessageBox::Ok);
    }

Calculadora::~Calculadora()
{
    delete ui;
}

void Calculadora::on_caciar_clicked()
{
    if (ui->dataInicial->date().daysTo(ui->dataLimite->date()) < 0){
        ui->erroLabel->setText("A data inicial não pode ser depois da data limite.");
        return;
    }

    ui->erroLabel->setText("");
    ui->avisoLabel->setText("");

    configuration config;
    map<QString, QString> mapaConfig = config.getConfig();

    tempInfo temp;
    nomesTemporario nomes;
    runtimeConsts runtime;

    auto formatarValor = [](double valor) {
        return QString::number(valor, 'f', 2);
    };

    auto formatarData = [](QString data){
        QString dataFormatada = "("+data+")";
        return dataFormatada;
    };

    // Diretório dos arquivos
    if (!tempFolderExists()){
        if (!createTempFolder()){
            ui->erroLabel->setText("Erro ao criar pasta temporária.");
            return;
        };
    };

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);

    if (!arquivoTaxas.open(QIODevice::WriteOnly | QIODevice::Text)){
        ui->erroLabel->setText("Erro ao ler arquivo .txt de taxas.");
        return;
    };

    QTextStream taxasTxt(&arquivoTaxas);

    // Formato de data
    QString formatoData = "dd/MM/yyyy";

    // Tempo atual
    QDate dataInicial = ui->dataInicial->date();
    QDate diaLimite = ui->dataLimite->date();

    // Expressão regular para validar números positivos ou negativos (inteiros ou decimais)
    const QString numPadrao = "^\\d*(\\.\\d+)?$";
    const QString numNegPadrao = "^-\\d*(\\.\\d+)?$";
    QRegularExpression re(numPadrao);
    QRegularExpression minusRe(numNegPadrao);

    // Vetores para armazenar os inputs e suas conversões
    QString inputs[] = {
        "0" + ui->taxaInput->text().replace(",", "."),
        "0" + ui->cdiInput->text().replace(",", "."),
        "0" + ui->valorInput->text().replace(",", "."),
        "0" + ui->aplicacaoMes->text().replace(",", "."),
        "0" + ui->diaInput->text().replace(",", ".")
    };
    double valores[5] = {0.0f};
    unsigned long int valorMaximoDinheiro = runtime.maxMoneyValue();
    unsigned int maximoDias = runtime.maxDays();
    QString muitoGrande = runtime.tooBig();

    // Procurar por numeros negativos em inputs

    int index = 0;
    for (auto v:inputs){
        if (v.contains("-")){
            inputs[index] = "-" + v.replace("-", "");
        };

        index++;
    }

    // Validar e converter os inputs
    for (int i = 0; i < 5; ++i) {
        if (re.match(inputs[i]).hasMatch()){
            valores[i] = inputs[i].toDouble();

            // Checar se o valor é valido
            if (valores[i] > valorMaximoDinheiro){
                ui->erroLabel->setText("Os valores não podem ser maior que "+QString::number(valorMaximoDinheiro)+".");
                return;
            };

        } else if (minusRe.match(inputs[i]).hasMatch()){
            ui->erroLabel->setText("Os campos devem ser números positivos.");
            return;
        }else {
            ui->erroLabel->setText("Os campos devem ser números.");
            return;
        }
    };

    //Verificação especial para o input de dia que tem que ser inteiro necessáriamente
    if (inputs[4].contains(".")){
        ui->erroLabel->setText("O dia precisa ser um numero inteiro.");
        return;
    };

    // Verifica se o dia inputado POSSIVELMENTE existe.
    if (valores[4] > maximoDias){
        ui->erroLabel->setText("O dia de aplicação não pode ser maior que "+QString::number(maximoDias)+".");
        return;
    }

    // Calcular taxas e estimativas
    double cdi = valores[1] / 100;
    double seliac = (valores[0] * cdi) / 100;
    double seliacMes = seliac / 12;

    int numDiasMes = 0;

    double valorAplicado = valores[2];
    double seliacDia = ((seliacMes*100) / numDiasMes) / 100;

    // Calcular estimativas finais
    double estimativaAno = valorAplicado * pow((1 + seliacMes), (13 - dataInicial.month()));
    int anoAtual = dataInicial.year();

    int diasAno = dataInicial.daysInYear();
    vector<vector<double>> taxas = calcYearIndex(seliacMes, diasAno, anoAtual);

    vector<double> taxaDiariaPorMes = taxas[0];
    vector<double> taxaMensal = taxas[1];

    double valorAtual = valorAplicado;

    int mesAtual = dataInicial.month();
    int fatorMultiplicador = 0;
    double diaAplicacao = valores[4];
    double deFactoDiaAplicacao = diaAplicacao;

    // IOF
    vector<double> iof = runtime.iof();

    unsigned long int diasPassados = 0;

    cout << "\n" << valores[4] << "\n";

    for (int dia = dataInicial.dayOfYear()+1; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);

        // Configuração do IOF
        double iofAtual = 0;

        if (diasPassados+1 <= iof.size()){
            iofAtual = iof[diasPassados];
        }

        cout << "\n" << diasPassados << " " << iofAtual;

        diasPassados++;

        // Até o ultimo dia de calculo
        if (data.daysTo(diaLimite) < 0){
            continue;
        };

        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            if (data.day() == deFactoDiaAplicacao){
                deFactoDiaAplicacao++;
            }
            continue;
        }

        // Aplicar o dinheiro a cada inicio de mês
        if (mesAtual <= data.month()){
            if (data.day() == deFactoDiaAplicacao || (data.day() == data.daysInMonth() && data.day() < diaAplicacao)){
                mesAtual++;

                fatorMultiplicador++;

                valorAtual += valores[3];

                double valorBruto = valorAplicado+(valores[3]*fatorMultiplicador);

                // Manter a GUI agradável aos olhos
                if (valorBruto <= valorMaximoDinheiro){
                    ui->finalAnoBurto->setText("R$ " + formatarValor(valorBruto).replace(".", ","));
                }else{
                    ui->finalAnoBurto->setText(muitoGrande);
                };

                deFactoDiaAplicacao = diaAplicacao;
            }
        }

        // Adicionar valor convertido
        int indexMes = data.month() - 1;

        cout << "INDEX MES: " << indexMes << endl;

        // Aplica taxa diária com efeito acumulado
        double taxaDiaria = taxaDiariaPorMes[indexMes];
        double valorAnterior = valorAtual;
        double valorAtualBruto = valorAtual * (1 + taxaDiaria);

        double jurosIOFAjustado = (valorAtualBruto - valorAnterior) - ((valorAtualBruto - valorAnterior)*iofAtual);

        valorAtual += jurosIOFAjustado;
        //valorAtual *= (1 + taxaDiaria);
    };

    estimativaAno = valorAtual;

    if (estimativaAno <= valorMaximoDinheiro){
        ui->finalAno->setText("R$ " + formatarValor(estimativaAno).replace(".", ","));
    }else{
        ui->finalAno->setText(muitoGrande);
    }

    // Exibir datas

    ui->dataMes_2->setText(formatarData(diaLimite.toString(formatoData)));

    // Exibir taxa total
    double selicComJuros = (valorAtual / valorAplicado) - 1.00;

    ui->taxaAnual->setText("Seliac: " + formatarValor(seliac * 100) + "%"+"\nAumento com juros: " + formatarValor(selicComJuros * 100)+"%");

    // Escrever as taxas no arquivo temporário
    taxasTxt << seliac << "\n" << seliacMes << "\n" << seliacDia << "\n" << valores[2] << "\n" << cdi << "\n" << valores[3] << "\n" << valores[4];

    ui->estimarValores->setEnabled(true);
    ui->salvar->setEnabled(true);
    ui->salvarInput->setEnabled(true);
    ui->selecionarDiretorio->setEnabled(true);

    if (mapaConfig["path"] == "*"){
        ui->salvarInput->setText(QDir::homePath());
    }else{
        ui->salvarInput->setText(mapaConfig["path"]);
    };

    // Checa por vígula e troca por ponto
    ui->taxaInput->setText(ui->taxaInput->text().replace(".", ","));
    ui->cdiInput->setText(ui->cdiInput->text().replace(".", ","));
    ui->valorInput->setText(ui->valorInput->text().replace(".", ","));
    ui->aplicacaoMes->setText(ui->aplicacaoMes->text().replace(".", ","));

    cout << ui->valorInput->text().toStdString();
    ui->avisoLabel->setText("Clique em relatório para gerar a estimativa completa até o fim do ano.");

    arquivoTaxas.close();

    if (mapaConfig["autorel"] == "1"){
        ui->estimarValores->click();
    }
}

void Calculadora::on_estimarValores_clicked()
{
    ui->avisoLabel->setText("");
    ui->erroLabel->setText("");

    tempInfo temp;
    nomesTemporario nomes;
    runtimeConsts runtime;


    double valores[7] = {0, 0, 0, 0, 0, 0, 0}; // Indexes: {anual, mensal, dia, valor, aplicacaomensal, diaaplicacao}

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);
    if (!arquivoTaxas.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->erroLabel->setText("Falha ao ler o arquivo temporário de taxas.");
        return;
    };

    QTextStream arquivo(&arquivoTaxas);

    int index = 0;
    while (!arquivo.atEnd()) {
        QString line = arquivo.readLine();
        valores[index] = line.toDouble();

        index++;
    }

    double diaAplicacao = valores[6];
    double aplicacaoMensal = valores[5];
    double cdi = valores[4];
    double valor = valores[3];
 // double taxaDia = valores[2];
    double taxaMes = valores[1];
    double taxaAno = valores[0];

    if (!QDir::setCurrent(ui->salvarInput->text())){
        ui->erroLabel->setText("Diretório no campo Salvar não existe.");
        return;
    };

    vector<QString> caminho = {nomes.pastaRelatorio,
                               mergeStrings({"selic_", convertFQString(taxaAno*100)}),
                               mergeStrings({"cdi_", convertFQString(cdi*100)}),
                               mergeStrings({"valor_", convertFQString(valor)})
    };

    for (auto pasta:caminho){
        if (!QDir::current().mkdir(pasta) && !QDir::current().exists(pasta)){
            ui->erroLabel->setText("Erro ao criar pasta para estimativas.");
            return;
        }

        QDir::setCurrent(QDir::currentPath()+QDir::separator()+pasta);
    }

    // Calcular a quantidade de fins de semana no ano atual
    QDate diaInicial = ui->dataInicial->date();

    QString nomeArquivo = diaInicial.toString("estimativa_yyyy-MM-dd")+".csv";

    // Criando e abrindo o arquivo de relatório para escrita
    QString caminhoRelatorio = QDir::currentPath()+QDir::separator()+nomeArquivo;

    int anoAtual = diaInicial.year();
    QDate diaLimite = ui->dataLimite->date();

    double deFactoDiaAplicacao = diaAplicacao;
    int diasAno = QDate(anoAtual, 12, 31).dayOfYear();

    // Gerar headers
    ui->avisoLabel->setText("Gerando relatório...");

    vector<QString> headers = {"Data", "Valor", "IOF%", "Valor rendido bruto", "Valor rendido %", "Rendimento total no periodo", "Rendimento total no periodo %", "Selic diário%", "Selic mensal%", "Selic anual%", "Selic anual base%", "CDI%"};
    vector<vector<QString>> linhas;

    //QString headerLinha = generateCSVLine(headers);

    //arquivoRel << headerLinha;

    vector<vector<double>> taxas = calcYearIndex(taxaMes, diasAno, anoAtual);

    // A taxa diaria por mes
    vector<double> taxaDiariaPorMes = taxas[0];
    vector<double> taxaMensal = taxas[1];
    double taxaAnoReal = 0;

    for (auto n:taxaMensal){
        taxaAnoReal += n;
    };

    long double valorOriginal = valor;
    long double aumentoRealDia = 0;
    long double valorAtual = valorOriginal;
    long double valorAnterior = valorOriginal;

    long double aumentoJurosAnterior = 0;
    long double aumentoJurosDia = 0;

    int mesAtual = diaInicial.month();
    int indexVal = 0;

    vector<double> iof = runtime.iof();
    unsigned long int diasPassados = 0;

    for (QDate data = diaInicial.addDays(1); data <= diaLimite; data = data.addDays(1)) {
        double iofAtual = 0;

        if (diasPassados+1 <= iof.size()){
            iofAtual = iof[diasPassados];
        }

        cout << "\n" << diasPassados << " " << iofAtual;

        diasPassados++;

        // Skip weekends
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            if (data.day() == deFactoDiaAplicacao){
                deFactoDiaAplicacao++;
            }
            continue;
        }

        vector<QString> linha;

        // Adiciona a data
        linha.push_back(data.toString("yyyy-MM-dd"));
        cout << "\n" << "Data atual: " << linha.back().toStdString();

        // Adicionar valor convertido
        int indexMes = data.month() - 1;

        cout << "INDEX MES: " << indexMes << endl;

        // Aplica taxa diária com efeito acumulado
        long double taxaDiaria = taxaDiariaPorMes[indexMes];

        valorAnterior = valorAtual;

        // Atualizar o valor no primeiro dia do mês
        if (mesAtual <= data.month()){
            if (data.day() == deFactoDiaAplicacao || (data.day() == data.daysInMonth() && data.day() < diaAplicacao)){
                mesAtual++;
                valorAtual += aplicacaoMensal;

                deFactoDiaAplicacao = diaAplicacao;
            }
        };

        long double valorAtualBruto = valorAtual * (1 + taxaDiaria);

        long double jurosIOFAjustado = (valorAtualBruto - valorAnterior) - ((valorAtualBruto - valorAnterior)*iofAtual);

        valorAtual += jurosIOFAjustado;

        long double diferencaValor = valorAtual - valorOriginal;
        long double aumentoBrutoJuros = valorAtual - valorAnterior;

        // Calculo com juros compostos
        aumentoJurosAnterior = aumentoRealDia;
        aumentoRealDia = (valorAtual / valorOriginal - 1);
        aumentoJurosDia = aumentoRealDia - aumentoJurosAnterior;

        QString valorString = mergeStrings({"R$ ", convertFQString(valorAtual)});
        QString iofString = mergeStrings({convertFQString(iofAtual*100), "%"});
        QString diffValor = mergeStrings({"R$ ", convertFQString(diferencaValor)});
        QString aumentoRealStr = mergeStrings({convertFQString(aumentoRealDia*100), "%"});
        QString aumentoJurosDiaStr = mergeStrings({convertFQString(aumentoJurosDia*100), "%"});
        QString aumentoBrutoJurosDiaStr = mergeStrings({"R$ ", convertFQString(aumentoBrutoJuros)});

        linha.push_back(valorString);
        linha.push_back(iofString);
        linha.push_back(aumentoBrutoJurosDiaStr);
        linha.push_back(aumentoJurosDiaStr);
        linha.push_back(diffValor);
        linha.push_back(aumentoRealStr);

        // Adiciona taxa diária
        linha.push_back(mergeStrings({convertFQString(taxaDiariaPorMes[data.month()-1]*100), "%"}));

        // Adicionar taxa mensal
        linha.push_back(mergeStrings({convertFQString(taxaMensal[indexMes]*100), "%"}));

        // Adicionar taxa anual
        linha.push_back(mergeStrings({convertFQString(taxaAnoReal*100), "%"}));

        // Adicionar taxa anual bruta
        double taxaAnualBruto = (taxaAno/cdi)*100;
        linha.push_back(mergeStrings({convertFQString(taxaAnualBruto), "%"}));

        //Adicionar CDI
        linha.push_back(mergeStrings({convertFQString(cdi*100), "%"}));

        linhas.push_back(linha);
        cout << "\n" << linha[0].toStdString() << "\n" << linhas.back()[0].toStdString() << "\n";

        QString linhaFinal = generateCSVLine(linha);

        cout << "Linha: " << linhaFinal.toStdString() << ", Index: " << indexVal;
        indexVal++;

        cout << "Processing date:" << data.toString("yyyy-MM-dd").toStdString() << "Limit date:" << diaLimite.toString("yyyy-MM-dd").toStdString() << "\n";

    };

    // Salvar no CSV
    //for (auto linha:csvCompleto){
    //    arquivoRel << linha;
    //};

    //relatorio.close();

    cout << "=== FIM CSV ===\n" << linhas.back()[0].toStdString() << "\n";

    if (writeCSV(headers, linhas, caminhoRelatorio)){
        ui->avisoLabel->setText("Relatório gerado como "+caminhoRelatorio+"!");
    }

    vector<int> tamanhoTabela = getColumnAndRowCount(caminhoRelatorio, true);
    QStringList listaHeaders = getHeaders(caminhoRelatorio);
    vector<vector<QString>> conteudoTabela = getCSVContent(caminhoRelatorio, true);

    ui->relatorio->clearContents();
    ui->relatorio->setColumnCount(tamanhoTabela[1]);
        ui->relatorio->setRowCount(tamanhoTabela[0]);

    ui->relatorio->setHorizontalHeaderLabels(listaHeaders);

    // Popular a tabela
    int linhaNum = 0;
    for (auto linha:conteudoTabela){
        int colunaNum = 0;
        cout << linha[0].toStdString() << "\n";
        for (auto coluna:linha){
            QTableWidgetItem* item = new QTableWidgetItem(coluna);

            ui->relatorio->setItem(linhaNum, colunaNum, item);
            colunaNum++;
        };

        linhaNum++;
    };
}

void Calculadora::on_Calculadora_destroyed()
{
    if (!deleteTempFolder()){
        cout << "Um erro ocorreu ao deletar o arquivo temporário.";
    }
    return;
}


void Calculadora::on_selecionarDiretorio_clicked()
{
    QString pasta = QFileDialog::getExistingDirectory(this, tr("Abrir pasta"),
                                                      QDir::homePath(),
                                                      QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks);

    if (pasta != ""){
        ui->salvarInput->setText(pasta);
    };
}

void Calculadora::on_configs_clicked()
{
    OptionMenu *wdg = new OptionMenu;
    wdg->show();
}

