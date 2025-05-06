#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QTimer>
#include "Person.h"
#include "CGM.h"
#include "Device.h"
#include "mytime.h"
class Simulator : public QObject
{
    Q_OBJECT

public:
    explicit Simulator(QObject* parent = nullptr);

    bool isPlaying() const;
    void pause();
    void play();

    Device& getDevice();
    void emptyDeviceCartridge();
    void replaceDeviceCartridge();
    void killDeviceBattery();
    void chargeDeviceBattery();

public slots:

    void processCurrentTimestep();
    void changePersonBloodGlucose(float diff);   // Quick way to adjust BG

private:
    std::shared_ptr<Person> m_person;
    std::shared_ptr<CGM> m_cgm;
    std::unique_ptr<Device> m_device;
    std::shared_ptr<Battery> m_battery;
    std::shared_ptr<Pump> m_pump;
    std::shared_ptr<Cartridge> m_cartridge;
    Time m_time;
    bool    m_isPlaying;
    QTimer* m_timer;

    void makeConnections();
    void advanceTime();

signals:
    void timeChanged(const Time& newTime);
};

#endif // SIMULATOR_H
