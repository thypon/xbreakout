/*
    Copyright 2008 Fela Winkelmolen <fela.kde@gmail.com> 
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

#include "gift.h"

#include "gameengine.h"
#include "brick.h"
#include "ball.h"

#include <QDebug>
#include <cmath>

Gift::Gift(const QString &type) :
    Item(type, GIFT_WIDTH, GIFT_HEIGHT)
{
    int sp = qrand() % (MAXIMUM_GIFT_SPEED - MINIMUM_GIFT_SPEED + 1);
    m_speedFactor = MINIMUM_GIFT_SPEED + sp;
    m_speed = 0;
}

void Gift::move(qreal speed, int updateInterval)
{
    for (int i = 0; i < updateInterval; ++i) {
        step(speed);
    }
}

void Gift::step(qreal speed)
{
    const qreal linearIncreaseFactor = 0.00008 * speed;
    const qreal exponentialIncreaseFactor = 0.002;
    m_speed += m_speedFactor * linearIncreaseFactor;
    m_speed += m_speed * exponentialIncreaseFactor;
    
    moveBy( 0, m_speed);
}

void Gift::startFall(int x, int y)
{
    moveTo(x, y);
    repaint();
    show();
}

void Gift::execute(GameEngine *gameEngine)
{
    m_game = gameEngine;
    m_game->addScore(GIFT_SCORE);
    
    if (identifier() == "Gift100Points") {
        m_game->addScore(100 - GIFT_SCORE);
    } 
    else if (identifier() == "Gift200Points") {
        m_game->addScore(200 - GIFT_SCORE);
    } 
    else if (identifier() == "Gift500Points") {
        m_game->addScore(500 - GIFT_SCORE);
    } 
    else if (identifier() == "Gift1000Points") {
        m_game->addScore(1000 - GIFT_SCORE);
    } 
    else if (identifier() == "GiftAddLife") {
        if (m_game->m_lives.count() < MAXIMUM_LIVES) {
            m_game->m_lives.append(new Life);
        }
    } 
    else if (identifier() == "GiftLoseLife") {
        m_game->handleDeath();
    } 
    else if (identifier() == "GiftNextLevel") {
        giftNextLevel();
    }
    else if (identifier() == "GiftMagicEye") {
        giftMagicEye();
    }
    else if (identifier() == "GiftMagicWand") {
        giftMagicWand();
    }
    else if (identifier() == "GiftSplitBall") {
        giftSplitBall();
    }
    else if (identifier() == "GiftAddBall") {
        m_game->m_balls.append(new Ball);
    } 
    else if (identifier() == "GiftUnstoppableBall") {
        giftUnstoppableBall();
    }
    else if (identifier() == "GiftBurningBall") {
        giftBurningBall();
    }
    else if (identifier() == "GiftDecreaseSpeed") {
        m_game->changeSpeed(1.0/CHANGE_SPEED_RATIO);
    }
    else if (identifier() == "GiftIncreaseSpeed") {
        m_game->changeSpeed(CHANGE_SPEED_RATIO);
    }
    else if (identifier() == "GiftEnlargeBar") {
        Bar *bar = &m_game->m_bar;
        bar->enlarge();
        m_game->moveBar(bar->position().x() + bar->getRect().width()/2);
    }
    else if (identifier() == "GiftShrinkBar") {
        m_game->m_bar.shrink();
    }
    else if (identifier() == "GiftStickyBar") {
        m_game->m_bar.setIdentifier("StickyBar");
    }
    else if (identifier() == "GiftMoreExplosion") {
        giftMoreExplosion();
    }
    else {
        qDebug() << "Unrecognized gift type!!!" << identifier();
    }
}

void Gift::giftNextLevel()
{
    m_game->loadNextLevel();
}

void Gift::giftMagicEye()
{
    // make all hidden bricks visible
    foreach (Brick *brick, m_game->m_bricks) {
        if (!brick->isDeleted() && !brick->isVisible()) {
            brick->show();
            ++m_game->m_remainingBricks;
        }
    }
}

void Gift::giftMagicWand()
{
    foreach (Brick *brick, m_game->m_bricks) {
        // make Unbreakbable Bricks Breakable
        if (!brick->isDeleted() && brick->identifier() == "UnbreakableBrick") {
            brick->setIdentifier("BreakableBrick");
            ++m_game->m_remainingBricks;
            qDebug() << m_game->m_remainingBricks;
        }
        
        // Make Multiple Bricks single
        if (brick->identifier() == "MultipleBrick3") {
            brick->setIdentifier("MultipleBrick1");
            m_game->addScore(AUTOBRICK_SCORE * 2);
        }
        if (brick->identifier() == "MultipleBrick2") {
            brick->setIdentifier("MultipleBrick1");
            m_game->addScore(AUTOBRICK_SCORE);
        }
    }
}

void Gift::giftSplitBall()
{
    // TODO: better copy (type, speed, etc...)
    QList<Ball *> newBalls;
    foreach (Ball *ball, m_game->m_balls) {
        Ball *newBall = new Ball;
        // give it a nice direction...
        newBall->directionX = ball->directionX;
        newBall->directionY = ball->directionY;
        if (ball->directionY > 0)
            newBall->directionY *= -1;
        else
            newBall->directionX *= -1;
        
        newBall->toBeFired = ball->toBeFired;
        newBall->setIdentifier(ball->identifier());
        newBall->moveTo(ball->position());
        newBalls.append(newBall);
    }
    m_game->m_balls += newBalls;
}

void Gift::giftUnstoppableBall()
{
    foreach (Ball *ball, m_game->m_balls) {
        if (ball->identifier() == "BurningBall") {
            ball->setIdentifier("UnstoppableBurningBall");
        } else if (ball->identifier() != "UnstoppableBurningBall") {
            ball->setIdentifier("UnstoppableBall");
        }
    }
}

void Gift::giftBurningBall()
{
    foreach (Ball *ball, m_game->m_balls) {
        if (ball->identifier() == "UnstoppableBall") {
            ball->setIdentifier("UnstoppableBurningBall");
        } else if (ball->identifier() != "UnstoppableBurningBall") {
            ball->setIdentifier("BurningBall");
        }
    }
}

void Gift::giftMoreExplosion()
{
    QList<Brick *> explodingBricks;
    foreach (Brick *brick, m_game->m_bricks) {
        if (!brick->isDeleted() && brick->identifier() == "ExplodingBrick") {
            explodingBricks.append(brick);
        }
    }
    
    foreach (Brick *brick, explodingBricks) {
        foreach (Brick *nearbyBrick, brick->nearbyBricks()) {
            if (nearbyBrick->identifier() == "UnbreakableBrick") {
                ++m_game->m_remainingBricks;
            }
            if (nearbyBrick->identifier() == "HiddenBrick" && 
                    !nearbyBrick->isVisible()) {
                nearbyBrick->show();
                ++m_game->m_remainingBricks;
            }
            
            nearbyBrick->setIdentifier("ExplodingBrick");
        }
    }
}
