#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), model(NULL)
{
    ui->setupUi(this);

//    connect(ui->action_Charger_une_MIB,SIGNAL(triggered(bool)),this,SLOT(on_action_Charger_une_MIB_triggered()));
    ui->centralWidget->addActions(ui->menu_Menu->actions());
    // Widget hide
    ui->identifierReadingWidgets->hide();
    ui->identifierWidgets->hide();
    ui->cheminDuSVGLabel->hide();
    ui->svgLineEdit->hide();
    ui->dansLaMIBLabel->hide();
    ui->inMIBCheckBox->hide();
    // Tested MIBs
//    loadMib("/home/bdy/Téléchargements/ECRESO-FM-TRANS-MIB.mib");
//    loadMib("/home/bdy/WorldcastSystems/WorldCastManager/drivers/DB6400-MIB.mib");
//    loadMib("/home/bdy/Documents/MIBs/IqoyaLink_SNMP_Mib.mib");
//    loadMib("/home/bdy/Documents/MIBs/TSB-FM-T340X2-SINGLETRANSMITTER-MIB.mib");// Contains error. Even SNMPB can't open it.
//    loadMib("/home/bdy/Documents/MIBs/ENENSYS-ENTERPRISE-MIB.mib");
//    loadMib("/home/bdy/Documents/MIBs/ENENSYS-MONITOR-MIB.mib");// Depends on previous MIB
    //
    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton exitDialog/*, oidUpdateDialog*/;
    exitDialog = QMessageBox::warning(this, tr("Confirmation de fermeture"),
                                  tr("Êtes-vous sûr(e) de vouloir quitter l'application ?"),
                                  QMessageBox::Ok | QMessageBox::Cancel);

//    oidUpdateDialog = QMessageBox::information(this, tr("Mise à jour des OIDs"),
//                                               tr("Une partie des identifiants est inconnue (les OIDs contiennent '???'), voulez-vous les remplacer par une séquence ?"),
//                                               QMessageBox::Yes | QMessageBox::No);
    // Ask for confirmation as ain't no way to save configuration (only export)
    if(exitDialog == QMessageBox::Ok)
    {
        // Check if the OIDs contains '???' and offer the possibility to replace them
//        if(model->getRoot()->getOid().contains("???")) { }
        event->accept();
    }
    else
    {
        event->ignore();
    }
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
        saveNodeUpdates(model->getMibNodeFromIndex(deselected.indexes().first()));
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
    ui->inMIBCheckBox->setChecked(false);
    ui->measureCheckBox->setChecked(false);
    // Name
    ui->nameLineEdit->clear();
    // OID
    ui->oidLineEdit->clear();
    // Min
    ui->minSpinBox->setMinimum(0);
    ui->minSpinBox->setMaximum(0);
    ui->minSpinBox->setValue(0);
    // Max
    ui->maxSpinBox->setMinimum(0);
    ui->maxSpinBox->setMaximum(0);
    ui->maxSpinBox->setValue(0);
    // Units
    ui->unitLineEdit->clear();
    // Tr
    ui->frLineEdit->clear();
    ui->enLineEdit->clear();
    ui->esLineEdit->clear();
    // Wcs Type
    ui->typeComboBox->setCurrentIndex(0);
    // Operator
    ui->operatorComboBox->setCurrentIndex(0);
    // Expected value
    ui->expectedValueLineEdit->clear();
    // Factor
    ui->factorLineEdit->clear();
    // Accuracy
    ui->precisionLineEdit->clear();
    // Refresh rate
    ui->refreshFactorLineEdit->clear();
    // Severity
    ui->severityComboBox->setCurrentIndex(0);
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
    // Min
    ui->minSpinBox->setMinimum(node->getMin());
    ui->minSpinBox->setMaximum(node->getMax());
    ui->minSpinBox->setValue(node->getWcsMin());
    // Max
    ui->maxSpinBox->setMinimum(node->getMin());
    ui->maxSpinBox->setMaximum(node->getMax());
    ui->maxSpinBox->setValue(node->getWcsMax());
    // Units
    ui->unitLineEdit->setText(node->getUnit());
    // Tr
    ui->frLineEdit->setText(node->getFr());
    ui->enLineEdit->setText(node->getEn());
    ui->esLineEdit->setText(node->getEs());
    // Wcs Type
    ui->typeComboBox->setCurrentIndex(node->getIdentifierType());
    // Operator
    ui->operatorComboBox->setCurrentIndex(node->getStrOperator());
    // Expected value
    ui->expectedValueLineEdit->setText(node->getExpectedValue());
    // Factor
    ui->factorLineEdit->setValue(node->getFactor());
    // Accuracy
    ui->precisionLineEdit->setValue(node->getPrecision());
    // Refresh rate
    ui->refreshFactorLineEdit->setValue(node->getRefreshFactor());
    // Severity
    ui->severityComboBox->setCurrentIndex((node->getSeverity() / 10) - 1);// How to avoid a switch/case...
}

void MainWindow::saveNodeUpdates(QMibItem *node)
{
    if(node == NULL)
    {
        return;
    }
    node->setWcsType(ui->measureCheckBox->isChecked() ? QMibItem::IdentifierReading : QMibItem::Identifier);
    node->setMib(ui->inMIBCheckBox->isChecked() ? model->getModuleName() : "EmptyMib");
    // Name
    node->setNameAndItem(ui->nameLineEdit->text());
    // OID
//    node->setOid(ui->oidLineEdit->text());
    // Type
//    ui->typeComboBox->
    // Min
    node->setWcsMin(ui->minSpinBox->value());
    // Max
    node->setWcsMax(ui->maxSpinBox->value());
    // Units
    node->setUnit(ui->unitLineEdit->text());
    // Tr
    node->setFr(ui->frLineEdit->text());
    node->setEn(ui->enLineEdit->text());
    node->setEs(ui->esLineEdit->text());
    // Wcs Type
    node->setIdentifierType(ui->typeComboBox->currentIndex());
    // Operator
    node->setStrOperator(ui->operatorComboBox->currentIndex());
    // Expected value
    node->setExpectedValue(ui->expectedValueLineEdit->text());
    // Factor
    node->setFactor(ui->factorLineEdit->value());
    // Accuracy
    node->setPrecision(ui->precisionLineEdit->value());
    // Refresh rate
    node->setRefreshFactor(ui->refreshFactorLineEdit->value());
    // Severity
    node->setSeverity((ui->severityComboBox->currentIndex() + 1) * 10);
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
    QString filename = QFileDialog::getSaveFileName(this, tr("Choisir une destination"), /*""*/ QDir::homePath(), tr("JSON file (*.json)"));
//    QString filename = "/tmp/testDriver.json";
    if(filename.isEmpty())
    {
        qInfo() << "Store canceled.";
        return;
    }

    ui->mibTreeView->selectionModel()->clearSelection();// To save the updates of the current item
    QList<QMibItem *> nodes2export = model->getCheckedItem();
    QJsonObject result, dbstring, fr, en, es, version, content, productFamilyNb, productFamily, product, vendor, vendorDetails;
    QJsonArray identifier, identifierReading, driver, productArray;
    QFile dest(filename);

    /*
     *  Create JSON
     */
    qInfo() << "Export driver" << nodes2export.size() << "nodes";
    // Main informations
    version["versionNumber"] = ui->driverVersionLineEdit->text();

    // Children
    foreach(QMibItem *node, nodes2export)
    {
        if(node->getIsLeaf())
        {
            QJsonObject jsonNode = node->write(model->getModuleName(), fr, en, es);
            switch(node->getWcsType())
            {
            case QMibItem::Identifier:
                identifier.append(jsonNode);
                break;
            case QMibItem::IdentifierReading:
                identifierReading.append(jsonNode);
                break;
            default:
                qWarning() << "Node" << node->getName() << "not seen as identifier or identifierReading. Skip it!";
            }
        }
    }

    // Driver
    QJsonObject driver1;
    driver1["version"] = ui->versionLineEdit->text();
    driver1["protocol"] = ui->protocolComboBox->currentIndex();
    driver1["refreshFrequency"] = ui->refreshRateSpinBox->value();
    driver1["identifier"] = identifier;
    driver1["identifierGroup"] = "";
    driver1["identifierReading"] = identifierReading;
    driver1["hasScriptEasy"] = ui->scriptEasyCheckBox->checkState() == Qt::Checked ? true : false;
    driver1["hasFMChannel"] = ui->fmChannelCheckBox->checkState() == Qt::Checked ? true : false;
    driver1["lien"] = ui->urlLineEdit->text();
    driver.append(driver1);

    // Product
    product["vendor"] = ui->vendorLineEdit->text().toLower().left(3);
    product["systemName"] = ui->systemNameLineEdit->text();
    product["systemType"] = ui->systemTypeLineEdit->text();
    product["driver"] = driver;
    product["version"] = ui->driverVersionSpinBox->value();
    product["flag"] = ui->flagCheckBox->checkState() == Qt::Checked ? 1 : 0;
    productArray.append(product);
    productFamily["product"] = productArray;
    productFamily["svgPath"] = ui->svgLineEdit->text();
    productFamily["familyName"] = ui->driverFamilyComboBox->currentText().split(":")[1].trimmed();
//    productFamilyNb[ui->driverFamilyComboBox->currentText().split(":")[0]] = productFamily;
    productFamilyNb[QString::number(ui->driverFamilyComboBox->currentIndex() + 1)] = productFamily;
    content["productFamily"] = productFamilyNb;

    // Translate strings
    dbstring["es"] = es;
    dbstring["fr"] = fr;
    dbstring["en"] = en;
    content["dbString"] = dbstring;

    // Vendor
    vendorDetails["vendorName"] = ui->vendorLineEdit->text();
    vendorDetails["url"] = ui->urlLineEdit->text();
    vendorDetails["logoPath"] = ui->svgLineEdit->text();
    vendor[ui->vendorLineEdit->text().toLower().left(3)] = vendorDetails;

    // End
    result["version"] = version;
    result["content"] = content;
    result["vendor"] = vendor;

    qInfo() << "JSON ready to be written";

    // Write
    if(!dest.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file");
        return;
    }
    QJsonDocument jsonFile(result);
    if(dest.write(jsonFile.toJson()) == -1)
    {
        qWarning("Error while writing file");
    }
    dest.close();
    qInfo("JSON stored");
}
