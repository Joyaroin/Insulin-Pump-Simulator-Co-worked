#include "manualbolus1.h"
#include "ui_manualbolus1.h"
#include "Device.h"
#include "defs.h"
#include <QDebug>
#include <algorithm>

ManualBolus1::ManualBolus1(QWidget *parent)
    : AbstractPage(parent)
    , ui(new Ui::ManualBolus1)
{
    ui->setupUi(this);

    makeConnections();
}

ManualBolus1::~ManualBolus1()
{
    delete ui;
}

void ManualBolus1::connectToDevice(Device& device)
{
    AbstractPage::connectToDevice(device);
    validateInputs();
}

void ManualBolus1::makeConnections()
{
    // 'Back' button press
    connect(ui->backButton, &QPushButton::released, this, [this]() {
        setPage("HOME");
    });

    // 'Fetch BG from CGM' button press
    connect(ui->fetchBloodGlucoseButton, &QPushButton::released, this, &ManualBolus1::onFetchBloodGlucoseButtonReleased);

    // Adjust displayed text depending on slider position
    connect(ui->extendedDeliveryPctSlider, &QSlider::valueChanged, this, &ManualBolus1::onSliderMoved);

    // Connect the Apply Bolus button
    connect(ui->applyBolusButton, &QPushButton::released, this, &ManualBolus1::onApplyBolusButtonReleased);

    // Connect input validation for carbs and blood glucose values
    connect(ui->carbsValue, QOverload<int>::of(&QSpinBox::valueChanged), this, &ManualBolus1::validateInputs);
    connect(ui->bloodGlucoseValue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ManualBolus1::validateInputs);
}

void ManualBolus1::onSliderMoved(int pct)
{
    if (pct == 0)
    {
        ui->deliveryPctSliderLabel->setText("Deliver all NOW");
    }
    else
    {
        QString s = QString("Deliver %1\% now, %2\% later").arg(100-pct).arg(pct);
        ui->deliveryPctSliderLabel->setText(s);
    }
}

void ManualBolus1::onApplyBolusButtonReleased()
{
    // Get values from UI
    int carbsAmount = ui->carbsValue->value();
    double bgValue = ui->bloodGlucoseValue->value();
    int extendedPct = ui->extendedDeliveryPctSlider->value();

    // Calculate insulin dose based on carbs, BG, and profile settings
    float totalInsulin = m_pDevice->calculateInsulinDose(carbsAmount, static_cast<float>(bgValue));

    if (m_pDevice) {
        try {
            if (extendedPct > 0) {
                // If there's an extended portion, use the extended bolus delivery
                // Scale the extended delivery time based on the percentage
                // Higher percentage = longer delivery time
                int extendedHours = 0;
                int extendedMins = std::max(30, extendedPct * 60 / 100); // Min 30 mins, scales up to 60 mins

                qDebug() << "[ManualBolus1] Delivering extended bolus: " << totalInsulin
                         << "units over " << extendedMins << " minutes";

                // Deliver the extended bolus through the device
                m_pDevice->deliverExtendedBolus(totalInsulin, extendedHours, extendedMins);
            } else {
                // If no extended portion, deliver as a standard manual bolus over 1 minute
                qDebug() << "[ManualBolus1] Delivering immediate bolus: " << totalInsulin << " units";
                m_pDevice->deliverManualBolus(totalInsulin, 1);  // Deliver over 1 minute
            }

            // Show confirmation and return to home screen
            setPage("HOME");
        } catch (const std::exception& e) {
            // Handle any exceptions from the device
            qWarning() << "[ManualBolus1] Error delivering bolus: " << e.what();
            // Could display an error message to the user here
        }
    }
}

void ManualBolus1::validateInputs()
{
    // Enable the apply button only if we have valid inputs
    bool validInputs = (ui->carbsValue->value() > 0 || ui->bloodGlucoseValue->value() > 0);
    QString disableReason;

    // Also check if the device is in a state where it can deliver insulin
    if (m_pDevice) {
        // Check device state - don't allow bolus if device is in error state or already delivering
        if (m_pDevice->status() == DeviceState::ERROR) {
            validInputs = false;
            disableReason = "Device is in error state";
        } else if (m_pDevice->status() == DeviceState::DELIVERING) {
            validInputs = false;
            disableReason = "Device is already delivering insulin";
        }

        // Check if battery is dead
        if (m_pDevice->getBattery().isDead()) {
            validInputs = false;
            disableReason = "Battery is dead";
        }

        // Check if blood glucose is too low
        if (ui->bloodGlucoseValue->value() < LOW_BLOOD_GLUCOSE) {
            validInputs = false;
            disableReason = "Blood glucose too low for bolus";
        }

        // Check if there's enough insulin in the cartridge (rough estimate)
        if (validInputs) { // Only calculate if we haven't already invalidated
            float estimatedInsulin = m_pDevice->calculateInsulinDose(ui->carbsValue->value(), static_cast<float>(ui->bloodGlucoseValue->value()));
            if (!m_pDevice->getCartridge().hasEnoughInsulin(estimatedInsulin)) {
                validInputs = false;
                disableReason = "Not enough insulin in cartridge";
            }
        }
    } else {
        validInputs = false;
        disableReason = "Device not available";
    }

    ui->applyBolusButton->setEnabled(validInputs);


    if (!validInputs && !disableReason.isEmpty()) {
        qDebug() << "[ManualBolus1] Bolus button disabled: " << disableReason;
    }
}

void ManualBolus1::onFetchBloodGlucoseButtonReleased()
{
    // Get the current blood glucose from the device's CGM
    if (m_pDevice) {
        double currentBG = m_pDevice->getCGM()->getReading();

        // Set the value in the UI
        ui->bloodGlucoseValue->setValue(currentBG);

        // Check if the glucose level is in a safe range
        if (currentBG < LOW_BLOOD_GLUCOSE) {
            qWarning() << "[ManualBolus1] Low blood glucose detected: " << currentBG;
            // Could display a warning to the user here
        } else if (currentBG > HIGH_BLOOD_GLUCOSE) {
            qWarning() << "[ManualBolus1] High blood glucose detected: " << currentBG;
            // Could display a warning to the user here
        }

        // Validate inputs after setting the BG value
        validateInputs();
    }
}
