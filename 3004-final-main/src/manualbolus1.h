#ifndef MANUALBOLUS1_H
#define MANUALBOLUS1_H

#include <QWidget>
#include "abstractpage.h"

namespace Ui {
class ManualBolus1;
}

class ManualBolus1 : public AbstractPage
{
    Q_OBJECT

public:
    explicit ManualBolus1(QWidget *parent = nullptr);
    ~ManualBolus1();
    
    void connectToDevice(Device& device) override;

private:
    Ui::ManualBolus1 *ui;

    void makeConnections();

private slots:
    void onSliderMoved(int pct);
    void onApplyBolusButtonReleased();
    void validateInputs();
    void onFetchBloodGlucoseButtonReleased();
};

#endif // MANUALBOLUS1_H
