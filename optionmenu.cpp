#include "optionmenu.h"
#include "ui_optionmenu.h"

#include <QFileDialog>

OptionMenu::OptionMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionMenu)
{
    ui->setupUi(this);
}

OptionMenu::~OptionMenu()
{
    delete ui;
}


void OptionMenu::on_selecionar_clicked()
{
    QString pasta = QFileDialog::getExistingDirectory(this, tr("Abrir pasta"),
                                                      QDir::homePath(),
                                                      QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks);

    ui->salvarVal->setText(pasta);
}

