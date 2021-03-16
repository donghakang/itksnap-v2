#include "View3DDialog.h"
#include "ui_View3DDialog.h"
#include "QFile"
#include "SNAPCommon.h"

View3DDialog::View3DDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::View3DDialog)
{
  ui->setupUi(this);

  
  // // Load the credits file
  // QFile fCredits(":root/credits.html");
  // if(fCredits.open(QFile::ReadOnly))
  //   ui->outCredits->setHtml(QString(fCredits.readAll()));

  // // Load the contents into the browser
  // QFile fLicense(":/root/license.txt");
  // if(fLicense.open(QFile::ReadOnly))
  //   ui->outLicense->setPlainText(QString(fLicense.readAll()));

  // Load the build information
  // ui->outBuild->setPlainText(QString::fromUtf8(SNAPBuildInfo));
  std::cout<< "constructor" << std::endl;
}

void View3DDialog::SetModel(GlobalUIModel *model) {
  std::cout<< "setmodel" << std::endl;
  this->m_Model = model;

  ui->panel3D->Initialize(model);
  
  m_ViewPanel = ui->panel3D;
  // TODO: update before set up in view panel
}

void View3DDialog::Update() {
  ui->panel3D->Update();
}

View3DDialog::~View3DDialog()
{
  delete ui;
}
