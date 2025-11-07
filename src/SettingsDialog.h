//
// Created by Martin Knopf on 2025-11-06.
//

#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QTimeEdit>
#include <QPushButton>
#include <QFormLayout>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    int glassesPerDay() const;
    QTime lastGlassTime() const;

private:
    QSpinBox* glassesInput;
    QTimeEdit* lastGlassInput;
};
