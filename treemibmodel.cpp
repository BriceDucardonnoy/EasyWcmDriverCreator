#include <QMessageBox>
#include <qtextstream.h>
#include <qdebug.h>
#include <qfile.h>

#include "mibitem.h"
#include "treemibmodel.h"

TreeMibModel::TreeMibModel(QObject *parent)
    : QStandardItemModel(parent)
{
    moduleIdentity = new MibItem();
}

TreeMibModel::~TreeMibModel()
{
    if(moduleIdentity != NULL)
    {
        delete(moduleIdentity);
        moduleIdentity = NULL;
    }
}

QString TreeMibModel::getName() const
{
    return name;
}

void TreeMibModel::setName(const QString &value)
{
    name = value;
}

QString TreeMibModel::getOid() const
{
    return oid;
}

void TreeMibModel::setOid(const QString &value)
{
    oid = value;
}

QString TreeMibModel::getType() const
{
    return type;
}

void TreeMibModel::setType(const QString &value)
{
    type = value;
}

bool TreeMibModel::getIsReadOnly() const
{
    return isReadOnly;
}

void TreeMibModel::setIsReadOnly(bool value)
{
    isReadOnly = value;
}

QString TreeMibModel::getKind() const
{
    return kind;
}

void TreeMibModel::setKind(const QString &value)
{
    kind = value;
}

QString TreeMibModel::toString() const
{
    return getOid() + " -> " + getName();
}

TreeMibModel::TreeMibModel(QString mibfile)
{
    moduleIdentity = new MibItem();
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

bool TreeMibModel::createModel(QFile *mibfile)
{
    QTextStream in(mibfile);// <=> QTextStream in = new QTextStream(&mibfile);

    MibItem *root = new MibItem();
    this->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    this->setHorizontalHeaderItem(1, new QStandardItem("OID"));

    appendRow(root->getItems());
    createModel(&in, root);

    return true;
}

void TreeMibModel::createModel(QTextStream *stream, MibItem *parent) {
    static bool identityFound = false;
    MibItem *child = new MibItem();
    bool nodeFound = false;

    while(!stream->atEnd())
    {
        QString line = stream->readLine();
        // Node not found, looking for a start
        if(line.contains("MODULE-IDENTITY", Qt::CaseInsensitive) && !line.contains("OBJECT-IDENTITY", Qt::CaseInsensitive))// We skip the IMPORTS line
        {
            moduleIdentity->setName(line.split(" ", QString::SkipEmptyParts)[0]);
            moduleIdentity->setOid("");
            nodeFound = true;
        }
        else if(!nodeFound &&
                (line.contains("OBJECT-IDENTITY", Qt::CaseInsensitive) || line.contains("OBJECT-TYPE", Qt::CaseInsensitive)))// Start of an object declaration
        {
            child->setName(line.split(" ", QString::SkipEmptyParts)[0]);
            qInfo() << "Node named found " << child->getName();
            nodeFound = true;

        }
        // Node found, looking for a end
        else if(nodeFound && line.contains("::="))
        {
            if(!identityFound && moduleIdentity->getOid().isEmpty())// We're founding the module identity data
            {
                identityFound = true;
                QStringList lineData = line.mid(line.indexOf("{") + 1, line.indexOf("}") - 1).trimmed().split(" ");
                moduleIdentity->setOid(lineData[1]);
                moduleIdentity->setIsLeaf(false);
                moduleIdentityParentName = lineData[0];
            }
            if(line.contains(parent->getName()))
            {
                QStringList lineData = line.mid(line.indexOf("{") + 1, line.indexOf("}") - 1).trimmed().split(" ");
                child->setOid(parent->getOid() + "." + lineData[1]);
//                qInfo() << "Parent is " << lineData[0];
                parent->addChild(child);
                parent->createOrUpdateItems();
                qInfo() << "Node " << child->toString() << " with parent " << parent->toString();
                if(child->getName().compare(moduleIdentityParentName) == 0)
                {
                    moduleIdentity->setOid(child->getOid() + "." + moduleIdentity->getOid());
                    moduleIdentity->createOrUpdateItems();
                    child->addChild(moduleIdentity);
                    child->createOrUpdateItems();
                    createModel(stream, moduleIdentity);
                }
                else
                {
                    createModel(stream, child);
                }
            }
            nodeFound = false;
        }
    }
}

QList<QStandardItem *> TreeMibModel::prepareRow(const int sz, ...)
{
    QList<QStandardItem*> items;
    va_list names;

    va_start(names, sz);
    for(int i = 0 ; i < sz ; i++) {
        items << new QStandardItem(va_arg(names, QString));
    }
    va_end(names);

    return items;
}
