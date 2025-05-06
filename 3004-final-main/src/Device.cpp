#include "Device.h"
#include "Battery.h"
#include "Cartridge.h"
#include "Pump.h"
#include "Person.h"
#include <QDebug>
#include <stdlib.h>
#include "mytime.h"
#include "defs.h"
#include <algorithm>
#include <QMessageBox>
#include <QVector>
#include <utility>

Device::Device(std::shared_ptr<CGM> cgm, std::shared_ptr<Person> person, std::shared_ptr<Battery> battery, std::shared_ptr<Pump> pump, std::shared_ptr<Cartridge> cartridge, Time time)
    : QObject(nullptr)
    , m_controlIQState(ControlIQ::State::NORMAL)
    , m_controlIQIcon(ControlIQ::Icon::GRAY_DIAMOND)
    , m_lastPredictedGlucose(0.0f)
    , m_manualBolus(nullptr)
    , m_autoCorrectionBolus(nullptr)
    , m_battery(battery)
    , m_cartridge(cartridge)
    , m_pump(pump)
    , m_person(person)
    , m_time(time)
    , m_status(DeviceState::IDLE)
    , m_cgm(cgm)
    , m_profiles()
    , m_pCurrentProfile(nullptr)
    , m_pin(DEFAULT_LOCKSCREEN_PIN)
    , m_profileToEdit(nullptr)
{

    QObject::connect(m_cgm.get(), &CGM::glucoseReadingChange, this, &Device::bgReadingChanged);

    QObject::connect(m_battery.get(), &Battery::batteryLow, this, &Device::handleBatteryLow);
    QObject::connect(m_battery.get(), &Battery::batteryDead, this, &Device::handleBatteryDead);
    QObject::connect(m_pump.get(), &Pump::isWorkingChanged, this, &Device::handlePumpWorkingChanged);

    QObject::connect(m_cartridge.get(), &Cartridge::insulinUnitsChanged, this, &Device::handleCartridgeUnitsChanged);
}

Cartridge& Device::getCartridge()
{
    return *m_cartridge;
}

Battery& Device::getBattery()
{
    return *m_battery;
}

std::shared_ptr<CGM> Device::getCGM()
{
    return m_cgm;
}

const Time& Device::getCurrentTime() const
{
    return m_time;
}

void Device::setCartridge(std::shared_ptr<Cartridge> cartridge)
{
    m_cartridge = cartridge;
}

DeviceState Device::status() const
{
    return m_status;
}

void Device::setStatus(DeviceState newState)
{
    if (m_status == newState)
        return;
    m_status = newState;
    emit statusChanged(m_status);
}

/**
 * \brief Deliver insulin in a single shot.
 *
 * This method checks the specification's steps:
 * 1) Verify battery is not dead.
 * 2) Verify cartridge has enough insulin.
 * 3) Switch device state to DELIVERING.
 * 4) Activate pump, remove insulin from cartridge, apply to Person.
 * 5) Switch back to IDLE after completion.
 */
void Device::deliverInsulin(float units)
{
    if (units == 0) {
        qDebug() << "[Device] Device Idle, not delivering insulin.";
        setStatus(DeviceState::IDLE);
        return;
    }

    qDebug() << "[Device] Request to deliver" << units << "unit(s) of insulin.";

    if (m_battery->isDead()) {
        QString error = "Battery is dead! Cannot deliver insulin.";
        qWarning() << "[Device]" << error;
        setStatus(DeviceState::ERROR);
        emit insulinDeliveryError(error);
        throw std::runtime_error(error.toStdString());
    }

    if (!m_cartridge->hasEnoughInsulin(units)) {
        QString error = QString("Not enough insulin in cartridge for %1 units.").arg(units);
        qWarning() << "[Device]" << error;
        setStatus(DeviceState::ERROR);
        emit insulinDeliveryError(error);
        throw std::runtime_error(error.toStdString());
    }

    if (!m_pump->isWorking()) {
        QString error = "Pump malfunction!";
        qWarning() << "[Device]" << error;
        setStatus(DeviceState::ERROR);
        emit insulinDeliveryError(error);
        throw std::runtime_error(error.toStdString());
    }

    setStatus(DeviceState::DELIVERING);

    try {
        m_pump->activate();
        m_cartridge->removeInsulin(units);
        m_person->receiveInsulin(units);
        qDebug() << "[Device] Insulin delivery of" << units << "unit(s) completed successfully.";
        setStatus(DeviceState::IDLE);
    } catch (const std::exception& e) {
        QString error = QString("Failed to deliver insulin: %1").arg(e.what());
        qWarning() << "[Device]" << error;
        setStatus(DeviceState::ERROR);
        emit insulinDeliveryError(error);
        throw;
    }
}

void Device::deliverManualBolus(float units, int durationMins)
{
    if (!canStartNewBolus(Bolus::Type::MANUAL)) {
        return;
    }

    // Ensure minimum duration of 1 minute for safety
    if (durationMins <= 0) {
        durationMins = 1;
    }

    qDebug() << "[Manual Bolus] Starting new bolus at time" << m_time.toHHMMString().c_str();
    qDebug() << "[Manual Bolus] Total units:" << units << "Duration:" << durationMins << "minutes";
    m_manualBolus = new Bolus(Bolus::Type::MANUAL, units, durationMins, m_time);
    startBolus(m_manualBolus);
}

void Device::deliverExtendedBolus(float units, int durationHours, int durationMins)
{
    if (canStartNewBolus(Bolus::Type::EXTENDED)) {
        Bolus* bolus = new Bolus(Bolus::Type::EXTENDED, units, durationHours, durationMins,
                                m_time, "Extended bolus request");
        startBolus(bolus);
    }
}



bool Device::canStartNewBolus(Bolus::Type type) const
{
    // Manual boluses can always start, they'll cancel any existing boluses
    if (type == Bolus::Type::MANUAL) {
        return true;
    }

    // Auto-correction boluses can only start if there's no manual bolus active
    if (type == Bolus::Type::AUTO_CORRECTION) {
        return m_manualBolus == nullptr || !m_manualBolus->isActive(m_time);
    }

    // Extended boluses follow same rules as manual boluses
    return true;
}

void Device::startBolus(Bolus* bolus)
{
    // Check battery first
    if (m_battery->isDead()) {
        emit bolusError("Cannot start bolus: Battery is dead");
        delete bolus;
        return;
    }

    // Calculate total insulin needed for this bolus
    float totalInsulin = bolus->getTotalDosage();
    if (!m_cartridge->hasEnoughInsulin(totalInsulin)) {
        emit bolusError(QString("Cannot start bolus: Not enough insulin (need %1 units)").arg(totalInsulin));
        delete bolus;
        return;
    }

    // Cancel any existing bolus of the same type
    if (bolus->getType() == Bolus::Type::MANUAL || bolus->getType() == Bolus::Type::EXTENDED) {
        if (m_manualBolus != nullptr) {
            delete m_manualBolus;
        }
        m_manualBolus = bolus;
    } else {
        if (m_autoCorrectionBolus != nullptr) {
            delete m_autoCorrectionBolus;
        }
        m_autoCorrectionBolus = bolus;
    }

    // Connect the finished signal
    connect(bolus, &Bolus::isFinished, this, &Device::onBolusFinished);

    // Emit bolus started signal
    emit bolusStarted(bolus->getType(), totalInsulin);

    // Start delivering the first dose
    float firstDose = bolus->getDoseForTimestep(m_time);
    if (firstDose > 0) {
        try {
            deliverInsulin(firstDose);
        } catch (const std::exception& e) {
            emit insulinDeliveryError(QString("Failed to deliver initial dose: %1").arg(e.what()));
            onBolusFinished(bolus); // This will clean up the bolus
            return;
        }
    }
}

void Device::onBolusFinished(Bolus* bolus)
{
    // Emit the finished signal before cleanup
    emit bolusFinished(bolus->getType());

    if (bolus == m_manualBolus) {
        delete m_manualBolus;
        m_manualBolus = nullptr;
    } else if (bolus == m_autoCorrectionBolus) {
        delete m_autoCorrectionBolus;
        m_autoCorrectionBolus = nullptr;
    }
}

void Device::controlIqAction(float glucoseLevel)
{
    qDebug() << "[Device] Current glucose level:" << glucoseLevel;

    // Predict glucose level 30 minutes in the future
    float predictedGlucose = estimateGlucose(glucoseLevel);
    m_lastPredictedGlucose = predictedGlucose;

    qDebug() << "[Device] Predicted glucose in 30 minutes:" << predictedGlucose;

    // Update Control-IQ state based on predicted glucose
    updateControlIQState(predictedGlucose);

    if (!m_pCurrentProfile) {
        qDebug() << "[Control-IQ] No active profile, cannot adjust insulin delivery";
        return;
    }

    // Get current profile segment based on time
    ProfileSegment* currentSegment = m_pCurrentProfile->getSegment(m_time.getMinutes());
    if (!currentSegment) {
        qDebug() << "[Control-IQ] No profile segment for current time";
        return;
    }

    float baseRate = currentSegment->getHourlyInsulinBasalRate();

    // Handle insulin delivery based on Control-IQ state
    switch (m_controlIQState) {
        case ControlIQ::State::NORMAL:
            // Deliver insulin at profile rate
            deliverInsulin(baseRate);
            break;

        case ControlIQ::State::DECREASING:
            // Decrease insulin from profile rate
            deliverInsulin(baseRate * 0.5f); // 50% reduction
            break;

        case ControlIQ::State::STOPPED:
            // Stop basal insulin (bolus can still be delivered)
            // Note: Bolus delivery still allowed through manual bolus commands
            break;

        case ControlIQ::State::INCREASING:
            // Increase insulin from profile rate
            deliverInsulin(baseRate * 1.5f); // 50% increase
            break;

        case ControlIQ::State::AUTO_CORRECTION:
            // Calculate and deliver automatic correction bolus
            float targetBG = currentSegment->getTargetGlucose();
            float correctionFactor = currentSegment->getCorrectionFactor();
            deliverAutomaticCorrectionBolus(predictedGlucose, targetBG, correctionFactor);
            break;
    }
}

float Device::estimateGlucose(float glucoseLevel)
{
    return glucoseLevel - (float)(rand() % 4) / 10.0f;
}


void Device::deliverAutomaticCorrectionBolus(float predictedGlucose, float targetGlucose, float correctionFactor)
{
    // Calculate correction bolus based on how far above target
    float glucoseAboveTarget = predictedGlucose - targetGlucose;

    // Calculate correction units using profile's correction factor
    float correctionUnits = glucoseAboveTarget * correctionFactor;

    // Limit maximum automatic correction bolus
    const float MAX_AUTO_CORRECTION = 6.0f; // Maximum 6 units for safety
    correctionUnits = std::min(correctionUnits, MAX_AUTO_CORRECTION);

    if (correctionUnits > 0 && canStartNewBolus(Bolus::Type::AUTO_CORRECTION)) {
        qDebug() << "[Control-IQ] Delivering automatic correction bolus:" << correctionUnits << "units";
        qDebug() << "[Control-IQ] Details: Predicted BG:" << predictedGlucose
                 << "Target:" << targetGlucose
                 << "Correction Factor:" << correctionFactor;

        // Create and start auto-correction bolus
        Bolus* bolus = new Bolus(Bolus::Type::AUTO_CORRECTION, correctionUnits, 1,
                                m_time, "Control-IQ correction for high glucose");
        startBolus(bolus);
    }
}

int Device::calculateInsulinUnits(float glucose)
{
    int insulinUnits;

    if (glucose < 3.90) {
        insulinUnits = 0;
    } else if (glucose >= 3.90 && glucose < 6.250) {
        insulinUnits = 1;
    } else if (glucose >= 6.250 && glucose < 8.250) {
        insulinUnits = 2;
    } else if (glucose >= 8.250 && glucose < 10.00) {
        insulinUnits = 3;
    } else if (glucose >= 10.00) {
        insulinUnits = 4;
    }

    return insulinUnits;
}

float Device::calculateInsulinDose(int carbs, float bloodGlucose)
{
    // Default values from defs.h
    double carbRatio = DEFAULT_CARB_RATIO;
    double correctionFactor = DEFAULT_CORRECTION_FACTOR;
    double targetBG = DEFAULT_TARGET_GLUCOSE;

    // Get the current profile settings based on the time of day
    Profile* currentProfile = getProfile(getCurrentProfileName());
    if (currentProfile) {
        // Get the current time to determine which profile segment to use
        int currentTimeHours = getCurrentTime().getHours();

        // Get the appropriate profile segment for the current time
        ProfileSegment* segment = currentProfile->getSegment(currentTimeHours);
        if (segment) {
            // Use the settings from the profile segment
            carbRatio = segment->getCarbRatio();
            correctionFactor = segment->getCorrectionFactor();
            targetBG = segment->getTargetGlucose();

            qDebug() << "[Device] Using profile settings: CarbRatio=" << carbRatio
                     << "CorrectionFactor=" << correctionFactor
                     << "TargetBG=" << targetBG;
        } else {
            qWarning() << "[Device] No profile segment found for current time, using defaults";
        }
    } else {
        qWarning() << "[Device] No current profile found, using default values";
    }

    // Calculate insulin for carbs
    double carbInsulin = carbs / carbRatio;

    // Calculate correction insulin if BG is above target
    double correctionInsulin = 0.0;
    if (bloodGlucose > targetBG) {
        correctionInsulin = (bloodGlucose - targetBG) / correctionFactor;
    }

    // Total insulin dose
    double totalInsulin = carbInsulin + correctionInsulin;

    // Account for insulin on board
    float insulinOnBoard = getInsulinOnBoard();
    if (insulinOnBoard > 0) {
        // Reduce the correction insulin by the amount of insulin already on board
        // but don't reduce below zero
        totalInsulin = std::max(carbInsulin, totalInsulin - insulinOnBoard);
        qDebug() << "[Device] Adjusting for insulin on board:" << insulinOnBoard
                 << "units, adjusted total:" << totalInsulin;
    }

    // Safety check - never return a negative value
    return (totalInsulin > 0) ? totalInsulin : 0.0;
}


void Device::onSimulatorTimeChanged(const Time& newTime)
{
    // guard against duplicate signal sends
    if (m_time == newTime) return;

    m_time = newTime;

    // Check and deliver manual bolus if active
    if (m_manualBolus != nullptr) {
        float dose = m_manualBolus->getDoseForTimestep(m_time);
        qDebug() << "[Manual Bolus] At time" << m_time.toHHMMString().c_str() << "- Calculated dose:" << dose << "units";
        if (dose > 0) {
            qDebug() << "[Manual Bolus] Delivering dose of" << dose << "units";
            deliverInsulin(dose);
        }
        // Check if bolus is finished
        if (!m_manualBolus->isActive(m_time)) {
            qDebug() << "[Manual Bolus] Completed at time" << m_time.toHHMMString().c_str();
            onBolusFinished(m_manualBolus);
        }
    }

    // Check and deliver auto correction bolus if active
    if (m_autoCorrectionBolus != nullptr) {
        float dose = m_autoCorrectionBolus->getDoseForTimestep(m_time);
        if (dose > 0) {
            deliverInsulin(dose);
        }
        // Check if bolus is finished
        if (!m_autoCorrectionBolus->isActive(m_time)) {
            onBolusFinished(m_autoCorrectionBolus);
        }
    }

    if (m_cgm) m_glucoseLogs.append(std::pair<int, float>(m_time.getTotalMinutes(), m_cgm->getReading()));
    emit timeChanged(m_time);
}

std::string Device::getCurrentProfileName() const
{
    if (m_profiles.empty() || m_pCurrentProfile == nullptr)
    {
        return "N/A";
    }
    else
    {
        return m_pCurrentProfile->getName();
    }
}

const std::vector<Profile>& Device::getProfiles() const
{
    return m_profiles;
}

Profile* Device::getProfile(std::string name)
{
    auto it = std::find_if(m_profiles.begin(), m_profiles.end(), [name](const Profile& p) {
        return p.getName() == name;
    });

    if (it == m_profiles.end())
    {
        return nullptr;
    }
    else
    {
        return &(*it);
    }
}

Profile* Device::getProfileToEdit()
{
    return m_profileToEdit;
}

void Device::setProfileToEdit(std::string name)
{
    m_profileToEdit = getProfile(name);
}

void Device::addOrReplaceProfile(Profile p)
{
    for (auto& profile : m_profiles)
    {
        if (profile.getName() == p.getName())
        {
            if (p.getName() == m_pCurrentProfile->getName())
            {
                profile = p;
                m_pCurrentProfile = &profile;
            }
            else
            {
                profile = p;
            }

            return;
        }
    }

    m_profiles.push_back(p);

}

void Device::deleteProfile(Profile* p)
{
    if (!p)
    {
        return;
    }

    if (m_pCurrentProfile == p)
    {
        m_pCurrentProfile = nullptr;
        qInfo("[Device]: The active profile was deleted. No profile is active now.");
    }

    auto it = std::find_if(m_profiles.begin(), m_profiles.end(), [p](const Profile& prof) {
        return &prof == p;
    });

    if (it != m_profiles.end()) m_profiles.erase(it);
    m_profileToEdit = nullptr;
}

void Device::setActiveProfile(std::string profileName)
{
    for ( Profile& p : m_profiles )
    {
        if (p.getName() == profileName)
        {
            m_pCurrentProfile = &p;
            qInfo("[Device]: Set the active profile to '%s'", m_pCurrentProfile->getName().c_str());
            return;
        }
    }
}

bool Device::isCorrectPin(std::string attempt) const
{
    return m_pin == attempt;
}

void Device::setPin(std::string newPin)
{
    m_pin = newPin;
}

float Device::getInsulinOnBoard() const
{
    float iob = 0;
    for(int i = m_glucoseLogs.size()-1; i >= 0; ++i)
    {
        int timeDiff = m_time.getTotalMinutes() - std::get<0>(m_glucoseLogs[i]);

        if (timeDiff >= INSULIN_FULL_DECAY_MINS) break;

        iob += (1 - ((float)timeDiff/(float)INSULIN_FULL_DECAY_MINS)) * std::get<1>(m_glucoseLogs[i]);
    }

    return iob;
}


void Device::handleBatteryLow()
{
    QMessageBox::warning(nullptr, "Battery Low", "Error: Battery Low! Please recharge soon.");
    qWarning() << "[Device] Warning: Battery low!";
    setStatus(DeviceState::ERROR);
    if (m_pump->isWorking()){
        m_pump->setWorking(false);
    }
    emit batteryLowWarning();
}

void Device::handleBatteryDead()
{
    QMessageBox::warning(nullptr, "Battery Dead", "Error: Battery dead! Please recharge immeddiately.");
    qWarning() << "[Device] Warning: Battery dead!";
    setStatus(DeviceState::ERROR);
    if (m_pump->isWorking()){
        m_pump->setWorking(false);
    }
    emit batteryDeadWarning();
}

void Device::handlePumpWorkingChanged(bool working)
{
    if (!working) {
        qWarning() << "[Device] Pump malfunction detected";
        setStatus(DeviceState::ERROR);
    } else {
        qDebug() << "[Device] Pump working status restored";
        setStatus(DeviceState::IDLE);
    }
    emit pumpMalfunctionWarning(working);
}

void Device::handleCartridgeUnitsChanged(int newUnits)
{
    if (newUnits <= 0){
        qWarning() << "[Device] Warning: Cartridge empty!";
        QMessageBox::warning(nullptr, "Cartridge empty", "Error: Empty cartridge! Please replace immeddiately.");
        setStatus(DeviceState::ERROR);
        if (m_pump->isWorking()){
            m_pump->setWorking(false);
        }
    } else if (newUnits <= LOW_INSULIN_THRESHOLD){
        qWarning() << "[Device] Cartridge insulin level low:" << newUnits << "units remaining";
        QMessageBox::warning(nullptr, "Cartridge Low", "Error: Low cartridge! Please replace soon.");
        emit cartridgeLowWarning(newUnits);
    }
}

void Device::bgReadingChanged(float glucoseLevel)
{
    if (glucoseLevel < LOW_BLOOD_GLUCOSE) {
        qWarning() << "[Device] Low blood glucose detected:" << glucoseLevel;
        QMessageBox::warning(nullptr, "Critically Low Glucose", QString("Warning: Blood glucose critically low (%1 mmol/L)!").arg(glucoseLevel));
        emit bgReadingWarning(glucoseLevel);
    } else if (glucoseLevel > HIGH_BLOOD_GLUCOSE) {
        qWarning() << "[Device] High blood glucose detected:" << glucoseLevel;
        QMessageBox::warning(nullptr, "Critically High Glucose", QString("Warning: Blood glucose critically High (%1 mmol/L)!").arg(glucoseLevel));
        emit bgReadingWarning(glucoseLevel);
    }
}

void Device::updateControlIQState(float predictedGlucose)
{
    ControlIQ::State newState;
    ControlIQ::Icon newIcon;

    // Determine state based on predicted glucose level
    if (predictedGlucose < ControlIQ::BG_VERY_LOW) {
        newState = ControlIQ::State::STOPPED;
        newIcon = ControlIQ::Icon::RED_DIAMOND_BOTTOM;
    }
    else if (predictedGlucose <= ControlIQ::BG_LOW) {
        newState = ControlIQ::State::DECREASING;
        newIcon = ControlIQ::Icon::ORANGE_DIAMOND_BOTTOM;
    }
    else if (predictedGlucose >= ControlIQ::BG_VERY_HIGH) {
        newState = ControlIQ::State::AUTO_CORRECTION;
        newIcon = ControlIQ::Icon::AUTO_CORRECTION_BOLUS;
    }
    else if (predictedGlucose > ControlIQ::BG_HIGH) {
        newState = ControlIQ::State::INCREASING;
        newIcon = ControlIQ::Icon::BLUE_DIAMOND_TOP;
    }
    else {
        newState = ControlIQ::State::NORMAL;
        newIcon = ControlIQ::Icon::GRAY_DIAMOND;
    }

    // Update state and icon if changed
    if (m_controlIQState != newState) {
        m_controlIQState = newState;
        emit controlIQStateChanged(newState);
    }

    if (m_controlIQIcon != newIcon) {
        m_controlIQIcon = newIcon;
        emit controlIQIconChanged(newIcon);
    }

    qDebug() << "[Control-IQ] State:" << static_cast<int>(newState) << "Icon:" << static_cast<int>(newIcon);
}

QVector<std::pair<int, float>>& Device::getGlucoseLogs()
{
    return m_glucoseLogs;
}
