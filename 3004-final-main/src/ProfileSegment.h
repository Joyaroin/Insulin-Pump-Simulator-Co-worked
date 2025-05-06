#ifndef PROFILESEGMENT_H
#define PROFILESEGMENT_H

class ProfileSegment
{
public:
    ProfileSegment();
    ProfileSegment(int startTime, float hourlyInsulinBasalRate, int carbRatio, float correctionFactor, float targetGlucose);

    // Getters
    int getStartTime() const;
    float getHourlyInsulinBasalRate() const;
    int getCarbRatio() const;
    float getCorrectionFactor() const;
    float getTargetGlucose() const;

    // Setters
    void setStartTime(int startTime);
    void setHourlyInsulinBasalRate(float rate);
    void setCarbRatio(int ratio);
    void setCorrectionFactor(float factor);
    void setTargetGlucose(float target);

private:
    int m_startTime;
    float m_hourlyInsulinBasalRate;
    int m_carbRatio;
    float m_correctionFactor;
    float m_targetGlucose;
};

#endif // PROFILESEGMENT_H
