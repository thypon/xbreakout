#include "button.h"

Button::Button(const QString& label, int width, int height) : TextItem()
{
    setText(label);
    this->width = width;
    this->height = height;
    buttonPressed = false;
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    buttonPressed = true;
    emit pressed();
}

Button::~Button() {}
