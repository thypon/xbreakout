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

#include "canvaswidget.h"
#include "item.h"
#include "gameengine.h"
#include "renderer.h"
#include "globals.h"

#include <QKeyEvent>
#include <QCursor>

#include <QDebug>

CanvasWidget::CanvasWidget(QWidget *parent) 
    : QGraphicsView(parent),
      rightPressed(false),
      leftPressed(false),
      usingKeys(0)
{    
    setScene(new QGraphicsScene());
    setFocus();
    setStyleSheet( "QGraphicsView { border-style: none; }" );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex); //Disable index for better performance
    //setSceneRect(parent->frameGeometry());
    setFrameStyle(QFrame::NoFrame);
    setCacheMode(QGraphicsView::CacheBackground); // Optimize caching
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    scene()->addItem(&background);
    background.show();
    scene()->addItem(&pauseOverlay);
    pauseOverlay.setZValue(1);
    
    moveBarTimer.setInterval(DEFAULT_UPDATE_INTERVAL);
    connect(&moveBarTimer, SIGNAL(timeout()), SLOT(moveBar()));
    
    updateBarTimer.setInterval(DEFAULT_UPDATE_INTERVAL);
    connect(&updateBarTimer, SIGNAL(timeout()), SLOT(updateBar()));
    lastMousePosition = QPoint(0, 0);
    
    updateBarTimer.start();
    setCursor(QCursor(Qt::BlankCursor));
}


void CanvasWidget::moveBar()
{
    if (barDirection != 0) {
        // delay to switch from keys to mouse
        const int DELAY = 20;
        usingKeys = DELAY;
    }

    if (barDirection == 1) emit barMovedRight();
    if (barDirection == -1) emit barMovedLeft();
}

void CanvasWidget::reloadSprites()
{
    QSize size(width(), height());
    QPixmap pixmap = Renderer::self()->renderedSvgElement("Background", size);
    background.setPixmap(pixmap);
    
    // pause overlay
    pixmap = QPixmap(size);
    pixmap.fill(QColor(100, 100, 100, 150));
    pauseOverlay.setPixmap(pixmap);
    
    emit spritesReloaded();
}

void CanvasWidget::handleGamePaused()
{
    releaseMouse();
    updateBarTimer.stop();
    moveBarTimer.stop();
    scene()->setFocusItem(&pauseOverlay);
    pauseOverlay.show();
    setCursor(QCursor(Qt::ArrowCursor));
}

void CanvasWidget::handleGameResumed()
{
    // give feedback
    pauseOverlay.hide();

    // move the mouse cursor to where the bar is
    handleResetMousePosition();

    QCursor newCursor(Qt::BlankCursor);
    newCursor.setPos(cursor().pos());

    grabMouse(newCursor);

    updateBarTimer.start();
}

void CanvasWidget::handleGameEnded()
{
    releaseMouse();
    //updateBarTimer.stop();
    moveBarTimer.stop();
    setCursor(QCursor(Qt::ArrowCursor));
}

void CanvasWidget::resizeEvent(QResizeEvent */*event*/)
{
    qDebug() << "resized!\n";
    reloadSprites();
}

void CanvasWidget::handleResetMousePosition()
{
    int barPosition = GameEngine::bar().center();
    int screenX = qRound(barPosition * Item::scale()) + Item::borderLeft();
    QPoint p = mapToGlobal(QPoint(screenX, 0));
    cursor().setPos(p.x(), cursor().pos().y());
}

void CanvasWidget::updateBar()
{
    QPoint p = mapFromGlobal(cursor().pos());
    if (lastMousePosition == p) {
        return;
    }
    lastMousePosition = p;

    // don't move the mouse if the user was using the keys to play
    // after a while however (when usingKeys == 0) the mouse gets released
    // this avoids accidentally moving the mouse while playing using the keys
    if (usingKeys > 0) {
        --usingKeys;
        if (usingKeys == 0) {
            handleResetMousePosition();
        }
        return;
    }

    // TODO: put scaling somewhere else... (???)
    // convert the screen position to scene position
    int posX = qRound((p.x() - Item::borderLeft()) / Item::scale());
    emit mouseMoved(posX);
}

void CanvasWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QGraphicsView::keyPressEvent(event);
        return;
    }
    int key = event->key();
    switch (key) {
    case Qt::Key_S:
        emit cheatSkipLevel();
        break;
    case Qt::Key_L:
        emit cheatAddLife();
        break;
    case Qt::Key_Right:
        rightPressed = true;
        barDirection = 1;
        break;
    case Qt::Key_Left:
        leftPressed = true;
        barDirection = -1;
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }

    if ((rightPressed || leftPressed) && !moveBarTimer.isActive()) {
        moveBarTimer.start();
    }
}

void CanvasWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QGraphicsView::keyReleaseEvent(event);
        return;
    }
    int key = event->key();
    switch (key) {
    case Qt::Key_Right:
        rightPressed = false;
        break;
    case Qt::Key_Left:
        leftPressed = false;
        break;
    default:
        QGraphicsView::keyReleaseEvent(event);
    }

    if (!rightPressed && !leftPressed) {
        moveBarTimer.stop();
    } else if (rightPressed && !leftPressed) {
        barDirection = 1;
    } else if (!rightPressed && leftPressed) {
        barDirection = -1;
    }
}

void CanvasWidget::focusOutEvent(QFocusEvent *event)
{
    emit focusLost();
    QGraphicsView::focusOutEvent(event);
}
