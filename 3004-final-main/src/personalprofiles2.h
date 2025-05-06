#ifndef PERSONALPROFILES2_H
#define PERSONALPROFILES2_H

#include "abstractpage.h"
#include "Profile.h"
namespace Ui {
class PersonalProfiles2;
}

class PersonalProfiles2 : public AbstractPage
{
    Q_OBJECT

public:
    explicit PersonalProfiles2(QWidget *parent = nullptr);
    ~PersonalProfiles2();

    void updateUI();

private:
    Ui::PersonalProfiles2 *ui;
    Profile* m_pProfile;

    bool isProfileNameUnique() const;
    bool isSegmentValuesValid() const;
    void reloadSegmentsList();

private slots:
    void onAddSegmentTimeChanged(int _);
    void upsertProfileSegment();
    void onSegmentSelectionChanged();
};

#endif // PERSONALPROFILES2_H
