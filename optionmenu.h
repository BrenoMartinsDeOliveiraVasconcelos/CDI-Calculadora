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
