#ifndef TREEMIBMODEL_H
#define TREEMIBMODEL_H

#include <QWidget>
//#include <QFileSystemModel>
#include <qstandarditemmodel.h>

#include "mibnode.h"
#include "mibitem.h"

//class TreeMibModel : public QFileSystemModel
class TreeMibModel : public QStandardItemModel
{
public:
    TreeMibModel(QObject *parent = 0);
//    TreeMibModel::TreeMibModel();
    TreeMibModel(QString mibfile);
    ~TreeMibModel();

    QString getName() const;
    void setName(const QString &value);

    QString getOid() const;
    void setOid(const QString &value);

    QString getType() const;
    void setType(const QString &value);

    bool getIsReadOnly() const;
    void setIsReadOnly(bool value);

    QString getKind() const;
    void setKind(const QString &value);

    QString toString() const;

    void createModel(QTextStream *stream, TreeMibModel *parent);
    void createModel(QTextStream *stream, MibItem *parent);
protected:
    QList<QStandardItem *> prepareRow(const int sz, ...);
    void createModel(QTextStream *stream, const MibItem *parent);
private:
    QString name;
    QString oid;
    QString type;
    bool isReadOnly;
    QString kind;
    MibItem *moduleIdentity;
    QString moduleIdentityParentName;
    bool createModel(QFile *mibfile);
    void findNode(QTextStream *stream, TreeMibModel *parent);
};

#endif // TREEMIBMODEL_H
