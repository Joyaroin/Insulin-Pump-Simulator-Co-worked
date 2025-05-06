#include "abstractpage.h"
#include <string>

AbstractPage::AbstractPage(QWidget *parent)
    : QWidget(parent)
    , m_pDevice(nullptr)
{
}

void AbstractPage::onKeypadValueReceived(AbstractPage* page, const std::string& value)
{
    Q_UNUSED(page);
    Q_UNUSED(value);
}

void AbstractPage::onNumpadValueReceived(AbstractPage* page, int value)
{
    Q_UNUSED(page);
    Q_UNUSED(value);
}

void AbstractPage::updateUI()
{
    // Optionally overridden in derived classes.
}

void AbstractPage::connectToDevice(Device& device)
{
    m_pDevice = &device;
}
