#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), model(NULL)
{
    ui->setupUi(this);

//    connect(ui->action_Charger_une_MIB,SIGNAL(triggered(bool)),this,SLOT(on_action_Charger_une_MIB_triggered()));
    ui->centralWidget->addActions(ui->menu_Menu->actions());

    // TODO BDY: remove the following code
    loadMib("/home/bdy/Téléchargements/ECRESO-FM-TRANS-MIB.mib");
//    loadMib("/home/bdy/WorldcastSystems/WorldCastManager/drivers/DB6400-MIB.mib");
    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadMib(QString mibPath)
{
    // Create model
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(model != NULL)
    {
        delete(model);
        model = NULL;
    }
    model = new QTreeMibModel(mibPath);
    ui->mibTreeView->setModel(model);
    ui->mibTreeView->setColumnWidth(0, 400);
    ui->mibTreeView->setColumnWidth(1, 250);
    ui->mibTreeView->setColumnWidth(2, 500);
    ui->mibTreeView->setAlternatingRowColors(true);
//    ui->mibTreeView->expandAll();
    ui->mibTreeView->expandToDepth(4);
    connect(ui->mibTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectedLineChanged(QItemSelection,QItemSelection)));
    ui->driverNameLineEdit->setText(model->getModuleName());
    ui->refreshRateSpinBox->setValue(15);
    ui->vendorLineEdit->setText(model->getVendor());
    on_measureCheckBox_stateChanged(ui->measureCheckBox->isChecked() == true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_action_Charger_une_MIB_triggered()
{
    qDebug("Load MIB");
    QString filename = QFileDialog::getOpenFileName(this, tr("Choisir une MIB"), /*""*/ QDir::homePath(), tr("MIB file (*.txt *.mib)"));
    qInfo() << "Filename is " << filename;
    loadMib(filename);
}

void MainWindow::selectedLineChanged(QItemSelection selected, QItemSelection deselected)
{
    if(!deselected.isEmpty())
    {
        saveNodeUpdates(model->getMibNodeFromIndex(selected.indexes().first()));
    }// To remove unused warning
    if(selected.isEmpty())
    {
        clearRightPane();
    }
    else
    {
        populateRightPane(model->getMibNodeFromIndex(selected.indexes().first()));
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
        qWarning() << "NULL NODE";
        clearRightPane();
        return;
    }
    qInfo() << node->toString();
    /*
     * Populate
     */
    ui->inMIBCheckBox->setChecked(node->getMib().compare("EmptyMib") != 0);
    if(node->getWcsType() == QMibItem::Unset)
    {
        ui->measureCheckBox->setChecked(node->getAsnBasicType() == QMibItem::Gauge);
    }
    else
    {
        ui->measureCheckBox->setChecked(node->getWcsType() == QMibItem::IdentifierReading);
    }
    // Name
    ui->nameLineEdit->setText(node->getName());
    // OID
    ui->oidLineEdit->setText(node->getOid());
    // Type
//    ui->typeComboBox->
    // TODO BDY
    // Min
    ui->minSpinBox->setValue(node->getMin());
    ui->minSpinBox->setMinimum(node->getMin());
    ui->minSpinBox->setMaximum(node->getMax());
    // Max
    ui->maxSpinBox->setValue(node->getMax());
    ui->maxSpinBox->setMinimum(node->getMin());
    ui->maxSpinBox->setMaximum(node->getMax());
    // Units
    ui->unitLineEdit->setText(node->getUnit());
}

void MainWindow::saveNodeUpdates(QMibItem *node)
{
    if(node == NULL)
    {
        return;
    }
    if(true) return;
    node->setWcsType(ui->measureCheckBox->isChecked() ? QMibItem::IdentifierReading : QMibItem::Identifier);
    node->setMib(ui->inMIBCheckBox->isChecked() ? model->getModuleName() : "EmptyMib");
    // Name
    ui->nameLineEdit->setText(node->getName());
    // OID
    ui->oidLineEdit->setText(node->getOid());
    // Type
//    ui->typeComboBox->
    // TODO BDY
    // Min
    ui->minSpinBox->setValue(node->getMin());
    ui->minSpinBox->setMinimum(node->getMin());
    ui->minSpinBox->setMaximum(node->getMax());
    // Max
    ui->maxSpinBox->setValue(node->getMax());
    ui->maxSpinBox->setMinimum(node->getMin());
    ui->maxSpinBox->setMaximum(node->getMax());
    // Units
    ui->unitLineEdit->setText(node->getUnit());
}

void MainWindow::on_measureCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0)// Unchecked
    {
        ui->identifierReadingWidgets->hide();
        ui->identifierWidgets->show();
    }
    else
    {
        ui->identifierWidgets->hide();
        ui->identifierReadingWidgets->show();
    }
}

void MainWindow::on_action_Sauver_le_Driver_triggered()
{
    // TODO BDY: open a file choose dialog
    QString filename = "/tmp/testDriver.json";
    qInfo() << "Export driver" << ((QTreeMibModel *) ui->mibTreeView->model())->getCheckedItem();
}
