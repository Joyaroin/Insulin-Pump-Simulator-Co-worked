#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

class Battery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int charge READ getCharge WRITE setCharge NOTIFY chargeChanged)

public:
    explicit Battery(QObject* parent = nullptr);
    Battery(int initialCharge, QObject* parent = nullptr);

    int  getCharge() const;
    bool isDead() const;
    bool isLow() const;

public slots:
    void setCharge(int newCharge);
    void drainBattery(int amount);

signals:
    void chargeChanged(int newCharge);
    void batteryLow();
    void batteryDead();

private:
    int m_charge; // 0..100
};

#endif // BATTERY_H
