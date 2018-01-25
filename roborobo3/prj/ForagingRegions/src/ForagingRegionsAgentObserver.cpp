/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "ForagingRegions/include/ForagingRegionsAgentObserver.h"
#include "ForagingRegions/include/ForagingRegionsSharedData.h"
#include "ForagingRegions/include/ForagingRegionsController.h"
#include "WorldModels/RobotWorldModel.h"
#include "World/PhysicalObject.h"
#include "RoboroboMain/roborobo.h"
#include "World/World.h"


ForagingRegionsAgentObserver::ForagingRegionsAgentObserver( RobotWorldModel *wm ) : TemplateEEAgentObserver ( wm )
{
    // superclass constructor called before
}

ForagingRegionsAgentObserver::~ForagingRegionsAgentObserver()
{
    // superclass destructor called before
}

/*
 * Manage foraging of energy items.
 * If walked on, the item disappear (callback to item object) and agent's fitness function is updated.
 * Assume that *only* energy point's footprint can be walked upon. That means that for this code to correctly run you should avoid placing any objects detectable through the robot's ground sensor.
 *
 */
void ForagingRegionsAgentObserver::step()
{
    // * update fitness (if needed)
    if ( _wm->isAlive() && PhysicalObject::isInstanceOf(_wm->getGroundSensorValue()) )
    {
        switch ( ForagingRegionsSharedData::foragingTask )
        {
            case 0:
                _wm->_fitnessValue = _wm->_fitnessValue + 1;
                break;
            case 1:
            {
                int targetIndex = _wm->getGroundSensorValue() - gPhysicalObjectIndexStartOffset;
                int threshold = ForagingRegionsSharedData::nbObjectsType1;
                if ( gPhysicalObjects[targetIndex]->getId() < threshold )
                {
                    _wm->_fitnessValue = _wm->_fitnessValue + 1;
                    ForagingRegionsController *ctl = dynamic_cast<ForagingRegionsController*>(getController());
                    ctl->nbForagedItemType0++;
                }
                else
                {
                    _wm->_fitnessValue = _wm->_fitnessValue - 1;
                    ForagingRegionsController *ctl = dynamic_cast<ForagingRegionsController*>(getController());
                    ctl->nbForagedItemType1++;
                }
                break;
            }
            default:
                std::cerr << "[ERROR] gForagingTask value is unkown. Exiting.\n";
                break;
        }
    }

    TemplateEEAgentObserver::step();
}



