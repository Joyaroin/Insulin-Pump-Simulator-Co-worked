#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Simulator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Simulator> sim;

    void makeConnections();

private slots:
    void onPauseButtonReleased();
    void onPlayButtonReleased();
    void onApplyGlucoseDiffButtonReleased();
    void onEmptyCartridgeButtonReleased();
    void onReplaceCartridgeButtonReleased();
    void onKillBatteryButtonReleased();
    void onChargeBatteryButtonReleased();
};

#endif // MAINWINDOW_H
