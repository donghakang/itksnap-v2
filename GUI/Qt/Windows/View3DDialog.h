#ifndef VIEW3DDIALOG_H
#define VIEW3DDIALOG_H 

#include <QDialog>
#include <QAbstractListModel>

class GlobalUIModel;

namespace Ui {
class View3DDialog;
}

class View3DDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit View3DDialog(QWidget *parent = 0);

  ~View3DDialog();
  
  void SetModel(GlobalUIModel *model);

  void Update();

private:
  Ui::View3DDialog *ui;

  QWidget *m_ViewPanel;

  GlobalUIModel *m_Model;
};

#endif // VIEW3DDIALOG_H
