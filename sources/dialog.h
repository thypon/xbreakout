#ifndef DIALOG_H
#define DIALOG_H

#include "textitems.h"
#include "button.h"
#include <QEventLoop>

class Dialog : public TextItem
{
public:
    enum {
        NO,
        YES
    };
    Dialog(const QString& label);
    int response();
private:
    Button yesButton;
    Button noButton;
    QEventLoop loop;
};

#endif // DIALOG_H
