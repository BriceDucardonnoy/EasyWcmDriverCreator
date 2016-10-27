#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QFileDialog>

#include "qtreemibmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    connect(ui->action_Charger_une_MIB,SIGNAL(triggered(bool)),this,SLOT(on_action_Charger_une_MIB_triggered()));
    ui->centralWidget->addActions(ui->menu_Menu->actions());

    // TODO BDY: remove the following code
    loadMib("/home/bdy/Téléchargements/ECRESO-FM-TRANS-MIB.mib");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadMib(QString mibPath)
{
    // Create model
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTreeMibModel *tmm = new QTreeMibModel(mibPath);
    ui->mibTreeView->setModel(tmm);
    ui->mibTreeView->setColumnWidth(0, 400);
    ui->mibTreeView->setColumnWidth(1, 250);
    ui->mibTreeView->setColumnWidth(2, 500);
    ui->mibTreeView->setAlternatingRowColors(true);
//    ui->mibTreeView->expandAll();
    ui->mibTreeView->expandToDepth(4);
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_action_Charger_une_MIB_triggered()
{
    qDebug("Load MIB");
    QString filename = QFileDialog::getOpenFileName(this, tr("Choisir une MIB"), /*""*/ QDir::homePath(), tr("MIB file (*.txt *.mib)"));
    qInfo() << "Filename is " << filename;
//    ui->mibTreeView->
    loadMib(filename);
}
