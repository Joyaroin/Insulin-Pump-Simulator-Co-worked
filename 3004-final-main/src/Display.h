#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QString>

class Display : public QObject
{
    Q_OBJECT
    // A simple property to hold “field”
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit Display(QObject* parent = nullptr);

    QString text() const;

public slots:
    void setText(const QString &newText);

    // Example method
    Q_INVOKABLE void showMessage(const QString &message);

signals:
    void textChanged(const QString &newText);

private:
    QString m_text;
};

#endif // DISPLAY_H
