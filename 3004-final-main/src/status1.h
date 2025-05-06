#ifndef STATUS1_H
#define STATUS1_H

#include "abstractpage.h"

namespace Ui {
class Status1;
}

class Status1 : public AbstractPage
{
    Q_OBJECT

public:
    explicit Status1(QWidget *parent = nullptr);
    ~Status1();

private:
    Ui::Status1 *ui;
};

#endif // STATUS1_H
