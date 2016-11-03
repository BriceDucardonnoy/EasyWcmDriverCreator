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
//    loadMib("/home/bdy/Téléchargements/ECRESO-FM-TRANS-MIB.mib");
    loadMib("/home/bdy/WorldcastSystems/WorldCastManager/drivers/DB6400-MIB.mib");
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
    QTreeMibModel *tmm = new QTreeMibModel(mibPath);
    ui->mibTreeView->setModel(tmm);
    ui->mibTreeView->setColumnWidth(0, 400);
    ui->mibTreeView->setColumnWidth(1, 250);
    ui->mibTreeView->setColumnWidth(2, 500);
    ui->mibTreeView->setAlternatingRowColors(true);
//    ui->mibTreeView->expandAll();
    ui->mibTreeView->expandToDepth(4);
    connect(ui->mibTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectedLineChanged(QItemSelection,QItemSelection)));
    ui->driverNameLineEdit->setText(tmm->getModuleName());
    ui->refreshRateSpinBox->setValue(15);
    ui->vendorLineEdit->setText(tmm->getVendor());
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
    /*
     * Populate
     */
    ui->widgets->findChild<QCheckBox *>("inMIBCheckBox")->setChecked(true);
    ui->widgets->findChild<QCheckBox *>("measureCheckBox")->setChecked(node->getAsnBasicType() == QMibItem::Gauge);
    // Name
    QLineEdit *lineEdit = ui->widgets->findChild<QLineEdit *>("nameLineEdit");
    if(lineEdit != NULL)
    {
        lineEdit->setText(node->getName());
    }
    // OID
    lineEdit = ui->widgets->findChild<QLineEdit *>("oidLineEdit");
    if(lineEdit != NULL)
    {
        lineEdit->setText(node->getOid());
    }
    // Type
    QComboBox *cbox = ui->identifierWidgets->findChild<QComboBox *>("typeComboBox");
    if(cbox != NULL)
    {
        // TODO BDY
    }
    // Min
    QSpinBox *spinBox = ui->widgets->findChild<QSpinBox *>("minSpinBox");
    if(spinBox != NULL)
    {
        spinBox->setValue(node->getMin());
        spinBox->setMinimum(node->getMin());
        spinBox->setMaximum(node->getMax());
    }
    // Max
    spinBox = ui->widgets->findChild<QSpinBox *>("maxSpinBox");
    if(spinBox != NULL)
    {
        spinBox->setValue(node->getMax());
        spinBox->setMinimum(node->getMin());
        spinBox->setMaximum(node->getMax());
    }
    // Units
    lineEdit = ui->widgets->findChild<QLineEdit *>("unitLineEdit");
    if(lineEdit != NULL)
    {
        lineEdit->setText(node->getUnit());
    }
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
