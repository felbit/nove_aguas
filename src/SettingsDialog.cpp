//
// Created by Martin Knopf on 2025-11-06.
//

#include "SettingsDialog.h"

#include <QSettings>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Settings");
    resize(250, 150);

    auto* form = new QFormLayout(this);

    glassesInput = new QSpinBox(this);
    glassesInput->setRange(1,20);

    lastGlassInput = new QTimeEdit(this);
    lastGlassInput->setDisplayFormat("HH:mm");

    QSettings settings("Coding Gecko", "Nove Aguas");
    glassesInput->setValue(settings.value("glasses", 8).toInt());
    lastGlassInput->setTime(settings.value("lastGlassTime", QTime(21, 00)).toTime());

    form->addRow("Glasses per day:", glassesInput);
    form->addRow("When do you drink the last glass?", lastGlassInput);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    form->addRow(buttons);
}

int SettingsDialog::glassesPerDay() const {
    return glassesInput->value();
}

QTime SettingsDialog::lastGlassTime() const {
    return lastGlassInput->time();
}


