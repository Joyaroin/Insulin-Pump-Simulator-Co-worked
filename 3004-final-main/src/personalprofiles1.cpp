#include "personalprofiles1.h"
#include "ui_personalprofiles1.h"
#include "defs.h"

PersonalProfiles1::PersonalProfiles1(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::PersonalProfiles1)
{
    ui->setupUi(this);

    QObject::connect(ui->backButton, &QPushButton::clicked, this, [this](){
        emit setPage("mypump1");
    });

    QObject::connect(ui->newButton, &QPushButton::clicked, this, [this](){
        Profile p;
        p.setName(DEFAULT_PROFILE_NAME);
        m_pDevice->addOrReplaceProfile(p);
        m_pDevice->setProfileToEdit(DEFAULT_PROFILE_NAME);

        emit setPage("profiles2");
    });

    QObject::connect(ui->editList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* i) {
        m_pDevice->setProfileToEdit(i->text().toStdString());
        emit setPage("profiles2");
    });

    // Switch/select profile
    QObject::connect(ui->selectList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* i) {
        std::string chosenProfileName = i->text().toStdString();
        m_pDevice->setActiveProfile(chosenProfileName);
        ui->activeProfile->setText(QString::fromStdString(m_pDevice->getCurrentProfileName()));
    });
}

PersonalProfiles1::~PersonalProfiles1()
{
    delete ui;
}

void PersonalProfiles1::updateUI()
{
    // current profile label
    ui->activeProfile->setText(QString::fromStdString(m_pDevice->getCurrentProfileName()));

    // populate profile select/edit lists
    ui->editList->clear();
    ui->selectList->clear();

    for (const Profile& p : m_pDevice->getProfiles())
    {
        ui->editList->addItem(QString::fromStdString(p.getName()));
        ui->selectList->addItem(QString::fromStdString(p.getName()));
    }
}



