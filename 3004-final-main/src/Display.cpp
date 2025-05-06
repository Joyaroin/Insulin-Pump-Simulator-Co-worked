#include "Display.h"
#include <QDebug>

Display::Display(QObject* parent)
    : QObject(parent)
{
}

QString Display::text() const
{
    return m_text;
}

void Display::setText(const QString &newText)
{
    if (m_text == newText)
        return;
    m_text = newText;
    emit textChanged(m_text);
}

void Display::showMessage(const QString &message)
{
    // This could set m_text, log to a console, update UI, etc.
    qDebug() << "[DISPLAY] Showing message:" << message;
    setText(message);
}
