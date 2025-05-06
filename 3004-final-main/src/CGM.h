#ifndef CGM_H
#define CGM_H

#include <QObject>
#include <memory>

class Person;

class CGM : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float currentReading READ getReading NOTIFY glucoseReadingChange)

public:
    explicit CGM(std::shared_ptr<Person> person, QObject* parent = nullptr);

    float getReading() const;

public slots:
    void measure();               // poll the Person's BG
    void onGlucoseReadingChange(float);  // handle the reading

signals:
    void glucoseReadingChange(float bgValue);

private:
    std::shared_ptr<Person> m_person;
    float     m_currentReading;
};

#endif // CGM_H
