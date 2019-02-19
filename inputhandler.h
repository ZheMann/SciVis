#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QObject>

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // INPUTHANDLER_H