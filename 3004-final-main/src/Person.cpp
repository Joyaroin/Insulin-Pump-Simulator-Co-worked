#include "Person.h"
#include <QDebug>

Person::Person()
    : QObject(nullptr)
    , m_bloodGlucoseLevel(PERSON_INITIAL_BLOOD_GLUCOSE)
{
    if (m_bloodGlucoseLevel < 0) {
        m_bloodGlucoseLevel = 0;
    }
}

float Person::bloodGlucoseLevel() const
{
    return m_bloodGlucoseLevel;
}

void Person::setBloodGlucoseLevel(float bg)
{
    if (bg < 0) {
        bg = 0;
    }
    if (m_bloodGlucoseLevel == bg)
        return;

    m_bloodGlucoseLevel = bg;
    qInfo("[Person]: BG set to %f.", m_bloodGlucoseLevel);
    emit bloodGlucoseLevelChanged(m_bloodGlucoseLevel);
}

void Person::eatCarbs(int grams)
{
    // Very rough example: + (grams / 2) BG
    setBloodGlucoseLevel(m_bloodGlucoseLevel + grams / 2);
}

void Person::absorbInsulin(int units)
{
    // Use the same logic as receiveInsulin
    receiveInsulin(static_cast<float>(units));
}

void Person::receiveInsulin(float units)
{
    // Simple model: each unit of insulin reduces blood glucose by 25 mg/dL
    float newGlucose = m_bloodGlucoseLevel - (units * 25.0f);
    setBloodGlucoseLevel(newGlucose);
    qInfo("[Person] Received %.2f units of insulin", units);
}


