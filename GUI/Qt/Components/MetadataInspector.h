#ifndef METADATAINSPECTOR_H
#define METADATAINSPECTOR_H

#include <SNAPComponent.h>
#include <SNAPCommon.h>
#include <QAbstractTableModel>
#include <QDialog>

class ImageInfoModel;
class MetadataTableQtModel;
class GlobalUIModel;

namespace Ui {
class MetadataInspector;
class MetadataTableQtModel;
}


class MetadataTableQtModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  MetadataTableQtModel(QWidget *parent) : QAbstractTableModel(parent) {}
  virtual ~MetadataTableQtModel() {}

  void SetModel(GlobalUIModel *model);

  void SetParentModel(ImageInfoModel *model);
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  QVariant data(const QModelIndex &index, int role) const;

public slots:

  void onModelUpdate();

  void on_btnExport_clicked();

protected:

  ImageInfoModel *m_ParentModel;

private:
  Ui::MetadataTableQtModel *ui;
  GlobalUIModel *m_Model;

};


class MetadataInspector : public SNAPComponent
{
  Q_OBJECT

public:
  explicit MetadataInspector(QWidget *parent = 0);
  ~MetadataInspector();

  // Set the model
  void SetModel(ImageInfoModel *model);

  // Respond to model updates
public slots:
  virtual void onModelUpdate(const EventBucket &bucket);

  virtual void on_btnExport_clicked();

private:
  Ui::MetadataInspector *ui;
  MetadataTableQtModel *m_TableModel;
  ImageInfoModel *m_Model;
};

#endif // METADATAINSPECTOR_H
