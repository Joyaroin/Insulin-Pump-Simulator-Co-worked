#ifndef PAGEWINDOW_H
#define PAGEWINDOW_H

#include <QWidget>
#include <QMap>
#include "abstractpage.h"
#include <string>

/*
 * This class manages the switching of pages (on the device, in this project's case).
 * Any widgets passed into this class via addPage(...) also pass ownership of the widget to this class.
 */

namespace Ui {
class PageWindow;
}

class PageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PageWindow(Device& device, QWidget *parent = nullptr);
    ~PageWindow();

    /*
     * Add a page to the pageStack on screen.
     * The pageStack takes ownership of the pointer, and will delete the added widget when the stack is destroyed.
     * A page won't be added if widget is null or the id already exists in the stack.
     */
    void addPage(const std::string &id, AbstractPage *widget);

    void showPage(const std::string &id);

private:
    Ui::PageWindow *ui;
    QMap<std::string, AbstractPage*> m_pageMap;
    Device& m_device;

    /*
     * Creates and adds all the device's pages to the page manager map.
     */
    void addAllPages();

private slots:
    void onShowPage(const std::string &id);

};

#endif // PAGEWINDOW_H
