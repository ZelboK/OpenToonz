#pragma once

#ifndef STYLEEDITOR_H
#define STYLEEDITOR_H

// TnzCore includes
#include "tcommon.h"
#include "tfilepath.h"
#include "tpixel.h"
#include "tpalette.h"
#include "saveloadqsettings.h"

// TnzLib includes
#include "toonz/tpalettehandle.h"
#include "toonz/txshlevelhandle.h"
#include "toonz/txshlevel.h"
#include "toonz/stylemanager.h"

// TnzQt includes
#include "toonzqt/checkbox.h"
#include "toonzqt/intfield.h"
#include "toonzqt/doublefield.h"
#include "toonzqt/colorfield.h"
#include "toonzqt/tabbar.h"
#include "toonzqt/glwidget_for_highdpi.h"
#include "toonzqt/hexcolornames.h"

// Qt includes
#include <QWidget>
#include <QFrame>
#include <QTabBar>
#include <QSlider>
#include <QToolButton>
#include <QScrollArea>
#include <QMouseEvent>
#include <QPointF>
#include <QSettings>
#include <QSplitter>

#undef DVAPI
#undef DVVAR
#ifdef TOONZQT_EXPORTS
#define DVAPI DV_EXPORT_API
#define DVVAR DV_EXPORT_VAR
#else
#define DVAPI DV_IMPORT_API
#define DVVAR DV_IMPORT_VAR
#endif

//=============================================

//    Forward declarations

class TColorStyle;
class TPalette;

class TXshLevelHandle;
class PaletteController;

class QGridLayout;
class QLabel;
class QStackedWidget;
class QSlider;
class QRadioButton;
class QButtonGroup;
class QPushButton;
class QTabWidget;
class QToolBar;
class QOpenGLFramebufferObject;

class ColorSquaredWheel;
class TabBarContainter;
class StyleChooser;
class StyleEditor;
class LutCalibrator;

//=============================================

//=============================================================================
namespace StyleEditorGUI {
//=============================================================================

enum ColorChannel {
  eRed = 0,
  eGreen,
  eBlue,
  eAlpha,
  eHue,
  eSaturation,
  eValue
};

//=============================================================================
/*! \brief The ColorModel provides an object to manage color change and
                its transformation from rgb value to hsv value and vice versa.

                This object change color using its rgb channel or its hsv
   channel;
                if you change a color channel class assure you that other
   channel not change.
*/
class DVAPI ColorModel {
  int m_channels[7];
  void rgb2hsv();
  void hsv2rgb();

public:
  ColorModel();

  void setTPixel(const TPixel32 &color);
  TPixel32 getTPixel() const;

  void setValue(ColorChannel channel, int value);
  void setValues(ColorChannel channel, int u, int v);
  int getValue(ColorChannel channel) const;
  void getValues(ColorChannel channel, int &u, int &v);

  inline int r() const { return m_channels[0]; }
  inline int g() const { return m_channels[1]; }
  inline int b() const { return m_channels[2]; }
  inline int a() const { return m_channels[3]; }
  inline int h() const { return m_channels[4]; }
  inline int s() const { return m_channels[5]; }
  inline int v() const { return m_channels[6]; }

  bool operator==(const ColorModel &cm) {
    int i;
    for (i = 0; i < 7; i++)
      if (m_channels[i] != cm.getValue(ColorChannel(i))) return false;
    return true;
  }
};

//=============================================

enum CurrentWheel { none, leftWheel, rightTriangle };

class DVAPI HexagonalColorWheel final : public GLWidgetForHighDpi {
  Q_OBJECT

  // backgoround color (R160, G160, B160)
  QColor m_bgColor;
  Q_PROPERTY(QColor BGColor READ getBGColor WRITE setBGColor)

  ColorModel m_color;
  QPointF m_wheelPosition;
  float m_triEdgeLen;
  float m_triHeight;
  QPointF m_wp[7], m_leftp[3];

  CurrentWheel m_currentWheel;

  // used for color calibration with 3DLUT
  QOpenGLFramebufferObject *m_fbo = NULL;
  LutCalibrator *m_lutCalibrator  = NULL;

  bool m_firstInitialized      = true;
  bool m_cuedCalibrationUpdate = false;

private:
  void drawCurrentColorMark();
  void clickLeftWheel(const QPoint &pos);
  void clickRightTriangle(const QPoint &pos);

public:
  HexagonalColorWheel(QWidget *parent);
  void setColor(const ColorModel &color) { m_color = color; };

  ~HexagonalColorWheel();

  void setBGColor(const QColor &color) { m_bgColor = color; }
  QColor getBGColor() const { return m_bgColor; }

  void updateColorCalibration();
  void cueCalibrationUpdate() { m_cuedCalibrationUpdate = true; }

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
  QSize SizeHint() const { return QSize(300, 200); };

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void showEvent(QShowEvent *) override;
signals:
  void colorChanged(const ColorModel &color, bool isDragging);

public slots:
  void onContextAboutToBeDestroyed();
};

//=============================================================================
/*! \brief The SquaredColorWheel is a squared color to change color.

                Inherits \b QWidget.

                This object show a square faded from one color channel to
   another color channel,
                the two channel represent x and y axis of square.
                It's possible to choose viewed shade using \b setChannel().
                Click in square change current SquaredColorWheel.
*/
class DVAPI SquaredColorWheel final : public QWidget {
  Q_OBJECT
  ColorChannel m_channel;
  ColorModel m_color;

public:
  SquaredColorWheel(QWidget *parent);

  /*! Doesn't call update(). */
  void setColor(const ColorModel &color);

protected:
  void paintEvent(QPaintEvent *event) override;

  void click(const QPoint &pos);
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
  /*! Connect channels to the two square axes:
                  \li eRed : connect x-axis to eGreen and y-axis to eBlue;
                  \li eGreen : connect x-axis to eRed and y-axis to eBlue;
                  \li eBlue : connect x-axis to eRed and y-axis to eGreen;
                  \li eHue : connect x-axis to eSaturation and y-axis to eValue;
                  \li eSaturation : connect x-axis to eHue and y-axis to eValue;
                  \li eValue : connect x-axis to eHue and y-axis to eSaturation;
     */
  void setChannel(int channel);

signals:
  void colorChanged(const ColorModel &color, bool isDragging);
};

//=============================================================================
/*! \brief The ColorSlider is used to set a color channel.

                Inherits \b QAbstractSlider.

                This object show a bar which colors differ from minimum to
   maximum channel color
                value.
*/
class DVAPI ColorSlider final : public QAbstractSlider {
  Q_OBJECT

public:
  ColorSlider(Qt::Orientation orientation, QWidget *parent = 0);

  /*! set channel and color. doesn't call update(). */
  void setChannel(ColorChannel channel);
  void setColor(const ColorModel &color);

  ColorChannel getChannel() const { return m_channel; }

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

  void chandleMouse(int x, int y);

  //	QIcon getFirstArrowIcon();
  //	QIcon getLastArrowIcon();
  //	QRect getFirstArrowRect();
  //	QRect getLastArrowRect();

private:
  ColorChannel m_channel;
  ColorModel m_color;
  static int s_chandle_size;
  static int s_chandle_tall;

public:
  static int s_slider_appearance;
};

//=============================================================================
// ArrowButton

class ArrowButton final : public QToolButton {
  Q_OBJECT

  Qt::Orientation m_orientaion;
  bool m_isFirstArrow;

  int m_timerId;
  int m_firstTimerId;

public:
  ArrowButton(QWidget *parent = 0, Qt::Orientation orientation = Qt::Horizontal,
              bool m_isFirstArrow = true);

protected:
  void stopTime(int timerId);
  void timerEvent(QTimerEvent *event) override;
  void notifyChanged();

protected slots:
  void onPressed();
  void onRelease();

signals:
  void add();
  void remove();
};

//=============================================================================
/*! \brief The ColorSliderBar is a colorSlider with  two arrow to add or remove
   one to current value.

                Inherits \b QToolBar.
*/
class DVAPI ColorSliderBar final : public QWidget {
  Q_OBJECT

  ColorSlider *m_colorSlider;

public:
  ColorSliderBar(QWidget *parent             = 0,
                 Qt::Orientation orientation = Qt::Vertical);

  void setValue(int value) { m_colorSlider->setValue(value); }
  void setRange(int minValue, int maxValue) {
    m_colorSlider->setRange(minValue, maxValue);
  }

  void setChannel(ColorChannel channel) {
    return m_colorSlider->setChannel(channel);
  }
  void setColor(const ColorModel &color) {
    return m_colorSlider->setColor(color);
  }

  ColorChannel getChannel() const { return m_colorSlider->getChannel(); }

protected slots:
  void onRemove();
  void onAdd();

signals:
  void valueChanged(int);
  void valueChanged();
};

//=============================================================================
/*! \brief The ChannelLineEdit is a cutomized version of IntLineEdit for channel
   value.
    It calls selectAll() at the moment of the first click.
*/
class ChannelLineEdit final : public DVGui::IntLineEdit {
  Q_OBJECT

  bool m_isEditing;

public:
  ChannelLineEdit(QWidget *parent, int value, int minValue, int maxValue)
      : IntLineEdit(parent, value, minValue, maxValue), m_isEditing(false) {}

protected:
  void mousePressEvent(QMouseEvent *) override;
  void focusOutEvent(QFocusEvent *) override;
  void paintEvent(QPaintEvent *) override;
};

//=============================================================================
/*! \brief ColorChannelControl is the widget used to show/edit a channel

                Inherits \b QWidget.

                The ColorChannelControl is composed of three object: a label \b
   QLabel
                to show the channel name, and an \b IntLineEdit and a
   ColorSlider to show/edit the
                channel value.
*/
class DVAPI ColorChannelControl final : public QWidget {
  Q_OBJECT
  QLabel *m_label;
  ChannelLineEdit *m_field;
  ColorSlider *m_slider;

  ColorChannel m_channel;
  ColorModel m_color;

  int m_value;
  bool m_signalEnabled;

public:
  ColorChannelControl(ColorChannel channel, QWidget *parent = 0);
  void setColor(const ColorModel &color);

protected slots:
  void onFieldChanged();
  void onSliderChanged(int value);
  void onSliderReleased();

  void onAddButtonClicked();
  void onSubButtonClicked();

signals:
  void colorChanged(const ColorModel &color, bool isDragging);
};

//=============================================================================
/*! \brief The StyleEditorPage is the base class of StyleEditor pages.

                Inherits \b QFrame.
                Inherited by \b PlainColorPage and \b StyleChooserPage.
*/
class StyleEditorPage : public QFrame {
public:
  StyleEditorPage(QWidget *parent);
};

//=============================================================================
/*! \brief The ColorParameterSelector is used for styles having more
    than one color parameter to select the current one.

                Inherits \b QWidget.
*/
class ColorParameterSelector final : public QWidget {
  Q_OBJECT

  std::vector<QColor> m_colors;
  int m_index;
  const QSize m_chipSize;
  const QPoint m_chipOrigin, m_chipDelta;

public:
  ColorParameterSelector(QWidget *parent);
  int getSelected() const { return m_index; }
  void setStyle(const TColorStyle &style);
  void clear();

signals:
  void colorParamChanged();

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *) override;

  QSize sizeHint() const override;
};

//=============================================================================
/*! \brief The PlainColorPage is used to control the color parameter.

                Inherits \b StyleEditorPage.

                The PlainColorPage is made of a \b SquaredColorWheel and a \b
   ColorSlider,
                a collection of \b ColorChannelControl, and a number of radio
   button (to control
                the ColorWheel behaviour).
*/
class PlainColorPage final : public StyleEditorPage {
  Q_OBJECT

  // ColorSliderBar *m_verticalSlider;
  // QRadioButton *m_modeButtons[7];
  ColorChannelControl *m_channelControls[7];
  // SquaredColorWheel *m_squaredColorWheel; //iwsw not used

  HexagonalColorWheel *m_hexagonalColorWheel;

  ColorModel m_color;
  bool m_signalEnabled;
  bool m_isVertical = true;
  int m_visibleParts;
  void updateControls();

  // QGridLayout *m_mainLayout;
  QFrame *m_slidersContainer;
  QSplitter *m_vSplitter;

public:
  PlainColorPage(QWidget *parent = 0);
  ~PlainColorPage() {}

  QFrame *m_wheelFrame;
  QFrame *m_hsvFrame;
  QFrame *m_alphaFrame;
  QFrame *m_rgbFrame;
  void setColor(const TColorStyle &style, int colorParameterIndex);

  void setIsVertical(bool isVertical);
  bool getIsVertical() { return m_isVertical; }
  QByteArray getSplitterState();
  void setSplitterState(QByteArray state);

  void updateColorCalibration();

protected:
  void resizeEvent(QResizeEvent *) override;

signals:
  void colorChanged(const ColorModel &, bool isDragging);

protected slots:
  void onWheelChanged(const ColorModel &color, bool isDragging);
  // void onWheelSliderChanged(int value);
  // void onWheelSliderReleased();

public slots:
  // void setWheelChannel(int channel);
  void onControlChanged(const ColorModel &color, bool isDragging);
  void toggleOrientation();
};

//=============================================================================
/*! \brief The StyleChooserPage is the base class of pages with texture,
    special style and custom style. It features a collection of selectable
   'chips'.

                Inherits \b StyleEditorPage.
*/
class StyleChooserPage : public StyleEditorPage {
  Q_OBJECT

protected:
  QPoint m_chipOrigin;
  QSize m_chipSize;
  int m_chipPerRow;
  static TFilePath m_rootPath;

  volatile bool m_pinsToTopDirty;
  BaseStyleManager *m_manager;
  StyleEditor *m_styleEditor;
  QAction *m_pinToTopAct;
  QAction *m_setPinsToTopAct;
  QAction *m_clrPinsToTopAct;

  enum ChipType {
    COMMONCHIP = 0,  // Common chip
    PINNEDCHIP = 1,  // Pin-to-top chip
    SOLIDCHIP  = 2   // Solid/Nobrush chip
  };

  QColor m_commonChipBoxColor;
  QColor m_pinnedChipBoxColor;
  QColor m_solidChipBoxColor;
  QColor m_selectedChipBoxColor;
  QColor m_selectedChipBox2Color;

  Q_PROPERTY(QColor CommonChipBoxColor READ getCommonChipBoxColor WRITE
                 setCommonChipBoxColor)
  Q_PROPERTY(QColor PinnedChipBoxColor READ getPinnedChipBoxColor WRITE
                 setPinnedChipBoxColor)
  Q_PROPERTY(QColor SolidChipBoxColor READ getSolidChipBoxColor WRITE
                 setSolidChipBoxColor)
  Q_PROPERTY(QColor SelectedChipBoxColor READ getSelectedChipBoxColor WRITE
                 setSelectedChipBoxColor)
  Q_PROPERTY(QColor SelectedChipBox2Color READ getSelectedChipBox2Color WRITE
                 setSelectedChipBox2Color)

  QColor getCommonChipBoxColor() const { return m_commonChipBoxColor; }
  QColor getPinnedChipBoxColor() const { return m_pinnedChipBoxColor; }
  QColor getSolidChipBoxColor() const { return m_solidChipBoxColor; }
  QColor getSelectedChipBoxColor() const { return m_selectedChipBoxColor; }
  QColor getSelectedChipBox2Color() const { return m_selectedChipBox2Color; }

  void setSolidChipBoxColor(const QColor &color) {
    m_solidChipBoxColor = color;
  }
  void setPinnedChipBoxColor(const QColor &color) {
    m_pinnedChipBoxColor = color;
  }
  void setCommonChipBoxColor(const QColor &color) {
    m_commonChipBoxColor = color;
  }
  void setSelectedChipBoxColor(const QColor &color) {
    m_selectedChipBoxColor = color;
  }
  void setSelectedChipBox2Color(const QColor &color) {
    m_selectedChipBox2Color = color;
  }

public:
  StyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0);

  void setChipSize(QSize chipSize);
  QSize getChipSize() const { return m_chipSize; }

  void applyFilter();
  void applyFilter(const QString text);

  virtual bool loadIfNeeded()      = 0;
  virtual int getChipCount() const = 0;

  virtual int drawChip(QPainter &p, QRect rect, int index) = 0;
  virtual void onSelect(int index) {}

  virtual bool isSameStyle(const TColorStyleP style, int index) = 0;

  virtual QString getChipDescription(int index) = 0;

  //! \see StyleEditor::setRootPath()
  // TOGLIERE
  static void setRootPath(const TFilePath &rootPath);
  static TFilePath getRootPath() { return m_rootPath; }

protected:
  // int m_currentIndex;

  int posToIndex(const QPoint &pos) const;

  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override { computeSize(); }

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;

  bool event(QEvent *e) override;

public slots:
  void patternAdded();
  void computeSize();
  void togglePinToTop();
  void doSetPinsToTop();
  void doClrPinsToTop();
  void doPinsToTopChange();

signals:
  void styleSelected(const TColorStyle &style);
};

//*****************************************************************************
//    CustomStyleChooser  definition
//*****************************************************************************

class CustomStyleChooserPage final : public StyleChooserPage {
public:
  CustomStyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0)
      : StyleChooserPage(styleEditor, parent) {
    static const QString filters(
        "*.pli *.tif *.png *.tga *.tiff *.sgi *.rgb *.pct *.pic *.exr");
    static CustomStyleManager theManager(
        "RasterImagePatternStrokeStyle:", "VectorImagePatternStrokeStyle:",
        TFilePath("custom styles"), filters, m_chipSize);
    m_manager = &theManager;
  }

  void showEvent(QShowEvent *) override {
    connect(m_manager, SIGNAL(patternAdded()), this, SLOT(patternAdded()));
    m_manager->loadItems();
  }
  void hideEvent(QHideEvent *) override {
    disconnect(m_manager, SIGNAL(patternAdded()), this, SLOT(patternAdded()));
  }
  bool loadIfNeeded() override { return false; }  // serve?
  /*
if(!m_loaded) {loadItems(); m_loaded=true;return true;}
else return false;
}
  */

  int getChipCount() const override { return m_manager->countData(); }

  int drawChip(QPainter &p, QRect rect, int index) override;
  void onSelect(int index) override;
  bool isSameStyle(const TColorStyleP style, int index) override;

  QString getChipDescription(int index) override;
};

//*****************************************************************************
//    VectorBrushStyleChooser  definition
//*****************************************************************************

class VectorBrushStyleChooserPage final : public StyleChooserPage {
public:
  VectorBrushStyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0)
      : StyleChooserPage(styleEditor, parent) {
    m_chipSize = QSize(60, 25);
    static CustomStyleManager theManager(
        "InvalidStyle", "VectorBrushStyle:", TFilePath("vector brushes"),
        "*.pli", m_chipSize);
    m_manager = &theManager;
  }

  void showEvent(QShowEvent *) override {
    bool ret =
        connect(m_manager, SIGNAL(patternAdded()), this, SLOT(patternAdded()));
    if (!ret) throw "!";
    m_manager->loadItems();
  }
  void hideEvent(QHideEvent *) override {
    disconnect(m_manager, SIGNAL(patternAdded()), this, SLOT(patternAdded()));
  }
  bool loadIfNeeded() override { return false; }

  int getChipCount() const override { return m_manager->countData() + 1; }

  int drawChip(QPainter &p, QRect rect, int index) override;
  void onSelect(int index) override;
  bool isSameStyle(const TColorStyleP style, int index) override;

  QString getChipDescription(int index) override;
};

//*****************************************************************************
//    TextureStyleChooser  definition
//*****************************************************************************

class TextureStyleChooserPage final : public StyleChooserPage {
public:
  TextureStyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0)
      : StyleChooserPage(styleEditor, parent) {
    m_chipSize = QSize(25, 25);
    static TextureStyleManager theManager(TFilePath("textures"), m_chipSize);
    m_manager = &theManager;
  }

  bool loadIfNeeded() override {
    if (!m_manager->isLoaded()) {
      m_manager->loadItems();
      return true;
    } else
      return false;
  }

  int getChipCount() const override { return m_manager->countData() + 1; }

  int drawChip(QPainter &p, QRect rect, int index) override;
  void onSelect(int index) override;
  bool isSameStyle(const TColorStyleP style, int index) override;

  QString getChipDescription(int index) override;
};

//*****************************************************************************
//    MyPaintBrushStyleChooserPage  definition
//*****************************************************************************

class MyPaintBrushStyleChooserPage final : public StyleChooserPage {
  MyPaintBrushStyleManager *m_mypManager;

public:
  MyPaintBrushStyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0)
      : StyleChooserPage(styleEditor, parent) {
    m_chipSize = QSize(64, 64);
    static MyPaintBrushStyleManager theManager(m_chipSize);
    m_manager    = &theManager;
    m_mypManager = &theManager;
  }

  bool loadIfNeeded() override {
    if (!m_manager->isLoaded()) {
      m_manager->loadItems();
      return true;
    } else
      return false;
  }

  TMyPaintBrushStyle &getBrush(int index) {
    return m_mypManager->getBrush(index);
  }
  int getChipCount() const override { return m_manager->countData() + 1; }

  int drawChip(QPainter &p, QRect rect, int index) override;
  void onSelect(int index) override;
  bool isSameStyle(const TColorStyleP style, int index) override;

  QString getChipDescription(int index) override;
};

//*****************************************************************************
//    SpecialStyleChooser  definition
//*****************************************************************************

class SpecialStyleChooserPage final : public StyleChooserPage {
public:
  SpecialStyleChooserPage(StyleEditor *styleEditor, QWidget *parent = 0,
                          const TFilePath &rootDir = TFilePath())
      : StyleChooserPage(styleEditor, parent) {
    static SpecialStyleManager theManager(m_chipSize);
    m_manager = &theManager;
  }

  bool loadIfNeeded() override {
    if (!m_manager->isLoaded()) {
      m_manager->loadItems();
      return true;
    } else
      return false;
  }
  int getChipCount() const override { return m_manager->countData() + 1; }

  int drawChip(QPainter &p, QRect rect, int index) override;
  void onSelect(int index) override;
  bool isSameStyle(const TColorStyleP style, int index) override;

  QString getChipDescription(int index) override;
};

//=============================================================================

/*!
  \brief    The SettingsPage is used to show/edit style parameters.

  \details  This class stores the GUI for editing a \a copy of the
            current color style. Updates of the actual current color
            style are \a not performed directly by this class.
*/

class SettingsPage final : public QScrollArea {
  Q_OBJECT

  QGridLayout *m_paramsLayout;

  QCheckBox *m_autoFillCheckBox;

  TColorStyleP m_editedStyle;  //!< A copy of the current style being edited by
                               //! the Style Editor.

  bool
      m_updating;  //!< Whether the page is copying style content to its widget,
                   //!  to be displayed.
private:
  int getParamIndex(const QWidget *widget);

public:
  SettingsPage(QWidget *parent);

  void setStyle(const TColorStyleP &editedStyle);
  void updateValues();

  void enableAutopaintToggle(bool enabled);

signals:

  void paramStyleChanged(
      bool isDragging);  //!< Signals that the edited style has changed.

private slots:

  void onAutofillChanged();
  void onValueChanged(bool isDragging = false);
  void onValueReset();
};

//=============================================================================
}  // namespace StyleEditorGUI
//=============================================================================

using namespace StyleEditorGUI;

//=============================================================================
// StyleEditor
//-----------------------------------------------------------------------------

class DVAPI StyleEditor final : public QWidget, public SaveLoadQSettings {
  Q_OBJECT

  PaletteController *m_paletteController;
  TPaletteHandle *m_paletteHandle;
  TPaletteHandle *m_cleanupPaletteHandle;
  DVGui::HexLineEdit *m_hexLineEdit;
  DVGui::HexColorNamesEditor *m_hexColorNamesEditor;
  QWidget *m_parent;
  TXshLevelHandle
      *m_levelHandle;  //!< for clearing the level cache when the color changed

  DVGui::TabBar *m_styleBar;
  QStackedWidget *m_styleChooser;

  DVGui::StyleSample
      *m_newColor;  //!< New style viewer (lower-right panel side).
  DVGui::StyleSample
      *m_oldColor;  //!< Old style viewer (lower-right panel side).
  QAction *m_toggleOrientationAction;
  QPushButton
      *m_autoButton;  //!< "Auto Apply" checkbox on the right panel side.
  QPushButton *m_applyButton;  //!< "Apply" button on the right panel side.

  QToolBar *m_toolBar;                               //!< Lower toolbar.
  ColorParameterSelector *m_colorParameterSelector;  //!< Secondary color
                                                     //! parameter selector in
  //! the lower toolbar.

  TabBarContainter *m_tabBarContainer;  //!< Tabs container for style types.

  // QLabel *m_statusLabel;  //!< showing the information of the current palette
  //! and style.

  PlainColorPage *m_plainColorPage;
  StyleChooserPage *m_textureStylePage;
  StyleChooserPage *m_specialStylePage;
  StyleChooserPage *m_customStylePage;
  StyleChooserPage *m_vectorBrushesStylePage;
  StyleChooserPage *m_mypaintBrushesStylePage;
  SettingsPage *m_settingsPage;
  QScrollArea *m_textureArea;
  QScrollArea *m_vectorsArea;
  QScrollArea *m_mypaintArea;
  QAction *m_wheelAction;
  QAction *m_hsvAction;
  QAction *m_alphaAction;
  QAction *m_rgbAction;
  QAction *m_hexAction;
  QAction *m_searchAction;
  QActionGroup *m_sliderAppearanceAG;
  QAction *m_hexEditorAction;

  QFrame *m_textureSearchFrame;
  QFrame *m_vectorsSearchFrame;
  QFrame *m_mypaintSearchFrame;
  QLineEdit *m_textureSearchText;
  QLineEdit *m_vectorsSearchText;
  QLineEdit *m_mypaintSearchText;
  QPushButton *m_textureSearchClear;
  QPushButton *m_vectorsSearchClear;
  QPushButton *m_mypaintSearchClear;

  TColorStyleP
      m_oldStyle;  //!< A copy of current style \a before the last change.
  TColorStyleP m_editedStyle;  //!< The currently edited style. Please observe
                               //! that this is
  //!  a \b copy of currently selected style, since style edits
  //!  may be not <I>automatically applied</I>.
  bool m_enabled;
  bool m_enabledOnlyFirstTab;
  bool m_enabledFirstAndLastTab;
  bool m_colorPageIsVertical = true;

public:
  StyleEditor(PaletteController *, QWidget *parent = 0);
  ~StyleEditor();

  void setPaletteHandle(TPaletteHandle *paletteHandle);
  TPaletteHandle *getPaletteHandle() const { return m_paletteHandle; }

  void setLevelHandle(TXshLevelHandle *levelHandle) {
    m_levelHandle = levelHandle;
  }

  TPalette *getPalette() { return m_paletteHandle->getPalette(); }
  int getStyleIndex() { return m_paletteHandle->getStyleIndex(); }
  const TColorStyleP getEditedStyle() const { return m_editedStyle; }

  /*! rootPath generally is STUFFDIR/Library. Contains directories 'textures'
     and
                  'custom styles' */
  // TOGLIERE
  void setRootPath(const TFilePath &rootPath);

  void enableAutopaintToggle(bool enabled) {
    m_settingsPage->enableAutopaintToggle(enabled);
  }

  // SaveLoadQSettings
  virtual void save(QSettings &settings) const override;
  virtual void load(QSettings &settings) override;

  void updateColorCalibration();

protected:
  /*! Return false if style is linked and style must be set to null.*/
  bool setStyle(TColorStyle *currentStyle);

  void setEditedStyleToStyle(const TColorStyle *style);  //!< Clones the
                                                         //! supplied style and
  //! considers that as
  //! the edited one.
  void setOldStyleToStyle(const TColorStyle *style);  //!< Clones the supplied
                                                      //! style and considers
  //! that as the previously
  //! current one.
  //!  \todo  Why is this not assimilated to setCurrentStyleToStyle()?

  /*! Return style parameter index selected in \b ColorParameterSelector. */
  int getColorParam() const { return m_colorParameterSelector->getSelected(); }

  /*! Set StyleEditor view to \b enabled. If \b enabledOnlyFirstTab or if \b
     enabledFirstAndLastTab
                  is true hide other tab, pay attention \b enabled must be true
     or StyleEditor is disabled. */
  void enable(bool enabled, bool enabledOnlyFirstTab = false,
              bool enabledFirstAndLastTab = false);

  void updateStylePages();

protected:
  void showEvent(QShowEvent *) override;
  void hideEvent(QHideEvent *) override;

protected slots:

  void onStyleSwitched();
  void onStyleChanged(bool isDragging);
  void onCleanupStyleChanged(bool isDragging);
  void onOldStyleClicked();
  void onNewStyleClicked();
  void updateOrientationButton();
  void checkPaletteLock();
  // called (e.g.) by PaletteController when an other StyleEditor change the
  // toggle
  void enableColorAutoApply(bool enabled);

  // when colorAutoApply==false this slot is called when the current color
  // changes
  void setColorSample(const TPixel32 &color);

  // chiamato quando viene modificato uno slider o la color wheel
  void onColorChanged(const ColorModel &, bool isDragging);

  void selectStyle(const TColorStyle &style);

  void applyButtonClicked();
  void autoCheckChanged(bool value);

  void setPage(int index);

  void onColorParamChanged();

  void onParamStyleChanged(bool isDragging);

  void onHexChanged();
  void onHexEditor();

  void onSearchVisible(bool on);

  void onSpecialButtonToggled(bool on);
  void onCustomButtonToggled(bool on);
  void onVectorBrushButtonToggled(bool on);

  void onSliderAppearanceSelected(QAction *);
  void onPopupMenuAboutToShow();

  void onTextureSearch(const QString &);
  void onTextureClearSearch();

  void onVectorsSearch(const QString &);
  void onVectorsClearSearch();

  void onMyPaintSearch(const QString &);
  void onMyPaintClearSearch();

private:
  QFrame *createBottomWidget();
  QFrame *createTexturePage();
  QFrame *createVectorPage();
  QFrame *createMyPaintPage();
  void updateTabBar();

  void copyEditedStyleToPalette(bool isDragging);
};

#endif  // STYLEEDITOR_H
