#include "surfacegraph.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QScreen>


#include <QtDataVisualization/Q3DInputHandler>

#include "simulation.h"
#include "visualization.h"
#include "inputhandler.h"

using namespace QtDataVisualization;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Q3DSurface *graph = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(graph);

    //Example code for displaying the graph
    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    widget->setWindowTitle(QStringLiteral("2D Smoke simulation"));

    QSlider *saturation_slider = new QSlider(Qt::Horizontal, widget);
    saturation_slider->setRange(0, 255);

    QComboBox *themeList = new QComboBox(widget);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));

    QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(1.0, Qt::black);
    grBtoY.setColorAt(0.67, Qt::blue);
    grBtoY.setColorAt(0.33, Qt::red);
    grBtoY.setColorAt(0.0, Qt::yellow);
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(widget);
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.0, Qt::darkGreen);
    grGtoR.setColorAt(0.5, Qt::yellow);
    grGtoR.setColorAt(0.2, Qt::red);
    grGtoR.setColorAt(0.0, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientGtoRPB = new QPushButton(widget);
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));


    QLinearGradient grRainbow(0,0,1,100);
    grRainbow.setColorAt(0.0, Qt::blue);
    grRainbow.setColorAt(0.25, Qt::cyan);
    grRainbow.setColorAt(0.5, Qt::green);
    grRainbow.setColorAt(.75, Qt::yellow);
    grRainbow.setColorAt(1, Qt::red);
    pmp.setBrush(QBrush(grRainbow));
    pmp.drawRect(0, 0, 24, 100);

    QPushButton *gradientRainbowButton = new QPushButton(widget);
    gradientRainbowButton->setIcon(QIcon(pm));
    gradientRainbowButton->setIconSize(QSize(24, 100));


    QLinearGradient grHeatmap(0,0,1,100);
    grHeatmap.setColorAt(0.0, Qt::blue);
    grHeatmap.setColorAt(0.5, Qt::white);
    grHeatmap.setColorAt(1, Qt::red);
    pmp.setBrush(QBrush(grHeatmap));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientHeatmapPressedButton = new QPushButton(widget);
    gradientHeatmapPressedButton->setIcon(QIcon(pm));
    gradientHeatmapPressedButton->setIconSize(QSize(24, 100));



    QLinearGradient grHeatmapRed(0, 0, 1, 100);
    grHeatmapRed.setColorAt(0.0, Qt::darkRed);
    grHeatmapRed.setColorAt(0.5, Qt::red);
    grHeatmapRed.setColorAt(1, Qt::white);
    pmp.setBrush(QBrush(grHeatmapRed));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientHeatmapRedPressedButton = new QPushButton(widget);
    gradientHeatmapRedPressedButton->setIcon(QIcon(pm));
    gradientHeatmapRedPressedButton->setIconSize(QSize(24, 100));


    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorHBox->addWidget(gradientRainbowButton);
    colorHBox->addWidget(gradientHeatmapPressedButton);
    colorHBox->addWidget(gradientHeatmapRedPressedButton);
    colorGroupBox->setLayout(colorHBox);

    QSlider *clampThresholdSlider = new QSlider(Qt::Horizontal, widget);
    clampThresholdSlider->setRange(0,100);
    clampThresholdSlider->setTickInterval(25);
    clampThresholdSlider->setTickPosition(QSlider::TicksAbove);

    vLayout->addWidget(new QLabel(QStringLiteral("Theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Saturation")));
    vLayout->addWidget(saturation_slider);
    vLayout->addWidget(colorGroupBox);
    vLayout->addWidget(clampThresholdSlider);

    widget->show();

    SurfaceGraph *m_graph = new SurfaceGraph(graph);

    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
                     m_graph, SLOT(changeTheme(int)));
    QObject::connect(saturation_slider, SIGNAL(valueChanged(int)),
                     m_graph, SLOT(setSaturation(int)));

    QObject::connect(gradientBtoYPB, &QPushButton::pressed,
                     m_graph, &SurfaceGraph::setBlackToYellowGradient);
    QObject::connect(gradientGtoRPB, &QPushButton::pressed,
                     m_graph, &SurfaceGraph::setGreenToRedGradient);

    QObject::connect(gradientRainbowButton, &QPushButton::pressed,
                     m_graph, &SurfaceGraph::setRainbowGradient);
    QObject::connect(gradientHeatmapPressedButton, &QPushButton::pressed,
                     m_graph, &SurfaceGraph::setHeatmapGradient);
    QObject::connect(gradientHeatmapRedPressedButton, &QPushButton::pressed,
                     m_graph, &SurfaceGraph::setHeatmapRedGradient);

    themeList->setCurrentIndex(2);

    // Value should be initialized with the max value (255) but a higher value than 1
    // causes a 'index out of range' exception. Therefore we initialize at zero.
    saturation_slider->setValue(0);

    m_graph->enableSqrtSinModel(true);
    m_graph->toggleModeItem();

    Simulation *simulation = new Simulation();
    InputHandler *input_handler = new InputHandler(graph, simulation);
    graph->setActiveInputHandler(input_handler);
    Visualization *visualization = new Visualization(m_graph, simulation);
    visualization->Start();


    QObject::connect(input_handler, &InputHandler::dragged,
                     simulation, &Simulation::drag);

    QObject::connect(clampThresholdSlider, SIGNAL(valueChanged(int)), visualization, SLOT(setClampingThreshold(int)));

    return app.exec();
}
