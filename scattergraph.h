#ifndef SCATTERGRAPH_H
#define SCATTERGRAPH_H
#include <QtDataVisualization>
#include <QObject>

using namespace QtDataVisualization;

class ScatterGraph : public QObject
{
    Q_OBJECT
public:
    explicit ScatterGraph(Q3DScatter *scatter= nullptr);
private:
    Q3DScatter * m_graph;
    QScatterDataArray *arrowsArray;
    QScatter3DSeries *arrows;

    void init();

signals:

public slots:
};

#endif // SCATTERGRAPH_H
