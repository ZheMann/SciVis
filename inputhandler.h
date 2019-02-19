#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QObject>
#include <QtDataVisualization/Q3DInputHandler>

using namespace QtDataVisualization;

class InputHandler : public Q3DInputHandler
{
    Q_OBJECT
public:
    explicit InputHandler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // INPUTHANDLER_H
