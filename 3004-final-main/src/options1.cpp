#include "options1.h"
#include "ui_options1.h"
#include "mypump1.h"

Options1::Options1(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::Options1)
{
    ui->setupUi(this);

    QObject::connect(ui->backButton, &QPushButton::clicked, this, [this](){
        emit setPage("HOME");
    });

    QObject::connect(ui->sleepButton, &QPushButton::clicked, this, [this](){
        emit setPage("sleep");
    });

    QObject::connect(ui->myPumpButton, &QPushButton::clicked, this, [this](){
        emit setPage("mypump1");
    });
}

Options1::~Options1()
{
    delete ui;
}
