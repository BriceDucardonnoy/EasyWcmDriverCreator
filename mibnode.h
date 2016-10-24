#ifndef MIBNODE_H
#define MIBNODE_H

#include <QModelIndex>

class MibNode : public QModelIndex
{
public:
    MibNode();
    ~MibNode();

    bool getIsLeaf() const;
    void setIsLeaf(bool value);

    QString getOid() const;
    void setOid(const QString &value);

    QString getType() const;
    void setType(const QString &value);

    bool getIsReadOnly() const;
    void setIsReadOnly(bool value);

    QString getKind() const;
    void setKind(const QString &value);

    int getSize() const;
    void setSize(int value);

    QString getDescription() const;
    void setDescription(const QString &value);

    QString getName() const;
    void setName(const QString &value);

    QVector<MibNode*> getChildren() const;

    void clearAll();
    QString toString();
    void addChild(MibNode *child);
    void removeChild(QString name, QString OID);
private:
    QString name;
    bool isLeaf;
    QString oid;
    QString type;
    bool isReadOnly;
    QString kind;
    int size;
    QString description;
    QVector<MibNode*> children;
};

#endif // MIBNODE_H
