/*
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
    Copyright 2012-2013 Andrea Brancaleoni <null@gmx.it>
  
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QApplication>

#include "mainwindow.h"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow* window = new MainWindow();
    Q_UNUSED(window);
    
    return app.exec();
}
