/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/
#include "editor.h"
#include <QStyleFactory>
#include <QApplication>
#include <QVBoxLayout>
#include <QDir>
#include <QToolBox>

#include "colorrgb.h"
#include "colorcmyk.h"
#include "colorhsv.h"
#include "cyan_layerwidget.h"

void Editor::setupStyle()
{
    // style app
    qApp->setStyle(QStyleFactory::create("fusion"));
    if (QIcon::themeName().isEmpty()) {
        // we include fallback hicolor icons
        QIcon::setThemeName("hicolor");
    }

    // set colors
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(196,110,33));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    // stylesheet (override various stuff in Qt)
    setStyleSheet(QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                          "QToolBar { border-color: none; }"
                          "QToolButton::menu-indicator { image: none; }"));
}

void Editor::setupUI()
{
    setupStyle();
    setupWidgets();
    setupMenus();
    setupToolbars();
    setupActions();
    setupButtons();
    //setupColorManagement();
    setupConnections();
    setupIcons();
    setupShortcuts();
    setupOptions();

    //setCentralWidget(mdi);
    setStatusBar(mainStatusBar);
    setMenuBar(mainMenu);

    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(colorMenu);
    //mainMenu->addMenu(layerMenu);
    mainMenu->addMenu(optMenu);
    windowsMenu->attachToMdiArea(mdi);
    mainMenu->addMenu(windowsMenu);
    mainMenu->addMenu(helpMenu);

    mainToolBar->addWidget(newButton);
    mainToolBar->addWidget(openButton);
    mainToolBar->addWidget(saveButton);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(moveButton);
    mainToolBar->addWidget(zoomButton);

    moveButton->addAction(viewMoveAct);
    moveButton->addAction(viewDrawAct);
    //mainToolBar->addAction(viewMoveAct);
    //mainToolBar->addAction(viewDrawAct);
    //mainToolBar->addWidget(textButton);
    //mainToolBar->addWidget(layerButton);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(convertButton);
    mainToolBar->addSeparator();
    //mainToolBar->addWidget(colorPicker);

    colorPicker->hide();


    newButton->addAction(newImageAct);
    newButton->addAction(newLayerAct);

    openButton->addAction(openImageAct);
    openButton->addAction(openLayerAct);

    zoomButton->addAction(viewZoom100Act);
    zoomButton->addAction(viewZoomFitAct);

    fileMenu->addAction(newImageAct);
    fileMenu->addAction(newLayerAct);
    fileMenu->addAction(newTextLayerAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAct);
    fileMenu->addAction(saveProjectAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openLayerAct);
    fileMenu->addAction(saveImageAct);
    fileMenu->addAction(saveLayerAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    helpMenu->addAction(aboutCyanAct);
    helpMenu->addAction(aboutImageMagickAct);
    helpMenu->addAction(aboutLcmsAct);
    helpMenu->addAction(aboutQtAct);

    //newMenu->addAction(newImageAct);
    //newMenu->addAction(newLayerAct);

    saveMenu->addAction(saveProjectAct);
    saveMenu->addAction(saveProjectAsAct);
    saveMenu->addAction(saveImageAct);
    saveMenu->addAction(saveLayerAct);

    colorMenu->addAction(convertRGBAct);
    colorMenu->addAction(convertCMYKAct);
    colorMenu->addAction(convertGRAYAct);
    colorMenu->addSeparator();
    colorMenu->addAction(convertAssignAct);
    colorMenu->addAction(convertExtractAct);
    colorMenu->addSeparator();
    colorMenu->addMenu(colorProfileRGBMenu);
    colorMenu->addMenu(colorProfileCMYKMenu);
    colorMenu->addMenu(colorProfileGRAYMenu);
    colorMenu->addSeparator();
    colorMenu->addMenu(colorIntentMenu);
    colorMenu->addAction(blackPointAct);

    viewMoveAct->setChecked(true);
    moveButton->setDefaultAction(viewMoveAct);

    populateColorProfileMenu(colorProfileRGBMenu,
                             Magick::sRGBColorspace);
    populateColorProfileMenu(colorProfileCMYKMenu,
                             Magick::CMYKColorspace);
    populateColorProfileMenu(colorProfileGRAYMenu,
                             Magick::GRAYColorspace);
    populateColorIntentMenu();

    QLabel *brushSizeLabel = new QLabel(this);
    brushSizeLabel->setPixmap(QIcon(":/icons/brushsize.png")
                              .pixmap(24, 24));


    QWidget *brushWidget = new QWidget(this);
    QVBoxLayout *brushLayout = new QVBoxLayout(brushWidget);

    QWidget *brushSizeWidget = new QWidget(this);
    QHBoxLayout *brushSizeLayout = new QHBoxLayout(brushSizeWidget);

    brushSizeLayout->addWidget(brushSizeLabel);
    brushSizeLayout->addWidget(brushSize);
    brushLayout->addWidget(brushSizeWidget);
    brushLayout->addStretch();

    brushDock = new QDockWidget(this);
    brushDock->setWindowTitle(tr("Brush"));
    brushDock->setObjectName(QString("brushDock"));
    brushDock->setWidget(brushWidget);
    addDockWidget(Qt::RightDockWidgetArea,
                  brushDock);

    //textWidget->setHidden(true);

    optMenu->setDisabled(true);

    mainSplitter->setOrientation(Qt::Horizontal);
    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(mdi);
    mainSplitter->addWidget(rightSplitter);

    setCentralWidget(mainSplitter);

    rightSplitter->addWidget(layersWidget);
}

void Editor::setupMenus()
{
    mainMenu = new QMenuBar(this);
    mainMenu->setObjectName(QString("mainMenu"));

    fileMenu = new QMenu(this);
    fileMenu->setTitle(tr("File"));

    optMenu = new QMenu(this);
    optMenu->setTitle(tr("Options"));

    helpMenu = new QMenu(this);
    helpMenu->setTitle(tr("Help"));

    saveMenu = new QMenu(this);
    saveMenu->setTitle(tr("Save"));

    colorMenu = new QMenu(this);
    colorMenu->setTitle(tr("Colors"));

    windowsMenu = new QtWindowListMenu(this);

    colorProfileRGBMenu = new QMenu(this);
    colorProfileRGBMenu->setTitle(tr("Default RGB profile"));
    colorProfileRGBMenu->setObjectName(QString("colorProfileRGBMenu"));

    colorProfileCMYKMenu = new QMenu(this);
    colorProfileCMYKMenu->setTitle(tr("Default CMYK profile"));
    colorProfileCMYKMenu->setObjectName(QString("colorProfileCMYKMenu"));

    colorProfileGRAYMenu = new QMenu(this);
    colorProfileGRAYMenu->setTitle(tr("Default GRAY profile"));
    colorProfileGRAYMenu->setObjectName(QString("colorProfileGRAYMenu"));

    colorIntentMenu = new QMenu(this);
    colorIntentMenu->setTitle(tr("Rendering Intent"));
}

void Editor::setupToolbars()
{
    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QString("mainToolBar"));
    mainToolBar->setWindowTitle(tr("Main"));
    mainToolBar->setMovable(false);

    addToolBar(Qt::LeftToolBarArea,
               mainToolBar);
}

void Editor::setupWidgets()
{

    // the main splitters
    // TODO! add top and bottom
    mainSplitter = new QSplitter(this);
    mainSplitter->setObjectName(QString("mainSplitter"));
    leftSplitter = new QSplitter(this);
    leftSplitter->setObjectName(QString("leftSplitter"));
    rightSplitter = new QSplitter(this);
    rightSplitter->setObjectName(QString("rightSplitter"));

    mdi = new Mdi(this);
    mdi->setBackground(QBrush(QColor(20, 20, 20)));

    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));

    // remove this!
    brushSize = new QSlider(this);
    brushSize->setRange(1,256);
    brushSize->setValue(20);
    brushSize->setOrientation(Qt::Vertical);

    // layer
    layersWidget = new CyanLayerWidget(this);

    // text (disabled for now)
    //textWidget = new CyanTextWidget(this);
    //textWidget->setDisabled(true);

    // add zoom % in status bar
    currentZoomStatusLabel = new QLabel(this);
    currentZoomStatusLabel->setText(QString("100%"));
    currentZoomStatusIcon = new QLabel(this);
    currentZoomStatusIcon->setPixmap(QIcon::fromTheme("zoom").pixmap(QSize(16, 16)));
    mainStatusBar->addPermanentWidget(currentZoomStatusLabel);
    mainStatusBar->addPermanentWidget(currentZoomStatusIcon);

    colorPicker = new QtColorPicker(this, -1, true, false);
    colorPicker->setIconSize(QSize(24,24));
    colorPicker->setStandardColors();
    connect(colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(handleColorChanged(QColor)));
}


void Editor::setupActions()
{

    newImageAct = new QAction(this);
    newImageAct->setText(tr("New image"));

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open"));

    saveImageAct = new QAction(this);
    saveImageAct->setText(tr("Export image"));

    saveProjectAct = new QAction(this);
    saveProjectAct->setText(tr("Save"));
    saveProjectAct->setDisabled(true);

    saveProjectAsAct = new QAction(this);
    saveProjectAsAct->setText(tr("Save as ..."));
    saveProjectAsAct->setDisabled(true);

    newLayerAct = new QAction(this);
    newLayerAct->setText(tr("New layer"));

    newTextLayerAct = new QAction(this);
    newTextLayerAct->setText(tr("New text layer"));
    newTextLayerAct->setDisabled(true);

    openLayerAct = new QAction(this);
    openLayerAct->setText(tr("Import image"));

    saveLayerAct = new QAction(this);
    saveLayerAct->setText(tr("Export layer"));

    quitAct = new QAction(this);
    quitAct->setText(tr("Quit"));

    viewMoveAct = new QAction(this);
    viewMoveAct->setText(tr("Move"));
    viewMoveAct->setCheckable(true);
    viewMoveAct->setChecked(false);

    viewDrawAct = new QAction(this);
    viewDrawAct->setText("Draw");
    viewDrawAct->setCheckable(true);
    viewDrawAct->setChecked(false);

    aboutCyanAct = new QAction(this);
    aboutCyanAct->setText(QString("%1 %2")
                          .arg(tr("About"))
                          .arg(qApp->applicationName()));

    aboutImageMagickAct = new QAction(this);
    aboutImageMagickAct->setText(QString("%1 ImageMagick").arg(tr("About")));

    aboutLcmsAct = new QAction(this);
    aboutLcmsAct->setText(QString("%1 Little CMS").arg(tr("About")));

    aboutQtAct = new QAction(this);
    aboutQtAct->setText(QString("%1 Qt").arg(tr("About")));

    convertRGBAct = new QAction(this);
    convertRGBAct->setText(tr("Convert to RGB"));

    convertCMYKAct = new QAction(this);
    convertCMYKAct->setText(tr("Convert to CMYK"));

    convertGRAYAct = new QAction(this);
    convertGRAYAct->setText(tr("Convert to GRAY"));

    convertAssignAct = new QAction(this);
    convertAssignAct->setText(tr("Assign color profile"));

    convertExtractAct = new QAction(this);
    convertExtractAct->setText(tr("Extract color profile"));

    blackPointAct = new QAction(this);
    blackPointAct->setText(tr("Black point compensation"));
    blackPointAct->setCheckable(true);

    viewZoom100Act = new QAction(this);
    viewZoom100Act->setText(tr("Zoom to actual size"));

    viewZoomFitAct = new QAction(this);
    viewZoomFitAct->setText(tr("Zoom to fit"));
    viewZoomFitAct->setCheckable(true);
}

void Editor::setupButtons()
{
    newButton = new QToolButton(this);
    newButton->setPopupMode(QToolButton::InstantPopup);
    newButton->setText(tr("New"));
    newButton->setToolTip(tr("New"));
    newButton->setIcon(QIcon::fromTheme("document-new"));
    newButton->setArrowType(Qt::NoArrow);

    openButton = new QToolButton(this);
    openButton->setPopupMode(QToolButton::InstantPopup);
    openButton->setText(tr("Open"));
    openButton->setToolTip(tr("Open"));
    openButton->setIcon(QIcon::fromTheme("document-open"));

    saveButton = new QToolButton(this);
    saveButton->setMenu(saveMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setText(tr("Save"));
    saveButton->setToolTip(tr("Save"));

    moveButton = new QToolButton(this);
    moveButton->setCheckable(false);
    moveButton->setPopupMode(QToolButton::InstantPopup);
    moveButton->setText(tr("View"));
    moveButton->setToolTip(tr("View"));
    moveButton->setIcon(QIcon::fromTheme("transform_move"));

    zoomButton = new QToolButton(this);
    zoomButton->setPopupMode(QToolButton::InstantPopup);
    zoomButton->setText(tr("Zoom"));
    zoomButton->setToolTip(tr("Zoom"));
    zoomButton->setIcon(QIcon::fromTheme("zoom"));

    convertButton = new QToolButton(this);
    convertButton->setMenu(colorMenu);
    convertButton->setPopupMode(QToolButton::InstantPopup);
    convertButton->setText(tr("Colors"));
    convertButton->setToolTip(tr("Color convert"));
}



void Editor::setupConnections()
{



    connect(viewZoom100Act, SIGNAL(triggered(bool)), this, SLOT(handleZoom100ActionTriggered()));
    connect(viewZoomFitAct, SIGNAL(triggered(bool)), this, SLOT(handleZoomFitActionTriggered(bool)));

    //connect(&common, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    //connect(&common, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));

    connect(newImageAct, SIGNAL(triggered(bool)), this, SLOT(newImageDialog()));


    connect(newLayerAct, SIGNAL(triggered(bool)), this, SLOT(newLayerDialog()));
    connect(newTextLayerAct, SIGNAL(triggered(bool)), this, SLOT(newTextLayerDialog()));



    connect(openImageAct, SIGNAL(triggered(bool)), this, SLOT(loadImageDialog()));
    connect(saveProjectAct, SIGNAL(triggered(bool)), this, SLOT(saveProjectDialog()));
    connect(saveImageAct, SIGNAL(triggered(bool)), this, SLOT(saveImageDialog()));
    connect(saveLayerAct, SIGNAL(triggered(bool)), this, SLOT(saveLayerDialog()));

    connect(quitAct, SIGNAL(triggered(bool)), this, SLOT(checkTabsOnClose()));

    connect(viewMoveAct, SIGNAL(triggered(bool)), this, SLOT(handleSetMoveMode(bool)));
    connect(viewDrawAct, SIGNAL(triggered(bool)), this, SLOT(handleSetDrawMode(bool)));

    connect(aboutCyanAct, SIGNAL(triggered()), this, SLOT(aboutCyan()));
    connect(aboutImageMagickAct, SIGNAL(triggered()), this, SLOT(aboutImageMagick()));
    connect(aboutLcmsAct, SIGNAL(triggered()), this, SLOT(aboutLcms()));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(convertRGBAct, SIGNAL(triggered()), this, SLOT(handleColorConvertRGB()));
    connect(convertCMYKAct, SIGNAL(triggered()), this, SLOT(handleColorConvertCMYK()));
    connect(convertGRAYAct, SIGNAL(triggered()), this, SLOT(handleColorConvertGRAY()));
    connect(convertAssignAct, SIGNAL(triggered()), this, SLOT(handleColorProfileAssign()));

    connect(this, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(this, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));
    connect(this, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(mdi, SIGNAL(openImages(QList<QUrl>)), this, SLOT(handleOpenImages(QList<QUrl>)));
    connect(brushSize, SIGNAL(valueChanged(int)), this, SLOT(handleBrushSize()));

    connect(mdi,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,
            SLOT(handleTabActivated(QMdiSubWindow*)));
    connect(mdi,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            layersWidget,
            SLOT(handleTabActivated(QMdiSubWindow*)));
    connect(layersWidget,
            SIGNAL(selectedLayer(int)),
            this,
            SLOT(handleLayerTreeSelectedLayer(int)));
    connect(layersWidget,
            SIGNAL(layerVisibilityChanged(int,bool)),
            this,
            SLOT(handleLayerVisibility(int,bool)));
    connect(layersWidget,
            SIGNAL(layerLabelChanged(int,QString)),
            this,
            SLOT(handleLayerLabel(int,QString)));
    connect(layersWidget,
            SIGNAL(layerLockChanged(int,bool)),
            this,
            SLOT(handleLayerLock(int,bool)));
    connect(layersWidget,
            SIGNAL(newLayer()),
            this,
            SLOT(newLayerDialog()));
    connect(layersWidget,
            SIGNAL(removeLayer(int)),
            this,
            SLOT(handleRemoveLayer(int)));
    connect(layersWidget,
            SIGNAL(moveLayerUp(int)),
            this,
            SLOT(handleMoveLayerUp(int)));
    connect(layersWidget,
            SIGNAL(moveLayerDown(int)),
            this,
            SLOT(handleMoveLayerDown(int)));
    connect(layersWidget,
            SIGNAL(duplicateLayer(int)),
            this,
            SLOT(handleDuplicateLayer(int)));
    connect(layersWidget,
            SIGNAL(layerOpacityChanged(double,int)),
            this,
            SLOT(handleLayersOpacity(double,int)));
    connect(layersWidget,
            SIGNAL(layerCompositeChanged(Magick::CompositeOperator,int)),
            this,
            SLOT(handleLayerCompChanged(Magick::CompositeOperator,int)));

    /*connect(textWidget,
            SIGNAL(textChanged()),
            this,
            SLOT(handleCurrentLayerTextChanged()));*/

    //connect(layerButton, SIGNAL(toggled(bool)), layerPopup, SLOT(pinPopup(bool)));
    //connect(textButton, SIGNAL(toggled(bool)), textPopup, SLOT(pinPopup(bool)));

}

void Editor::setupIcons()
{
    setWindowIcon(QIcon::fromTheme(qApp->applicationName()));


    viewZoom100Act->setIcon(QIcon::fromTheme("zoom-original"));
    viewZoomFitAct->setIcon(QIcon::fromTheme("zoom-fit-best"));

    newImageAct->setIcon(QIcon::fromTheme("document-new"));
    newLayerAct->setIcon(QIcon::fromTheme("layer",
                                          QIcon::fromTheme("document-new")));
    newTextLayerAct->setIcon(QIcon::fromTheme("font",
                                              QIcon::fromTheme("document-new")));

    openImageAct->setIcon(QIcon::fromTheme("document-open"));
    saveButton->setIcon(QIcon::fromTheme("document-save"));
    saveImageAct->setIcon(QIcon::fromTheme("document-new"));
    saveProjectAct->setIcon(QIcon::fromTheme("document-save"));
    saveLayerAct->setIcon(QIcon::fromTheme("layer"));
    saveProjectAsAct->setIcon(QIcon::fromTheme("document-save-as"));
    quitAct->setIcon(QIcon::fromTheme("application-exit"));

    viewMoveAct->setIcon(QIcon::fromTheme("transform_move"));
    viewDrawAct->setIcon(QIcon::fromTheme("paintbrush"));

    QIcon colorsIcon = QIcon::fromTheme("colors");
    QIcon colorWheelIcon = QIcon::fromTheme("color-wheel");

    convertRGBAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertCMYKAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertGRAYAct->setIcon(QIcon::fromTheme("convert_color_to_gray"));
    convertAssignAct->setIcon(colorsIcon);
    convertExtractAct->setIcon(colorsIcon);
    colorProfileRGBMenu->setIcon(colorWheelIcon);
    colorProfileCMYKMenu->setIcon(colorWheelIcon);
    colorProfileGRAYMenu->setIcon(colorWheelIcon);
    colorIntentMenu->setIcon(QIcon::fromTheme("video-display"));
    blackPointAct->setIcon(colorWheelIcon);

    aboutQtAct->setIcon(QIcon::fromTheme("help-about"));
    aboutLcmsAct->setIcon(QIcon::fromTheme("help-about"));
    aboutImageMagickAct->setIcon(QIcon::fromTheme("help-about"));
    aboutCyanAct->setIcon(QIcon::fromTheme("help-about"));

    convertButton->setIcon(colorsIcon);
}

void Editor::setupShortcuts()
{
    newImageAct->setShortcut(QKeySequence(tr("Ctrl+N")));
    newLayerAct->setShortcut(QKeySequence(tr("Ctrl+L")));
    newTextLayerAct->setShortcut(QKeySequence(tr("Ctrl+Shift+L")));
    openImageAct->setShortcut(QKeySequence(tr("Ctrl+O")));
    quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));
}

void Editor::setupOptions()
{

}

