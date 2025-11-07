//
// Created by Martin Knopf on 2025-11-07.
//

#include "ProgressWidget.h"

#include <QPainter>
#include <algorithm>

ProgressWidget::ProgressWidget(QWidget* parent)
    : QWidget(parent),
      m_full(":/assets/glass_full.png"),
      m_empty(":/assets/glass_empty.png")
{}

void ProgressWidget::setTotalGlasses(const int count) {
    if (m_total == count) return;
    m_total = count;
    recalcLayout();
    update();
}

void ProgressWidget::setGlassesDrunk(const int count) {
    m_drunk = std::clamp(count, 0, m_total);
    update();
}

void ProgressWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    recalcLayout();
}

void ProgressWidget::recalcLayout() {
    if (m_total <= 0) {
        m_iconsPerRow = 1;
        return;
    }

    const int w = width();
    constexpr int spacing = 6;
    const int iconWithSpacing = m_iconSize + spacing * 2;
    const int perRow = std::max(1, w / iconWithSpacing);

    m_iconsPerRow = std::max(1, perRow);
}

void ProgressWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int x = 0;
    int y = 0;

    constexpr int spacing = 6;
    const int iconsInLastRow = m_total % m_iconsPerRow;
    const int lastRowOffset = (m_iconsPerRow - iconsInLastRow) * (m_iconSize + spacing*2) / 2;

    for (int i = 0; i < m_total; ++i) {
        const bool isFull = (i < m_drunk);
        const QPixmap &pm = isFull ? m_full : m_empty;

        const int col = i % m_iconsPerRow;
        const int row = i / m_iconsPerRow;

        const int offset = ((m_total - 1) - i) < iconsInLastRow ? lastRowOffset : 0;

        x = offset + col * (m_iconSize + spacing*2) + spacing;
        y = row * (m_iconSize + spacing*2) + spacing;

        p.drawPixmap(x, y, pm);
    }
}
