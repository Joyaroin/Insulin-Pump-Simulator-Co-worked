#ifndef PERSONALPROFILES1_H
#define PERSONALPROFILES1_H

#include "abstractpage.h"

namespace Ui {
class PersonalProfiles1;
}

class PersonalProfiles1 : public AbstractPage
{
    Q_OBJECT

public:
    explicit PersonalProfiles1(QWidget *parent = nullptr);
    ~PersonalProfiles1();

private:
    Ui::PersonalProfiles1 *ui;

    void updateUI() override;
};

#endif // PERSONALPROFILES1_H
