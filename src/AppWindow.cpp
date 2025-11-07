//
// Created by Martin Knopf on 2025-11-06.
//

#include "AppWindow.h"
#include "SettingsDialog.h"
#include "ProgressWidget.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QSettings>


AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Nove Copos de Água");
    setMinimumSize(320, 320);

    // interesting to know:
    // as long as we parent objects with QObjects, we get memory management "for free".
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setSpacing(12);
    layout->setContentsMargins(24, 24, 24, 24);

    statusLbl = new QLabel("Press Start to begin tracking!", this);
    statusLbl->setObjectName("statusLabel");
    statusLbl->setAlignment(Qt::AlignCenter);
    statusLbl->setWordWrap(true);
    statusLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusLbl->setMinimumHeight(50);

    // progressWidget = new QWidget(this);
    // progressWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // progressLayout = new QVBoxLayout(progressWidget);
    // progressLayout->setSpacing(6);
    // progressLayout->setAlignment(Qt::AlignCenter);

    auto* buttonRow = new QWidget(this);
    auto* buttonRowLayout = new QHBoxLayout(buttonRow);
    buttonRowLayout->setAlignment(Qt::AlignCenter);
    buttonRowLayout->setSpacing(0);
    buttonRowLayout->setContentsMargins(0, 0, 0, 0);

    actionButton = new QPushButton("Start", this);
    connect(actionButton, &QPushButton::clicked, this, [this]() {
        if (!trackingActive) {
            onStartTracking();
        } else {
            onDrinkNow();
        }
    });

    auto* settingsBtn = new QPushButton("⚙️", this);
    settingsBtn->setFixedSize(36, 36);
    settingsBtn->setToolTip("Settings");
    settingsBtn->setStyleSheet(
        "QPushButton { background-color: #e1e5f2; border-radius: 6px; } "
        "QPushButton:hover { background-color: #cdd2e0; }"
    );

    connect(settingsBtn, &QPushButton::clicked, this, [this]() {
        if (SettingsDialog dialog(this); dialog.exec() == QDialog::Accepted) {
            QSettings settings("Coding Gecko", "Nove Aguas");

            const int newGoal = dialog.glassesPerDay();
            const QTime newLastTime = dialog.lastGlassTime();

            settings.setValue("glasses", dialog.glassesPerDay());
            settings.setValue("lastGlassTime", QDateTime(QDate::currentDate(), dialog.lastGlassTime()));

            const bool goalChanged = (newGoal != totalGlasses);
            const bool endTimeChanged = (newLastTime != lastGlassTime.time());

            totalGlasses = newGoal;
            lastGlassTime = QDateTime(QDate::currentDate(), newLastTime);

            if (trackingActive) {
                if (glassesDrunk > totalGlasses) {
                    trackingActive = false;
                    timer.stop();
                    updateStatusText("Goal reached for today!");
                } else {
                    const int remainingGlasses = totalGlasses - glassesDrunk;
                    if (recalculateNextFrom(QDateTime::currentDateTime(), remainingGlasses)) {
                        updateStatusText(QString("Goal updated to %1 glasses. %2 remaining.").arg(totalGlasses).arg(remainingGlasses));
                        if (!timer.isActive())
                            timer.start(1000);
                    } else {
                        trackingActive = false,
                        timer.stop();
                        updateStatusText("We seemed to have entered a time loop!");
                    }
                }
            } else {
                updateStatusText("Settings updated. Press 'Start' to begin tracking!");
            }

            updateProgressIcons();
            updateButtonState();
            saveState();
        }
    });

    buttonRowLayout->addWidget(actionButton);
    buttonRowLayout->addWidget(settingsBtn);

    progressWidget = new ProgressWidget(this);

    layout->addWidget(progressWidget);
    layout->addWidget(statusLbl);
    layout->addWidget(buttonRow);

    setCentralWidget(central);
    adjustSize();

    connect(&timer, &QTimer::timeout, this, &AppWindow::onUpdateCountdown);

    loadState();
}

void AppWindow::handleDrinkAt(const QDateTime &when) {
    drinkDue = false;
    glassesDrunk++;

    if (glassesDrunk >= totalGlasses) {
        trackingActive = false;
        timer.stop();
        updateStatusText("All glasses done for today.");
        updateProgressIcons();
        updateButtonState();
        saveState();
        return;
    }

    if (const bool ok = recalculateNextFrom(when, -1); !ok) {
        trackingActive = false;
        timer.stop();
        updateStatusText("End time reached.");
    } else {
        if (!timer.isActive())
            timer.start(1000);
    }

    updateProgressIcons();
    updateButtonState();
    saveState();
}

bool AppWindow::recalculateNextFrom(const QDateTime &base, int remainingGlassesOverride) {
    if (base >= lastGlassTime) return false;

    const int remainingGlasses = (remainingGlassesOverride > 0)
        ? remainingGlassesOverride
        : totalGlasses - glassesDrunk;

    if (remainingGlasses <= 0) return false;

    const qint64 minsUntilLast = base.secsTo(lastGlassTime) / 60;
    intervalMinutes = std::max<qint64>(1, minsUntilLast / remainingGlasses);
    nextGlassTime = base.addSecs(intervalMinutes * 60);

    return true;
}

void AppWindow::saveState() {
    QSettings settings("Coding Gecko", "Nove Aguas");

    settings.setValue("sessionDate", QDate::currentDate());
    settings.setValue("glasses", totalGlasses);
    settings.setValue("glassesDrunk", glassesDrunk);
    settings.setValue("lastGlassTime", lastGlassTime);
    settings.setValue("startTime", startTime);
    settings.setValue("nextGlassTime", nextGlassTime);
    settings.setValue("intervalMinutes", intervalMinutes);
    settings.setValue("trackingActive", trackingActive);
    settings.setValue("drinkDue", drinkDue);
}

void AppWindow::loadState() {
    const QSettings settings("Coding Gecko", "Nove Aguas");

    if (!settings.contains("glasses")) {
        updateStatusText("Press Start to begin tracking!");
        return;
    }

    const QDate savedDate = settings.value("sessionDate").toDate();
    const QDate today = QDate::currentDate();

    const int lastGoal = settings.value("glasses", 8).toInt();

    if (savedDate != today) {
        resetForNewDay();
        return;
    }

    totalGlasses = settings.value("glasses").toInt();
    glassesDrunk = settings.value("glassesDrunk").toInt();
    lastGlassTime = settings.value("lastGlassTime").toDateTime();
    startTime = settings.value("startTime").toDateTime();
    nextGlassTime = settings.value("nextGlassTime").toDateTime();
    intervalMinutes = settings.value("intervalMinutes").toLongLong();
    trackingActive = settings.value("trackingActive", false).toBool();
    drinkDue = settings.value("drinkDue", false).toBool();

    if (trackingActive) {
        if (drinkDue) {
            timer.stop();
            updateStatusText("Time to drink!");
        } else {
            if (const QDateTime now = QDateTime::currentDateTime(); now > nextGlassTime) {
                drinkDue = true;
                timer.stop();
                updateStatusText("Time to drink!");
            } else {
                timer.start(1000);
                updateStatusText("Resumed tracking... ");
            }
        }
    } else {
        updateStatusText("Press Start to begin tracking.");
    }

    updateButtonState();
}

void AppWindow::resetForNewDay() {
    glassesDrunk = 0;
    trackingActive = false;
    timer.stop();

    updateStatusText("New day detected. Set your goal for today and press Start!");

    QSettings settings("Coding Gecko", "Nove Aguas");
    const int lastGoal = settings.value("glasses", 8).toInt();
    settings.clear();
    settings.setValue("glasses", lastGoal);

    updateProgressIcons();
    updateButtonState();
}

void AppWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    static int lastWidth = 0;

    if (const int newWidth = event->size().width();
        std::abs(newWidth - lastWidth) > 10)
    {
        progressWidget->setUpdatesEnabled(false); // prevent flickering
        updateProgressIcons();
        progressWidget->setUpdatesEnabled(true); // prevent flickering

        lastWidth = newWidth;
    }
}

void AppWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    static bool initialized = false;
    if (!initialized) {
        updateProgressIcons();
        initialized = true;
    }
}

void AppWindow::updateButtonState() {
    if (trackingActive)
        actionButton->setText("Drink Now");
    else
        actionButton->setText("Start");
}

void AppWindow::updateProgressIcons() {
    progressWidget->setTotalGlasses(totalGlasses);
    progressWidget->setGlassesDrunk(glassesDrunk);
    // if (!progressLayout) return;
    //
    // auto* newLayout = new QVBoxLayout();
    // newLayout->setSpacing(6);
    // newLayout->setAlignment(Qt::AlignCenter);
    //
    // if (totalGlasses <= 0) {
    //     progressWidget->setLayout(newLayout);
    //     return;
    // }
    //
    // const int widgetWidth = progressWidget->width();
    // constexpr int iconSize = 32;
    // const int spacing = newLayout->spacing();
    // const int iconWithSpacing = iconSize + spacing * 2;
    //
    // int iconsPerRow = std::max(1, widgetWidth / iconWithSpacing);
    //
    // if (iconsPerRow % 2 != totalGlasses % 2 && iconsPerRow>1)
    //     iconsPerRow -= 1;
    //
    // int remaining = totalGlasses;
    // int glassIndex = 0;
    //
    // while (remaining > 0) {
    //     const int countThisRow = std::min(iconsPerRow, remaining);
    //
    //     auto* rowLayout = new QHBoxLayout();
    //     rowLayout->setAlignment(Qt::AlignCenter);
    //     rowLayout->setSpacing(spacing);
    //
    //     for (int i = 0; i < countThisRow; ++i) {
    //         auto *glass = new QLabel(progressWidget);
    //         glass->setAlignment(Qt::AlignCenter);
    //
    //         const QString path = (glassIndex < glassesDrunk)
    //             ? ":/assets/glass_full.png"
    //             : ":/assets/glass_empty.png";
    //
    //         QPixmap iconPixmap(path);
    //         glass->setPixmap(iconPixmap.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //         glass->setStyleSheet("margin: 2px;");
    //
    //         rowLayout->addWidget(glass);
    //         glassIcons.push_back(glass);
    //
    //         ++glassIndex;
    //     }
    //
    //     newLayout->addLayout(rowLayout);
    //     remaining -= countThisRow;
    // }
    //
    // // this seems excessive ...
    // delete progressWidget->layout();
    // progressWidget->setLayout(newLayout);
    // progressLayout = newLayout;
    // progressWidget->update();
}

void AppWindow::updateStatusText(const QString &message) {
    statusLbl->setText(message);
}

void AppWindow::onDrinkNow() {
    if (!trackingActive) return;
    handleDrinkAt(QDateTime::currentDateTime());
}

void AppWindow::onStartTracking() {
    QSettings settings("Coding Gecko", "Nove Aguas");
    totalGlasses = settings.value("glasses", 8).toInt();

    lastGlassTime = settings.value(
        "lastGlassTime",
        QDateTime(QDate::currentDate(), QTime(21, 0))
        ).toDateTime();

    const QDateTime now = QDateTime::currentDateTime();
    if (lastGlassTime <= now) {
        QMessageBox::warning(this, "Invalid Time",
                             "Please choose a time later than the current time.");
        return;
    }

    startTime = now;
    glassesDrunk = 0;
    trackingActive = true;

    const qint64 minutesUntilLast = now.secsTo(lastGlassTime) / 60;
    intervalMinutes = minutesUntilLast / totalGlasses;
    nextGlassTime = startTime.addSecs(intervalMinutes * 60);

    timer.start(1000);
    updateButtonState();
    updateStatusText(
    QString("Tracking %1 glasses today. First reminder in %2 min.")
        .arg(totalGlasses).arg(intervalMinutes));
    updateProgressIcons();
    saveState();
}

void AppWindow::onUpdateCountdown() {
    if (!trackingActive) return;

    const QDateTime now = QDateTime::currentDateTime();
    const qint64 secondsLeft = now.secsTo(nextGlassTime);

    if (secondsLeft <= 0) {
        drinkDue = true;
        timer.stop();
        updateStatusText("Time to Drink!");
        saveState();
    } else {
        const int minutes = secondsLeft / 60;
        const int seconds = secondsLeft % 60;
        updateStatusText(
            QString("Next glass in %1m %2s").arg(minutes).arg(seconds));
    }
}
