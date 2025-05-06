#include "ProfileSegment.h"

ProfileSegment::ProfileSegment()
    : m_startTime(0)
    , m_hourlyInsulinBasalRate(0.0f)
    , m_carbRatio(0)
    , m_correctionFactor(0.0f)
    , m_targetGlucose(0.0f)
{
}

ProfileSegment::ProfileSegment(int startTime, float hourlyInsulinBasalRate, int carbRatio, float correctionFactor, float targetGlucose)
    : m_startTime(startTime)
    , m_hourlyInsulinBasalRate(hourlyInsulinBasalRate)
    , m_carbRatio(carbRatio)
    , m_correctionFactor(correctionFactor)
    , m_targetGlucose(targetGlucose)
{
}

int ProfileSegment::getStartTime() const {
    return m_startTime;
}

float ProfileSegment::getHourlyInsulinBasalRate() const {
    return m_hourlyInsulinBasalRate;
}

int ProfileSegment::getCarbRatio() const {
    return m_carbRatio;
}

float ProfileSegment::getCorrectionFactor() const {
    return m_correctionFactor;
}

float ProfileSegment::getTargetGlucose() const {
    return m_targetGlucose;
}

void ProfileSegment::setStartTime(int startTime) {
    m_startTime = startTime;
}

void ProfileSegment::setHourlyInsulinBasalRate(float rate) {
    m_hourlyInsulinBasalRate = rate;
}

void ProfileSegment::setCarbRatio(int ratio) {
    m_carbRatio = ratio;
}

void ProfileSegment::setCorrectionFactor(float factor) {
    m_correctionFactor = factor;
}

void ProfileSegment::setTargetGlucose(float target) {
    m_targetGlucose = target;
}
