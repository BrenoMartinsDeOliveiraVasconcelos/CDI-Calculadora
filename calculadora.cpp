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

        ui->taxaInput->setText(mapaConfig["selic"]);
        ui->cdiInput->setText(mapaConfig["cdi"]);
        ui->aplicacaoMes->setText("0,00");

        // Inicializar a tabela
        ui->relatorio->setColumnCount(1024);
        ui->relatorio->setRowCount(1024);

        ui->relatorio->horizontalHeader()->setMinimumSectionSize(200);
    }

Calculadora::~Calculadora()
{
    delete ui;
}

void Calculadora::on_caciar_clicked()
{
    ui->erroLabel->setText("");
    ui->avisoLabel->setText("");

    configuration config;
    map<QString, QString> mapaConfig = config.getConfig();

    tempInfo temp;
    nomesTemporario nomes;

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
    QDate dataAtual = QDate::currentDate();

    QDate dataAamanha = dataAtual.addDays(1);
    QDate dataMesSeguinte = QDate(dataAtual.year(), dataAtual.month(), dataAtual.daysInMonth());
    QDate dataAnoSeguinte = QDate(dataAtual.year(), 12, 31);


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
        "0" + ui->aplicacaoMes->text().replace(",", ".")
    };
    double valores[4] = {0.0f};

    // Procurar por numeros negativos em inputs

    int index = 0;
    for (auto v:inputs){
        if (v.contains("-")){
            inputs[index] = "-" + v.replace("-", "");
        };

        index++;
    }

    // Validar e converter os inputs
    for (int i = 0; i < 4; ++i) {
        if (re.match(inputs[i]).hasMatch()){
            valores[i] = inputs[i].toDouble();
        } else if (minusRe.match(inputs[i]).hasMatch()){
            ui->erroLabel->setText("Os campos devem ser números positivos.");
            return;
        }else {
            ui->erroLabel->setText("Os campos devem ser números.");
            return;
        }
    };

    // Calcular taxas e estimativas
    double cdi = valores[1] / 100;
    double seliac = (valores[0] * cdi) / 100;
    double seliacMes = seliac / 12;

    int numDiasMes = 0;

    double valorAplicado = valores[2];
    double seliacDia = ((seliacMes*100) / numDiasMes) / 100;

    // Calcular estimativas finais
    double estimativaAno = valorAplicado * pow((1 + seliacMes), (13 - dataAtual.month()));
    int anoAtual = dataAtual.year();

    int diasAno = dataAtual.daysInYear();
    vector<vector<double>> taxas = calcYearIndex(seliacMes, diasAno, anoAtual);

    vector<double> taxaDiariaPorMes = taxas[0];
    vector<double> taxaMensal = taxas[1];

    double valorAtual = valorAplicado;

    int mesAtual = dataAtual.month();
    int fatorMultiplicador = 0;

    for (int dia = dataAtual.dayOfYear()+1; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            continue;
        }

        // Aplicar o dinheiro a cada inicio de mês
        if (mesAtual < data.month()){
            mesAtual++;
            fatorMultiplicador++;

            valorAtual += valores[3];

            ui->finalAnoBurto->setText("R$ " + formatarValor(valorAplicado+(valores[3]*fatorMultiplicador)).replace(".", ","));
        }

        // Adicionar valor convertido
        int indexMes = data.month() - 1;

        cout << "INDEX MES: " << indexMes << endl;

        // Aplica taxa diária com efeito acumulado
        double taxaDiaria = taxaDiariaPorMes[indexMes];
        valorAtual *= (1 + taxaDiaria);
    };

    estimativaAno = valorAtual;

    ui->finalAno->setText("R$ " + formatarValor(estimativaAno).replace(".", ","));

    // Exibir datas
    QString amanha = dataAamanha.toString(formatoData);
    QString mesSeguinte = dataMesSeguinte.toString(formatoData);
    QString anoSeguinte = dataAnoSeguinte.toString(formatoData);

    ui->dataMes_2->setText(formatarData(anoSeguinte));

    // Exibir taxa total
    double selicComJuros = (valorAtual / valorAplicado) - 1.00;

    ui->taxaAnual->setText("Seliac: " + formatarValor(seliac * 100) + "%"+"\nAumento com juros: " + formatarValor(selicComJuros * 100)+"%");

    // Escrever as taxas no arquivo temporário
    taxasTxt << seliac << "\n" << seliacMes << "\n" << seliacDia << "\n" << ui->valorInput->text() << "\n" << cdi << "\n" << valores[3];

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
}

void Calculadora::on_estimarValores_clicked()
{
    ui->avisoLabel->setText("");
    ui->erroLabel->setText("");

    tempInfo temp;
    nomesTemporario nomes;


    double valores[6] = {0, 0, 0, 0, 0, 0}; // Indexes: {anual, mensal, dia, valor, aplicacaomensal}

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
    QDate diaAtual = QDate::currentDate();

    QString nomeArquivo = diaAtual.toString("estimativa_yyyy-MM-dd")+".csv";

    // Criando e abrindo o arquivo de relatório para escrita
    QString caminhoRelatorio = QDir::currentPath()+QDir::separator()+nomeArquivo;
    QFile relatorio(caminhoRelatorio);

    if (!relatorio.open(QIODevice::WriteOnly | QIODevice::Text)){
        ui->erroLabel->setText("O diretório está inacessível.");
        return;
    };

    relatorio.close();

    if (!relatorio.open(QIODevice::Append | QIODevice::Text)){
        ui->erroLabel->setText("Erro ao abrir o arquivo de relatório para escrita.");
        return;
    };

    QTextStream arquivoRel(&relatorio);

    int anoAtual = diaAtual.year();
    int numDia = diaAtual.dayOfYear();
    int diasAno = QDate(anoAtual, 12, 31).dayOfYear();

    // Gerar headers
    ui->avisoLabel->setText("Gerando relatório...");

    vector<QString> headers = {"Data", "Valor", "Valor rendido bruto", "Valor rendido %", "Rendimento total no periodo", "Rendimento total no periodo %", "Selic diário%", "Selic mensal%", "Selic anual%", "Selic anual base%", "CDI%"};
    QString headerLinha = generateCSVLine(headers);

    arquivoRel << headerLinha;

    vector<vector<double>> taxas = calcYearIndex(taxaMes, diasAno, anoAtual);

    // A taxa diaria por mes
    vector<double> taxaDiariaPorMes = taxas[0];
    vector<double> taxaMensal = taxas[1];
    double taxaAnoReal = 0;

    for (auto n:taxaMensal){
        taxaAnoReal += n;
    };

    double valorOriginal = valor;
    double aumentoRealDia = 0;
    double valorAtual = valorOriginal;
    double valorAnterior = valorOriginal;

    double aumentoJurosAnterior = 0;
    double aumentoJurosDia = 0;

    int mesAtual = diaAtual.month();

    for (int dia = numDia+1; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            continue;
        }

        vector<QString> linha;

        // Adiciona a data
        linha.push_back(data.toString("yyyy-MM-dd"));

        // Adicionar valor convertido
        int indexMes = data.month() - 1;

        cout << "INDEX MES: " << indexMes << endl;

        // Aplica taxa diária com efeito acumulado
        double taxaDiaria = taxaDiariaPorMes[indexMes];

        valorAnterior = valorAtual;

        // Atualizar o valor no primeiro dia do mês
        if (mesAtual < data.month()){
            mesAtual++;

            valorAtual += aplicacaoMensal;
        };

        valorAtual *= (1 + taxaDiaria);

        double diferencaValor = valorAtual - valorOriginal;
        double aumentoBrutoJuros = valorAtual - valorAnterior;

        // Calculo com juros compostos
        aumentoJurosAnterior = aumentoRealDia;
        aumentoRealDia = (valorAtual / valorOriginal - 1);
        aumentoJurosDia = aumentoRealDia - aumentoJurosAnterior;

        QString valorString = mergeStrings({"R$ ", convertFQString(valorAtual)});
        QString diffValor = convertFQString(diferencaValor);
        QString aumentoRealStr = mergeStrings({convertFQString(aumentoRealDia*100), "%"});
        QString aumentoJurosDiaStr = mergeStrings({convertFQString(aumentoJurosDia*100), "%"});
        QString aumentoBrutoJurosDiaStr = mergeStrings({"R$ ", convertFQString(aumentoBrutoJuros)});


        if (diferencaValor >= 0){
            diffValor = "+"+diffValor;
        };

        linha.push_back(valorString);
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

        QString linhaFinal = generateCSVLine(linha);

        arquivoRel << linhaFinal;

    };

    ui->avisoLabel->setText("Relatório gerado como "+caminhoRelatorio+"!");
    relatorio.close();

    vector<int> tamanhoTabela = getColumnAndRowCount(caminhoRelatorio, true);
    QStringList listaHeaders = getHeaders(caminhoRelatorio);
    vector<vector<QString>> conteudoTabela = getCSVContent(caminhoRelatorio, true);

    ui->relatorio->setColumnCount(tamanhoTabela[1]);
        ui->relatorio->setRowCount(tamanhoTabela[0]);

    ui->relatorio->setHorizontalHeaderLabels(listaHeaders);

    // Popular a tabela
    int linhaNum = 0;
    for (auto linha:conteudoTabela){
        int colunaNum = 0;
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

    ui->salvarInput->setText(pasta);
}

void Calculadora::on_configs_clicked()
{
    OptionMenu *wdg = new OptionMenu;
    wdg->show();
}

