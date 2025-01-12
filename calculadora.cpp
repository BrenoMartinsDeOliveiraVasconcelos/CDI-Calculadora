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

    // Validar e converter os inputs
    for (int i = 0; i < 3; ++i) {
        if (re.match(inputs[i]).hasMatch()) {
            valores[i] = inputs[i].toFloat();
        } else {
            ui->erroLabel->setText("Os campos devem ser números positivos.");
            return;
        }
    }

    // Calcular taxas e estimativas
    float cdi = valores[1] / 100.0f;
    float seliac = (valores[0] * cdi) / 100.0f;
    float seliacMes = seliac / 12.0f;
    float seliacDia = seliac / 365.0f;

    auto formatarValor = [](float valor) {
        return QString::number(valor, 'f', 2);
    };

    // Exibir taxas mensais e diárias
    ui->aumentoMes->setText(formatarValor(seliacMes * 100) + "% ao mês");
    ui->aumentoDia->setText(formatarValor(seliacDia * 100) + "% ao dia");

    // Calcular estimativas finais
    float valorAplicado = valores[2];
    float estimativaDia = valorAplicado * seliacDia;
    float estimativaMes = valorAplicado * seliacMes;
    float estimativaAno = valorAplicado * seliac;

    ui->finalDia->setText("R$ " + formatarValor(valorAplicado + estimativaDia) + " no final do dia");
    ui->finalMes->setText("R$ " + formatarValor(valorAplicado + estimativaMes) + " no final do mês");
    ui->finalAno->setText("R$ " + formatarValor(valorAplicado + estimativaAno) + " no final do ano");

    // Exibir taxa total
    ui->erroLabel->setText("Taxa total - " + formatarValor(seliac * 100) + "%");
}



