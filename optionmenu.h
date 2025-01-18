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

private:
    Ui::OptionMenu *ui;
};

#endif // OPTIONMENU_H
