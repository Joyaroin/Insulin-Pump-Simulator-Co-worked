#ifndef SLEEPSCREEN_H
#define SLEEPSCREEN_H

#include <QWidget>
#include "abstractpage.h"

namespace Ui {
class SleepScreen;
}

class SleepScreen : public AbstractPage
{
    Q_OBJECT

public:
    explicit SleepScreen(QWidget *parent = nullptr);
    ~SleepScreen();

private:
    Ui::SleepScreen *ui;
    bool checkPin();
    void updateUI();
};

#endif // SLEEPSCREEN_H
