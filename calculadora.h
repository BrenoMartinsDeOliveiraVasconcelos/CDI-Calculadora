#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class Calculadora;
}
QT_END_NAMESPACE

class Calculadora : public QMainWindow
{
    Q_OBJECT

public:


    Calculadora(QWidget *parent = nullptr);
    ~Calculadora();

private slots:
    void on_caciar_clicked();

    void on_Calculadora_destroyed();

    void on_estimarValores_clicked();

    void on_selecionarDiretorio_clicked();

    void on_configs_clicked();

private:
    Ui::Calculadora *ui;
};

#endif // CALCULADORA_H
