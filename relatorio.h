#ifndef RELATORIO_H
#define RELATORIO_H

#include <QDialog>

namespace Ui {
class Relatorio;
}

class Relatorio : public QDialog
{
    Q_OBJECT

public:
    explicit Relatorio(QWidget *parent = nullptr);
    ~Relatorio();

private:
    Ui::Relatorio *ui;
};

#endif // RELATORIO_H
