#include "Profile.h"

#include "defs.h"

#include <algorithm>

Profile::Profile() : size(0) {
    // init base segment at 12:00.
    m_segments.emplace_back(0, DEFAULT_BASAL_INSULIN, DEFAULT_CARB_RATIO, DEFAULT_CORRECTION_FACTOR, DEFAULT_TARGET_GLUCOSE);
}


ProfileSegment* Profile::getSegment(int time) {
    auto it = std::find_if(m_segments.begin(), m_segments.end(), [time](ProfileSegment ps) {
        return ps.getStartTime() == time;
    });

    if (it != m_segments.end())
    {
        return &(*it);
    }
    else
    {
        return nullptr;
    }
}

void Profile::addSegment(int startTime, float hourlyInsulinBasalRate, int carbRatio, float correctionFactor, float targetGlucose) {
    m_segments.emplace_back(startTime, hourlyInsulinBasalRate, carbRatio, correctionFactor, targetGlucose);

    std::sort(m_segments.begin(), m_segments.end(), [](const ProfileSegment& ps1, const ProfileSegment& ps2) {
        return ps1.getStartTime() < ps2.getStartTime();
    });
}

void Profile::removeSegment(int startTime) {
    auto it = std::find_if(m_segments.begin(), m_segments.end(), [startTime](ProfileSegment ps) {
        return ps.getStartTime() == startTime;
    });

    if (it != m_segments.end()) m_segments.erase(it);
}

void Profile::setName(const std::string& name) {
    this->name = name;
}

std::string Profile::getName() const {
    return name;
}

const std::vector<ProfileSegment>& Profile::getSegments() const
{
    return m_segments;
}
