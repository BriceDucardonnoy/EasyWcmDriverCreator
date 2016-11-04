#include <QMessageBox>
#include <qdebug.h>

#include "qmibitem.h"
#include "qtreemibmodel.h"

QTreeMibModel::QTreeMibModel(QObject *parent)
    : QStandardItemModel(parent)
{
    moduleIdentity = new QMibItem();
}

QTreeMibModel::QTreeMibModel(QString mibfile)
    : moduleName(""), vendor("")
{
    moduleIdentity = new QMibItem();
    QFile file(mibfile);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, tr("Erreur"), tr("Impossible d'ouvrir le fichier %1").arg(mibfile));
    }
//    else
//    {
//        QMessageBox::information(0, tr("Success"), tr("Ouverture de %1"'").arg(mibfile));
//    }
    createModel(&file);

    file.close();
}

QTreeMibModel::~QTreeMibModel()
{
    if(root != NULL)
    {
        delete(root);
        root = NULL;
    }
    if(moduleIdentity != NULL)
    {
        delete(moduleIdentity);
        moduleIdentity = NULL;
    }
}

bool QTreeMibModel::createModel(QFile *mibfile)
{
    QTextStream in(mibfile);// <=> QTextStream in = new QTextStream(&mibfile);

    root = new QMibItem();
    this->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    this->setHorizontalHeaderItem(1, new QStandardItem(tr("OID")));
    this->setHorizontalHeaderItem(2, new QStandardItem(tr("Description")));

    appendRow(root->getItems());
    createModel(&in, root);
    // Bind the (un)check event
    connect(this,SIGNAL(itemChanged(QStandardItem*)),this, SLOT(checkItemStates(QStandardItem*)));

    return true;
}

void QTreeMibModel::checkItemStates(QStandardItem *item)
{
    qInfo() << "(Un)checked" << item->text() << "at row" << item->row() << "and column" << item->column();

    QMibItem *node = root->findChildByName(item->text());
    if(node != NULL)
    {
//        qInfo() << "Node is" << node->toString();
        disconnect(this,SIGNAL(itemChanged(QStandardItem*)),0, 0);
        node->setCheckStateRecursive(item->checkState());
        connect(this,SIGNAL(itemChanged(QStandardItem*)),this, SLOT(checkItemStates(QStandardItem*)));
    }
}

QString QTreeMibModel::getVendor() const
{
    return vendor;
}

void QTreeMibModel::setVendor(const QString &value)
{
    vendor = value;
}

QList<QMibItem *> QTreeMibModel::getCheckedItem()
{
    QList<QMibItem *> checked;
    qInfo() << "Number of children" << root->getCheckedNodes().size();

    return checked;
}

QString QTreeMibModel::getModuleName() const
{
    return moduleName;
}

/**
 * @brief QTreeMibModel::getMibNodeFromIndex Find the MibNode from its QModelIndex
 * @param index The QModelIndex
 * @return The MibNode* or NULL if not found
 */
QMibItem *QTreeMibModel::getMibNodeFromIndex(const QModelIndex &index)
{
    QStandardItem *item = itemFromIndex(index);
    if(item == NULL) return NULL;
    return root->findChildByName(item->text());
}

/**
 * @brief TreeMibModel::createModel Recursive method to create the tree model
 * @param stream The QTextStream of the MIB file
 * @param parent The parent node to look children for
 */
void QTreeMibModel::createModel(QTextStream *stream, QMibItem *parent) {
    static bool identityFound = false;
    bool nodeFound = false;
    bool endNodeReached = false;
    QMibItem *child = NULL;
    QString line;

    while(!stream->atEnd())
    {
        if(endNodeReached)
        {
            endNodeReached = false;
        }
        else {
            line = stream->readLine();
        }
        // Node not found, looking for a start
        if(line.contains("DEFINITIONS") && line.contains("::=") && line.contains("BEGIN"))
        {
            moduleName = line.split(" ", QString::SkipEmptyParts)[0];
            continue;
        }
        else if(line.contains("MODULE-IDENTITY", Qt::CaseInsensitive) && !line.contains("OBJECT-IDENTITY", Qt::CaseInsensitive))// We skip the IMPORTS line
        {
            moduleIdentity->setName(line.split(" ", QString::SkipEmptyParts)[0]);
            moduleIdentity->setOid("");
            child = moduleIdentity;
            nodeFound = true;
        }
        else if(!nodeFound &&
                (line.contains("OBJECT-IDENTITY", Qt::CaseInsensitive) ||
                 line.contains("OBJECT-TYPE", Qt::CaseInsensitive) ||
                 line.contains("OBJECT IDENTIFIER", Qt::CaseInsensitive)))// Start of an object declaration
        {
            child = new QMibItem();
            child->setName(line.split(" ", QString::SkipEmptyParts)[0]);
//            qInfo() << "Node named found " << child->getName();
            child->setAsnBasicType(QMibItem::Leaf);
            nodeFound = true;
        }
        else if(!nodeFound && line.contains("NOTIFICATION-TYPE", Qt::CaseInsensitive))// Start of an object declaration
        {
            child = new QMibItem();
            child->setName(line.split(" ", QString::SkipEmptyParts)[0]);
            child->setAsnBasicType(QMibItem::Trap);
            child->getItems()[0]->setCheckable(false);// We don't export traps
            nodeFound = true;
        }
        // Node found, looking for a end
        if(nodeFound && line.contains("::="))
        {
            if(!identityFound && !moduleIdentity->getName().isEmpty() && moduleIdentity->getOid().isEmpty())// We're founding the module identity data
            {
                identityFound = true;
                QStringList lineData = line.mid(line.indexOf("{") + 1, line.indexOf("}") - line.indexOf("{") - 1).trimmed().split(" ");
                moduleIdentity->setOid(lineData[1]);
                moduleIdentity->setIsLeaf(false);
                moduleIdentityParentName = lineData[0];
            }
            if(line.contains(parent->getName()))
            {
                QStringList lineData = line.mid(line.indexOf("{") + 1, line.indexOf("}") - line.indexOf("{") - 1).trimmed().split(" ");
                child->setOid(parent->getOid() + "." + lineData[1]);
                parent->addChild(child);
                parent->createOrUpdateItems();
                if(parent->getName().compare("enterprises", Qt::CaseInsensitive) == 0)
                {
                    vendor = child->getName();
                    vendor[0] = vendor[0].toUpper();
                }
//                qInfo() << "Node " << child->toString() << " with parent " << parent->toString();
                if(child->getName().compare(moduleIdentityParentName) == 0)
                {
                    moduleIdentity->setOid(child->getOid() + "." + moduleIdentity->getOid());
                    moduleIdentity->createOrUpdateItems();
                    child->addChild(moduleIdentity);
                    child->createOrUpdateItems();
                    qint64 pos = stream->pos();
                    createModel(stream, moduleIdentity);
                    stream->seek(pos);
                }
                else
                {
                    qint64 pos = stream->pos();
                    createModel(stream, child);// Stream is at end because of reference given
                    stream->seek(pos);
                }
            }
            nodeFound = false;
            child = NULL;
        }
        else if(nodeFound && line.contains("SYNTAX", Qt::CaseInsensitive))
        {
            if(parent->getAsnBasicType() == QMibItem::Sequence)
            {
                child->setAsnBasicType(QMibItem::Entry);
            }
            else if(line.contains("Gauge32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::Gauge);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Integer32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::S32);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Unsigned32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::U32);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Integer", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::EnumInt);
            }
            else if(line.contains("DisplayString", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::OctetString);
                if(line.contains("SIZE(", Qt::CaseInsensitive))
                {
                    QStringList lineData = line.mid(line.indexOf("SIZE(") + 1,
                                                    line.indexOf("))") - line.indexOf("SIZE(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("SEQUENCE OF", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(QMibItem::Sequence);
            }
        }
        else if(nodeFound && line.contains("MAX-ACCESS", Qt::CaseInsensitive))
        {
            child->setIsReadOnly(line.trimmed().split(" ", QString::SkipEmptyParts)[1].compare("read-only", Qt::CaseInsensitive) == 0);
        }
        else if(nodeFound && line.contains("STATUS", Qt::CaseInsensitive))
        {
            child->setIsCurrent(line.trimmed().split(" ", QString::SkipEmptyParts)[1].compare("obsolete", Qt::CaseInsensitive) != 0);
        }
        else if(nodeFound && line.contains("UNITS", Qt::CaseInsensitive))
        {
            child->setUnit(line.trimmed().remove("\"").split(" ", QString::SkipEmptyParts)[1]);
        }
        else if(nodeFound && line.contains("DESCRIPTION", Qt::CaseInsensitive))
        {
            QString descr = "";
            while(!stream->atEnd() && !(line = stream->readLine()).contains("::="))
            {
                descr += line;
            }
            child->setDescription(descr.remove("DESCRIPTION").remove("\"").remove("No description").trimmed());
            endNodeReached = true;
        }
    }
}
