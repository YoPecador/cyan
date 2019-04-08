/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

#include "editor.h"
#include <QStyleFactory>
#include <QApplication>
#include <QVBoxLayout>
#include <QDir>

#include "colorrgb.h"
#include "colorcmyk.h"
#include "colorhsv.h"
#include "cyan_layerwidget.h"

void Editor::setupStyle()
{
    // style app
    qApp->setStyle(QStyleFactory::create("fusion"));

    // set icon theme path(s)
    QStringList iconsPath = QIcon::themeSearchPaths();
    QString iconsHomeLocal = QString("%1/.local/share/icons").arg(QDir::homePath());
    QString iconsHome = QString("%1/.icons").arg(QDir::homePath());
    QString iconsDeploy = QString("%1/etc/icons").arg(qApp->applicationDirPath());

    if (QFile::exists(iconsHomeLocal) &&
        !iconsPath.contains(iconsHomeLocal)) { iconsPath.prepend(iconsHomeLocal); }
    if (QFile::exists(iconsHome) &&
        !iconsPath.contains(iconsHome)) { iconsPath.prepend(iconsHome); }
    if (QFile::exists(iconsDeploy)) { iconsPath.prepend(iconsDeploy); }

    iconsPath << QString("%1/../share/icons").arg(qApp->applicationDirPath());
    iconsPath << QString("%1/icons").arg(qApp->applicationDirPath());
    QIcon::setThemeSearchPaths(iconsPath);
    qDebug() << "using icon theme search path" << QIcon::themeSearchPaths();

    // set icon theme
    if (QIcon::themeName().isEmpty() ||
        QIcon::themeName() == QString("hicolor"))
    {
        QIcon::setThemeName("Cyan");
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

    // stylesheet
    setStyleSheet(QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                          "QToolBar { border-color: none; }"));
}

void Editor::setupUI()
{
    setupStyle();
    setupWidgets();
    setupMenus();
    setupToolbars();
    setupActions();
    setupButtons();
    setupColorManagement();
    setupConnections();
    setupIcons();
    setupShortcuts();
    setupOptions();

    setCentralWidget(mdi);
    setStatusBar(mainStatusBar);
    setMenuBar(mainMenu);

    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(colorMenu);
    mainMenu->addMenu(layerMenu);
    mainMenu->addMenu(optMenu);
    windowsMenu->attachToMdiArea(mdi);
    mainMenu->addMenu(windowsMenu);
    mainMenu->addMenu(helpMenu);

    mainToolBar->addAction(newImageAct);
    mainToolBar->addAction(openImageAct);
    mainToolBar->addWidget(saveButton);
    mainToolBar->addSeparator();
    mainToolBar->addAction(viewMoveAct);
    mainToolBar->addAction(viewDrawAct);
    mainToolBar->addWidget(textButton);
    //mainToolBar->addWidget(layerButton);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(colorPicker);




    fileMenu->addAction(newImageAct);
    fileMenu->addAction(newLayerAct);
    fileMenu->addAction(newTextLayerAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAct);
    fileMenu->addAction(saveProjectAsAct);
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

    //newMenu = new QMenu(this);
    //newMenu->setTitle(tr("New"));

    saveMenu = new QMenu(this);
    saveMenu->setTitle(tr("Save"));

    colorMenu = new QMenu(this);
    colorMenu->setTitle(tr("Colors"));

    layerMenu = new QMenu(this);
    layerMenu->setTitle(tr("Layers"));

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
    mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
    mainToolBar->setMovable(false);

    addToolBar(Qt::TopToolBarArea,
               mainToolBar);
}

void Editor::setupWidgets()
{
    mdi = new Mdi(this);
    mdi->setBackground(QBrush(QColor(20, 20, 20)));
    //mdi->setViewMode(QMdiArea::TabbedView);

    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));

    brushSize = new QSlider(this);
    brushSize->setRange(1,256);
    brushSize->setValue(20);
    brushSize->setOrientation(Qt::Vertical);

    // layer
    layersWidget = new CyanLayerWidget(this);
    layersDock = new QDockWidget(this);
    layersDock->setObjectName(QString("layersDock"));
    layersDock->setWindowTitle(tr("Layers"));
    layersDock->setWidget(layersWidget);
    addDockWidget(Qt::RightDockWidgetArea, layersDock);

    // text
    textButton = new QToolButton(this);
    textButton->setToolTip(tr("Text"));
    textButton->setIcon(QIcon::fromTheme("font"));
    textButton->setCheckable(true);

    textPopup = new ctkPopupWidget(textButton);
    QVBoxLayout *textPopupLayout = new QVBoxLayout(textPopup);
    textWidget = new CyanTextWidget(textPopup);
    textWidget->setDisabled(true);

    textPopupLayout->addWidget(textWidget);
    textPopup->setAutoShow(true);
    textPopup->setAutoHide(true);
}


void Editor::setupActions()
{

    newImageAct = new QAction(this);
    newImageAct->setText(tr("New image"));

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open"));

    saveImageAct = new QAction(this);
    saveImageAct->setText(tr("Save image"));

    saveProjectAct = new QAction(this);
    saveProjectAct->setText(tr("Save project"));

    saveProjectAsAct = new QAction(this);
    saveProjectAsAct->setText(tr("Save project as ..."));
    saveProjectAsAct->setDisabled(true);

    newLayerAct = new QAction(this);
    newLayerAct->setText(tr("New layer"));

    newTextLayerAct = new QAction(this);
    newTextLayerAct->setText(tr("New text layer"));

    openLayerAct = new QAction(this);

    saveLayerAct = new QAction(this);
    saveLayerAct->setText(tr("Save layer as ..."));

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
    aboutCyanAct->setText(tr("About Cyan FX"));

    aboutImageMagickAct = new QAction(this);
    aboutImageMagickAct->setText(tr("About ImageMagick"));

    aboutLcmsAct = new QAction(this);
    aboutLcmsAct->setText(tr("About Little CMS"));

    aboutQtAct = new QAction(this);
    aboutQtAct->setText(tr("About Qt"));

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
}

void Editor::setupButtons()
{
    /*newButton = new QToolButton(this);
   // newButton->setIconSize(QSize(32, 32));
    newButton->setMenu(newMenu);
    newButton->setPopupMode(QToolButton::InstantPopup);
    newButton->setText(tr("New"));
    newButton->setToolTip(tr("New"));
    //newButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);*/

    saveButton = new QToolButton(this);
    //saveButton->setIconSize(QSize(32, 32));
    saveButton->setMenu(saveMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setText(tr("Save"));
    saveButton->setToolTip(tr("Save"));
    //saveButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);




}

void Editor::setupColorManagement()
{
    colorPicker = new QtColorPicker(this, -1, true, false);
    colorPicker->setIconSize(QSize(24,24));
    colorPicker->setStandardColors();

    colorPopup = new ctkPopupWidget(colorPicker);
    QVBoxLayout *colorPopupLayout = new QVBoxLayout(colorPopup);

    colorPopup->setAutoShow(true);
    colorPopup->setAutoHide(true);

    colorTriangle = new QtColorTriangle(colorPopup);
    colorTriangle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    colorPopupLayout->addWidget(colorTriangle);
    colorTriangle->setMinimumSize(QSize(175, 175));

    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorPicker, SLOT(setCurrentColor(QColor)));
    connect(colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(handleColorChanged(QColor)));
    connect(colorPicker, SIGNAL(colorChanged(QColor)), colorTriangle, SLOT(setColor(QColor)));

   // QIcon colorsIcon = QIcon::fromTheme("smartart_change_color_gallery");

    ColorRGB *colorRGB = new ColorRGB(colorPopup);
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorRGB, SLOT(setColor(QColor)));
    connect(colorRGB, SIGNAL(colorChanged(QColor)), colorTriangle, SLOT(setColor(QColor)));

    ColorCMYK *colorCMYK = new ColorCMYK(this);
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorCMYK, SLOT(setColor(QColor)));
    connect(colorCMYK, SIGNAL(colorChanged(QColor)), colorTriangle, SLOT(setColor(QColor)));

    ColorHSV *colorHSV = new ColorHSV(this);
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorHSV, SLOT(setColor(QColor)));
    connect(colorHSV, SIGNAL(colorChanged(QColor)), colorTriangle, SLOT(setColor(QColor)));

    colorTriangle->setColor(QColor(Qt::black));

    colorPopupLayout->addWidget(colorHSV);
    colorPopupLayout->addWidget(colorRGB);
    colorPopupLayout->addWidget(colorCMYK);
}

void Editor::setupConnections()
{

    //connect(&common, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    //connect(&common, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));

    connect(newImageAct, SIGNAL(triggered(bool)), this, SLOT(newImageDialog()));


    connect(newLayerAct, SIGNAL(triggered(bool)), this, SLOT(newLayerDialog()));
    connect(newTextLayerAct, SIGNAL(triggered(bool)), this, SLOT(newTextLayerDialog()));



    connect(openImageAct, SIGNAL(triggered(bool)), this, SLOT(loadImageDialog()));
    connect(saveProjectAct, SIGNAL(triggered(bool)), this, SLOT(saveProjectDialog()));
    connect(saveImageAct, SIGNAL(triggered(bool)), this, SLOT(saveImageDialog()));
    connect(saveLayerAct, SIGNAL(triggered(bool)), this, SLOT(saveLayerDialog()));

    connect(quitAct, SIGNAL(triggered(bool)), this, SLOT(close()));

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

    connect(textWidget,
            SIGNAL(textChanged()),
            this,
            SLOT(handleCurrentLayerTextChanged()));

    //connect(layerButton, SIGNAL(toggled(bool)), layerPopup, SLOT(pinPopup(bool)));
    connect(textButton, SIGNAL(toggled(bool)), textPopup, SLOT(pinPopup(bool)));

}

void Editor::setupIcons()
{
    setWindowIcon(QIcon::fromTheme("cyan"));

    newImageAct->setIcon(QIcon::fromTheme("document-new"));
    newLayerAct->setIcon(QIcon::fromTheme("document-new"));
    newTextLayerAct->setIcon(QIcon::fromTheme("document-new"));



    openImageAct->setIcon(QIcon::fromTheme("document-open"));
    saveButton->setIcon(QIcon::fromTheme("document-save"));
    saveImageAct->setIcon(QIcon::fromTheme("document-save"));
    saveProjectAct->setIcon(QIcon::fromTheme("document-save"));
    saveLayerAct->setIcon(QIcon::fromTheme("document-save"));
    saveProjectAsAct->setIcon(QIcon::fromTheme("document-save-as"));
    quitAct->setIcon(QIcon::fromTheme("application-exit"));

    viewMoveAct->setIcon(QIcon::fromTheme("transform_move"));
    viewDrawAct->setIcon(QIcon::fromTheme("paintbrush"));

    QIcon colorsIcon = QIcon::fromTheme("smartart_change_color_gallery");
    QIcon colorWheelIcon = QIcon::fromTheme("color_wheel");

    convertRGBAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertCMYKAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertGRAYAct->setIcon(QIcon::fromTheme("convert_color_to_gray"));
    convertAssignAct->setIcon(colorsIcon);
    convertExtractAct->setIcon(colorsIcon);
    colorProfileRGBMenu->setIcon(colorWheelIcon);
    colorProfileCMYKMenu->setIcon(colorWheelIcon);
    colorProfileGRAYMenu->setIcon(colorWheelIcon);
    colorIntentMenu->setIcon(QIcon::fromTheme("monitor_window_flow"));
    blackPointAct->setIcon(colorWheelIcon);

    aboutQtAct->setIcon(QIcon::fromTheme("help-about"));
    aboutLcmsAct->setIcon(QIcon::fromTheme("help-about"));
    aboutImageMagickAct->setIcon(QIcon::fromTheme("help-about"));
    aboutCyanAct->setIcon(QIcon::fromTheme("help-about"));

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

