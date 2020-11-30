#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

#pragma pack(push)
#pragma  pack(1)
struct FIRMWARE_HEADER {
    unsigned int size;
    unsigned char version[3];
    unsigned char MD5[16];
};
#pragma  pack(pop)

#define HEADER FIRMWARE_HEADER


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    struct HEADER m_Header;
};

#endif // MAINWINDOW_H
