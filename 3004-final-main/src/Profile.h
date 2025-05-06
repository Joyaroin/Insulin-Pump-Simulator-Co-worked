#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>
#include "ProfileSegment.h"
class Profile
{
public:
    Profile();
    ProfileSegment* getSegment(int time);
    void addSegment(int startTime, float hourlyInsulinBasalRate, int carbRatio, float correctionFactor, float targetGlucose);
    void removeSegment(int startTime);
    void setName(const std::string& name);
    std::string getName() const;

    const std::vector<ProfileSegment>& getSegments() const;

private:
    std::vector<ProfileSegment> m_segments;
    int size;
    std::string name;
};

#endif // PROFILE_H
