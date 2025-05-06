#include "status1.h"
#include "ui_status1.h"

Status1::Status1(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::Status1)
{
    ui->setupUi(this);

    QObject::connect(ui->backButton, &QPushButton::clicked, this, [this](){
        emit setPage("HOME");
    });
}

Status1::~Status1()
{
    delete ui;
}
