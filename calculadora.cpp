#include "calculadora.h"
#include "ui_calculadora.h"

// Headers pessoais
#include "temp_management.h"
#include "qcsv.h"
#include "qtdecimalconversion.h"
#include "qstringutils.h"

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
#include <QFileDialog>

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
}

Calculadora::~Calculadora()
{
    delete ui;
}

void Calculadora::on_caciar_clicked()
{
    ui->avisoLabel->setText("");
    ui->erroLabel->setText("");

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
    QDate dataMesSeguinte = QDate(dataAtual.year(), dataAtual.month(), dataAtual.daysInMonth());
    QDate dataAnoSeguinte = QDate(dataAtual.year(), 12, 31);


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
    };

    // Calcular taxas e estimativas
    float cdi = valores[1] / 100;
    float seliac = (valores[0] * cdi) / 100;
    float seliacMes = seliac / 12;

    int numDiasMes = 0;



    float valorAplicado = valores[2];
    float seliacDia = ((seliacMes*100) / numDiasMes) / 100;

    // Calcular estimativas finais
    float estimativaAno = valorAplicado * pow((1 + seliacMes), (13 - dataAtual.month()));

    ui->finalAno->setText("R$ " + formatarValor(estimativaAno));

    // Exibir datas
    QString amanha = dataAamanha.toString(formatoData);
    QString mesSeguinte = dataMesSeguinte.toString(formatoData);
    QString anoSeguinte = dataAnoSeguinte.toString(formatoData);

    ui->dataMes_2->setText(formatarData(anoSeguinte));

    // Exibir taxa total
    ui->taxaAnual->setText("Taxa total - " + formatarValor(seliac * 100) + "%");

    // Escrever as taxas no arquivo temporário
    taxasTxt << seliac << "\n" << seliacMes << "\n" << seliacDia << "\n" << ui->valorInput->text() << "\n" << cdi;

    ui->estimarValores->setEnabled(true);
    ui->salvar->setEnabled(true);
    ui->salvarInput->setEnabled(true);
    ui->selecionarDiretorio->setEnabled(true);

    ui->salvarInput->setText(QDir::homePath());

    ui->avisoLabel->setText("Clique em relatório para ver a estimativa completa até o fim do ano.");
}

void Calculadora::on_estimarValores_clicked()
{
    ui->avisoLabel->setText("");
    ui->erroLabel->setText("");

    tempInfo temp;
    nomesTemporario nomes;


    float valores[5] = {0, 0, 0, 0, 0}; // Indexes: {anual, mensal, dia, valor}

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);
    if (!arquivoTaxas.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->erroLabel->setText("Falha ao ler o arquivo temporário de taxas.");
        return;
    };

    QTextStream arquivo(&arquivoTaxas);

    int index = 0;
    while (!arquivo.atEnd()) {
        QString line = arquivo.readLine();
        valores[index] = line.toFloat();

        index++;
    }

    float cdi = valores[4];
    float valor = valores[3];
 // float taxaDia = valores[2];
    float taxaMes = valores[1];
    float taxaAno = valores[0];

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
    QFile relatorio(QDir::currentPath()+QDir::separator()+nomeArquivo);

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

    vector<QString> headers = {"Valor", "Aumento Bruto", "Aumento Real%", "Data", "Taxa Dia", "Taxa Mes%", "Taxa Ano%", "Taxa Ano Bruto%", "CDI%"};
    QString headerLinha = generateCSVLine(headers);

    arquivoRel << headerLinha;

    // A taxa diaria por mes
    vector<float> taxaDiariaPorMes = {};
    vector<float> taxaMensal = {};
    float taxaAnoReal = 0;

    int mes = 1;
    int numDiasMes = 0;
    for (int dia = 1; dia <= diasAno; ++dia){
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);

        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday){
            continue;
        };

        ++numDiasMes;

        if (data.month() > mes || dia==diasAno){
            float taxaDiaria = ((taxaMes*100)/numDiasMes)/100;

            taxaDiariaPorMes.push_back(taxaDiaria);
            taxaMensal.push_back(taxaDiaria*numDiasMes);

            ++mes;
            numDiasMes = 0;
        };

        cout << dia << " - " << mes << " - " << numDiasMes << endl;
    };

    for (auto n:taxaMensal){
        taxaAnoReal += n;
    };

    float valorOriginal = valor;
    float aumentoRealDia = 0;
    float valorAtual = valorOriginal;

    for (int dia = numDia+1; dia <= diasAno; ++dia) {
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);
        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday) {
            continue;
        }
        vector<QString> linha;

        // Adicionar valor convertido
        int indexMes = data.month() - 1;

        cout << "INDEX MES: " << indexMes << endl;

        // Aplica taxa diária com efeito acumulado
        float taxaDiaria = taxaDiariaPorMes[indexMes];
        valorAtual *= (1 + taxaDiaria);

        float diferencaValor = valorAtual - valorOriginal;
        aumentoRealDia = (valorAtual / valorOriginal - 1);

        QString valorString = convertFQString(valorAtual);
        QString diffValor = convertFQString(diferencaValor);
        QString aumentoRealStr = mergeStrings({convertFQString(aumentoRealDia*100), "%"});

        if (diferencaValor >= 0){
            diffValor = "+"+diffValor;
        };

        linha.push_back(valorString);
        linha.push_back(diffValor);
        linha.push_back(aumentoRealStr);

        // Adiciona a data
        linha.push_back(data.toString("yyyy-MM-dd"));

        // Adiciona taxa diária
        linha.push_back(mergeStrings({convertFQString(taxaDiariaPorMes[data.month()-1]*100), "%"}));

        // Adicionar taxa mensal
        linha.push_back(mergeStrings({convertFQString(taxaMensal[indexMes]*100), "%"}));

        // Adicionar taxa anual
        linha.push_back(mergeStrings({convertFQString(taxaAnoReal*100), "%"}));

        // Adicionar taxa anual bruta
        float taxaAnualBruto = (taxaAno/cdi)*100;
        linha.push_back(mergeStrings({convertFQString(taxaAnualBruto), "%"}));

        //Adicionar CDI
        linha.push_back(mergeStrings({convertFQString(cdi*100), "%"}));

        QString linhaFinal = generateCSVLine(linha);

        arquivoRel << linhaFinal;

    };

    ui->avisoLabel->setText("Relatório gerado como "+nomeArquivo+"!");
}

void Calculadora::on_Calculadora_destroyed()
{
    if (!deleteTempFolder()){
        cout << "Um erro ocorreu ao deletar o arquivo temporário.";
    }
    return;
}



void Calculadora::on_pushButton_clicked()
{

}


void Calculadora::on_selecionarDiretorio_clicked()
{
    QString pasta = QFileDialog::getExistingDirectory(this, tr("Abrir pasta"),
                                                      QDir::homePath(),
                                                      QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks);

    ui->salvarInput->setText(pasta);
}

