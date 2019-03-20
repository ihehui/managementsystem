/*
 * networkmanagerinstance.h
 *
 *  Created on: 2011-7-23
 *      Author: 贺辉
 */

#ifndef SERVERSERVICERESOURCEMANAGERINSTANCE_H_
#define SERVERSERVICERESOURCEMANAGERINSTANCE_H_

#include <QObject>

#include "../sharedms/resourcesmanager.h"

#include "HHSharedCore/Singleton"

namespace HEHUI
{


class ResourcesManagerInstance: public ResourcesManager, public Singleton<ResourcesManagerInstance>
{
    Q_OBJECT

    friend class Singleton<ResourcesManagerInstance> ;

public:

    ResourcesManagerInstance(QObject *parent = 0);
    virtual ~ResourcesManagerInstance();



signals:



public slots:



private slots:


private:



};

} //namespace HEHUI

#endif /* SERVERSERVICERESOURCEMANAGERINSTANCE_H_ */
