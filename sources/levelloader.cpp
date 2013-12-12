/*
    Copyright 2011 Julian Helfferich <julian.helfferich@gmail.com> 
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

#include "levelloader.h"
#include "brick.h"
#include "gift.h"

#include <QtXml/QDomDocument>
#include <QFile>

#include <QDebug>

LevelLoader::LevelLoader(GameEngine *gameEngine)
  : m_game( gameEngine )
{
    m_levelname = QString();
    m_level = 0;
    m_levelset = 0;
}

LevelLoader::~LevelLoader()
{
    delete m_levelset;
}

int LevelLoader::level() const
{
    return m_level;
}

void LevelLoader::setLevel(int level)
{
    m_level = level;
}

QString LevelLoader::levelset() const
{
    return m_levelname;
}

void LevelLoader::setLevelset(const QString& levelname)
{
    if( levelname == m_levelname ){
        return;
    }
    m_levelname = levelname;

    // Loading document model
    // Locating the path in the filesystem
    QString path = ":/levelsets/" + m_levelname + ".levelset";
    // --
    
    delete m_levelset;
    
    m_levelset = new QDomDocument( m_levelname );
    QFile file( path );
    if( !file.open( QIODevice::ReadOnly ) ){
        qDebug() << "Can't open file " << path << endl;
    }
  
    QString errorString;
    int errorLine;
    int errorColumn;
    m_levelset->setContent( &file, false, &errorString, &errorLine, &errorColumn );
    file.close();
    // --
}

void LevelLoader::loadLevel(QList< Brick* >& bricks)
{   
    // Selecting the correct level
    m_level++;
    
    if( m_levelset == 0 ){
        qDebug() << "Error: No levelset specified" << endl;
        return;
    }
    
    QDomElement levels = m_levelset->documentElement();
    QDomNode node = levels.firstChild();
    for( int i = 1; i < m_level; i++ ){
        node = node.nextSibling();
    }
    // --
    
    // Load level information
    if( node.isNull() || node.toElement().tagName() != "Level" ){
        // Level not found or no more levels
        return;
    }
    
    QDomAttr attribute;
    QDomElement level = node.toElement();
    if( level.isNull() ){
        qDebug() << "Invalid Levelset " << m_levelname << ": Can't read level information";
    }
    attribute = level.attributeNode("Name");
    QString levelName;
    if( !attribute.isNull() ){
        levelName = level.attributeNode("Name").value();
    }
    node = node.firstChild();
    // --
    
    // Load bricks and gifts
    m_lineNumber = 0;
    while( !node.isNull() ){
        QDomElement info = node.toElement();
        if( info.isNull() ){ qDebug() << "Invalid levelset " << m_levelname << ": Can't read level information."; }
            
        if( info.tagName() == "Line" ){
            // Load one line of bricks
            loadLine( info, bricks );
        } else if( info.tagName() == "Gift" ){
            // Load one gift type
            loadGift( info, bricks );
        } else {
            qDebug() << "Invalid tag name " << info.tagName() << " has occured in level "
                     << levelName << " in levelset " << m_levelname << endl;
        }
        
        node = node.nextSibling();
    }
}

void LevelLoader::loadLine(QDomElement lineNode, QList< Brick* >& bricks)
{
    // Reading the line number
    QDomAttr attribute = lineNode.attributeNode("Number");
    QDomElement attributeNode = lineNode.firstChildElement("Number");
    if( !attribute.isNull() ){
        m_lineNumber = attribute.value().toInt();
    } else if( !attributeNode.isNull() ) {
        m_lineNumber = attributeNode.text().toInt();
    } else {
        // Standard line numbering: load next line
        m_lineNumber++;
    }
    
    // Reading the brick information
    attribute = lineNode.attributeNode("Bricks");
    attributeNode = lineNode.firstChildElement("Bricks");
    QString line;
    if( !attribute.isNull() ){
        line = attribute.value();
    } else if( !attributeNode.isNull() ) {
        line = attributeNode.text();
    } else {
        line = lineNode.text();
    }

    if( line.size() > WIDTH ){
        qDebug() << "Invalid levelset " << m_levelname << ": too many bricks in line "
                 << m_lineNumber << endl;
    }
    
    // Convert line information to bricks
    for( int x = 0; x < line.size(); x++ ){
        char charType = line[x].toLatin1();
        if (charType != '-') {
            bricks.append( new Brick(m_game, getTypeFromChar(charType), x+1, m_lineNumber) );
        }
    }
}

void LevelLoader::loadGift(QDomElement giftNode, QList< Brick* >& bricks)
{
    // Build list of bricks without a gift
    QList<Brick *> bricksLeft = bricks;
    QMutableListIterator<Brick *> i(bricksLeft);
    while (i.hasNext()) {
        Brick *brick = i.next();
        if (brick->identifier() == "UnbreakableBrick" || brick->hasGift() ){
            i.remove();
        }
    }

    bool nodeTextRead = false;
    // Reading the brick type
    QDomAttr attribute = giftNode.attributeNode("Type");
    QDomElement attributeNode = giftNode.firstChildElement("Type");
    QString giftType;
    if( !attribute.isNull() ){
        giftType = attribute.value();
    } else if( !attributeNode.isNull() ){
        giftType = attributeNode.text();
        nodeTextRead = true;
    } else {
        giftType = giftNode.text();
        nodeTextRead = true;
    }

    // Reading number of gifts to be distributed. If not specified one gift is placed.
    attribute = giftNode.attributeNode("Count");
    attributeNode = giftNode.firstChildElement("Count");
    int times = 1;
    bool ok = true;
    if( !attribute.isNull() ){
        times = attribute.value().toInt( &ok );
    } else if( !attributeNode.isNull() ){
        times = attributeNode.text().toInt( &ok );
        nodeTextRead = true;
    } else if( !nodeTextRead ){
        times = giftNode.text().toInt( &ok );
        if( !ok ){ times = 1; }
    }
    
    if( bricksLeft.count() < times ){
        qDebug() << "Invalid levelset " << m_levelname << ": In Level " << m_level
                 << " are too many gifts of type " << giftType << endl;
    }
    
    // If only one brick to be placed: see if position is given
    QPoint position;
    if( times == 1 ){
        attribute = giftNode.attributeNode("Position");
        attributeNode = giftNode.firstChildElement("Position");
        if( !attribute.isNull() ){
            position = positionFromString( attribute.value() );
        } else if( !attributeNode.isNull() ){
            position = positionFromString( attributeNode.text() );
            nodeTextRead = true;
        } else if( !nodeTextRead && giftNode.text().contains(',') ){
            position = positionFromString( giftNode.text() );
            nodeTextRead = true;
        }
    }
        
    if( !position.isNull() ){
        // Put gift at given position
        Brick *giftBrick = brickAt( position, bricks ); 
        if( giftBrick == 0 ){
            qDebug() << "Invalid levelset " << m_levelname << ": Can't place gift at position ("
                     << position.x() << "," << position.y() << "). There is no brick.\n";
        } else {
            if( giftBrick->hasGift() ){
                // Brick already has a gift -> move this gift to a random position
                int index = qrand() % bricksLeft.count();
                bricksLeft.at(index)->setGift( giftBrick->gift() );
            }
            Gift *newgift = new Gift(giftType);
            newgift->hide();
            giftBrick->setGift(newgift);
        }
    } else {
        // Distribute gifts randomly
        for( int i = 0; i < times; i++ ){
            Gift *gift = new Gift(giftType);
            gift->hide();
            
            int index = qrand() % bricksLeft.count();
            bricksLeft.at(index)->setGift(gift);
            bricksLeft.removeAt(index);
        }
    }
}

QString LevelLoader::getTypeFromChar(char type) 
{
    switch (type) {
    case '1': return "PlainBrick1";
    case '2': return "PlainBrick2";
    case '3': return "PlainBrick3";
    case '4': return "PlainBrick4";
    case '5': return "PlainBrick5";
    case '6': return "PlainBrick6";
    case 'm': return "MultipleBrick3";
    case 'x': return "ExplodingBrick";
    case 'u': return "UnbreakableBrick";
    case 'h': return "HiddenBrick";
    default:
        qDebug() << "Invalid File: unknown character '" 
                    << type << "'\n";
        return "PlainBrick1";
    }
}

Brick *LevelLoader::brickAt( const QPoint& position, QList<Brick *> &bricks )
{
    Brick *result = 0;
    foreach( Brick *testbrick, bricks ){
        if( testbrick->position().x() / BRICK_WIDTH + 1 == position.x()
            && testbrick->position().y() / BRICK_HEIGHT + 1 == position.y() ){
            result = testbrick;
            break;
        }
    }
    return result;
}

QPoint LevelLoader::positionFromString(const QString& posString)
{
    int seperatorPosition = posString.indexOf(',');
    int length = posString.length();
    return QPoint( posString.left(seperatorPosition).toInt(), posString.right(length-seperatorPosition-1).toInt() );
}
