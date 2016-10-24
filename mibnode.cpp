#include <qstring.h>
#include <qvector.h>

#include "mibnode.h"

MibNode::MibNode() :
    isLeaf(false)
{
}

MibNode::~MibNode()
{
    foreach (MibNode *child, children) {
        delete(child);
    }
}

bool MibNode::getIsLeaf() const
{
    return isLeaf;
}

void MibNode::setIsLeaf(bool value)
{
    isLeaf = value;
}

QString MibNode::getOid() const
{
    return oid;
}

void MibNode::setOid(const QString &value)
{
    oid = value;
}

QString MibNode::getType() const
{
    return type;
}

void MibNode::setType(const QString &value)
{
    type = value;
}

bool MibNode::getIsReadOnly() const
{
    return isReadOnly;
}

void MibNode::setIsReadOnly(bool value)
{
    isReadOnly = value;
}

QString MibNode::getKind() const
{
    return kind;
}

void MibNode::setKind(const QString &value)
{
    kind = value;
}

int MibNode::getSize() const
{
    return size;
}

void MibNode::setSize(int value)
{
    size = value;
}

QString MibNode::getDescription() const
{
    return description;
}

void MibNode::setDescription(const QString &value)
{
    description = value;
}

QString MibNode::getName() const
{
    return name;
}

void MibNode::setName(const QString &value)
{
    name = value;
}

void MibNode::clearAll()
{
    // TODO BDY: NYI
}

QString MibNode::toString()
{
    return getOid() + " -> " + getName();
}

QVector<MibNode*> MibNode::getChildren() const
{
    return children;
}

void MibNode::addChild(MibNode *child)
{
    children.append(child);
}

void MibNode::removeChild(QString name, QString OID)
{
    MibNode *toDelete;
    foreach (MibNode *child, children)
    {
        if(child->getName().compare(name, Qt::CaseInsensitive) == 0 && child->getOid().compare(OID) == 0)
        {
            toDelete = child;
            break;
        }
    }
    children.removeOne(toDelete);
}
