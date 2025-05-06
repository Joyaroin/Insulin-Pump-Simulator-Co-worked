#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <QObject>

class Cartridge : public QObject
{
    Q_OBJECT

public:
    explicit Cartridge(QObject* parent = nullptr);
    int getInsulinUnits() const;
    int getMaxCapacity() const;
    void setInsulinUnits(int units);
    void setMaxCapacity(int capacity);

    // New methods for insulin management
    bool hasEnoughInsulin(float units) const;
    void removeInsulin(float units);
signals:
    void insulinUnitsChanged(int newUnits);
    void maxCapacityChanged(int newCapacity);

private:
    int m_insulinUnits;
    int m_maxCapacity;
};

#endif // CARTRIDGE_H
