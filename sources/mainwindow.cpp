/*
    Copyright 2007-2009 Fela Winkelmolen <fela.kde@gmail.com> 
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

#include "mainwindow.h"
#include "gameengine.h"
#include "canvasitems.h"
#include "canvaswidget.h"
#include "renderer.h"
#include "settings.h"

#include <QGraphicsScene>
#include <QPointer>
#include <QMessageBox>
#include <QIcon>
#include <QAction>
#include <QTime>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      canvasWidget(new CanvasWidget(this)),
      gameIsPaused(true)
{
    Item::setCanvas(canvasWidget);
    new Background; // the background put's itself into the canvasWidget
    gameEngine = new GameEngine(this); // must be called after Item::setCanvas()
    
    connect(canvasWidget, SIGNAL(mouseMoved(int)),
            gameEngine, SLOT(moveBar(int)));
    connect(canvasWidget, SIGNAL(barMovedLeft()),
            gameEngine, SLOT(moveBarLeft()));
    connect(canvasWidget, SIGNAL(barMovedRight()),
            gameEngine, SLOT(moveBarRight()));
    connect(canvasWidget, SIGNAL(focusLost()),
            this, SLOT(pauseGame()));
    
    connect(gameEngine, SIGNAL(gamePaused()), 
            canvasWidget, SLOT(handleGamePaused()));
    connect(gameEngine, SIGNAL(gameResumed()),
            canvasWidget, SLOT(handleGameResumed()));
    connect(gameEngine, SIGNAL(gameResumed()),
            this, SLOT(handleGameResumed()));
    connect(gameEngine, SIGNAL(resetMousePosition()),
            canvasWidget, SLOT(handleResetMousePosition()));
    connect(gameEngine, SIGNAL(gameEnded(int,int,int)), 
            SLOT(handleEndedGame(int,int,int)));
    
    // cheating keys, debugging and testing only TODO: REMOVE
    connect(canvasWidget, SIGNAL(cheatSkipLevel()),
            gameEngine, SLOT(cheatSkipLevel()));
    connect(canvasWidget, SIGNAL(cheatAddLife()),
            gameEngine, SLOT(cheatAddLife()));

    connect(gameEngine, SIGNAL(levelChanged()),
            this, SLOT(handleLevelChanged()));

    connect(Settings::self(), SIGNAL(themeChanged()),
            canvasWidget, SLOT(reloadSprites()));
    
    setCentralWidget(canvasWidget);
    
    setupActions();
    setFocusProxy(canvasWidget);
    
    QSize defaultSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    // show here (instead of in main) else the mouse can't be grabbed
    show();
    gameEngine->start(Settings::self()->getLevelset());
}
 
MainWindow::~MainWindow()
{
    delete gameEngine;
    delete canvasWidget;
}
 
void MainWindow::setupActions()
{
    QAction *newGame = new QAction(this);
    newGame->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(newGame, SIGNAL(triggered()), this, SLOT(startNewGame()));
    this->addAction(newGame);
    
    QAction *quit = new QAction(this);
    quit->setShortcut(Qt::CTRL + Qt::Key_C);
    connect(quit, SIGNAL(triggered()), this, SLOT(close()));
    this->addAction(quit);

    QAction *fireAction = new QAction(this);
    fireAction->setShortcut(Qt::Key_Space);
    connect(fireAction, SIGNAL(triggered()), this, SLOT(fire()));
    this->addAction(fireAction);

    pauseAction = new QAction(this);
    QList<QKeySequence> keys;
    keys.append(Qt::Key_Escape);
    keys.append(Qt::Key_Pause);
    pauseAction->setShortcuts(keys);
    connect(pauseAction, SIGNAL(triggered()), this, SLOT(toggleGamePaused()));
    this->addAction(pauseAction);
}

void MainWindow::startNewGame()
{
    int ret = QMessageBox::warning(
            this,
            "New Game",
            "Starting a new game will end the current one!",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );
        
    if (ret == QMessageBox::Yes) {
        gameEngine->start(Settings::self()->getLevelset());
    }
}

void MainWindow::pauseGame()
{
    if (!gameIsPaused) {
        pauseAction->activate(QAction::Trigger);
    }
}

void MainWindow::toggleGamePaused() {
    gameIsPaused = !gameIsPaused;
    gameEngine->setGamePaused(gameIsPaused);
}

void MainWindow::handleGameResumed()
{
    gameIsPaused = false;
    gameEngine->setGamePaused(gameIsPaused);
    canvasWidget->reloadSprites();
}

void MainWindow::fire()
{
    if (gameEngine->gameIsPaused()) {
        pauseAction->activate(QAction::Trigger);
    } else {
        gameEngine->fire();
    }
}

void MainWindow::handleLevelChanged() {
    canvasWidget->reloadSprites();
}

void MainWindow::handleEndedGame(int score, int level, int time)
{
    
    QTime t = QTime(0, 0).addSecs(time);
    // TODO: check int overflow and fix 24 hours "overflow"
    QString timeString = t.toString("HH:mm");
    
    canvasWidget->handleGameEnded();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (gameEngine->gameIsPaused()) {
        pauseAction->activate(QAction::Trigger);
        QMainWindow::mousePressEvent(event);
        return;
    }

    gameEngine->fire();
    QMainWindow::mousePressEvent(event);
}

