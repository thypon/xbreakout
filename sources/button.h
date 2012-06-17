#ifndef BUTTON_H
#define BUTTON_H

#include "textitems.h"

class Button : public TextItem
{
    Q_OBJECT
public:
    Button(const QString& label, int width = 0, int height = 0);
    virtual ~Button();
    using TextItem::moveTo;
    bool isButtonPressed() { return buttonPressed; }
    int getId() { return id; }
    void setId(int id) { this->id = id; }
signals:
    void pressed();
private:
    int id;
    bool buttonPressed;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    friend class Dialog;
};

#endif // BUTTON_H
