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

/**
 * @brief TreeMibModel::createModel Recursive method to create the tree model
 * @param stream The QTextStream of the MIB file
 * @param parent The parent node to look children for
 */
void TreeMibModel::createModel(QTextStream *stream, MibItem *parent) {
    static bool identityFound = false;
    bool nodeFound = false;
    MibItem *child;

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
            child = new MibItem();
            child->setName(line.split(" ", QString::SkipEmptyParts)[0]);
//            qInfo() << "Node named found " << child->getName();
            child->setAsnBasicType(MibItem::Leaf);
            nodeFound = true;
        }
        else if(!nodeFound && line.contains("NOTIFICATION-TYPE", Qt::CaseInsensitive))// Start of an object declaration
        {
            child = new MibItem();
            child->setName(line.split(" ", QString::SkipEmptyParts)[0]);
            child->setAsnBasicType(MibItem::Trap);
            nodeFound = true;
        }
        // Node found, looking for a end
        else if(nodeFound && line.contains("::="))
        {
            if(!identityFound && moduleIdentity->getOid().isEmpty())// We're founding the module identity data
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
        }
        else if(nodeFound && line.contains("SYNTAX", Qt::CaseInsensitive))
        {
            if(line.contains("Gauge32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(MibItem::Gauge);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Integer32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(MibItem::S32);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Unsigned32", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(MibItem::U32);
                if(line.contains("("))
                {
                    QStringList lineData = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
            }
            else if(line.contains("Integer", Qt::CaseInsensitive)) child->setAsnBasicType(MibItem::EnumInt);
            else if(line.contains("DisplayString", Qt::CaseInsensitive))
            {
                child->setAsnBasicType(MibItem::OctetString);
                if(line.contains("SIZE(", Qt::CaseInsensitive))
                {
                    QStringList lineData = line.mid(line.indexOf("SIZE(") + 1,
                                                    line.indexOf("))") - line.indexOf("SIZE(") - 1).trimmed().split("..");
                    child->setMin(lineData[0].toInt());
                    child->setMax(lineData[1].toInt());
                }
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
        else if(nodeFound && line.contains("DESCRIPTION", Qt::CaseInsensitive))
        {

        }
    }
}
