#include "QTimer"
#include "Simulator.h"
#include "Person.h"
#include "defs.h"
#include "Device.h"
#include <QDebug>
#include <memory>
#include "Battery.h"
#include "Pump.h"
#include "Cartridge.h"

Simulator::Simulator(QObject* parent)
    : QObject(parent)
    , m_isPlaying(false)
    , m_time(0)
    , m_timer(nullptr)
{
    m_person = std::shared_ptr<Person>(new Person());
    m_cgm = std::shared_ptr<CGM>(new CGM(m_person));
    m_pump = std::shared_ptr<Pump>(new Pump());
    m_battery = std::shared_ptr<Battery>(new Battery(100));
    m_cartridge = std::shared_ptr<Cartridge>(new Cartridge());
    m_device = std::unique_ptr<Device>(new Device(m_cgm, m_person, m_battery, m_pump, m_cartridge, m_time));

    makeConnections();
}

void Simulator::makeConnections()
{
    connect(this, &Simulator::timeChanged, m_device.get(), &Device::onSimulatorTimeChanged);
}

bool Simulator::isPlaying() const
{
    return m_isPlaying;
}

void Simulator::processCurrentTimestep()
{
    qDebug() << "[Simulator]: Emitting t =" << m_time.toHHMMString().c_str();

    emit timeChanged(m_time);

    qDebug() << "[Simulator]: Done t =" << m_time.toHHMMString().c_str();
}

void Simulator::changePersonBloodGlucose(float val)
{
    if (!m_person) {
        return;
    }

    m_person->setBloodGlucoseLevel(val);
}

void Simulator::pause()
{
    m_isPlaying = false;
    if (m_timer) {
        m_timer->stop();
    }
}

void Simulator::play()
{
    qInfo("[Simulator]: Playing sim.");
    m_isPlaying = true;

    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, [this]() {
            if (!m_isPlaying) {
                m_timer->stop();
                return;
            }
            processCurrentTimestep();
            advanceTime();
        });
    }

    m_timer->start(MS_BETWEEN_TIMESTEPS); // runs again every MS_BETWEEN_TIMESTEPS until playback is paused.
}

void Simulator::advanceTime()
{
    m_time += SIMULATED_MINS_PER_TIMESTEP;

    // Make CGM re-check the person's BG each tick.
    // (cgm operates independently from device -- the device just asks for the most recent reading --
    //      it doesn't tell the CGM when to measure blood glucose.)
    m_cgm->measure();

    emit timeChanged(m_time);
}

Device& Simulator::getDevice()
{
    return *m_device;
}

void Simulator::emptyDeviceCartridge()
{
    m_device->getCartridge().setInsulinUnits(0);
}

void Simulator::replaceDeviceCartridge()
{
    m_device->setCartridge(std::shared_ptr<Cartridge>(new Cartridge()));
}

void Simulator::killDeviceBattery()
{
    m_device->getBattery().setCharge(0);
}

void Simulator::chargeDeviceBattery()
{
    m_device->getBattery().setCharge(100);
}
