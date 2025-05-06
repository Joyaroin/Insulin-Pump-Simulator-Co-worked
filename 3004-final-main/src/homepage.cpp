#include "homepage.h"
#include "ui_homepage.h"

#include "Battery.h"
#include "Cartridge.h"
#include <QVector>
#include <utility>
#include "defs.h"

HomePage::HomePage(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::HomePage),
    graphHistoryLengthMins(1*MINS_PER_HOUR)
{
    ui->setupUi(this);

    // connect homepage elements
    QObject::connect(ui->bolusButton, &QPushButton::clicked, this, [this](){
        emit setPage("bolus1");
    });
    QObject::connect(ui->optionsButton, &QPushButton::clicked, this, [this](){
        emit setPage("options1");
    });
    QObject::connect(ui->statusButton, &QPushButton::clicked, this, [this](){
        emit setPage("status1");
    });

    QObject::connect(ui->cycleGraphLengthButton, &QPushButton::released, this, [this](){
        onCycleGraphLengthButtonReleased();
    });

    // Initialize graph
    ui->BGgraph->xAxis->setLabel("Time");
    ui->BGgraph->yAxis->setLabel("Blood Glucose (mmol/L)");
    ui->BGgraph->xAxis->scaleRange(1.05, ui->BGgraph->xAxis->range().center());
    ui->BGgraph->yAxis->scaleRange(1.05, ui->BGgraph->yAxis->range().center());
    ui->BGgraph->addGraph();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::connectToDevice(Device& device)
{
    m_pDevice = &device;
    
    // Connect to the device's timeChanged signal
    QObject::connect(m_pDevice, &Device::timeChanged, this, &HomePage::onTimeChanged);
    
    // Connect to the device's CGM glucoseReadingChange signal
    QObject::connect(m_pDevice->getCGM().get(), &CGM::glucoseReadingChange, this, &HomePage::onGlucoseChange);
}

void HomePage::onGlucoseChange(float glucose)
{
    ui->glucoseLevelLabel->setText(QString::number(glucose, 'f', 1));
}

void HomePage::onTimeChanged(const Time& newTime)
{
    ui->timeLabel->setText(QString::fromStdString(newTime.toHHMMString()));

    // battery level
    ui->batteryLabel->setText(QString::number(m_pDevice->getBattery().getCharge()));

    // glucose level (from CGM)
    ui->glucoseLevelLabel->setText(QString::number(m_pDevice->getCGM()->getReading()));

    // insulin cartridge level
    ui->insulinLeftLabel->setText(QString::number(m_pDevice->getCartridge().getInsulinUnits()) + " units");

    // update Insulin-on-board values
    ui->IOBLabel->setText(QString::number(m_pDevice->getInsulinOnBoard()) + "u");

    // update graph
    refreshGraph();

    // update time
    ui->timeLabel->setText(QString::fromStdString(newTime.toHHMMString()));
}

void HomePage::refreshGraph()
{
    // fake data, for now
    QVector<double> x, y;

    auto& history = m_pDevice->getGlucoseLogs();

    int datapoints = (graphHistoryLengthMins/SIMULATED_MINS_PER_TIMESTEP);
    if (history.size() < datapoints)
    {
        datapoints = history.size();
    }

    for (int i = (history.size() - datapoints); i < history.size(); ++i)
    {
        x.append(std::get<0>(history[i]));
        y.append((double)std::get<1>(history[i]));
    }

    ui->BGgraph->graph()->setLineStyle(QCPGraph::lsLine); // Line between points
    ui->BGgraph->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->BGgraph->graph()->setData(x,y);

    // generate tick labels (times instead of mins since sim started)
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    // Set the ticks and labels
    for (int i = 0; i < x.size(); ++i)
    {
        if ( i % (graphHistoryLengthMins/MINS_PER_HOUR) == 0)
        {
            textTicker->addTick(x[i], QString::fromStdString(Time(x[i]).toHHMMString()));
        }
    }

    // Assign the ticker to the xAxis
    ui->BGgraph->xAxis->setTicker(textTicker);

    // rescale axes to fit data comfortably
    ui->BGgraph->rescaleAxes();
    ui->BGgraph->replot();
}

void HomePage::onCycleGraphLengthButtonReleased()
{
    switch(graphHistoryLengthMins)
    {
    case 1*MINS_PER_HOUR:
        graphHistoryLengthMins = 3*MINS_PER_HOUR;
        break;

    case 3*MINS_PER_HOUR:
        graphHistoryLengthMins = 6*MINS_PER_HOUR;
        break;

    case 6*MINS_PER_HOUR:
    default:
        graphHistoryLengthMins = 1*MINS_PER_HOUR;
        break;
    }

    ui->cycleGraphLengthButton->setText(QString("Graph Hrs: %1").arg(graphHistoryLengthMins/MINS_PER_HOUR));
}
