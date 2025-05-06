#include "Battery.h"

Battery::Battery(QObject* parent)
    : QObject(parent)
    , m_charge(100) // default to 100%
{
}

Battery::Battery(int initialCharge, QObject* parent)
    : QObject(parent)
    , m_charge(initialCharge)
{
    if (m_charge < 0)   m_charge = 0;
    if (m_charge > 100) m_charge = 100;
}

int Battery::getCharge() const
{
    return m_charge;
}

bool Battery::isDead() const
{
    return (m_charge <= 0);
}

bool Battery::isLow() const
{
    return (m_charge <= 20);
}

void Battery::setCharge(int newCharge)
{
    if (newCharge < 0)   newCharge = 0;
    if (newCharge > 100) newCharge = 100;

    if (m_charge == newCharge)
        return;

    m_charge = newCharge;
    emit chargeChanged(m_charge);

    if (isDead()) {
        emit batteryDead();
    } else if (isLow()) {
        emit batteryLow();
    }
}

void Battery::drainBattery(int amount)
{
    if (amount <= 0) return;
    setCharge(m_charge - amount);
}


