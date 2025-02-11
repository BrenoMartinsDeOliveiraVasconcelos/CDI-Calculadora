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

// Para evitar certos bugs ai


#if defined(_WIN64)
#define big    long long
#else
#define big     long
#endif

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

// Bibliotecas QT
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
#include <QDesktopServices>
#include <QClipboard>

using namespace std;

// variavel global
appManager app;

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

        map<QString, QString> mapaConfig = config.getConfig();

        int w = mapaConfig["h"].toInt();
        int h = mapaConfig["w"].toInt();

        // Colocar a tela no tamanho
        if (w != 0 && h != 0){
            resize(h, w);
        };

        // Inicializar valores
        ui->taxaInput->setText(mapaConfig["selic"]);
        ui->cdiInput->setText(mapaConfig["cdi"]);
        ui->aplicacaoMes->setText("0,00");
        ui->diaInput->setText("1");
        ui->numDiasInput->setText("0");

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

        QMessageBox::warning(this, "Aviso", "Esse programa realiza uma estimativa considerando uma taxa de juros estável durante o periodo, junto a uma margem de erro de 0,003%. O valor final deve ser usado de base e não como valor absoluto.", QMessageBox::Ok);
    }

Calculadora::~Calculadora()
{
    delete ui;
}

void Calculadora::on_caciar_clicked()
{

    app.delay();

    if (ui->dataInicial->date().daysTo(ui->dataLimite->date()) < 0){
        QMessageBox::critical(this, "Erro", "A data inicial não pode ser depois da data limiste.", QMessageBox::Ok);
        return;
    }

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
            QMessageBox::critical(this, "Erro", "Não foi possível criar a pasta temporária.", QMessageBox::Ok);
            return;
        };
    };

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);

    if (!arquivoTaxas.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Erro", "Erro ao ler o arquivo temporário.", QMessageBox::Ok);

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
        "0" + ui->diaInput->text().replace(",", "."),
        "0" + ui->numDiasInput->text().replace(",", ".")
    };

    QString valoresInteiros[] = {
        inputs[4],
        inputs[5]
    };

    double valores[6] = {0.0f};
    unsigned big int valorMaximoDinheiro = runtime.maxMoneyValue();
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
    for (int i = 0; i < 6; ++i) {
        if (re.match(inputs[i]).hasMatch()){
            valores[i] = inputs[i].toDouble();

            // Checar se o valor é valido
            if (valores[i] > valorMaximoDinheiro){
                QMessageBox::critical(this, "Erro", "Os valores não podem ser maior que "+QString::number(valorMaximoDinheiro)+".", QMessageBox::Ok);
                return;
            };

        } else if (minusRe.match(inputs[i]).hasMatch()){
            QMessageBox::critical(this, "Erro", "Os campos devem ser números positivos", QMessageBox::Ok);
            return;
        }else {
            QMessageBox::critical(this, "Erro", "Os campos devem ser números.", QMessageBox::Ok);
            return;
        }
    };

    //Verificação especial para o input de dia que tem que ser inteiro necessáriamente
    for (auto i:valoresInteiros){
        if (i.contains(".")){
            QMessageBox::critical(this, "Erro", "O valor '"+i.removeFirst()+"' não é inteiro.", QMessageBox::Ok);
            return;
        };
    };

    // Verifica se o dia inputado POSSIVELMENTE existe.
    if (valores[4] > maximoDias){
        QMessageBox::critical(this, "Erro", "O dia de aplicação não pode ser maior que "+QString::number(maximoDias)+".", QMessageBox::Ok);
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

    unsigned big int diasPassados = valoresInteiros[1].toULong();

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
        cout << "APP" << deFactoDiaAplicacao << "APP\n";
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
    };

    estimativaAno = valorAtual;

    int numeroMeses = diaLimite.month() - dataInicial.month();
    double rendimentoMedio = (valorAtual - (valores[2]+(valores[3]*numeroMeses))) / numeroMeses; // (valor atual - (valor inicial + (aplicação mensal * numero de meses no periodo))) / numero de meses no periodo

    if (estimativaAno <= valorMaximoDinheiro){
        ui->finalAno->setText("R$ " + formatarValor(estimativaAno).replace(".", ","));
    }else{
        ui->finalAno->setText(muitoGrande);
    }

    ui->rendimentoPorMes->setText("R$ "+ formatarValor(rendimentoMedio).replace(".", ","));

    // Exibir datas

    ui->dataMes_2->setText(formatarData(diaLimite.toString(formatoData)));

    // Exibir taxa total
    double selicComJuros = (valorAtual / valorAplicado) - 1.00;

    ui->taxaAnual->setText("Selic: " + formatarValor(seliac * 100) + "%"+"\nAumento com juros: " + formatarValor(selicComJuros * 100)+"%");

    // Escrever as taxas no arquivo temporário500ms
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
    arquivoTaxas.close();

    if (mapaConfig["autorel"] == "1"){
        ui->estimarValores->click();
    }else{
        QMessageBox::information(this, "Sucesso", "Clique em em \"Relatório\" para gerar a estimativa completa.", QMessageBox::Ok);

    }
}

void Calculadora::on_estimarValores_clicked()
{
    app.delay();

    tempInfo temp;
    nomesTemporario nomes;
    runtimeConsts runtime;
    configuration config;


    double valores[7] = {0, 0, 0, 0, 0, 0, 0}; // Indexes: {anual, mensal, dia, valor, aplicacaomensal, diaaplicacao}

    QFile arquivoTaxas(temp.tempFolderAbsolute+nomes.taxas);
    if (!arquivoTaxas.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Erro", "Erro ao ler o arquivo temporário.", QMessageBox::Ok);
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

    cout << aplicacaoMensal;

    if (!QDir::setCurrent(ui->salvarInput->text())){
        QMessageBox::critical(this, "Erro", "Pasta não existe.", QMessageBox::Ok);
        return;
    };

    vector<QString> caminho = {nomes.pastaRelatorio};

    for (auto pasta:caminho){
        if (!QDir::current().mkdir(pasta) && !QDir::current().exists(pasta)){
            QMessageBox::critical(this, "Erro", "Erro ao criar pasta para salvar o arquivo.", QMessageBox::Ok);
            return;
        }

        QDir::setCurrent(QDir::currentPath()+QDir::separator()+pasta);
    }

    // Calcular a quantidade de fins de semana no ano atual
    QDate diaInicial = ui->dataInicial->date();

    QString nomeArquivo = "0.csv";
    // Gerar um nome
    while (QDir::current().exists(nomeArquivo)){
        nomeArquivo = QString::number(nomeArquivo.replace(".csv", "").toUInt()+1)+".csv";
    };

    // Criando e abrindo o arquivo de relatório para escrita
    QString caminhoRelatorio = QDir::currentPath()+QDir::separator()+nomeArquivo;

    int anoAtual = diaInicial.year();
    QDate diaLimite = ui->dataLimite->date();

    double deFactoDiaAplicacao = diaAplicacao;
    int diasAno = QDate(anoAtual, 12, 31).dayOfYear();

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
    unsigned big int diasPassados = ui->numDiasInput->text().toLong();

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

        long double valorAtualBruto = valorAtual * (1 + taxaDiaria);

        long double jurosIOFAjustado = (valorAtualBruto - valorAtual) - ((valorAtualBruto - valorAtual)*iofAtual);

        valorAtual += jurosIOFAjustado;

        // Atualizar o valor no dia do mês específicado
        if (mesAtual <= data.month()){
            if (data.day() == deFactoDiaAplicacao || (data.day() == data.daysInMonth() && data.day() < diaAplicacao)){
                mesAtual++;
                valorAtual += aplicacaoMensal;
                cout << "\n\n" << "Valor aumentado:" << valorAtual << " Aplic: " << aplicacaoMensal;

                deFactoDiaAplicacao = diaAplicacao;
            }
        };

        cout << "VALOR ATUAL: " << valorAtual << "\n";

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

    if (!writeCSV(headers, linhas, caminhoRelatorio)){
        QMessageBox::critical(this, "Erro", "Um erro ocorreu e o relatório não foi salvo. Impossóvel continuar.", QMessageBox::Ok);
        return;
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

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(caminhoRelatorio);

    // Por fim, perguntar ao usuário se deseja abrir o arquivo com o programa responsável.
    int opcaoAcao = config.getConfig()["acaorel"].toInt();

    if (opcaoAcao == 0)
    {
        QDesktopServices::openUrl(caminhoRelatorio);
    }else if (opcaoAcao == 1){
        QDesktopServices::openUrl(QDir::currentPath());
    }else if (opcaoAcao == 2){
        QMessageBox::information(this, "Sucesso", "O relatório foi gerado e o caminho do arquivo foi copiado para a área de transferência.");
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


void Calculadora::on_close_clicked(){
    configuration config;


    // Salvar as proporções da tela
    QString w = QString::number(this->width());
    QString h = QString::number(this->height());

    map<QString, QString> configMapa = config.getConfig();

    configMapa["w"] = w;
    configMapa["h"] = h;

    config.clearConfig();

    // Iterar sobre as configs
    for (auto const& [key, val] : configMapa){
        config.setConfig(key, val);
    }
}

