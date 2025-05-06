#include "Bolus.h"

#include "defs.h"

Bolus::Bolus(Type type, float totalInsulinDosage, int deliveryDurationMins,
           const Time& currentTime, const std::string& reason)
    : m_type(type)
    , m_totalDosage(totalInsulinDosage)
    , m_insulinPerMinute(totalInsulinDosage / deliveryDurationMins)
    , m_endTime(Time(currentTime.getTotalMinutes() + deliveryDurationMins))
    , m_reason(reason)
{}

Bolus::Bolus(Type type, float totalInsulinDosage, int deliveryDurationHours, int deliveryDurationMins,
           const Time& currentTime, const std::string& reason)
    : m_type(type)
    , m_totalDosage(totalInsulinDosage)
    , m_insulinPerMinute(totalInsulinDosage / (deliveryDurationMins + (MINS_PER_HOUR * deliveryDurationHours)))
    , m_endTime(Time(currentTime.getTotalMinutes() + (deliveryDurationMins + (MINS_PER_HOUR * deliveryDurationHours))))
    , m_reason(reason)
{}

float Bolus::getDoseForTimestep(const Time& currentTime)
{
    if (currentTime >= m_endTime)
    {
        emit isFinished(this);
        return 0;
    }
    else
    {
        return m_insulinPerMinute * SIMULATED_MINS_PER_TIMESTEP;
    }
}

bool Bolus::isActive(const Time& currentTime) const
{
    return currentTime < m_endTime;
}

