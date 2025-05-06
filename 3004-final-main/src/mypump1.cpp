#include "mypump1.h"
#include "ui_mypump1.h"

MyPump1::MyPump1(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::MyPump1)
{
    ui->setupUi(this);

    QObject::connect(ui->backButton, &QPushButton::clicked, this, [this](){
        emit setPage("options1");
    });

    QObject::connect(ui->personalProfilesButton, &QPushButton::clicked, this, [this](){
        emit setPage("profiles1");
    });
}

MyPump1::~MyPump1()
{
    delete ui;
}

