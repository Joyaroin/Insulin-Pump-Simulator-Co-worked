#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "DeviceStates.h"
#include "CGM.h"
#include "Battery.h"
#include "Cartridge.h"
#include "mytime.h"
#include "Profile.h"
#include "ControlIQ.h"
#include "Bolus.h"
#include <string>
#include <QMap>

class Battery;
class Cartridge;
class Pump;
class Person;
class Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeviceState status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(ControlIQ::State controlIQState READ getControlIQState NOTIFY controlIQStateChanged)
    Q_PROPERTY(ControlIQ::Icon controlIQIcon READ getControlIQIcon NOTIFY controlIQIconChanged)


public:
    explicit Device(std::shared_ptr<CGM> cgm, std::shared_ptr<Person> person, std::shared_ptr<Battery> battery, std::shared_ptr<Pump> pump, std::shared_ptr<Cartridge> cartridge, Time time);

    // Setters for component pointers
    void setCartridge(std::shared_ptr<Cartridge> cartridge);
    void setProfileToEdit(std::string name);
    void setActiveProfile(std::string profileName);

    // Accessors
    DeviceState status() const;
    void setStatus(DeviceState newState);

    Device& getDevice();
    std::shared_ptr<CGM> getCGM();
    Cartridge& getCartridge();
    Battery& getBattery();
    const Time& getCurrentTime() const;

    std::string getCurrentProfileName() const;
    const std::vector<Profile>& getProfiles() const;
    Profile* getProfile(std::string name);
    Profile* getProfileToEdit();

    void addOrReplaceProfile(Profile p);
    void deleteProfile(Profile* p);

    void setPin(std::string newPin);
    bool isCorrectPin(std::string attempt) const;

    float getInsulinOnBoard() const;
    QVector<std::pair<int, float>>& getGlucoseLogs();

    // Control-IQ related methods
    ControlIQ::State getControlIQState() const { return m_controlIQState; }
    ControlIQ::Icon getControlIQIcon() const { return m_controlIQIcon; }
    void updateControlIQState(float predictedGlucose);
    void deliverAutomaticCorrectionBolus(float predictedGlucose, float targetGlucose, float correctionFactor);

    // Insulin calculation methods
    float calculateInsulinDose(int carbs, float bloodGlucose);

signals:
    void controlIQStateChanged(ControlIQ::State state);
    void controlIQIconChanged(ControlIQ::Icon icon);

private:
    // Control-IQ state
    ControlIQ::State m_controlIQState;
    ControlIQ::Icon m_controlIQIcon;
    float m_lastPredictedGlucose;

    // Active boluses
    Bolus* m_manualBolus;
    Bolus* m_autoCorrectionBolus;

    // Logging history (pairs of time/glucose)
    QVector<std::pair<int, float>> m_glucoseLogs;

    // Bolus management
    void onBolusFinished(Bolus* bolus);
    bool canStartNewBolus(Bolus::Type type) const;
    void startBolus(Bolus* bolus);

public slots:
    // Insulin delivery
    void deliverManualBolus(float units, int durationMins = 1);
    void deliverExtendedBolus(float units, int durationHours, int durationMins);
    void onSimulatorTimeChanged(const Time& newTime);

private slots:
    void deliverInsulin(float units);  // Internal method for actual insulin delivery

    void handleBatteryLow();
    void handleBatteryDead();
    void handlePumpWorkingChanged(bool working);
    void handleCartridgeUnitsChanged(int newUnits);
    void bgReadingChanged(float glucoseLevel);

signals:
    void statusChanged(DeviceState newState);
    void timeChanged(const Time& newTime);
    void bolusError(const QString& message);
    void bolusStarted(Bolus::Type type, float units);
    void bolusFinished(Bolus::Type type);
    void insulinDeliveryError(const QString& message);
    void batteryLowWarning();
    void batteryDeadWarning();
    void pumpMalfunctionWarning(bool working);
    void cartridgeLowWarning(int unitsRemaining);
    void bgReadingWarning(float glucoseLevel);

private:
    // references to sub-components
    std::shared_ptr<Battery> m_battery;
    std::shared_ptr<Cartridge> m_cartridge;
    std::shared_ptr<Pump> m_pump;
    std::shared_ptr<Person> m_person;
    Time m_time;

    DeviceState m_status;
    std::shared_ptr<CGM> m_cgm;
    std::vector<Profile> m_profiles;
    Profile* m_pCurrentProfile;
    std::string m_pin;

    // For passing data around UI components
    Profile* m_profileToEdit;

    void controlIqAction(float glucose);
    float estimateGlucose(float glucose);
    int calculateInsulinUnits(float glucose);

};

#endif // DEVICE_H
