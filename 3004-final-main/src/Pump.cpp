#include "Pump.h"
#include <QDebug>

Pump::Pump(QObject* parent)
    : QObject(parent),
    m_isWorking(true)
{
}

void Pump::setWorking(bool working)
{
    if (working){
        qDebug() << "[Pump] Pump normal operation";
    }
    else{
        qDebug() << "[Pump] Pump malfunction";
    }

    m_isWorking = working;
    emit isWorkingChanged(m_isWorking);
}

bool Pump::isWorking() const
{
    return m_isWorking;
}

void Pump::activate()
{
    if (!m_isWorking) {
        qDebug() << "[Pump] Cannot activate: pump is not working";
        return;
    }
    qDebug() << "[Pump] Activating for insulin delivery";
}

