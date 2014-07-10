// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef MainFrame2_h
#define MainFrame2_h
#include <FL/Fl.H>
/** Main window class generated from FLTK. */
#include <FL/Fl_Color_Chooser.H>
#include "forcepad_config.h"
#include <iostream>
#include <string>
#include <Fl/Fl_Sys_Menu_Bar.H>
#include "SplashFrame.h"
#include "OptSettings.h"
#include "TabletToolbar.h"
#include "VizMixerToolbar.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include "Fl_HoverButton.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include "FlPaintView.h"
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Button.H>

class CMainFrame : CPVModeChangeEvent, CPVViewModeChangeEvent, CGSStatusMessageEvent, CGSLogMessageEvent, CGSContinueCalcEvent, CPVViewModeErrorEvent, CPVModelChangedEvent, CPVRulerChangedEvent, CPVVisualisationModeChangedEvent, CPVModelLoadedEvent, CPVNewModelEvent {
  bool m_continueCalc; 
  CPaintView::TEditMode m_saveEditMode; 
  CPaintView::TEditMode m_sketchEditMode; 
  CPaintView::TEditMode m_physicsEditMode; 
  CTabletToolbar* m_tabletToolbar; 
  CVizMixerToolbar* m_vizMixer; 
  COptSettings* m_optSettings; 
public:
  CMainFrame();
  Fl_Double_Window *wndMain;
  Fl_Sys_Menu_Bar *mainMenu;
  static Fl_Menu_Item menu_mainMenu[];
  static Fl_Menu_Item *fileMenu;
  static Fl_Menu_Item *fileMenuNew;
private:
  inline void cb_fileMenuNew_i(Fl_Menu_*, void*);
  static void cb_fileMenuNew(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *fileMenuOpen;
private:
  inline void cb_fileMenuOpen_i(Fl_Menu_*, void*);
  static void cb_fileMenuOpen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *fileMenuSave;
private:
  inline void cb_fileMenuSave_i(Fl_Menu_*, void*);
  static void cb_fileMenuSave(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *fileMenuSaveAs;
private:
  inline void cb_fileMenuSaveAs_i(Fl_Menu_*, void*);
  static void cb_fileMenuSaveAs(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *fileMenuSaveMatlab;
  static Fl_Menu_Item *fileMenuSaveNumPy;
  static Fl_Menu_Item *fileMenuExit;
private:
  inline void cb_fileMenuExit_i(Fl_Menu_*, void*);
  static void cb_fileMenuExit(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *editMenu;
  static Fl_Menu_Item *editMenuUndo;
private:
  inline void cb_editMenuUndo_i(Fl_Menu_*, void*);
  static void cb_editMenuUndo(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *editMenuCut;
private:
  inline void cb_editMenuCut_i(Fl_Menu_*, void*);
  static void cb_editMenuCut(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *editMenuCopy;
private:
  inline void cb_editMenuCopy_i(Fl_Menu_*, void*);
  static void cb_editMenuCopy(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *editMenuPaste;
private:
  inline void cb_editMenuPaste_i(Fl_Menu_*, void*);
  static void cb_editMenuPaste(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *viewMenu;
  static Fl_Menu_Item *viewMenuZoomIn;
private:
  inline void cb_viewMenuZoomIn_i(Fl_Menu_*, void*);
  static void cb_viewMenuZoomIn(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *viewMenuZoomOut;
private:
  inline void cb_viewMenuZoomOut_i(Fl_Menu_*, void*);
  static void cb_viewMenuZoomOut(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *modeMenu;
  static Fl_Menu_Item *modeMenuSketch;
private:
  inline void cb_modeMenuSketch_i(Fl_Menu_*, void*);
  static void cb_modeMenuSketch(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *modeMenuPhysics;
private:
  inline void cb_modeMenuPhysics_i(Fl_Menu_*, void*);
  static void cb_modeMenuPhysics(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *modeMenuAction;
private:
  inline void cb_modeMenuAction_i(Fl_Menu_*, void*);
  static void cb_modeMenuAction(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *settingsMenu;
  static Fl_Menu_Item *settingsMenuCalculation;
private:
  inline void cb_settingsMenuCalculation_i(Fl_Menu_*, void*);
  static void cb_settingsMenuCalculation(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *settingsMenuGeneral;
private:
  inline void cb_settingsMenuGeneral_i(Fl_Menu_*, void*);
  static void cb_settingsMenuGeneral(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *settingsMenuSnapToGrid;
private:
  inline void cb_settingsMenuSnapToGrid_i(Fl_Menu_*, void*);
  static void cb_settingsMenuSnapToGrid(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *helpMenu;
  static Fl_Menu_Item *helpMenuContents;
  static Fl_Menu_Item *helpMenuAbout;
private:
  inline void cb_helpMenuAbout_i(Fl_Menu_*, void*);
  static void cb_helpMenuAbout(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *helpMenuLog;
private:
  inline void cb_helpMenuLog_i(Fl_Menu_*, void*);
  static void cb_helpMenuLog(Fl_Menu_*, void*);
public:
  Fl_Scroll *scrLeftToolbar;
  Fl_HoverButton *btnBrushTools;
private:
  inline void cb_btnBrushTools_i(Fl_HoverButton*, void*);
  static void cb_btnBrushTools(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnFloodFill;
private:
  inline void cb_btnFloodFill_i(Fl_HoverButton*, void*);
  static void cb_btnFloodFill(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnDrawingTools;
private:
  inline void cb_btnDrawingTools_i(Fl_HoverButton*, void*);
  static void cb_btnDrawingTools(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnSelectTools;
private:
  inline void cb_btnSelectTools_i(Fl_HoverButton*, void*);
  static void cb_btnSelectTools(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnOptLayer;
private:
  inline void cb_btnOptLayer_i(Fl_HoverButton*, void*);
  static void cb_btnOptLayer(Fl_HoverButton*, void*);
public:
  Fl_Group *paintGroup;
  CFlPaintView *paintView;
  Fl_Scroll *scrLeftResultToolbar;
  Fl_HoverButton *btnStress;
private:
  inline void cb_btnStress_i(Fl_HoverButton*, void*);
  static void cb_btnStress(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnDisplacements;
private:
  inline void cb_btnDisplacements_i(Fl_HoverButton*, void*);
  static void cb_btnDisplacements(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnMisesStress;
private:
  inline void cb_btnMisesStress_i(Fl_HoverButton*, void*);
  static void cb_btnMisesStress(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnMoveLoad;
private:
  inline void cb_btnMoveLoad_i(Fl_HoverButton*, void*);
  static void cb_btnMoveLoad(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnRotateLoad;
private:
  inline void cb_btnRotateLoad_i(Fl_HoverButton*, void*);
  static void cb_btnRotateLoad(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnOptimize;
private:
  inline void cb_btnOptimize_i(Fl_HoverButton*, void*);
  static void cb_btnOptimize(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnStructure;
private:
  inline void cb_btnStructure_i(Fl_HoverButton*, void*);
  static void cb_btnStructure(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnVizMixer;
private:
  inline void cb_btnVizMixer_i(Fl_HoverButton*, void*);
  static void cb_btnVizMixer(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnZoomResults;
private:
  inline void cb_btnZoomResults_i(Fl_HoverButton*, void*);
  static void cb_btnZoomResults(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightForceToolbar;
  Fl_HoverButton *btnAddForce;
private:
  inline void cb_btnAddForce_i(Fl_HoverButton*, void*);
  static void cb_btnAddForce(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEraseForces;
private:
  inline void cb_btnEraseForces_i(Fl_HoverButton*, void*);
  static void cb_btnEraseForces(Fl_HoverButton*, void*);
public:
  Fl_Value_Input *forceMagnitude;
private:
  inline void cb_forceMagnitude_i(Fl_Value_Input*, void*);
  static void cb_forceMagnitude(Fl_Value_Input*, void*);
public:
  Fl_HoverButton *btnUseWeight;
private:
  inline void cb_btnUseWeight_i(Fl_HoverButton*, void*);
  static void cb_btnUseWeight(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightBCToolbar;
  Fl_HoverButton *btnAddBc2;
private:
  inline void cb_btnAddBc2_i(Fl_HoverButton*, void*);
  static void cb_btnAddBc2(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnRemoveBc2;
private:
  inline void cb_btnRemoveBc2_i(Fl_HoverButton*, void*);
  static void cb_btnRemoveBc2(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnRotateBc2;
private:
  inline void cb_btnRotateBc2_i(Fl_HoverButton*, void*);
  static void cb_btnRotateBc2(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightDrawingToolbar;
  Fl_HoverButton *btnRectangle;
private:
  inline void cb_btnRectangle_i(Fl_HoverButton*, void*);
  static void cb_btnRectangle(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEllipse;
private:
  inline void cb_btnEllipse_i(Fl_HoverButton*, void*);
  static void cb_btnEllipse(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnLine;
private:
  inline void cb_btnLine_i(Fl_HoverButton*, void*);
  static void cb_btnLine(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnArch;
private:
  inline void cb_btnArch_i(Fl_HoverButton*, void*);
  static void cb_btnArch(Fl_HoverButton*, void*);
  inline void cb__i(Fl_HoverButton*, void*);
  static void cb_(Fl_HoverButton*, void*);
  inline void cb_1_i(Fl_HoverButton*, void*);
  static void cb_1(Fl_HoverButton*, void*);
  inline void cb_2_i(Fl_HoverButton*, void*);
  static void cb_2(Fl_HoverButton*, void*);
  inline void cb_3_i(Fl_HoverButton*, void*);
  static void cb_3(Fl_HoverButton*, void*);
  inline void cb_4_i(Fl_HoverButton*, void*);
  static void cb_4(Fl_HoverButton*, void*);
  inline void cb_5_i(Fl_HoverButton*, void*);
  static void cb_5(Fl_HoverButton*, void*);
  inline void cb_6_i(Fl_HoverButton*, void*);
  static void cb_6(Fl_HoverButton*, void*);
  inline void cb_7_i(Fl_HoverButton*, void*);
  static void cb_7(Fl_HoverButton*, void*);
  inline void cb_8_i(Fl_HoverButton*, void*);
  static void cb_8(Fl_HoverButton*, void*);
  inline void cb_9_i(Fl_HoverButton*, void*);
  static void cb_9(Fl_HoverButton*, void*);
  inline void cb_a_i(Fl_HoverButton*, void*);
  static void cb_a(Fl_HoverButton*, void*);
  inline void cb_b_i(Fl_HoverButton*, void*);
  static void cb_b(Fl_HoverButton*, void*);
public:
  Fl_Value_Slider *sldLineWidth;
private:
  inline void cb_sldLineWidth_i(Fl_Value_Slider*, void*);
  static void cb_sldLineWidth(Fl_Value_Slider*, void*);
public:
  Fl_Scroll *scrRightStressToolbar;
  Fl_HoverButton *btnTensionCompression;
private:
  inline void cb_btnTensionCompression_i(Fl_HoverButton*, void*);
  static void cb_btnTensionCompression(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnTension;
private:
  inline void cb_btnTension_i(Fl_HoverButton*, void*);
  static void cb_btnTension(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnCompression;
private:
  inline void cb_btnCompression_i(Fl_HoverButton*, void*);
  static void cb_btnCompression(Fl_HoverButton*, void*);
public:
  Fl_Roller *rolArrowSize;
private:
  inline void cb_rolArrowSize_i(Fl_Roller*, void*);
  static void cb_rolArrowSize(Fl_Roller*, void*);
public:
  Fl_Roller *rolTransparency;
private:
  inline void cb_rolTransparency_i(Fl_Roller*, void*);
  static void cb_rolTransparency(Fl_Roller*, void*);
public:
  Fl_Roller *rolStep;
private:
  inline void cb_rolStep_i(Fl_Roller*, void*);
  static void cb_rolStep(Fl_Roller*, void*);
public:
  Fl_HoverButton *btnLockScale;
private:
  inline void cb_btnLockScale_i(Fl_HoverButton*, void*);
  static void cb_btnLockScale(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightDisplacementToolbar;
  Fl_Slider *sldDisplacementScale;
private:
  inline void cb_sldDisplacementScale_i(Fl_Slider*, void*);
  static void cb_sldDisplacementScale(Fl_Slider*, void*);
public:
  Fl_Scroll *scrRightSelectToolbar;
  Fl_HoverButton *btnSelect;
private:
  inline void cb_btnSelect_i(Fl_HoverButton*, void*);
  static void cb_btnSelect(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnCut;
private:
  inline void cb_btnCut_i(Fl_HoverButton*, void*);
  static void cb_btnCut(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnCopy;
private:
  inline void cb_btnCopy_i(Fl_HoverButton*, void*);
  static void cb_btnCopy(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnPaste;
private:
  inline void cb_btnPaste_i(Fl_HoverButton*, void*);
  static void cb_btnPaste(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnPasteWin;
private:
  inline void cb_btnPasteWin_i(Fl_HoverButton*, void*);
  static void cb_btnPasteWin(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnExpandImage;
private:
  inline void cb_btnExpandImage_i(Fl_HoverButton*, void*);
  static void cb_btnExpandImage(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightFillToolbar;
private:
  inline void cb_c_i(Fl_HoverButton*, void*);
  static void cb_c(Fl_HoverButton*, void*);
  inline void cb_d_i(Fl_HoverButton*, void*);
  static void cb_d(Fl_HoverButton*, void*);
  inline void cb_e_i(Fl_HoverButton*, void*);
  static void cb_e(Fl_HoverButton*, void*);
  inline void cb_f_i(Fl_HoverButton*, void*);
  static void cb_f(Fl_HoverButton*, void*);
  inline void cb_10_i(Fl_HoverButton*, void*);
  static void cb_10(Fl_HoverButton*, void*);
  inline void cb_11_i(Fl_HoverButton*, void*);
  static void cb_11(Fl_HoverButton*, void*);
  inline void cb_12_i(Fl_HoverButton*, void*);
  static void cb_12(Fl_HoverButton*, void*);
  inline void cb_13_i(Fl_HoverButton*, void*);
  static void cb_13(Fl_HoverButton*, void*);
  inline void cb_14_i(Fl_HoverButton*, void*);
  static void cb_14(Fl_HoverButton*, void*);
  inline void cb_15_i(Fl_HoverButton*, void*);
  static void cb_15(Fl_HoverButton*, void*);
  inline void cb_16_i(Fl_HoverButton*, void*);
  static void cb_16(Fl_HoverButton*, void*);
  inline void cb_17_i(Fl_HoverButton*, void*);
  static void cb_17(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightMisesStressToolbar;
  Fl_Check_Button *chkInvertColorMap;
private:
  inline void cb_chkInvertColorMap_i(Fl_Check_Button*, void*);
  static void cb_chkInvertColorMap(Fl_Check_Button*, void*);
public:
  Fl_HoverButton *btnColorMap1;
private:
  inline void cb_btnColorMap1_i(Fl_HoverButton*, void*);
  static void cb_btnColorMap1(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnColorMap2;
private:
  inline void cb_btnColorMap2_i(Fl_HoverButton*, void*);
  static void cb_btnColorMap2(Fl_HoverButton*, void*);
public:
  Fl_Slider *sldUpperMisesTreshold;
private:
  inline void cb_sldUpperMisesTreshold_i(Fl_Slider*, void*);
  static void cb_sldUpperMisesTreshold(Fl_Slider*, void*);
public:
  Fl_Scroll *scrRightDrawToolbar;
  Fl_HoverButton *btnBrush;
private:
  inline void cb_btnBrush_i(Fl_HoverButton*, void*);
  static void cb_btnBrush(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnErase;
private:
  inline void cb_btnErase_i(Fl_HoverButton*, void*);
  static void cb_btnErase(Fl_HoverButton*, void*);
public:
  Fl_Group *grpBrushTypes;
  Fl_HoverButton *btnBrush4;
private:
  inline void cb_btnBrush4_i(Fl_HoverButton*, void*);
  static void cb_btnBrush4(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnBrush8;
private:
  inline void cb_btnBrush8_i(Fl_HoverButton*, void*);
  static void cb_btnBrush8(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnBrush16;
private:
  inline void cb_btnBrush16_i(Fl_HoverButton*, void*);
  static void cb_btnBrush16(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnBrush32;
private:
  inline void cb_btnBrush32_i(Fl_HoverButton*, void*);
  static void cb_btnBrush32(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnBrush64;
private:
  inline void cb_btnBrush64_i(Fl_HoverButton*, void*);
  static void cb_btnBrush64(Fl_HoverButton*, void*);
  inline void cb_18_i(Fl_HoverButton*, void*);
  static void cb_18(Fl_HoverButton*, void*);
  inline void cb_19_i(Fl_HoverButton*, void*);
  static void cb_19(Fl_HoverButton*, void*);
  inline void cb_1a_i(Fl_HoverButton*, void*);
  static void cb_1a(Fl_HoverButton*, void*);
  inline void cb_1b_i(Fl_HoverButton*, void*);
  static void cb_1b(Fl_HoverButton*, void*);
  inline void cb_1c_i(Fl_HoverButton*, void*);
  static void cb_1c(Fl_HoverButton*, void*);
  inline void cb_1d_i(Fl_HoverButton*, void*);
  static void cb_1d(Fl_HoverButton*, void*);
  inline void cb_1e_i(Fl_HoverButton*, void*);
  static void cb_1e(Fl_HoverButton*, void*);
  inline void cb_1f_i(Fl_HoverButton*, void*);
  static void cb_1f(Fl_HoverButton*, void*);
  inline void cb_20_i(Fl_HoverButton*, void*);
  static void cb_20(Fl_HoverButton*, void*);
  inline void cb_21_i(Fl_HoverButton*, void*);
  static void cb_21(Fl_HoverButton*, void*);
  inline void cb_22_i(Fl_HoverButton*, void*);
  static void cb_22(Fl_HoverButton*, void*);
  inline void cb_23_i(Fl_HoverButton*, void*);
  static void cb_23(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrLeftPhysicsToolbar;
  Fl_HoverButton *btnForce;
private:
  inline void cb_btnForce_i(Fl_HoverButton*, void*);
  static void cb_btnForce(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnConstraint2;
private:
  inline void cb_btnConstraint2_i(Fl_HoverButton*, void*);
  static void cb_btnConstraint2(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnConstraintHinge;
private:
  inline void cb_btnConstraintHinge_i(Fl_HoverButton*, void*);
  static void cb_btnConstraintHinge(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnDimensionTools;
private:
  inline void cb_btnDimensionTools_i(Fl_HoverButton*, void*);
  static void cb_btnDimensionTools(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnThickness;
private:
  inline void cb_btnThickness_i(Fl_HoverButton*, void*);
  static void cb_btnThickness(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightBCHingeToolbar;
  Fl_HoverButton *btnAddBcHinge;
private:
  inline void cb_btnAddBcHinge_i(Fl_HoverButton*, void*);
  static void cb_btnAddBcHinge(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnRemoveBcHinge;
private:
  inline void cb_btnRemoveBcHinge_i(Fl_HoverButton*, void*);
  static void cb_btnRemoveBcHinge(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightThicknessToolbar;
  Fl_Value_Input *thickness;
private:
  inline void cb_thickness_i(Fl_Value_Input*, void*);
  static void cb_thickness(Fl_Value_Input*, void*);
public:
  Fl_Scroll *scrRightDimensionToolbar;
  Fl_Value_Input *actualLength;
private:
  inline void cb_actualLength_i(Fl_Value_Input*, void*);
  static void cb_actualLength(Fl_Value_Input*, void*);
public:
  Fl_Value_Output *pixelLength;
  Fl_Group *statusBar;
  Fl_Output *statusOutput;
  Fl_Progress *calcProgress;
  Fl_Button *btnStopCalculation;
private:
  inline void cb_btnStopCalculation_i(Fl_Button*, void*);
  static void cb_btnStopCalculation(Fl_Button*, void*);
public:
  Fl_Button *btnTabletToolbar;
private:
  inline void cb_btnTabletToolbar_i(Fl_Button*, void*);
  static void cb_btnTabletToolbar(Fl_Button*, void*);
public:
  Fl_Group *scrRightToolbar;
  Fl_HoverButton *btnPhysical;
private:
  inline void cb_btnPhysical_i(Fl_HoverButton*, void*);
  static void cb_btnPhysical(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnAction;
private:
  inline void cb_btnAction_i(Fl_HoverButton*, void*);
  static void cb_btnAction(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnSketch;
private:
  inline void cb_btnSketch_i(Fl_HoverButton*, void*);
  static void cb_btnSketch(Fl_HoverButton*, void*);
public:
  Fl_Scroll *scrRightStructureToolbar;
  Fl_HoverButton *transferViewToImage;
private:
  inline void cb_transferViewToImage_i(Fl_HoverButton*, void*);
  static void cb_transferViewToImage(Fl_HoverButton*, void*);
public:
  ~CMainFrame();
  bool onContinueCalc();
  void show();
  void close();
private:
  void resetButtons();
public:
  void resetButtons(Fl_HoverButton* self);
  void setPixelWeight(double weight);
  void setExternalForce(double force);
  void setCommandLine(int argc, char **argv);
  void onModeChange(CPaintView::TEditMode oldMode, CPaintView::TEditMode newMode);
  void onViewModeChange(CPaintView::TViewMode oldMode, CPaintView::TViewMode newMode);
  void onStatusMessage(const std::string& message, const int progress);
  void onLogMessage(const std::string& context, const std::string& message);
  void hideLeftToolbars();
  void hideRightToolbars();
  void showLeftToolbar(Fl_Widget* toolbar);
  void showRightToolbar(Fl_Widget* toolbar);
  void onViewModeError(CPaintView::TViewMode oldMode, CPaintView::TViewMode newMode);
  void centerWindow(Fl_Window* window);
  Fl_Window* getMainWindow();
  void onModelChanged(const std::string& newModelName);
  void onRulerChanged(CRuler* ruler);
  void onVisualisationModeChanged(CPaintView::TVisualisationMode oldMode, CPaintView::TVisualisationMode newMode);
  void disableUserInterface();
  void enableUserInterface();
  void onNewModel();
  void onModelLoaded();
};
#endif
