#include "Cartridge.h"

Cartridge::Cartridge(QObject* parent)
    : QObject(parent)
    , m_insulinUnits(300)
    , m_maxCapacity(300)
{
}

int Cartridge::getInsulinUnits() const
{
    return m_insulinUnits;
}

int Cartridge::getMaxCapacity() const
{
    return m_maxCapacity;
}

void Cartridge::setInsulinUnits(int units)
{
    if (units < 0) units = 0;
    if (units > m_maxCapacity) units = m_maxCapacity;

    m_insulinUnits = units;
    emit insulinUnitsChanged(m_insulinUnits);
}

void Cartridge::setMaxCapacity(int capacity)
{
    if (capacity < 0) capacity = 0;
    if (m_maxCapacity == capacity)
        return;

    m_maxCapacity = capacity;
    if (m_insulinUnits > m_maxCapacity) {
        m_insulinUnits = m_maxCapacity;
        emit insulinUnitsChanged(m_insulinUnits);
    }
    emit maxCapacityChanged(m_maxCapacity);
}

bool Cartridge::hasEnoughInsulin(float units) const
{
    return m_insulinUnits >= units;
}

void Cartridge::removeInsulin(float units)
{
    if (units <= 0) return;

    float newAmount = m_insulinUnits - units;
    if (newAmount < 0) newAmount = 0;

    setInsulinUnits(static_cast<int>(newAmount));
}

