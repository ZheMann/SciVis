#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QtGui/QImage>
#include <QtCore/qmath.h>
#include <QColor>

using namespace QtDataVisualization;

const int sampleCountX = 50;
const int sampleCountZ = 50;
const int heightMapGridStepX = 6;
const int heightMapGridStepZ = 6;
const float sampleMin = -8.0f;
const float sampleMax = 8.0f;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface)
{
    QCustom3DItem *arrow = new QCustom3DItem();
    arrow->setScaling(QVector3D(0.07f, 0.07f, 0.07f));
    arrow->setMeshFile(QStringLiteral(":/mesh/narrowarrow.obj"));
    QImage sunColor = QImage(2, 2, QImage::Format_RGB32);
    sunColor.fill(QColor(0xff, 0xbb, 0x00));
    arrow->setTextureImage(sunColor);
    m_graph->addCustomItem(arrow);

    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_graph->setReflection(false);
    m_graph->activeTheme()->setType(Q3DTheme::ThemeEbony);
    m_graph->activeTheme()->setGridEnabled(false);
    m_graph->activeTheme()->setBackgroundEnabled(false);
    m_graph->activeTheme()->setLabelTextColor("transparent");

    m_graph_data = new QSurfaceDataProxy();
    m_sqrtSinSeries = new QSurface3DSeries(m_graph_data);

    m_graph ->setOrthoProjection(true);
    m_graph -> scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetDirectlyAbove);

    m_graph ->setReflectivity(0);


}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}

void SurfaceGraph::SetArrayData(QSurfaceDataArray *data)
{
    m_graph_data->resetArray(data);
}

void SurfaceGraph::fillSqrtSinProxy()
{
    float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
    float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (int i = 0 ; i < sampleCountZ ; i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
        // Keep values within range bounds, since just adding step can cause minor drift due
        // to the rounding errors.
        float z = qMin(sampleMax, (i * stepZ + sampleMin));
        int index = 0;
        for (int j = 0; j < sampleCountX; j++) {
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float R = qSqrt(z * z + x * x) + 0.01f;
            float y = (qSin(R) / R + 0.24f) * 1.61f;
            (*newRow)[index++].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }

    m_graph_data->resetArray(dataArray);
}

void SurfaceGraph::setClamp(float min, float max){
    m_graph->axisY()->setRange(min, max);
}

void SurfaceGraph::enableSqrtSinModel(bool enable)
{
    if (enable) {
        m_sqrtSinSeries->setDrawMode(QSurface3DSeries::DrawSurface);
        m_sqrtSinSeries->setItemLabelVisible(false);
        m_sqrtSinSeries->setFlatShadingEnabled(false);


        m_graph->axisX()->setRange(.0f, 50.0f);
        m_graph->axisY()->setRange(.0f, .25f);
        m_graph->axisZ()->setRange(.0f, 50.0f);

        m_graph->setAspectRatio(20);
        m_graph->scene()->activeCamera()->setZoomLevel(160);

        //Set current data remove other
        m_graph->addSeries(m_sqrtSinSeries);

        m_graph->axisX()->setLabelAutoRotation(30);
        m_graph->axisY()->setLabelAutoRotation(90);
        m_graph->axisZ()->setLabelAutoRotation(30);
    }
}

void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setRainbowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::blue);
    gr.setColorAt(0.25, Qt::cyan);
    gr.setColorAt(0.5, Qt::green);
    gr.setColorAt(.75, Qt::yellow);
    gr.setColorAt(1, Qt::red);


    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setHeatmapRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkRed);
    gr.setColorAt(0.5, Qt::red);
    gr.setColorAt(1, Qt::white);


    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setHeatmapGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::blue);
    gr.setColorAt(0.5, Qt::white);
    gr.setColorAt(1, Qt::red);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}


void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
