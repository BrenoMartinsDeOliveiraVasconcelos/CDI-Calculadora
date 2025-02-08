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

#include "calculadora.h"

#include <applicationclass.h>
#include <configmanager.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setStyle("fusion");

    runtimeConsts consts;

    int returnCode = 0;

    // Para reiniciar
    do{
        QApplication a(argc, argv);
        Calculadora w;
        w.show();
        returnCode = a.exec();
    }while (returnCode == consts.restartCode());

    return returnCode;
}
