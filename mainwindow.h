#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SCALE_W_ 8
#define SCALE_H_ 8
#define LCD_SCREEN_WIDTH 128
#define LCD_SCREEN_HEIGHT 64

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
