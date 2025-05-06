#include "mytime.h"
#include "defs.h"
#include <string>


Time::Time(int minsSinceSimulationStart)
    : totalMinutes(minsSinceSimulationStart)
{}

bool Time::operator==(const Time& other) const {
    return this->totalMinutes == other.totalMinutes;
}

bool Time::operator!=(const Time& other) const {
    return !(*this == other);
}

bool Time::operator<(const Time& other) const {
    return this->totalMinutes < other.totalMinutes;
}

bool Time::operator<=(const Time& other) const {
    return this->totalMinutes <= other.totalMinutes;
}

bool Time::operator>(const Time& other) const {
    return this->totalMinutes > other.totalMinutes;
}

bool Time::operator>=(const Time& other) const {
    return this->totalMinutes >= other.totalMinutes;
}

void Time::operator+=(int mins)
{
    totalMinutes += mins;
}

void Time::operator-=(int mins)
{
    totalMinutes -= mins;
}

int Time::getMinutes() const
{
    return totalMinutes % 60;
}

int Time::getHours() const
{
    return (totalMinutes / MINS_PER_HOUR) % HOURS_PER_DAY;
}

int Time::getTotalMinutes() const
{
    return totalMinutes;
}

std::string Time::toHHMMString() const
{
    int hours = getHours();
    int minutes = getMinutes();

    std::string result;

    if (hours < 10) result += "0";
    result += std::to_string(hours);
    result += ":";

    if (minutes < 10) result += "0";
    result += std::to_string(minutes);

    return result;
}

void Time::operator=(int mins)
{
    totalMinutes = mins;
}
