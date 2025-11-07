//
// Created by Martin Knopf on 2025-11-07.
//

#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QResizeEvent>

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr);

    void setTotalGlasses(int count);
    void setGlassesDrunk(int count);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void recalcLayout();

    int m_total = 0;
    int m_drunk = 0;
    int m_iconsPerRow = 1;
    int m_iconSize = 32;
    QPixmap m_full;
    QPixmap m_empty;
};