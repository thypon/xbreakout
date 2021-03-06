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

#include "canvasitems.h"
#include "globals.h"

Bar::Bar() :
    Item("PlainBar", DEFAULT_BAR_WIDTH, BRICK_HEIGHT)
{
    moveTo(300, (HEIGHT-1)*BRICK_HEIGHT);

    //loadSprite();
}

void Bar::reset()
{
    int oldWidth = width;
    width = DEFAULT_BAR_WIDTH;
    setIdentifier("PlainBar");
    loadSprite();
    
    // move the bar to retain it's center
    int d = (oldWidth - width)/2;
    
    moveBy(d, 0);
}

void Bar::enlarge()
{
    int oldWidth = width;
    width = qRound((double)width*RESIZE_BAR_RATIO);
    if (width > MAX_BAR_WIDTH) {
        width = MAX_BAR_WIDTH;
    }
    loadSprite();
    
    // move the bar to retain it's center
    int d = (oldWidth - width)/2;
    
    moveBy(d, 0);
}

void Bar::shrink()
{
    int oldWidth = width;
    width = qRound((double)width/RESIZE_BAR_RATIO);
    if (width < MIN_BAR_WIDTH) {
        width = MIN_BAR_WIDTH;
    }
    loadSprite();
    
    // move the bar to retain it's center
    int d = (oldWidth - width)/2;
    
    moveBy(d, 0);
}

int Bar::center() const
{
    return getRect().left() + getRect().width()/2;
}

int Life::count = 0;

Life::Life() :
    Item("PlainBar", qRound(BRICK_WIDTH/1.3), qRound(BRICK_HEIGHT/1.3))
{
    ++count;
    
    int x = -5 + qRound((double) BRICK_WIDTH * WIDTH 
                        - (BRICK_WIDTH * count) );
    int y = qRound((double) -1.2 * BRICK_HEIGHT);
     
    moveTo(x, y);
    
    loadSprite();
}

Life::~Life()
{
    --count;
}

Background::Background() :
    Item("BackgroundOverlay", BRICK_WIDTH * WIDTH, BRICK_HEIGHT * HEIGHT)
{
    //qreal border = BACKGROUND_BORDER * w;
    // BORDER_RATIO is to make it a little bit bigger...
    // TODO: not sure (souldn't it be 2??? but this seems to work better...)
    width = width + 1;
    height = height + 1;
    moveTo(-1, -1);
        
    loadSprite();
}
