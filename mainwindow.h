#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include "qmibitem.h"

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

    void clearRightPane();
    void populateRightPane(QMibItem *node);
private slots:
    void on_action_Charger_une_MIB_triggered();
    void selectedLineChanged(QItemSelection, QItemSelection);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
