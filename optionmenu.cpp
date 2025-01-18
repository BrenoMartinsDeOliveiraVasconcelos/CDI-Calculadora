#include "optionmenu.h"
#include "ui_optionmenu.h"

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

void OptionMenu::on_buttonBox_clicked(QAbstractButton *button)
{

}

