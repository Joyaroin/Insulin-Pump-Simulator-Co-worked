/**
 * @class Pump - pretty basic, has working/error state
 */

#ifndef PUMP_H
#define PUMP_H

#include <QObject>

class Pump : public QObject
{
    Q_OBJECT

public:
    explicit Pump(QObject* parent = nullptr);
    bool isWorking() const;
    void activate();  // Activates the pump for insulin delivery

public slots:
    void setWorking(bool working);

signals:
    void isWorkingChanged(bool newStatus);

private:
    bool m_isWorking;
};

#endif // PUMP_H
