#include "surfacegraph.h"
#include <QTextStream>
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QtGui/QImage>
#include <QtCore/qmath.h>
#include <QColor>
#include <QDebug>

using namespace QtDataVisualization;

const int sampleCountX = 50;
const int sampleCountZ = 50;
const int heightMapGridStepX = 6;
const int heightMapGridStepZ = 6;
const float sampleMin = -8.0f;
const float sampleMax = 8.0f;

const std::string blackToYellowGradient = "BTYG";
const std::string rainbowGradient = "RG";
const std::string heatmapGradient = "HG";
const std::string heatmapRedGradient = "HRG";
const std::string greenToRedGradient = "GRG";

std::string current_gradient = "";

SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface)
{
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

void SurfaceGraph::InitArrows(int gridSize, int nArrows)
{
    QQuaternion arrowTilt = QQuaternion::fromEulerAngles(0,90,90);

    for (int i = 0; i <= nArrows; i++)
    {
        for(int j = 0; j <= nArrows; j++)
        {
            QCustom3DItem *arrow = new QCustom3DItem();
            arrow->setScaling(QVector3D(0.07f, 0.07f, 0.07f));
            arrow->setMeshFile(QStringLiteral(":/mesh/narrowarrow.obj"));
            QImage sunColor = QImage(2, 2, QImage::Format_RGB32);
            sunColor.fill(QColor(0xff, 0xbb, 0x00));
            arrow->setTextureImage(sunColor);
            arrow->setRotation(arrowTilt);
            arrow->setPosition(QVector3D( (i + .5f) * float(gridSize)/float(nArrows + 1), 0, (j + .5f) * float(gridSize)/float(nArrows + 1) ));
            m_arrows.append(arrow);
            m_graph->addCustomItem(arrow);
        }
    }
}

void SurfaceGraph::LoadArrows(SimulationData data)
{
    QCustom3DItem * arrow;
    for(int i = 0; i < m_arrows.size(); i++)
    {
        arrow = m_arrows[i];
        float * pitch;
        float * yaw;
        float * roll;
        arrow->rotation().getEulerAngles(pitch, yaw, roll);
        arrow->setRotation(QQuaternion::fromEulerAngles(*pitch, (int(*yaw) + 2) % 360, *roll));

        QVector3D position = arrow->position();
        int x = position.x();
        int z = int(position.z());

//        float test = m_graph_data->itemAt(1, 1)->y();
//        qDebug("%f", test);

        //position.setY(data.density[index]);
        //arrow->setPosition(position);
    }
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

        this->InitArrows(50, 15);
    }
}

void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setSaturation(int saturation_value)
{
    //qDebug("Value %d", saturation_value);

    if (current_gradient == blackToYellowGradient) {
        adjustBlackToYellowGradient(saturation_value);
    } else if (current_gradient == heatmapGradient) {
        adjustHeatmapGradient(saturation_value);
    } else if (current_gradient == heatmapRedGradient) {
        adjustHeatmapRedGradient(saturation_value);
    } else if (current_gradient == greenToRedGradient) {
        adjustGreenToRedGradient(saturation_value);
    } else {
        // Rainbow
        adjustRainbowGradient(saturation_value);
    }
}

void SurfaceGraph::adjustBlackToYellowGradient(int saturation_value)
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(0, saturation_value, 0));
    gr.setColorAt(0.33, QColor::fromHsv(240, saturation_value, 255));
    gr.setColorAt(0.67, QColor::fromHsv(0, saturation_value, 255));
    gr.setColorAt(1.0, QColor::fromHsv(60, saturation_value, 255));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(0, 255, 0));
    gr.setColorAt(0.33, QColor::fromHsv(240, 255, 255));
    gr.setColorAt(0.67, QColor::fromHsv(0, 255, 255));
    gr.setColorAt(1.0, QColor::fromHsv(60, 255, 255));

    current_gradient = blackToYellowGradient;

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::adjustRainbowGradient(int saturation_value) {
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(240,saturation_value,255));
    gr.setColorAt(0.25, QColor::fromHsv(180,saturation_value,255));
    gr.setColorAt(0.5, QColor::fromHsv(120,saturation_value,255));
    gr.setColorAt(0.75, QColor::fromHsv(60,saturation_value,255));
    gr.setColorAt(1, QColor::fromHsv(0,saturation_value,255));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setRainbowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(240,255,255));
    gr.setColorAt(0.25, QColor::fromHsv(180,255,255));
    gr.setColorAt(0.5, QColor::fromHsv(120,255,255));
    gr.setColorAt(0.75, QColor::fromHsv(60,255,255));
    gr.setColorAt(1, QColor::fromHsv(0,255,255));

    current_gradient = rainbowGradient;
    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::adjustHeatmapRedGradient(int saturation_value)
{
    QLinearGradient gr;

    gr.setColorAt(0.0, QColor::fromHsv(0,saturation_value,128));
    gr.setColorAt(0.5, QColor::fromHsv(0,saturation_value,255));

    // White needs to stay white so we keep saturation 0
    gr.setColorAt(1, QColor::fromHsv(0,0,255));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setHeatmapRedGradient()
{
    QLinearGradient gr;

    gr.setColorAt(0.0, QColor::fromHsv(0,255,128));
    gr.setColorAt(0.5, QColor::fromHsv(0,255,255));
    gr.setColorAt(1, QColor::fromHsv(0,0,255));

    current_gradient = heatmapRedGradient;
    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::adjustHeatmapGradient(int saturation_value)
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(240,saturation_value,255));

    // White needs to stay white so we keep saturation 0
    gr.setColorAt(0.5, QColor::fromHsv(0,0,255));

    gr.setColorAt(1, QColor::fromHsv(0,saturation_value,255));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setHeatmapGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(240,255,255));
    gr.setColorAt(0.5, QColor::fromHsv(0,0,255));
    gr.setColorAt(1, QColor::fromHsv(0,255,255));

    current_gradient = heatmapGradient;
    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::adjustGreenToRedGradient(int saturation_value)
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(120,saturation_value,128));
    gr.setColorAt(0.33, QColor::fromHsv(60,saturation_value,255));
    gr.setColorAt(0.67, QColor::fromHsv(0,saturation_value,255));
    gr.setColorAt(1, QColor::fromHsv(0,saturation_value,128));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, QColor::fromHsv(120,255,128));
    gr.setColorAt(0.33, QColor::fromHsv(60,255,255));
    gr.setColorAt(0.67, QColor::fromHsv(0,255,255));
    gr.setColorAt(1, QColor::fromHsv(0,255,128));

    current_gradient = greenToRedGradient;
    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
