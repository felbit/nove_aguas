#include <QApplication>
#include "AppWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    const QString style = R"(
        QWidget {
            background-color: #f5f6fa;
            color: #2f3640;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            font-size: 14px;
        }

        QMainWindow {
            background-color: #f5f6fa;
        }

        QLabel {
            color: #2f3640;
        }

        QSpinBox, QTimeEdit {
            border: 1px solid #dcdde1;
            border-radius: 6px;
            padding: 4px 6px;
            background-color: #ffffff;
            selection-background-color: #9c88ff;
            selection-color: #ffffff;
        }

        QPushButton {
            background-color: #4070f4;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 18px;
            font-weight: 600;
            font-size: 15px;
        }

        QPushButton:hover {
            background-color: #5b82f8;
        }

        QPushButton:pressed {
            background-color: #345de3;
        }

        QPushButton[text~="⚙️"] {
            background-color: #e1e5f2;
            color: #2f3640;
            font-size: 16px;
            font-weight: 600;
            border-radius: 8px;
            min-width: 36px;
        }

        QLabel#statusLabel {
            font-size: 15px;
            font-weight: 600;
            color: #273c75;
        }

        QWidget#progressWidget {
            background-color: #ffffff;
            border: 1px solid #dcdde1;
            border-radius: 10px;
            padding: 8px;
        }
    )";
    app.setStyleSheet(style);

    AppWindow window;
    window.show();

    return app.exec();
}