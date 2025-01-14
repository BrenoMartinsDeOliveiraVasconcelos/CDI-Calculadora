#include "calculadora.h"
#include "ui_calculadora.h"

// Headers pessoais
#include "temp_management.h"
#include "qcsv.h"

// Bibliotecas C++
#include <iostream>
#include <vector>

// Bibliotecas qt
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

using namespace std;

class nomesTemporario{
public:
    QString taxas = "taxas.txt";
};

Calculadora::Calculadora(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculadora)
{
    ui->setupUi(this);
}

Calculadora::~Calculadora()
{
    delete ui;
}

void Calculadora::on_caciar_clicked()
{
    tempInfo temp;
    nomesTemporario nomes;

    auto formatarValor = [](float valor) {
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
    QDate dataMesSeguinte = dataAtual.addMonths(1);
    QDate dataAnoSeguinte = dataAtual.addYears(1);

    //int mesAtual = dataAtual.month();
    int anoAtual = dataAtual.year();

    // Expressão regular para validar números positivos (inteiros ou decimais)
    const QString numPadrao = "^\\d*(\\.\\d+)?$";
    QRegularExpression re(numPadrao);

    // Vetores para armazenar os inputs e suas conversões
    QString inputs[] = {
        "0" + ui->taxaInput->text(),
        "0" + ui->cdiInput->text(),
        "0" + ui->valorInput->text()
    };
    float valores[3] = {0.0f};

    // Procurar por inputs vazios ou com apenas "0"
    for (int c=0; c<3; c++){
        if (inputs[c].length() <= 1){
            ui->erroLabel->setText("Os dados devem ser um número maior que zero.");
            return;
        };

    };

    // Validar e converter os inputs
    for (int i = 0; i < 3; ++i) {
        if (re.match(inputs[i]).hasMatch()) {
            valores[i] = inputs[i].toFloat();
        } else {
            ui->erroLabel->setText("Os campos devem ser números positivos.");
            return;
        }
    }

    // Calcular a quantidade de fins de semana no mês atual
    //int diasMes = dataAtual.daysInMonth();
    //int fimSemanaMes = 0;
    //for (int dia = 1; dia <= diasMes; ++dia) {
    //    QDate data(dia, mesAtual, anoAtual);
    //    if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
    //        ++fimSemanaMes;
    //    }
    //}

    // Calcular a quantidade de fins de semana no ano atual
    int diasAno = QDate(anoAtual, 12, 31).dayOfYear();
    int fimSemanaAno = 0;
    for (int dia = 1; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            ++fimSemanaAno;
        }
    }

    //int totalDiasMes = diasMes - fimSemanaMes;
    int totalDiasAno = diasAno - fimSemanaAno;

    // Calcular taxas e estimativas
    float cdi = valores[1] / 100.0f;
    float seliac = (valores[0] * cdi) / 100.0f;
    float seliacMes = seliac / 12;
    float seliacDia = seliac / totalDiasAno;



    // Exibir taxas mensais e diárias
    ui->aumentoMes->setText(formatarValor(seliacMes * 100) + "% ao mês");
    ui->aumentoDia->setText(formatarValor(seliacDia * 100) + "% ao dia");

    // Calcular estimativas finais
    float valorAplicado = valores[2];
    float estimativaDia = valorAplicado * seliacDia;
    float estimativaMes = valorAplicado * seliacMes;
    float estimativaAno = valorAplicado * seliac;

    ui->finalDia->setText("R$ " + formatarValor(valorAplicado + estimativaDia));
    ui->finalMes->setText("R$ " + formatarValor(valorAplicado + estimativaMes));
    ui->finalAno->setText("R$ " + formatarValor(valorAplicado + estimativaAno));

    // Exibir datas
    QString amanha = dataAamanha.toString(formatoData);
    QString mesSeguinte = dataMesSeguinte.toString(formatoData);
    QString anoSeguinte = dataAnoSeguinte.toString(formatoData);

    ui->dataDia->setText(formatarData(amanha));
    ui->dataMes->setText(formatarData(mesSeguinte));
    ui->dataMes_2->setText(formatarData(anoSeguinte));

    // Exibir taxa total
    ui->erroLabel->setText("Taxa total - " + formatarValor(seliac * 100) + "%");

    // Escrever as taxas no arquivo temporário
    taxasTxt << seliac << "\n" << seliacMes << "\n" << seliacDia << "\n" << ui->valorInput->text();

    ui->estimarValores->setEnabled(true);
}

void Calculadora::on_estimarValores_clicked()
{
    tempInfo temp;
    nomesTemporario nomes;

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);

    float valores[4] = {0, 0, 0, 0}; // Indexes: {anual, mensal, dia, valor}

    if (!arquivoTaxas.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->erroLabel->setText("Falha ao ler o arquivo temporário de taxas.");
        return;
    };

    // Gerar relatório diário e mensal até o fim do ano
    QTextStream arquivo(&arquivoTaxas);
    int index = 0;
    while (!arquivo.atEnd()) {
        QString line = arquivo.readLine();
        valores[index] = line.toFloat();

        index++;
    }

    // Calcular a quantidade de fins de semana no ano atual
    QDate diaAtual = QDate::currentDate();

    // Criando e abrindo o arquivo de relatório para escrita
    QFile relatorio(QDir::currentPath()+QDir::separator()+"output.csv");

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
    vector<QString> headers = {"a", "b", "c"};
    QString headerLinha = generateCSVLine(headers);

    arquivoRel << headerLinha << "\n";

    for (int dia = numDia; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            continue;
        }
    };
}

void Calculadora::on_Calculadora_destroyed()
{
    if (!deleteTempFolder()){
        cout << "Um erro ocorreu ao deletar o arquivo temporário.";
    }
    return;
}


