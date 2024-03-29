#include "SliceDefaultViewPanel.h"
#include "ui_SliceDefaultViewPanel.h"

#include "GlobalUIModel.h"
#include "SNAPEvents.h"
#include "IRISException.h"
#include "IRISApplication.h"
#include "GenericImageData.h"
#include "itkCommand.h"
#include "CrosshairsRenderer.h"
#include "PolygonDrawingRenderer.h"
#include "PaintbrushRenderer.h"
#include "SnakeROIRenderer.h"
#include "SnakeROIModel.h"
#include "SliceWindowCoordinator.h"
#include "PolygonDrawingModel.h"
#include "InteractiveRegistrationModel.h"
#include "QtWidgetActivator.h"
#include "SnakeModeRenderer.h"
#include "SnakeWizardModel.h"
#include "DisplayLayoutModel.h"
#include "PaintbrushModel.h"
#include "SliceWindowDecorationRenderer.h"
#include "LayerInspectorDialog.h"
#include "MainImageWindow.h"
#include "RegistrationRenderer.h"
#include "SNAPQtCommon.h"
#include "QtScrollbarCoupling.h"
#include "QtSliderCoupling.h"
#include <QCursor>
#include <QBitmap>
#include <QToolButton>
#include "IntensityCurveModel.h"

#include <QStackedLayout>
#include <QMenu>

SliceDefaultViewPanel::SliceDefaultViewPanel(QWidget *parent) :
    SNAPComponent(parent),
    ui(new Ui::SliceDefaultViewPanel)
{
  ui->setupUi(this);

  // Initialize
  m_GlobalUI = NULL;
  m_SliceModel = NULL;
  m_IntensityModel = NULL;

  QString menuStyle = "font-size: 12px;";

  // // Connect the actions to the toolbar buttons (sucks to do this by hand)
  // ui->btnAcceptPolygon->setDefaultAction(ui->actionAccept);
  // ui->btnPastePolygon->setDefaultAction(ui->actionPaste);
  // ui->btnClearDrawing->setDefaultAction(ui->actionClearDrawing);
  // ui->btnCloseLoop->setDefaultAction(ui->actionComplete);
  // ui->btnDeleteNodes->setDefaultAction(ui->actionDeleteSelected);
  // ui->btnDeletePolygon->setDefaultAction(ui->actionClearPolygon);
  // ui->btnSplitNodes->setDefaultAction(ui->actionSplitSelected);
  // ui->btnUndoLast->setDefaultAction(ui->actionUndo);


  this->addAction(ui->actionZoom_In);
  this->addAction(ui->actionZoom_Out);

  // // Connect the context menu signal from polygon mode to this widget
  // connect(ui->imPolygon, SIGNAL(contextMenuRequested()), SLOT(onContextMenu()));

  // // Arrange the interaction modes into a tree structure. The first child of
  // // every interaction mode is an empty QWidget. The tree is used to allow
  // // events to fall through from one interaction mode to another

  // // TODO: can we think of a more dynamic system for doing this? Essentially,
  // // there is a pipeline through which events pass through, which is
  // // reconfigured when the mode changes.


  // // Add all the individual interactors to the main panel using a stacked
  // // layout. This assures that all of the interactors have the same size as
  // // the sliceView. The actual propagation of events is handled by the
  // // event filter logic.
  // /*
  QStackedLayout *loMain = new QStackedLayout();
  loMain->setContentsMargins(0,0,0,0);
  loMain->addWidget(ui->imCrosshairs);
  loMain->addWidget(ui->imZoomPan);
  loMain->addWidget(ui->imThumbnail);
  // loMain->addWidget(ui->imPolygon);
  // loMain->addWidget(ui->imSnakeROI);
  // loMain->addWidget(ui->imPaintbrush);
  delete ui->sliceView->layout();
  ui->sliceView->setLayout(loMain);
  // */

  // Configure the initial event chain
  m_CurrentEventFilter = NULL;
  ConfigureEventChain(ui->imCrosshairs);

  // Send wheel events from Crosshairs mode to the slider
  ui->imCrosshairs->SetWheelEventTargetWidget(ui->inSlicePosition);
  // ui->imCrosshairs->SetWheelEventIntensityMaxWidget(ui->inWindowIntensityPosition);
  // ui->imCrosshairs->SetWheelEventIntensityMinWidget(ui->inLevelIntensityPosition);

  // // Set page size on the slice position widget
  // ui->inSlicePosition->setPageStep(5);
  // ui->inWindowIntensityPosition->setPageStep(100);
  // ui->inLevelIntensityPosition->setPageStep(100);

  // // ui->inWindowIntensityPosition->setVisible(false);        // not-visible scroll bars that deals with 
  // // ui->inLevelIntensityPosition->setVisible(false);

  // // Set up the drawing cursor
  // QBitmap bmBitmap(":/root/crosshair_cursor_bitmap.png");
  // QBitmap bmMask(":/root/crosshair_cursor_mask.png");
  // m_DrawingCrosshairCursor = new QCursor(bmBitmap, bmMask, 7, 7);

}

SliceDefaultViewPanel::~SliceDefaultViewPanel()
{
  delete ui;
}

GenericSliceView * SliceDefaultViewPanel::GetSliceView()
{
  return ui->sliceView;
}

void SliceDefaultViewPanel::Initialize(GlobalUIModel *model)
{
  // Store the model
  this->m_GlobalUI = model;
  this->m_Index = AXIAL;
  // Get the slice model  
  m_SliceModel = m_GlobalUI->GetSliceModel(AXIAL);
  
  m_IntensityModel = m_GlobalUI->GetIntensityCurveModel();

  // Initialize the slice view
  ui->sliceView->SetDefaultModel(m_SliceModel);


  // Initialize the interaction modes
  // ui->imCrosshairs->SetModel(m_GlobalUI->GetCursorNavigationModel(index));
  ui->imZoomPan->SetModel(m_GlobalUI->GetCursorNavigationModel(this->m_Index));
  ui->imZoomPan->SetMouseButtonBehaviorToZoomPanMode();

//   // // ui->labelQuickList->SetModel(m_GlobalUI);

  // Initialize the 'orphan' renderers (without a custom widget)
  GenericSliceRenderer *parentRenderer =
      static_cast<GenericSliceRenderer *>(ui->sliceView->GetRenderer());

}
//   // m_DecorationRenderer->SetParentRenderer(parentRenderer);
//   // m_SnakeModeRenderer->SetParentRenderer(parentRenderer);
//   // m_SnakeModeRenderer->SetModel(m_GlobalUI->GetSnakeWizardModel());

  // Listen to cursor change events, which require a repaint of the slice view
  // connectITK(m_GlobalUI->GetDriver(), CursorUpdateEvent());
  // connectITK(m_GlobalUI->GetDriver(), MainImageDimensionsChangeEvent());

//   // // Add listener for changes to the model
//   // connectITK(m_SliceModel, ModelUpdateEvent());

//   // // Listen to toolbar change events
//   // connectITK(m_GlobalUI, ToolbarModeChangeEvent());

//   // // Listen to the Snake ROI model too
//   // connectITK(m_GlobalUI->GetSnakeROIModel(index),
//   //            ModelUpdateEvent());

//   // // Listen to paintbrush motion
//   // connectITK(m_GlobalUI->GetPaintbrushModel(index),
//   //            PaintbrushModel::PaintbrushMovedEvent());

//   // // Listen to registration changes
//   // connectITK(m_GlobalUI->GetInteractiveRegistrationModel(index), ModelUpdateEvent());

//   // // Listen to all (?) events from the snake wizard as well
//   // connectITK(m_GlobalUI->GetSnakeWizardModel(), IRISEvent());

  

//   // //TODO: Widget coupling 
//   // makeCoupling(ui->inSlicePosition, m_SliceModel->GetSliceIndexModel());  
//   // makeCoupling(ui->inLevelIntensityPosition, m_IntensityModel->GetManualIntensityRangeModel(IntensityCurveModel::LEVEL));
//   // makeCoupling(ui->inWindowIntensityPosition, m_IntensityModel->GetManualIntensityRangeModel(IntensityCurveModel::WINDOW));
//   // // makeCoupling(ui->inLevelIntensityPosition, )

  

//   // // Activation
//   // activateOnFlag(this, m_GlobalUI, UIF_BASEIMG_LOADED);

//   // // Activation for the tile/thumb controls
//   // activateOnFlag(ui->btnToggleLayout, m_GlobalUI, UIF_MULTIPLE_BASE_LAYERS,
//   //                QtWidgetActivator::HideInactive);

//   // // Set up activation for polygon buttons
//   // PolygonDrawingModel *pm = m_GlobalUI->GetPolygonDrawingModel(index);

//   // activateOnAllFlags(ui->actionAccept, pm, UIF_EDITING, UIF_HAVEPOLYGON);
//   // activateOnAllFlags(ui->actionPaste, pm, UIF_INACTIVE, UIF_HAVECACHED);
//   // activateOnAllFlags(ui->actionClearDrawing, pm, UIF_DRAWING, UIF_HAVEPOLYGON);
//   // activateOnAllFlags(ui->actionComplete, pm, UIF_DRAWING, UIF_HAVEPOLYGON);
//   // activateOnAllFlags(ui->actionCompleteAndAccept, pm, UIF_DRAWING, UIF_HAVEPOLYGON);
//   // activateOnAllFlags(ui->actionDeleteSelected, pm, UIF_EDITING, UIF_HAVE_VERTEX_SELECTION);
//   // activateOnAllFlags(ui->actionSplitSelected, pm, UIF_EDITING, UIF_HAVE_EDGE_SELECTION);
//   // activateOnAllFlags(ui->actionUndo, pm, UIF_DRAWING, UIF_HAVEPOLYGON);
//   // activateOnAllFlags(ui->actionClearPolygon, pm, UIF_EDITING, UIF_HAVEPOLYGON);

//   // // Update the expand view button
//   // this->UpdateExpandViewButton();

//   // // Listen to the events affecting the expand view button
//   // DisplayLayoutModel *dlm = m_GlobalUI->GetDisplayLayoutModel();
//   // connectITK(dlm, DisplayLayoutModel::ViewPanelLayoutChangeEvent());
//   // connectITK(dlm, DisplayLayoutModel::LayerLayoutChangeEvent());

//   // // Arrange the rendering overlays and widgets based on current mode
//   // this->OnToolbarModeChange();

//   // // Listen for hover changes to move and activate widgets
//   // connectITK(m_SliceModel->GetHoveredImageLayerIdModel(), ValueChangedEvent(),
//   //            SLOT(OnHoveredLayerChange(const EventBucket &)));

//   // connectITK(m_SliceModel->GetHoveredImageIsThumbnailModel(), ValueChangedEvent(),
//   //            SLOT(OnHoveredLayerChange(const EventBucket &)));

// }

// void SliceDefaultViewPanel::onModelUpdate(const EventBucket &eb)
// {
//   if(eb.HasEvent(ToolbarModeChangeEvent()) ||
//      eb.HasEvent(StateMachineChangeEvent()))
//     {
//     OnToolbarModeChange();
//     }
//   if(eb.HasEvent(DisplayLayoutModel::ViewPanelLayoutChangeEvent()) ||
//      eb.HasEvent(DisplayLayoutModel::LayerLayoutChangeEvent()))
//     {
//     UpdateExpandViewButton();
//     }
//   ui->sliceView->update();
// }

void SliceDefaultViewPanel::on_inSlicePosition_valueChanged(int value)
{
  // Update the text output
  int pos = ui->inSlicePosition->value();
  int lim = ui->inSlicePosition->maximum();
  ui->lblSliceInfo->setText(QString("%1 of %2").arg(pos+1).arg(lim+1));
  
}


// void SliceDefaultViewPanel::on_inLevelIntensityPosition_valueChanged(int value) {  }
// void SliceDefaultViewPanel::on_inWindowIntensityPosition_valueChanged(int value) {  }

void SliceDefaultViewPanel::ConfigureEventChain(QWidget *w)
{
  // Remove all event filters from the slice view
  QObjectList kids = ui->sliceView->children();
  for(QObjectList::Iterator it = kids.begin(); it!=kids.end(); ++it)
    ui->sliceView->removeEventFilter(*it);

  // Now add the event filters in the order in which we want them to react
  // to events. The last event filter is first to receive events, and should
  // thus be the thumbnail interaction mode. The first event filter is always
  // the crosshairs interaction mode, which is the fallback for all others.
  ui->sliceView->installEventFilter(ui->imCrosshairs);

  // If the current mode is not crosshairs mode, add it as the filter
  if(w != ui->imCrosshairs)
    {
    ui->sliceView->installEventFilter(w);
    }

  // The last guy in the chain is the thumbnail interactor
  ui->sliceView->installEventFilter(ui->imThumbnail);
}

// TODO: implement semi-transparent rendering on widgets on top of the
// OpenGL scene using code from
// http://www.qtcentre.org/wiki/index.php?title=Accelerate_your_Widgets_with_OpenGL
void SliceDefaultViewPanel::enterEvent(QEvent *)
{
  /*
  ui->mainToolbar->show();
  ui->sidebar->show();
  */
}

void SliceDefaultViewPanel::leaveEvent(QEvent *)
{
  /*
  ui->mainToolbar->hide();
  ui->sidebar->hide();
  */
}

// void SliceDefaultViewPanel::SetActiveMode(QWidget *mode, bool clearChildren)
// {
//   // If the widget does not have a stacked layout, do nothing, we've reached
//   // the end of the recursion
//   QStackedLayout *loParent =
//       dynamic_cast<QStackedLayout *>(mode->parentWidget()->layout());

//   if(loParent)
//     {
//     // Set the mode as the current widget in the parent
//     loParent->setCurrentWidget(mode);

//     // Make sure the parent widget is also the current widget
//     SetActiveMode(mode->parentWidget(), false);

//     // Make sure no children are selected
//     if(clearChildren)
//       {
//       // If the selected mode has child modes, make sure none is selected
//       QStackedLayout *lo = dynamic_cast<QStackedLayout *>(mode->layout());
//       if(lo)
//         lo->setCurrentIndex(0);
//       }
//     }
// }

// void SliceDefaultViewPanel::OnToolbarModeChange()
// {
//   // Get the renderer and configure its overlays
//   GenericSliceRenderer *ren = (GenericSliceRenderer *) ui->sliceView->GetRenderer();

//   // Configure the renderers
//   GenericSliceRenderer::RendererDelegateList &ovTiled = ren->GetTiledOverlays();
//   GenericSliceRenderer::RendererDelegateList &ovGlobal = ren->GetGlobalOverlays();

//   // Append the overlays in the right order
//   ovTiled.clear();
//   ovTiled.push_back(m_SnakeModeRenderer);
//   ovTiled.push_back(ui->imCrosshairs->GetRenderer());
//   ovTiled.push_back(ui->imPolygon->GetRenderer());

//   ovGlobal.clear();
//   ovGlobal.push_back(m_DecorationRenderer);

//   switch(m_GlobalUI->GetGlobalState()->GetToolbarMode())
//     {
//     case POLYGON_DRAWING_MODE:
//       ConfigureEventChain(ui->imPolygon);
//       break;
//     case PAINTBRUSH_MODE:
//       ConfigureEventChain(ui->imPaintbrush);
//       ovTiled.push_back(ui->imPaintbrush->GetRenderer());
//       break;
//     case REGISTRATION_MODE:
//       ConfigureEventChain(ui->imRegistration);
//       ovTiled.push_back(ui->imRegistration->GetRenderer());
//       break;
//     case ROI_MODE:
//       ConfigureEventChain(ui->imSnakeROI);
//       ovTiled.push_back(ui->imSnakeROI->GetRenderer());
//       break;
//     case CROSSHAIRS_MODE:
//       ConfigureEventChain(ui->imCrosshairs);
//       break;
//     case NAVIGATION_MODE:
//       ConfigureEventChain(ui->imZoomPan);
//       break;
//     }

//   // Need to change to the appropriate page
//   QStackedLayout *loPages =
//       static_cast<QStackedLayout *>(ui->toolbar->layout());


//   if(m_GlobalUI->GetGlobalState()->GetToolbarMode() == POLYGON_DRAWING_MODE)
//     {
//     switch(m_GlobalUI->GetPolygonDrawingModel(m_Index)->GetState())
//       {
//       case PolygonDrawingModel::DRAWING_STATE:
//         loPages->setCurrentWidget(ui->pagePolygonDraw); break;
//       case PolygonDrawingModel::EDITING_STATE:
//         loPages->setCurrentWidget(ui->pagePolygonEdit); break;
//       case PolygonDrawingModel::INACTIVE_STATE:
//         loPages->setCurrentWidget(ui->pagePolygonInactive); break;
//       }
//     }
//   else
//     {
//     loPages->setCurrentWidget(ui->pageDefault);           //TODO: fix this back to pageDefault
//     }

  
//   if (this->DEFAULT_MODE) {
//     loPages->setCurrentWidget(ui->pageDefaultMore);
//   }
// }

// void SliceDefaultViewPanel::zoomToFit() {
//   m_GlobalUI->GetSliceCoordinator()->ResetViewToFitInOneWindow(m_Index);
// }

void SliceDefaultViewPanel::on_btnZoomToFit_clicked()
{
  std::cout << "btnZoomToFit   SEGMENTATION   " << m_SliceModel->GetSliceSize() << std::endl;
}

// void SliceDefaultViewPanel::on_btnZoomToFit_2_clicked()
// {
//   this->zoomToFit();
// }

// void SliceDefaultViewPanel::onContextMenu()
// {
//   if(m_GlobalUI->GetGlobalState()->GetToolbarMode() == POLYGON_DRAWING_MODE)
//     {
//     QMenu *menu = NULL;
//     switch(m_GlobalUI->GetPolygonDrawingModel(m_Index)->GetState())
//       {
//       case PolygonDrawingModel::DRAWING_STATE:
//         menu = m_MenuPolyDrawing; break;
//       case PolygonDrawingModel::EDITING_STATE:
//         menu = m_MenuPolyEditing; break;
//       case PolygonDrawingModel::INACTIVE_STATE:
//         menu = m_MenuPolyInactive; break;
//       }

//     if(menu)
//       {
//       menu->popup(QCursor::pos());
//       }
//     }
// }

// void SliceDefaultViewPanel::SetMouseMotionTracking(bool enable)
// {
//   ui->sliceView->setMouseTracking(enable);
//   // TODO: in the future, consider using a better cursor for polygon drawing operations
//   /*
//   if(enable)
//     this->setCursor(*m_DrawingCrosshairCursor);
//   else
//     this->setCursor(QCursor(Qt::ArrowCursor));
//     */
// }

// void SliceDefaultViewPanel::on_btnExpand_clicked()
// {
//   // Get the layout applied when the button is pressed
//   DisplayLayoutModel *dlm = m_GlobalUI->GetDisplayLayoutModel();
//   std::cout << m_Index << std::endl;
//   DisplayLayoutModel::ViewPanelLayout layout =
//       dlm->GetViewPanelExpandButtonActionModel(m_Index)->GetValue();

//   // Apply this layout
//   dlm->GetViewPanelLayoutModel()->SetValue(layout);
// }

void SliceDefaultViewPanel::UpdateExpandViewButton() {
//   // Get the layout applied when the button is pressed
//   DisplayLayoutModel *dlm = m_GlobalUI->GetDisplayLayoutModel();
//   DisplayLayoutModel::ViewPanelLayout layout =
//       dlm->GetViewPanelExpandButtonActionModel(m_Index)->GetValue();

//   // Set the appropriate icon
//   static const char* iconNames[] =
//     { "fourviews", "axial", "sagittal", "coronal", "default" };

//   QString iconFile = QString(":/root/dl_%1.png").arg(iconNames[layout]);
//   ui->btnExpand->setIcon(QIcon(iconFile));

//   // Set the tooltip
//   if(layout == DisplayLayoutModel::VIEW_ALL)
//     {
//     ui->btnExpand->setToolTip("Restore the four-panel display configuration");
//     }
//   else
//     {
//     ui->btnExpand->setToolTip("Expand this view to occupy the entire window");
//     }

//   // Also expand the tile/cascade button
//   LayerLayout ll = dlm->GetSliceViewLayerLayoutModel()->GetValue();
//   if(ll == LAYOUT_TILED)
//     {
//     ui->btnToggleLayout->setIcon(QIcon(":/root/layout_thumb_16.png"));
//     }
//   else if(ll == LAYOUT_STACKED)
//     {
//     ui->btnToggleLayout->setIcon(QIcon(":/root/layout_tile_16.png"));
//     }
}


// void SliceDefaultViewPanel::on_btnScreenshot_clicked()
// {
//   MainImageWindow *parent = findParentWidget<MainImageWindow>(this);
//   parent->ExportScreenshot(m_Index);
// }

// void SliceDefaultViewPanel::on_btnToggleLayout_clicked()
// {
//   m_GlobalUI->GetDisplayLayoutModel()->ToggleSliceViewLayerLayout();
// }

void SliceDefaultViewPanel::on_actionZoom_In_triggered()
{
  // Zoom in
  // m_GlobalUI->GetSliceCoordinator()->ZoomInOrOutInOneWindow(m_Index, 1.1);
}

void SliceDefaultViewPanel::on_actionZoom_Out_triggered()
{
  // Zoom out
  // m_GlobalUI->GetSliceCoordinator()->ZoomInOrOutInOneWindow(m_Index, 1.0 / 1.1);
}

// void SliceDefaultViewPanel::OnHoveredLayerChange(const EventBucket &eb)
// {
//   // Determine the position where to draw the button
//   const SliceViewportLayout::SubViewport *vp = m_SliceModel->GetHoveredViewport();
//   if(vp)
//     {
//     // Set up the location of the button
//     int vppr = m_SliceModel->GetSizeReporter()->GetViewportPixelRatio();
//     int x = (vp->pos[0] + vp->size[0]) / vppr - m_ContextToolButton->width();
//     int y = (ui->sliceView->height() - 1) - (vp->pos[1] + vp->size[1]) / vppr;
//     m_ContextToolButton->setVisible(true);
//     m_ContextToolButton->move(x, y);

//     // Set up the context menu on the button
//     MainImageWindow *winmain = findParentWidget<MainImageWindow>(this);
//     LayerInspectorDialog *inspector = winmain->GetLayerInspector();

//     // Get the corresponding context menu
//     QMenu *menu = inspector->GetLayerContextMenu(
//                     m_SliceModel->GetDriver()->GetCurrentImageData()->FindLayer(
//                       m_SliceModel->GetHoveredImageLayerId(), false, ALL_ROLES));

//     // Show the menu
//     m_ContextToolButton->setMenu(menu);
//     m_ContextToolButton->setDown(false);
//     }
//   else
//     {
//     m_ContextToolButton->setVisible(false);
//     m_ContextToolButton->setMenu(NULL);
//     m_ContextToolButton->setDown(false);
//     }
// }

// void SliceDefaultViewPanel::IsDefaultMode() {
//   this->DEFAULT_MODE = true;
  
// }


void SliceDefaultViewPanel::resetTo(unsigned int index) {

  // Get the slice model
  m_SliceModel = m_GlobalUI->GetSliceModel(index);

  // Initialize the slice view
  ui->sliceView->SetDefaultModel(m_SliceModel);

  // Update
  // this->UpdateExpandViewButton();
  ui->sliceView->update();
}

void SliceDefaultViewPanel::on_btnAxialView_clicked() {
  std::cout << "btn Axial clicked" << std::endl;
  this->resetTo(AXIAL);
}

void SliceDefaultViewPanel::on_btnCoronalView_clicked() {
  std::cout << "btn Coronal clicked" << std::endl;
  this->resetTo(CORONAL);
}

void SliceDefaultViewPanel::on_btnSagittalView_clicked() {
  std::cout << "btn Sagittal clicked" << std::endl;
  this->resetTo(SAGITTAL);
}

