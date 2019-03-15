#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization>
#include <QtWidgets/QSlider>

using namespace QtDataVisualization;

class SurfaceGraph : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraph(Q3DSurface *surface);
    ~SurfaceGraph();

    void SetArrayData(QSurfaceDataArray *data);

    void enableSqrtSinModel(bool enable);

    void toggleModeItem() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem); }

    void setClamp(float min, float max);

    void setBlackToYellowGradient();
    void adjustBlackToYellowGradient(int saturation_value);

    void setGreenToRedGradient();
    void adjustGreenToRedGradient(int saturation_value);

    void setRainbowGradient();
    void adjustRainbowGradient(int saturation_value);

    void setHeatmapGradient();
    void adjustHeatmapGradient(int saturation_value);

    void setHeatmapRedGradient();
    void adjustHeatmapRedGradient(int saturation_value);

public Q_SLOTS:
    void changeTheme(int theme);
    void setSaturation(int saturation_value);

private:
    Q3DSurface *m_graph;

    QSurfaceDataProxy *m_graph_data;
    QSurface3DSeries *m_sqrtSinSeries;
    Q3DInputHandler *input_handler;


    float m_rangeMinX;
    float m_rangeMinZ;
    float m_stepX;
    float m_stepZ;


    void fillSqrtSinProxy();
};

#endif // SURFACEGRAPH_H
