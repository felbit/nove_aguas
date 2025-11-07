//
// Created by Martin Knopf on 2025-11-06.
//

#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimeEdit>
#include <QTimer>

#include "ProgressWidget.h"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppWindow(QWidget* parent = nullptr);

private slots:
    // slots declaration is technically not needed anymore in modern Qt,
    // but it seems to be good practice to be explicit about the intended
    // use of slot functions
    void onStartTracking();
    void onUpdateCountdown();
    void onDrinkNow();

private:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void saveState();
    void loadState();
    void resetForNewDay();
    void handleDrinkAt(const QDateTime& when);
    bool recalculateNextFrom(const QDateTime& base, int remainingGlassesOverride);
    void updateStatusText(const QString& message);
    void updateProgressIcons();
    void updateButtonState();

    QLabel* statusLbl;
    QPushButton* actionButton;
    QTimer timer;

    ProgressWidget* progressWidget;
    // QVBoxLayout* progressLayout;
    // std::vector<QLabel*> glassIcons;

    int totalGlasses = 0;
    int glassesDrunk = 0;
    QDateTime startTime;
    QDateTime lastGlassTime;
    QDateTime nextGlassTime;
    qint64 intervalMinutes = 0;
    bool trackingActive = false;
    bool drinkDue = false;
};
