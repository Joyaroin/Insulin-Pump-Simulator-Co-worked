#include "CGM.h"
#include "Person.h"
#include <QDebug> // or <iostream>

CGM::CGM(std::shared_ptr<Person> person, QObject* parent)
    : QObject(parent)
    , m_person(person)
    , m_currentReading(-1)
{
    QObject::connect(m_person.get(), &Person::bloodGlucoseLevelChanged, this, &CGM::onGlucoseReadingChange);
}

float CGM::getReading() const
{
    return m_currentReading;
}

void CGM::measure()
{
    if (!m_person) {
        return;
    }
    float bg = m_person->bloodGlucoseLevel();
    if (bg != m_currentReading) {
        m_currentReading = bg;
        emit glucoseReadingChange(m_currentReading);
    }
}

void CGM::onGlucoseReadingChange(float reading)
{
    qDebug() << "[CGM] Sending reading signal:" << reading;
    emit glucoseReadingChange(reading);
}
