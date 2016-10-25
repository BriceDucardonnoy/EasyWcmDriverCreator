#ifndef TREEMIBMODEL_H
#define TREEMIBMODEL_H

#include <QWidget>
//#include <QFileSystemModel>
#include <qstandarditemmodel.h>

#include "mibitem.h"

//class TreeMibModel : public QFileSystemModel
class TreeMibModel : public QStandardItemModel
{
public:
    TreeMibModel(QObject *parent = 0);
//    TreeMibModel::TreeMibModel();
    TreeMibModel(QString mibfile);
    ~TreeMibModel();

    void createModel(QTextStream *stream, TreeMibModel *parent);
    void createModel(QTextStream *stream, MibItem *parent);

protected:
    void createModel(QTextStream *stream, const MibItem *parent);

private:
    MibItem *moduleIdentity;
    QString moduleIdentityParentName;
    bool createModel(QFile *mibfile);
    void findNode(QTextStream *stream, TreeMibModel *parent);
};

#endif // TREEMIBMODEL_H
