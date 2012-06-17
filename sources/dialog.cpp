#include "dialog.h"
#include <QEventLoop>

Dialog::Dialog(const QString& label) :
    TextItem(),
    yesButton("Yes"),
    noButton("No")
{
    width = BRICK_WIDTH * WIDTH * 8 / 10;
    height = BRICK_HEIGHT * 3;

    int x = (BRICK_WIDTH * WIDTH - width) / 2;
    int y = BRICK_HEIGHT * HEIGHT / 2 - height - BRICK_HEIGHT / 4;
    moveTo(x, y);

    setText(label);

    y += height + BRICK_HEIGHT / 2;
    yesButton.width = width / 2 - BRICK_HEIGHT / 4;
    yesButton.height = height;
    yesButton.setId(YES);
    yesButton.moveTo(x, y);

    x = WIDTH * BRICK_WIDTH / 2 + BRICK_HEIGHT / 4;
    noButton.width = width / 2 - BRICK_HEIGHT / 4;
    noButton.height = height;
    noButton.setId(NO);
    noButton.moveTo(x, y);

    connect(&yesButton, SIGNAL(pressed()), &loop, SLOT(quit()));
    connect(&noButton, SIGNAL(pressed()), &loop, SLOT(quit()));
}

int Dialog::response() {
    loop.exec();

    if (yesButton.isButtonPressed()) {
        return yesButton.getId();
    }

    if (noButton.isButtonPressed()) {
        return noButton.getId();
    }

    return -1;
}
