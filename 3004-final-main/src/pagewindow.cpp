#include "pagewindow.h"
#include "homepage.h"
#include "ui_pagewindow.h"
#include "abstractpage.h"
#include "options1.h"
#include "mypump1.h"
#include "personalprofiles1.h"
#include "personalprofiles2.h"
#include "sleepscreen.h"
#include "manualbolus1.h"

PageWindow::PageWindow(Device& device, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PageWindow)
    , m_device(device)
{
    ui->setupUi(this);

    addAllPages();

    // Default to lockscreen
    showPage("sleep");
}

PageWindow::~PageWindow()
{
    delete ui;
}

void PageWindow::addAllPages()
{
    addPage("HOME", new HomePage());
    addPage("options1", new Options1());
    addPage("mypump1", new MyPump1());
    addPage("profiles1", new PersonalProfiles1());
    addPage("profiles2", new PersonalProfiles2());
    addPage("sleep", new SleepScreen());
    addPage("bolus1", new ManualBolus1());
}

void PageWindow::addPage(const std::string &id, AbstractPage *widget)
{
    if (!widget || m_pageMap.contains(id))
        return;

    // Add to map (map takes ownership of widget lifetimes)
    ui->pageStack->addWidget(widget);
    m_pageMap[id] = widget;

    // Connect to page-switcher slots, and make any connections to device if necessary
    connect(widget, &AbstractPage::setPage, this, &PageWindow::showPage);

    widget->connectToDevice(m_device);
}

void PageWindow::showPage(const std::string &id)
{
    if (!m_pageMap.contains(id))
    {
        qInfo("[PageWindow]: Cannot switch to page %s. Page does not exist. (Was it added to the PageWindow map?)", id.c_str());
        return;
    }
    else
    {
        m_pageMap[id]->updateUI();
    }

    ui->pageStack->setCurrentWidget(m_pageMap[id]);
}

void PageWindow::onShowPage(const std::string &id)
{
    showPage(id);
}

