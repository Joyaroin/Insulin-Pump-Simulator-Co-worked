#ifndef MYPUMP1_H
#define MYPUMP1_H

#include "abstractpage.h"

namespace Ui {
class MyPump1;
}

class MyPump1 : public AbstractPage
{
    Q_OBJECT

public:
    explicit MyPump1(QWidget *parent = nullptr);
    ~MyPump1();

private:
    Ui::MyPump1 *ui;
};

#endif // MYPUMP1_H
