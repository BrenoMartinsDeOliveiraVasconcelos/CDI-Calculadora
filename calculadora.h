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

    void on_resumo_clicked();

private:
    Ui::Calculadora *ui;
};

#endif // CALCULADORA_H
