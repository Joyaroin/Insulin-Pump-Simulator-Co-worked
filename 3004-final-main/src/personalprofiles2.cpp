#include "personalprofiles2.h"
#include "ui_personalprofiles2.h"

#include "defs.h"
#include <QSpinBox>

PersonalProfiles2::PersonalProfiles2(QWidget *parent) :
    AbstractPage(parent),
    ui(new Ui::PersonalProfiles2),
    m_pProfile(nullptr)
{
    ui->setupUi(this);

    // Disable 'update name' button if entered name not unique
    QObject::connect(ui->profileName, &QLineEdit::textChanged, this, [this](){
            ui->updateProfileNameButton->setEnabled(isProfileNameUnique());
    });

    // En/Dis-abler: Switch add/update timesegment depending on hours/mins spinboxes
    QObject::connect(ui->newSegmentHours, QOverload<int>::of(&QSpinBox::valueChanged), this, &PersonalProfiles2::onAddSegmentTimeChanged);
    QObject::connect(ui->newSegmentMins, QOverload<int>::of(&QSpinBox::valueChanged), this, &PersonalProfiles2::onAddSegmentTimeChanged);

    // Updater: show segment info in spinboxes when a segment is selected
    // + En/Dis-abler: disable 'remove segment' button when the first (mandatory) time segment is selected
    // For convenience, also do it when double clicking one.
    QObject::connect(ui->segmentTimes, &QListWidget::itemSelectionChanged, this, &PersonalProfiles2::onSegmentSelectionChanged);
    QObject::connect(ui->segmentTimes, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* i) {
        Q_UNUSED(i);
        onSegmentSelectionChanged();
    });

    // Handler: 'Done' button
    QObject::connect(ui->doneButton, &QPushButton::clicked, this, [this](){
        emit setPage("profiles1");
    });

    // Handler: 'Add Segment' and 'Update Segment' buttons
    QObject::connect(ui->addSegmentButton, &QPushButton::clicked, this, &PersonalProfiles2::upsertProfileSegment);
    QObject::connect(ui->updateSegmentButton, &QPushButton::clicked, this, &PersonalProfiles2::upsertProfileSegment);

    // Handler: 'Delete Profile' button
    QObject::connect(ui->deleteProfileButton, &QPushButton::clicked, this, [this](){
        m_pDevice->deleteProfile(m_pDevice->getProfileToEdit());
        emit setPage("profiles1");
    });

    // Handler: 'Update Profile Name' button
    QObject::connect(ui->updateProfileNameButton, &QPushButton::clicked, this, [this]() {
        if (isProfileNameUnique())
        {
            m_pProfile->setName(ui->profileName->text().toStdString());
        }
    });

    // Handler: 'Remove Segment' button
    QObject::connect(ui->removeSegmentButton, &QPushButton::clicked, this, [this]() {
        QListWidgetItem* selected = ui->segmentTimes->currentItem();
        if (selected && selected->data(Qt::UserRole) == 0)
        {
            ui->removeSegmentButton->setEnabled(false);
        }
        else
        {
            ui->removeSegmentButton->setEnabled(true);
        }
    });
}

PersonalProfiles2::~PersonalProfiles2()
{
    delete ui;
}

void PersonalProfiles2::updateUI()
{
    // reset all stored ptrs on load, re-fetch them later.
    m_pProfile = nullptr;

    // reset volatile buttons purely so nothing dumb can happen without checks happening again
    ui->removeSegmentButton->setDisabled(true);
    ui->addSegmentButton->setDisabled(true);
    ui->updateProfileNameButton->setDisabled(true);

    // leaving update button specifically in case they happen to want to edit whatever is already typed in
    ui->updateSegmentButton->setEnabled(true);

    // called each time this page is shown on screen (by the page engine).
    if (!m_pDevice->getProfileToEdit())
    {
        qInfo("[PersonalProfiles2]: Error: No profile to edit. Redirecting to profile selection page.");
        emit setPage("profiles1");
        return;
    }

    m_pProfile = m_pDevice->getProfileToEdit();

    // Put selected profile name into profile name box
    ui->profileName->setText(QString::fromStdString(m_pProfile->getName()));

    // populate timesegments list
    reloadSegmentsList();

    // pre-select time-segment zero and load it
    ui->segmentTimes->setCurrentItem(ui->segmentTimes->item(0));
}

bool PersonalProfiles2::isProfileNameUnique() const
{
    std::string enteredName = ui->profileName->text().toStdString();

    // Disallow the default name being used, at all.
    if (enteredName == DEFAULT_PROFILE_NAME || enteredName.empty()) return false;

    // returns true if no profiles other than m_pProfile (i.e. self) have the same name.
    for (const Profile& p : m_pDevice->getProfiles())
    {
        if (p.getName() == enteredName && (&p != m_pProfile))
        {
            return false;
        }
    }

    return true;
}

bool PersonalProfiles2::isSegmentValuesValid() const
{
    bool bOk = true;

    bOk = bOk && (ui->basalInsulinDeliveryValue->value() > 0);
    bOk = bOk && (ui->correctionFactorValue->value() > 0);
    bOk = bOk && (ui->insulinToCarbRatioValue->value() > 0);
    bOk = bOk && (ui->targetBGValue->value() > 0);

    return bOk;
}

void PersonalProfiles2::onAddSegmentTimeChanged(int _)
{
    Q_UNUSED(_);

    int hrs = ui->newSegmentHours->value();
    int mins = ui->newSegmentMins->value();

    if (m_pProfile->getSegment( (MINS_PER_HOUR * hrs) + mins ))
    {
        ui->updateSegmentButton->setEnabled(true);
        ui->addSegmentButton->setEnabled(false);
    }
    else
    {
        ui->updateSegmentButton->setEnabled(false);
        ui->addSegmentButton->setEnabled(true);
    }
}

void PersonalProfiles2::onSegmentSelectionChanged()
{
    QListWidgetItem* selected = ui->segmentTimes->currentItem();

    // guard against removing the mandatory 00:00 segment
    if (!selected)
    {
        ui->removeSegmentButton->setEnabled(false);
        return;
    }


    // enable remove button if needed, and populate inputs with segment values
    ProfileSegment* ps = m_pProfile->getSegment(selected->data(Qt::UserRole).toInt());
    if (!ps) return;

    if (ps->getStartTime() != 0) ui->removeSegmentButton->setEnabled(true);

    ui->newSegmentHours->setValue(Time(ps->getStartTime()).getHours());
    ui->newSegmentMins->setValue(Time(ps->getStartTime()).getMinutes());
    ui->basalInsulinDeliveryValue->setValue(ps->getHourlyInsulinBasalRate());
    ui->insulinToCarbRatioValue->setValue(ps->getCarbRatio());
    ui->correctionFactorValue->setValue(ps->getCorrectionFactor());
    ui->targetBGValue->setValue(ps->getTargetGlucose());
}


void PersonalProfiles2::upsertProfileSegment()
{
    // update or insert profile segment. will not do this if the values are not valid.
    if (!isSegmentValuesValid()) return;

    const int start = (MINS_PER_HOUR * ui->newSegmentHours->value()) + ui->newSegmentMins->value();
    const int basal = ui->basalInsulinDeliveryValue->value();
    const int corr = ui->correctionFactorValue->value();
    const int carbRatio = ui->insulinToCarbRatioValue->value();
    const int target = ui->targetBGValue->value();

    // Remove before re-adding to basically do an upsert into the segments vector
    m_pProfile->removeSegment(start);
    m_pProfile->addSegment(start, basal, carbRatio, corr, target);

    reloadSegmentsList();
}

void PersonalProfiles2::reloadSegmentsList()
{
    ui->segmentTimes->clear();
    for (const ProfileSegment& ps : m_pProfile->getSegments())
    {
        QListWidgetItem* item = new QListWidgetItem(Time(ps.getStartTime()).toHHMMString().c_str());
        item->setData(Qt::UserRole, ps.getStartTime());
        ui->segmentTimes->addItem(item);
    }
}
