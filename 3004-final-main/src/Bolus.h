#ifndef BOLUS_H
#define BOLUS_H

/*
 * This class represents a bolus of insulin to deliver.
 * A call to getNextDose(Time currentTime) will return the dosage required for the timestep.
 * A Bolus instance emits the isFinished(this) signal when requested to provide a dose after the initially set end time.
 *
 * As of now, this class's logic is relatively simple and depends on a #define in defs.h to assume minutes per timestep.
 */

#include <QObject>
#include <mytime.h>
#include <string>

class Bolus : public QObject
{
    Q_OBJECT

public:
    enum class Type {
        MANUAL,             // User-requested bolus
        AUTO_CORRECTION,    // Control-IQ automatic correction
        EXTENDED            // Extended bolus delivery
    };

    // Constructor for simple duration in minutes (for manual and auto-correction boluses)
    Bolus(Type type, float totalInsulinDosage, int deliveryDurationMins,
          const Time& currentTime, const std::string& reason = "");

    // Constructor for extended bolus with hours and minutes
    Bolus(Type type, float totalInsulinDosage, int deliveryDurationHours,
          int deliveryDurationMins, const Time& currentTime, const std::string& reason = "");

    /* Returns the dosage to be delivered at a timestep. Is zero if bolus expired */
    float getDoseForTimestep(const Time& currentTime);

    /* Getters for bolus properties */
    Type getType() const { return m_type; }
    std::string getReason() const { return m_reason; }
    float getTotalDosage() const { return m_totalDosage; }
    bool isActive(const Time& currentTime) const;

private:
    Type m_type;
    float m_totalDosage;
    float m_insulinPerMinute;
    Time m_endTime;
    std::string m_reason;

signals:
    void isFinished(Bolus* self);
};

#endif // BOLUS_H
