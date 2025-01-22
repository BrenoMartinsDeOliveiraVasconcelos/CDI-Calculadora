#include "optionmenu.h"
#include "ui_optionmenu.h"

#include <configmanager.h>

#include <QFileDialog>
#include <QErrorMessage>

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


void OptionMenu::on_confirmarCancelar_accepted()
{
    // Salvar configuraçãoes
    configuration conf;

    // Procura por virgulas e troca por pontos
    QString cdiVal = ui->cdiVal->text().replace(",", ".");
    QString selicVal = ui->selicVAl->text().replace(",", ".");


    bool cdiBool = conf.setConfig("cdi", cdiVal);
    bool selicBool = conf.setConfig("selic", selicVal);
    bool salvarBool = conf.setConfig("save", ui->salvarVal->text());

    if (!cdiBool || !selicBool || !salvarBool){
        QErrorMessage erro;
        erro.showMessage("Erro ao salvar as configurações.");
    }
}

