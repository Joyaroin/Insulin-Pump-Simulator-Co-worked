#include "sleepscreen.h"
#include "ui_sleepscreen.h"

#include <sstream>

SleepScreen::SleepScreen(QWidget *parent)
    : AbstractPage(parent)
    , ui(new Ui::SleepScreen)
{
    ui->setupUi(this);

    QObject::connect(ui->wakeButton, &QPushButton::released, this, [this](){
        if (checkPin()) emit setPage("HOME");
    });
}

SleepScreen::~SleepScreen()
{
    delete ui;
}

void SleepScreen::updateUI()
{
    ui->pin1->setValue(0);
    ui->pin2->setValue(0);
    ui->pin3->setValue(0);
    ui->pin4->setValue(0);
}

bool SleepScreen::checkPin()
{
    std::stringstream ss;
    ss << ui->pin1->value() << ui->pin2->value() << ui->pin3->value() << ui->pin4->value();

    return m_pDevice->isCorrectPin(ss.str());
}
