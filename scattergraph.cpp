#include "scattergraph.h"

ScatterGraph::ScatterGraph(Q3DScatter *scatter) :
    m_graph(scatter),
    arrows(new QScatter3DSeries),
    arrowsArray(0)
{
    //Arrows move
    arrows->setItemSize(0.2f);
    arrows->setMesh(QAbstract3DSeries::MeshUserDefined);
    arrows->setUserDefinedMesh(QStringLiteral(":/mesh/narrowarrow.obj"));
    QLinearGradient fieldGradient(0, 0, 16, 1024);
    fieldGradient.setColorAt(0.0, Qt::black);
    fieldGradient.setColorAt(1.0, Qt::white);
    arrows->setBaseGradient(fieldGradient);
    arrows->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    m_graph->addSeries(this->arrows);

    init();

}

void ScatterGraph::init(){
    int m_fieldLines = 12;
    int m_arrowsPerLine = 16;
    float m_angleOffset = 0.0f;
    float m_angleStep = 10;
    float doublePi = float(M_PI) *2.0f;
    float radiansToDegrees = 360.0f / doublePi;

    // Reusing existing array is computationally cheaper than always generating new array, even if
          // all data items change in the array, if the array size doesn't change.
          if (!arrowsArray)
              arrowsArray = new QScatterDataArray;

          int arraySize = m_fieldLines * m_arrowsPerLine;
          if (arraySize != arrowsArray->size())
              arrowsArray->resize(arraySize);

          QScatterDataItem *ptrToDataArray = &arrowsArray->first();

          for (float i = 0; i < m_fieldLines; i++) {
              float verticalRange = 8.0f;
              float horizontalRange = verticalRange;
              float ellipse_a = horizontalRange / 3.0f;
              float ellipse_b = verticalRange;

              float horizontalAngle = (doublePi * i) / m_fieldLines;
              float xCenter = ellipse_a * qCos(horizontalAngle);
              float zCenter = ellipse_a * qSin(horizontalAngle);

              // Rotate - arrow always tangential to origin
              QQuaternion yRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, horizontalAngle * radiansToDegrees);

              for (float j = 0; j < m_arrowsPerLine; j++) {
                  // Calculate point on ellipse centered on origin and parallel to x-axis
                  float verticalAngle = ((doublePi * j) / m_arrowsPerLine) + m_angleOffset;
                  float xUnrotated = ellipse_a * qCos(verticalAngle);
                  float y = ellipse_b * qSin(verticalAngle);

                  // Rotate the ellipse around y-axis
                  float xRotated = xUnrotated * qCos(horizontalAngle);
                  float zRotated = xUnrotated * qSin(horizontalAngle);

                  // Add offset
                  float x = xCenter + xRotated;
                  float z = zCenter + zRotated;

                  QQuaternion zRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, verticalAngle * radiansToDegrees);
                  QQuaternion totalRotation = yRotation * zRotation;

                  ptrToDataArray->setPosition(QVector3D(x, y, z));
                  ptrToDataArray->setRotation(totalRotation);
                  ptrToDataArray++;
              }
          }

          if (m_graph->selectedSeries() == arrows)
              m_graph->clearSelection();

          arrows->dataProxy()->resetArray(arrowsArray);
}
