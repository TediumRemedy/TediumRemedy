#ifndef CHINASTRANGER_H
#define CHINASTRANGER_H

#include <QObject>
#include "cometclient.h"

class ChinaStranger : public CometClient
{
    Q_OBJECT
public:
    explicit ChinaStranger(QObject *parent = 0);

signals:

public slots:

};

#endif // CHINASTRANGER_H
