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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameEngine;
class CanvasWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

private slots:
    void handleEndedGame(int score, int level, int time);
    void startNewGame();
    void fire();
    void close();
    // calls the action
    void pauseGame();
    void toggleGamePaused();
    // assures the action not checked anymore
    void handleGameResumed();
    void handleLevelChanged();
    
private:
    void setupActions();
    bool thereIsAnotherDialog;
    int dialog(const QString& label);

    QAction *pauseAction;
    
    bool gameIsPaused;
    CanvasWidget *canvasWidget;
    GameEngine *gameEngine;
    
protected:
    bool event(QEvent *event);
};
 
#endif
