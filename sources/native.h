#ifndef NATIVE_H
#define NATIVE_H

#include "qabstractmessagebox.h"

class MEssageBox : public NMessageBox
{
public:
    NMessageBox(QWidget *parent = 0, Qt::WFlags flags = 0);
};

#endif // NATIVE_H
