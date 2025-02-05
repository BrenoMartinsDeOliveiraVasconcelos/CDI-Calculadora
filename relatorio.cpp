#include "relatorio.h"
#include "ui_relatorio.h"

Relatorio::Relatorio(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Relatorio)
{
    ui->setupUi(this);
}

Relatorio::~Relatorio()
{
    delete ui;
}
