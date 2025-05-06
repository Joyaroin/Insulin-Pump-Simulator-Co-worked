#include "mainwindow.h"
#include "pagewindow.h"
#include "ui_mainwindow.h"
#include "QVBoxLayout"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // create the sim
    sim = std::unique_ptr<Simulator>(new Simulator());

    ui->setupUi(this);

    // PageWindow gets deleted when MainWindow gets destroyed.
    PageWindow *pageWindow = new PageWindow(sim->getDevice(), this);

    QVBoxLayout *layout = new QVBoxLayout(ui->deviceDisplayWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(pageWindow);

    ui->pauseButton->setDisabled(true);
    makeConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeConnections()
{
    // Playback controls
    connect(ui->playButton, SIGNAL(released()), this, SLOT(onPlayButtonReleased()));
    connect(ui->pauseButton, SIGNAL(released()), this, SLOT(onPauseButtonReleased()));

    // Person blood glucose shifter
    connect(ui->applyBGdiffButton, SIGNAL(released()), this, SLOT(onApplyGlucoseDiffButtonReleased()));

    // Battery and Cartridge modifiers
    connect(ui->killBatteryButton, SIGNAL(released()), this, SLOT(onKillBatteryButtonReleased()));
    connect(ui->chargeBatteryButton, SIGNAL(released()), this, SLOT(onChargeBatteryButtonReleased()));

    connect(ui->emptyCartridgeButton, SIGNAL(released()), this, SLOT(onEmptyCartridgeButtonReleased()));
    connect(ui->replaceCartridgeButton, SIGNAL(released()), this, SLOT(onReplaceCartridgeButtonReleased()));
}

void MainWindow::onPlayButtonReleased()
{
    ui->playButton->setDisabled(true);
    ui->pauseButton->setEnabled(true);
    sim->play();
}

void MainWindow::onPauseButtonReleased()
{
    ui->playButton->setEnabled(true);
    ui->pauseButton->setDisabled(true);
    sim->pause();
}

void MainWindow::onApplyGlucoseDiffButtonReleased()
{
    double diff = (double)ui->BGdiffValue->value();
    sim->changePersonBloodGlucose(diff);
}

void MainWindow::onEmptyCartridgeButtonReleased()
{
    sim->emptyDeviceCartridge();
}

void MainWindow::onReplaceCartridgeButtonReleased()
{
    sim->replaceDeviceCartridge();
}

void MainWindow::onKillBatteryButtonReleased()
{
    sim->killDeviceBattery();
}

void MainWindow::onChargeBatteryButtonReleased()
{
    sim->chargeDeviceBattery();
}
