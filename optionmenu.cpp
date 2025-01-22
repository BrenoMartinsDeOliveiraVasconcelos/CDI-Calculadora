#include "optionmenu.h"
#include "ui_optionmenu.h"

#include <configmanager.h>

#include <QFileDialog>
#include <QErrorMessage>
#include <map>

using namespace std;

OptionMenu::OptionMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionMenu)
{
    configuration conf;

    ui->setupUi(this);

    map<QString, QString> valoresConf = conf.getConfig();
    ui->cdiVal->setText(valoresConf["cdi"]);
    ui->selicVAl->setText(valoresConf["selic"]);
    ui->salvarVal->setText(valoresConf["save"]);
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

    conf.clearConfig();

    // Procura por virgulas e troca por pontos
    QString cdiVal = ui->cdiVal->text().replace(",", ".");
    QString selicVal = ui->selicVAl->text().replace(",", ".");


    bool cdiBool = conf.setConfig("cdi", cdiVal);
    bool selicBool = conf.setConfig("selic", selicVal);
    bool salvarBool = conf.setConfig("path", ui->salvarVal->text());

    if (!cdiBool || !selicBool || !salvarBool){
        QErrorMessage erro;
        erro.showMessage("Erro ao salvar as configurações.");
    }
}

