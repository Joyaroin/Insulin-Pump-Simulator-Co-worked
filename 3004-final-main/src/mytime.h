#ifndef MYTIME_H
#define MYTIME_H

/*
 * Represents a time measured in (simulation) minutes since simulation start.
 * This file is named mytime.h to avoid conflicts with the stdlib time.h include.
 */

#include <string>

class Time
{
public:
    Time(int minsSinceSimulationStart);

    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;
    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;

    void operator=(int mins);
    void operator+=(int mins);
    void operator-=(int mins);

    int getHours() const;
    int getMinutes() const;
    int getTotalMinutes() const;

    std::string toHHMMString() const;

private:
    int totalMinutes;
};

#endif // MYTIME_H
