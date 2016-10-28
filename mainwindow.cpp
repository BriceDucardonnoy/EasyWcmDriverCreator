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
//    loadMib("/home/bdy/WorldcastSystems/WorldCastManager/drivers/DB6400-MIB.mib");
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
    connect(ui->mibTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectedLineChanged(QItemSelection,QItemSelection)));
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

void MainWindow::selectedLineChanged(QItemSelection selected, QItemSelection deselected)
{
    if(deselected.isEmpty()) { ; }// To remove unused warning
    if(selected.isEmpty())
    {
        clearRightPane();
    }
    else
    {
        populateRightPane(((QTreeMibModel*) ui->mibTreeView->model())->getMibNodeFromIndex(selected.indexes().first()));
    }
}

void MainWindow::clearRightPane()
{

}

void MainWindow::populateRightPane(QMibItem *node)
{
//    QList<QLineEdit *> allLineEdits = ui->widget->findChildren<QLineEdit *>();
    if(node == NULL)
    {
        clearRightPane();
        return;
    }
    qInfo() << node->toString();
    ui->widget->findChild<QCheckBox *>("dansLaMIBCheckBox")->setChecked(true);
    ui->widget->findChild<QCheckBox *>("mesureCheckBox")->setChecked(node->getAsnBasicType() == QMibItem::Gauge);
    QLineEdit *name = ui->widget->findChild<QLineEdit *>("nomLineEdit");
    if(name != NULL)
    {
        name->setText(node->getName());
    }
    QLineEdit *oid = ui->widget->findChild<QLineEdit *>("oidLineEdit");
    if(oid != NULL)
    {
        oid->setText(node->getOid());
    }
}
