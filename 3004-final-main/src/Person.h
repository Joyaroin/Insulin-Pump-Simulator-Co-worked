#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include "defs.h"

class Person : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float bloodGlucoseLevel READ bloodGlucoseLevel WRITE setBloodGlucoseLevel NOTIFY bloodGlucoseLevelChanged)

public:
    explicit Person();

    float bloodGlucoseLevel() const;

public slots:
    void setBloodGlucoseLevel(float bg);

    //added methods (idk if we need it tho)
    void eatCarbs(int grams);
    void absorbInsulin(int units);
    void receiveInsulin(float units);  // Receive insulin from pump


signals:
    void bloodGlucoseLevelChanged(float newBG);

private:
    float m_bloodGlucoseLevel;
};

#endif // PERSON_H
