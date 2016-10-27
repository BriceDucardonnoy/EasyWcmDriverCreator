#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadMib(QString mibPath);

private slots:
    void on_action_Charger_une_MIB_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
