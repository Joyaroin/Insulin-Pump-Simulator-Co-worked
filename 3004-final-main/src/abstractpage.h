#ifndef ABSTRACTPAGE_H
#define ABSTRACTPAGE_H
#include <string>
#include <QWidget>
#include "Device.h"

class AbstractPage : public QWidget
{
    Q_OBJECT
public:
    AbstractPage(QWidget *parent = nullptr);

    virtual void connectToDevice(Device& device);
    virtual void updateUI();

signals:
    void setPage(const std::string pageName);
    void requestKeypad();
    void requestNumpad();

public slots:
    virtual void onKeypadValueReceived(AbstractPage* page, const std::string& value);
    virtual void onNumpadValueReceived(AbstractPage* page, int value);

protected:
    Device* m_pDevice;
};

#endif // ABSTRACTPAGE_H
