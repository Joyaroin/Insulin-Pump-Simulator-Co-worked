#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>
#include "abstractpage.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class HomePage;
}
QT_END_NAMESPACE

class HomePage : public AbstractPage
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage();

    void connectToDevice(Device& device) override;

private:
    Ui::HomePage *ui;
    int graphHistoryLengthMins;

    void refreshGraph();


public slots: 
    void onGlucoseChange(float glucose);
    void onTimeChanged(const Time& newTime);
    void onCycleGraphLengthButtonReleased();
};
#endif // HOMEPAGE_H
