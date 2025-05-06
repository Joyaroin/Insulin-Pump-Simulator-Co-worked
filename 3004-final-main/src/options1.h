#ifndef OPTIONS1_H
#define OPTIONS1_H

#include "abstractpage.h"

namespace Ui {
class Options1;
}

class Options1 : public AbstractPage
{
    Q_OBJECT

public:
    explicit Options1(QWidget *parent = nullptr);
    ~Options1();

private:
    Ui::Options1 *ui;
};

#endif // OPTIONS1_H
