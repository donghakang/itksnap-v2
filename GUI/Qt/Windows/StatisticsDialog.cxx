#include "StatisticsDialog.h"
#include "ui_StatisticsDialog.h"

#include "GlobalUIModel.h"
#include "IRISApplication.h"
#include "SegmentationStatistics.h"
#include "HistoryManager.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QMimeData>
#include <QClipboard>
#include <SNAPQtCommon.h>
#include <QtCursorOverride.h>
#include <SimpleFileDialogWithHistory.h>

StatisticsDialog::StatisticsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::StatisticsDialog)
{
  ui->setupUi(this);
  this->setObjectName("dlgStatistics");
  m_ItemModel = new QStandardItemModel(this);
  ui->tvVolumes->setModel(m_ItemModel);
  m_Stats = new SegmentationStatistics();
}

StatisticsDialog::~StatisticsDialog()
{
  delete ui;
  delete m_Stats;
}

void StatisticsDialog::SetModel(GlobalUIModel *model)
{
  m_Model = model;
}

void StatisticsDialog::Activate()
{
  QtCursorOverride cursy(Qt::WaitCursor);
  this->FillTable();
  this->show();
  this->raise();
  this->activateWindow();
}

void StatisticsDialog::FillTable()
{
  // Compute the segmentation statistics
  m_Stats->Compute(m_Model->GetDriver());

  // Fill out the item model
  m_ItemModel->clear();

  // Set the column names
  QStringList header;
  header << "Label Name" << "Voxel Count" << "Volume (mm3)";
  m_ItemModel->setHorizontalHeaderLabels(header);

  const std::vector<std::string> &cols = m_Stats->GetImageStatisticsColumns();
  for(int j = 0; j < cols.size(); j++)
  {
    // QString 
    QString label_volume_ratio = QString("Volume (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
	  QString label = QString("Intensity Mean %1 SD\n(%02)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Skewness_label = QString("Skewness").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Kurtosis_label = QString("Kurtosis").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Min_label = QString("Intensity 0%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Two_Point_Five_label = QString("Intensity 2.5%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Five_label = QString("Intensity 5%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Ten_label = QString("Intensity 10%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Twenty_Five_label = QString("Intensity 25%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Median_label = QString("Intensity 50%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Seventy_Five_label = QString("Intensity 75%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Ninety_label = QString("Intensity 90%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Ninety_Five_label = QString("Intensity 95%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString NinetySeven_Point_Five = QString("Intensity 97.5%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Max_label = QString("Intensity 100%").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Tissue_Frac_label = QString("Tissue (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Tissue_Vmm_label = QString("Tissue (mm3)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Air_Frac_label = QString("Air (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Air_Vmm_label = QString("Air (mm3)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString HAA_ratio_label = QString("HAA (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString HAA_Vmm_label = QString("HAA (mm3)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n950_label = QString("below-950 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n920_label = QString("below-920 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n910_label = QString("below-910 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n856_label = QString("below-856 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n601_label = QString("below-601 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_below_n250_label = QString("below-250 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
    QString Percent_above_0_label = QString("above 0 (%)").arg(QChar(0x00B1)).arg(from_utf8(cols[j]));
        
    // QStandardItem Constructor
	  QStandardItem *volume_ratio_item = new QStandardItem();
    QStandardItem *skewness_item = new QStandardItem();
    QStandardItem *kurtosis_item = new QStandardItem();
    QStandardItem *item = new QStandardItem();
    QStandardItem *Min = new QStandardItem();
    QStandardItem *Two_Point_Five = new QStandardItem();
    QStandardItem *Five = new QStandardItem();
    QStandardItem *Ten = new QStandardItem();
    QStandardItem *Twenty_Five = new QStandardItem();
    QStandardItem *Median = new QStandardItem();
    QStandardItem *Seventy_Five = new QStandardItem();
    QStandardItem *Ninety = new QStandardItem();
    QStandardItem *Ninety_Five = new QStandardItem();
    QStandardItem *Ninety_Seven_Point_Five = new QStandardItem();
    QStandardItem *Max = new QStandardItem();
    QStandardItem *Tissue_Frac_item = new QStandardItem();
    QStandardItem *Tissue_Vmm_item = new QStandardItem();
    QStandardItem *Air_Frac_item = new QStandardItem();
    QStandardItem *Air_Vmm_item = new QStandardItem();
    QStandardItem *HAA_ratio_item = new QStandardItem();
    QStandardItem *HAA_Vmm_item = new QStandardItem();
    QStandardItem *Percent_below_n950_item = new QStandardItem();
    QStandardItem *Percent_below_n920_item = new QStandardItem();
    QStandardItem *Percent_below_n910_item = new QStandardItem();
    QStandardItem *Percent_below_n856_item = new QStandardItem();
    QStandardItem *Percent_below_n601_item = new QStandardItem();
    QStandardItem *Percent_below_n250_item = new QStandardItem();
    QStandardItem *Percent_above_0_item = new QStandardItem();
        
    // Set Text
    volume_ratio_item->setText(label_volume_ratio);
    skewness_item->setText(Skewness_label);
    kurtosis_item->setText(Kurtosis_label);
    item->setText(label);
    Min->setText(Min_label);
    Two_Point_Five->setText(Two_Point_Five_label);
    Five->setText(Five_label);
    Ten->setText(Ten_label);
    Twenty_Five->setText(Twenty_Five_label);
    Median->setText(Median_label);
    Seventy_Five->setText(Seventy_Five_label);
    Ninety->setText(Ninety_label);
    Ninety_Five->setText(Ninety_Five_label);
    Ninety_Seven_Point_Five->setText(NinetySeven_Point_Five);
    Max->setText(Max_label);
    Tissue_Frac_item->setText(Tissue_Frac_label);
    Tissue_Vmm_item->setText(Tissue_Vmm_label);
    Air_Frac_item->setText(Air_Frac_label);
    Air_Vmm_item->setText(Air_Vmm_label);
    HAA_ratio_item->setText(HAA_ratio_label);
    HAA_Vmm_item->setText(HAA_Vmm_label);
    Percent_below_n950_item->setText(Percent_below_n950_label);
    Percent_below_n920_item->setText(Percent_below_n920_label);
    Percent_below_n910_item->setText(Percent_below_n910_label);
    Percent_below_n856_item->setText(Percent_below_n856_label);
    Percent_below_n601_item->setText(Percent_below_n601_label);
    Percent_below_n250_item->setText(Percent_below_n250_label);
    Percent_above_0_item->setText(Percent_above_0_label);

    // Set Tool Tip
    volume_ratio_item->setToolTip(QString("Volume %1").arg(from_utf8(cols[j])));
    item->setToolTip(QString("Mean intensity and standard deviation for layer %1").arg(from_utf8(cols[j])));
    skewness_item->setToolTip(QString("Skewness for layer %1").arg(from_utf8(cols[j])));
    kurtosis_item->setToolTip(QString("Kurtosis for layer %1").arg(from_utf8(cols[j])));
    Min->setToolTip(QString("Min for layer %1").arg(from_utf8(cols[j])));
    Two_Point_Five->setToolTip(QString("2.5% intensity for layer %1").arg(from_utf8(cols[j])));
    Five->setToolTip(QString("5% intensity for layer %1").arg(from_utf8(cols[j])));
    Ten->setToolTip(QString("10% intensity for layer %1").arg(from_utf8(cols[j])));
    Twenty_Five->setToolTip(QString("25% intensity for layer %1").arg(from_utf8(cols[j])));
    Median->setToolTip(QString("Median for layer %1").arg(from_utf8(cols[j])));
    Seventy_Five->setToolTip(QString("75% intensity for layer %1").arg(from_utf8(cols[j])));
    Ninety->setToolTip(QString("90% intensity for layer %1").arg(from_utf8(cols[j])));
    Ninety_Five->setToolTip(QString("95% intensity for layer %1").arg(from_utf8(cols[j])));
    Ninety_Seven_Point_Five->setToolTip(QString("97.5% intensity for layer %1").arg(from_utf8(cols[j])));
    Max->setToolTip(QString("Max for layer %1").arg(from_utf8(cols[j])));
    Tissue_Frac_item->setToolTip(QString("Tissue Frac for layer %1").arg(from_utf8(cols[j])));
    Tissue_Vmm_item->setToolTip(QString("Tissye vmm for layer %1").arg(from_utf8(cols[j])));
    Air_Frac_item->setToolTip(QString("Air Frac for layer %1").arg(from_utf8(cols[j])));
    Air_Vmm_item->setToolTip(QString("Air vmm for layer %1").arg(from_utf8(cols[j])));
    HAA_ratio_item->setToolTip(QString("HAA ratio for layer %1").arg(from_utf8(cols[j])));
    HAA_Vmm_item->setToolTip(QString("HAA vmm for layer %1").arg(from_utf8(cols[j])));
    Percent_below_n950_item->setToolTip(QString("Below -950 (%) %1").arg(from_utf8(cols[j])));
    Percent_below_n920_item->setToolTip(QString("Below -920 (%) %1").arg(from_utf8(cols[j])));
    Percent_below_n910_item->setToolTip(QString("Below -910 (%) %1").arg(from_utf8(cols[j])));
    Percent_below_n856_item->setToolTip(QString("Below -856 (%) %1").arg(from_utf8(cols[j])));
    Percent_below_n601_item->setToolTip(QString("Below -601 (%) %1").arg(from_utf8(cols[j])));
    Percent_below_n250_item->setToolTip(QString("Below -250 (%) %1").arg(from_utf8(cols[j])));
    Percent_above_0_item->setToolTip(QString("Above 0 (%) %1").arg(from_utf8(cols[j])));
    
    // Set Horizontal Header Item
    m_ItemModel->setHorizontalHeaderItem(j+3, volume_ratio_item);
    m_ItemModel->setHorizontalHeaderItem(j+4, item);
    m_ItemModel->setHorizontalHeaderItem(j+5, skewness_item);
    m_ItemModel->setHorizontalHeaderItem(j+6, kurtosis_item);
    m_ItemModel->setHorizontalHeaderItem(j+7, Min);
    m_ItemModel->setHorizontalHeaderItem(j+8, Two_Point_Five);
    m_ItemModel->setHorizontalHeaderItem(j+9, Five);
    m_ItemModel->setHorizontalHeaderItem(j+10, Ten);
    m_ItemModel->setHorizontalHeaderItem(j+11, Twenty_Five);
    m_ItemModel->setHorizontalHeaderItem(j+12, Median);
    m_ItemModel->setHorizontalHeaderItem(j+13, Seventy_Five);
    m_ItemModel->setHorizontalHeaderItem(j+14, Ninety);
    m_ItemModel->setHorizontalHeaderItem(j+15, Ninety_Five);
    m_ItemModel->setHorizontalHeaderItem(j+16, Ninety_Seven_Point_Five);
    m_ItemModel->setHorizontalHeaderItem(j+17, Max);
    m_ItemModel->setHorizontalHeaderItem(j+18, Tissue_Frac_item);
    m_ItemModel->setHorizontalHeaderItem(j+19, Tissue_Vmm_item);
    m_ItemModel->setHorizontalHeaderItem(j+20, Air_Frac_item);
    m_ItemModel->setHorizontalHeaderItem(j+21, Air_Vmm_item);
    m_ItemModel->setHorizontalHeaderItem(j+22, HAA_ratio_item);
    m_ItemModel->setHorizontalHeaderItem(j+23, HAA_Vmm_item);
    m_ItemModel->setHorizontalHeaderItem(j+24, Percent_below_n950_item);
    m_ItemModel->setHorizontalHeaderItem(j+25, Percent_below_n920_item);
    m_ItemModel->setHorizontalHeaderItem(j+26, Percent_below_n910_item);
    m_ItemModel->setHorizontalHeaderItem(j+27, Percent_below_n856_item);
    m_ItemModel->setHorizontalHeaderItem(j+28, Percent_below_n601_item);
    m_ItemModel->setHorizontalHeaderItem(j+29, Percent_below_n250_item);
    m_ItemModel->setHorizontalHeaderItem(j+30, Percent_above_0_item);
  }

  // Add all the rows
  for(SegmentationStatistics::EntryMap::const_iterator it = m_Stats->GetStats().begin();
      it != m_Stats->GetStats().end(); ++it)
  {
    LabelType i = it->first;
    const SegmentationStatistics::Entry &row = it->second;
    const ColorLabel &cl = m_Model->GetDriver()->GetColorLabelTable()->GetColorLabel(i);
    if(row.count)
    {
      QList<QStandardItem *> qsi;
      QIcon icon = CreateColorBoxIcon(16,16, QColor(cl.GetRGB(0), cl.GetRGB(1), cl.GetRGB(2)));
      qsi.append(new QStandardItem(icon, cl.GetLabel()));
      qsi.append(new QStandardItem(QString("%1").arg(row.count)));
      qsi.append(new QStandardItem(QString("%1").arg(row.volume_mm3,0,'g',4)));

      // Volume (%)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString volume_ratio = QString("%1").arg(row.volume_ratio[j], 0, 'f', 1);
        qsi.append(new QStandardItem(volume_ratio));
      }

      // intensity mean +- SD
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString text = QString("%1%2%3")
            .arg(row.mean[j],0,'f',4)
            .arg(QChar(0x00B1))
            .arg(row.stdev[j],0,'f',4);
        qsi.append(new QStandardItem(text));
      }

      // Skewness
      for (int j = 0; j < row.mean.size(); j++)
      {
          QString skewness = QString("%1").arg(row.skewness[j]);
          qsi.append(new QStandardItem(skewness));
      }

      // Kurtosis
      for (int j = 0; j < row.mean.size(); j++)
      {
          QString kurtosis = QString("%1").arg(row.kurtosis[j]);
          qsi.append(new QStandardItem(kurtosis));
      }
          
	    // Intensity 0
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString min = QString("%1").arg(row.min[j]);
        // QString min = QString("%1").arg(11);
        qsi.append(new QStandardItem(min));
      }

      // Intensity 2.5%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString two_point_five = QString("%1").arg(row.two_point_five[j]);
        // QString two_point_five = QString("%1").arg(12);
        qsi.append(new QStandardItem(two_point_five));
      }

      // Intensity 5%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString five = QString("%1").arg(row.five[j]);
        // QString five = QString("%1").arg(13);
        qsi.append(new QStandardItem(five));
      }

      // Intensity 10%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString ten = QString("%1").arg(row.ten[j]);
        // QString ten = QString("%1").arg(14);
        qsi.append(new QStandardItem(ten));
      }

      // Intensity 25%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString twenty_five = QString("%1").arg(row.twenty_five[j]);
        // QString twenty_five = QString("%1").arg(15);
        qsi.append(new QStandardItem(twenty_five));
      }

      // Intensity 50% -- median
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString median = QString("%1").arg(row.median[j]);
        // QString median = QString("%1").arg(16);
        qsi.append(new QStandardItem(median));
      }

      // Intensity 75%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString seventy_five = QString("%1").arg(row.seventy_five[j]);
        qsi.append(new QStandardItem(seventy_five));
      }

      // Intensity 90%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString ninety = QString("%1").arg(row.ninety[j]);
        qsi.append(new QStandardItem(ninety));
      }

      // Intensity 95%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString ninety_five = QString("%1").arg(row.ninety_five[j]);
        qsi.append(new QStandardItem(ninety_five));
      }

      // Intensity 97.5%
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString ninetyseven_point_five = QString("%1").arg(row.ninetyseven_point_five[j]);
        qsi.append(new QStandardItem(ninetyseven_point_five));
      }

      // Intensity 100% -- max
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString max = QString("%1").arg(row.max[j]);
        qsi.append(new QStandardItem(max));
      }

      // Tissue (%)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString Tissue_Frac = QString("%1").arg(row.tissue_frac[j], 0, 'f', 1);
        // QString Tissue_Frac = QString("%1").arg(16);
        qsi.append(new QStandardItem(Tissue_Frac));
      }

      // Tissue (mm3)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString Tissue_Vmm = QString("%1").arg(row.tissue_vmm[j]);
        // QString Tissue_Vmm = QString("%1").arg(3);
        qsi.append(new QStandardItem(Tissue_Vmm));
      }

      // Air (%)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString Air_Frac = QString("%1").arg(row.air_frac[j], 0, 'f', 1);
        // QString Air_Frac = QString("%1").arg(51);
        qsi.append(new QStandardItem(Air_Frac));
      }

      // Air (mm3)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString Air_Vmm = QString("%1").arg(row.air_vmm[j]);
        // QString Air_Vmm = QString("%1").arg(123);
        qsi.append(new QStandardItem(Air_Vmm));
      }

      // HAA (%)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString haa_ratio = QString("%1").arg(row.HAA_ratio[j], 0, 'f', 1);
        // QString haa_ratio = QString("%1").arg(2);
        qsi.append(new QStandardItem(haa_ratio));
      }

      // HAA (mm3)
      for (int j = 0; j < row.mean.size(); j++)
      {
        QString haa_vmm = QString("%1").arg(row.HAA_Vmm[j]);
        // QString haa_vmm = QString("%1").arg(53);
        qsi.append(new QStandardItem(haa_vmm));
      }
            
      // Below
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below950 = QString("%1").arg(row.below950[j], 0, 'f', 2);
        // QString below950 = QString("%1").arg(950);
        qsi.append(new QStandardItem(below950));
      }
    
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below920 = QString("%1").arg(row.below920[j], 0, 'f', 2);
        // QString below920 = QString("%1").arg(920);
        qsi.append(new QStandardItem(below920));
      }
   
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below910 = QString("%1").arg(row.below910[j], 0, 'f', 2);
        // QString below910 = QString("%1").arg(910);
        qsi.append(new QStandardItem(below910));
      }
    
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below856 = QString("%1").arg(row.below856[j], 0, 'f', 2);
        // QString below856 = QString("%1").arg(856);
        qsi.append(new QStandardItem(below856));
      }
    
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below601 = QString("%1").arg(row.below601[j], 0, 'f', 2);
        // QString below601 = QString("%1").arg(601);
        qsi.append(new QStandardItem(below601));
      }
          
      for(int j = 0; j < row.mean.size(); j++)
      {
        QString below250 = QString("%1").arg(row.below250[j], 0, 'f', 2);
        // QString below250 = QString("%1").arg(250);
        qsi.append(new QStandardItem(below250));
      }
          
      // Above
      for(int j = 0; j < row.mean.size(); j++)
      {
         QString above0 = QString("%1").arg(row.above0[j], 0, 'f', 2);
        // QString above0 = QString("%1").arg(6);
        qsi.append(new QStandardItem(above0));
      }





      m_ItemModel->appendRow(qsi);
      m_ItemModel->setVerticalHeaderItem(m_ItemModel->rowCount()-1,
                                         new QStandardItem(QString("%1").arg(i)));
    }
  }

  // Perform a smart resize of the column widts
  ui->tvVolumes->resizeColumnsToContents();
  for(int col = 0; col < ui->tvVolumes->horizontalHeader()->count(); col++)
  {
    if(ui->tvVolumes->horizontalHeader()->sectionSize(col) > 150)
      ui->tvVolumes->horizontalHeader()->resizeSection(col, 150);
  }
}

void StatisticsDialog::on_btnUpdate_clicked()
{
  QtCursorOverride cursy(Qt::WaitCursor);
  this->FillTable();
}


void StatisticsDialog::on_btnCopy_clicked()
{
  std::ostringstream oss;
  m_Stats->Export(oss, "\t", *m_Model->GetDriver()->GetColorLabelTable());
  QString tsv = QString::fromStdString(oss.str());

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(tsv);
}

#include <fstream>

void StatisticsDialog::on_btnExport_clicked()
{
  std::cout << "Export" << std::endl;
  // Ask for a filename
  QString selection = ShowSimpleSaveDialogWithHistory(
        this, m_Model, "Statistics",
        "Export Volumes and Statistics - ITK-SNAP",
        "Volumes and Statistics File",
        "Text Files (*.txt);; Comma Separated Value Files (*.csv)",
        true);

  // Open the labels from the selection
  if(selection.length())
    {
    try
      {
      std::ofstream fout(selection.toUtf8());
      if(selection.endsWith(".csv", Qt::CaseInsensitive))
        {
        m_Stats->Export(fout, ",", *m_Model->GetDriver()->GetColorLabelTable());
        }
      else
        {
        m_Stats->Export(fout, "\t", *m_Model->GetDriver()->GetColorLabelTable());
        }
      m_Model->GetSystemInterface()->GetHistoryManager()->
          UpdateHistory("Statistics", to_utf8(selection), true);
      }
    catch(std::exception &exc)
      {
      ReportNonLethalException(this, exc, "Volume and Statistics IO Error",
                               QString("Failed to export volumes and statistics"));
      }
    }
}
