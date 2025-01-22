#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include <QDialog>

namespace Ui {
class OptionMenu;
}

class OptionMenu : public QDialog
{
    Q_OBJECT

public:
    explicit OptionMenu(QWidget *parent = nullptr);
    ~OptionMenu();

private slots:
    void on_selecionar_clicked();

    void on_confirmarCancelar_accepted();

private:
    Ui::OptionMenu *ui;
};

#endif // OPTIONMENU_H
