/*
    Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
    Copyright 2012-2013 Andrea Brancaleoni <null@gmx.it>

    This program is free software: you can redistribute it and/or modify


qit under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "renderer.h"

#include <QPainter>

#include <QtSvg/QSvgRenderer>
#include <QDebug>
#include <QFile>

Renderer* Renderer::self()
{
    static Renderer instance;
    return &instance;
}

Renderer::Renderer()
{
    m_renderer = new QSvgRenderer();

    if(!loadTheme("simple"))
        qDebug() << "Failed to load any game theme!";
}

Renderer::~Renderer()
{
    delete m_renderer;
}

bool Renderer::loadTheme(const QString& themeName)
{
    QString path = "themes/" + themeName + ".svgz";

    if (themeName == NULL) return false;
    if (!QFile::exists(path)) return false;


    if(!m_currentTheme.isEmpty() && m_currentTheme == themeName) {
        qDebug() << "Notice: not loading the same theme";
        return true; // this is not an error
    }

    m_currentTheme = themeName;


    bool res = m_renderer->load( path );
    if (!res) return false;

    return true;
}

QPixmap Renderer::renderedSvgElement(const QString &id, const QSize &size) const
{
    int const w = size.width();
    int const h = size.height();

    QPixmap pixmap;
    QString cacheName = 
            QString("%1-%2x%3").arg(id).arg(w).arg(h);
    
    qDebug() << "re-rendering pixmap" << cacheName;
    pixmap = QPixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    
    if (!m_renderer->elementExists(id)) {
        qDebug() << "Invalid elementId: " << id;
        return pixmap;
    }
    
    m_renderer->render(&p, id/*, size*/);
    p.end();
    
    return pixmap;
}
