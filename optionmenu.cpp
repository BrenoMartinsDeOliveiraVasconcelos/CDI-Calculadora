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


#include "optionmenu.h"
#include "ui_optionmenu.h"

#include <configmanager.h>
#include <applicationclass.h>

#include <QFileDialog>
#include <QErrorMessage>
#include <QMessageBox>

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
    ui->salvarVal->setText(valoresConf["path"]);
    ui->autoRelatorio->setChecked(valoresConf["autorel"].toInt());
    ui->relatorioAcao->setCurrentIndex(valoresConf["acaorel"].toInt());
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

    // Só troca a config se tiver algo
    if (pasta != ""){
        ui->salvarVal->setText(pasta);
    };
}


void OptionMenu::on_confirmarCancelar_accepted()
{
    // Salvar configuraçãoes
    configuration conf;
    runtimeConsts consts;

    QString w = conf.getConfig()["w"];
    QString h = conf.getConfig()["h"];

    conf.clearConfig();

    // Procura por virgulas e troca por pontos
    QString cdiVal = ui->cdiVal->text().replace(",", ".");
    QString selicVal = ui->selicVAl->text().replace(",", ".");
    QString autorelatorio = ui->autoRelatorio->isChecked() ? "1" : "0";
    QString relatorioAcao = QString::number(ui->relatorioAcao->currentIndex());


    bool cdiBool = conf.setConfig("cdi", cdiVal);
    bool selicBool = conf.setConfig("selic", selicVal);
    bool salvarBool = conf.setConfig("path", ui->salvarVal->text());
    bool autorelatorioBool = conf.setConfig("autorel", autorelatorio);
    bool relatorioAcaoBool = conf.setConfig("acaorel", relatorioAcao);

    conf.setConfig("w", w);
    conf.setConfig("h", h);

    if (!cdiBool || !selicBool || !salvarBool || !autorelatorioBool || !relatorioAcaoBool){
        QErrorMessage erro;
        erro.showMessage("Erro ao salvar as configurações.");
    }


    if (QMessageBox::Yes == QMessageBox::question(this, "Reiniciar programa", "É necessário reiniciar o programa para carregar algumas configurações. Fazer isso agora?", QMessageBox::Yes | QMessageBox::No))
    {
        qApp->exit(consts.restartCode());
    };

}


void OptionMenu::on_resetar_clicked()
{
    configuration config;
    runtimeConsts constantes;

    config.clearConfig();
    config.generateConfigFolder(true);

    if (QMessageBox::Yes == QMessageBox::question(this, "Reiniciar programa", "É necessário reiniciar o programa para carregar algumas configurações. Fazer isso agora?", QMessageBox::Yes | QMessageBox::No))
    {
        qApp->exit(constantes.restartCode());
    };

}

