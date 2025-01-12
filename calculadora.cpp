#include "calculadora.h"
#include "ui_calculadora.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
    QString numPadrao = "^\\d*(\\.\\d+)?$";
    QRegularExpression re;


    float seliac = 0;
    float seliacMes = 0;
    float seliacDia = 0;
    float cdi = 0;

    QString variaveis[3] = {"0"+ui->taxaInput->text(), "0"+ui->cdiInput->text(), "0"+ui->valorInput->text()};
    float variaveisConv[3] = {};


    // Verificar se os inputs são numeros validos
    re.setPattern(numPadrao);

    for (int index=0; index<3; index++){
        QRegularExpressionMatch match = re.match(variaveis[index]);
        bool temMatch = match.hasMatch();

        if (temMatch){
            variaveisConv[index] = variaveis[index].toFloat();
        }else{
            ui->erroLabel->setText("Os campos devem ser números positivos.");
            return;
        }
    }

    // Se tudo der certo, vamos fazer os calculos

    // Calcular as taxas mensais e dirarias
    float cdiAdd = (variaveisConv[1]) / 100; // Só a adição

    seliac = (variaveisConv[0] * cdiAdd) / 100; // Taxa seliac
    seliacMes = seliac / 12; // Seliac mensal
    seliacDia = seliac / 365; // Por dia considerando ano não bissexto
    QString valorOutput;

    valorOutput.setNum(seliacMes*100, 'f', 2);
    ui->aumentoMes->setText(valorOutput+"% ao mês");

    valorOutput.setNum(seliacDia*100, 'f', 2);
    ui->aumentoDia->setText(valorOutput+"% ao dia");

    // Estimativas diárias, mensais e anuais.
    float valorAplicado = variaveisConv[2];

    float estimativaDia = valorAplicado * seliacDia;
    float estimativaMes = valorAplicado * seliacMes;
    float estimativaAno = valorAplicado * seliac;

    valorOutput.setNum(valorAplicado + estimativaDia, 'f', 2);
    ui->finalDia->setText("R$ "+valorOutput+" no final do dia");

    valorOutput.setNum(valorAplicado + estimativaMes, 'f', 2);
    ui->finalMes->setText("R$ "+valorOutput+" no final do mês");

    valorOutput.setNum(valorAplicado + estimativaAno, 'f', 2);
    ui->finalAno->setText("R$ "+valorOutput+" no final do ano");


    valorOutput.setNum(seliac*100, 'f', 2);
    ui->erroLabel->setText("Taxa total - "+valorOutput+"%");
}


