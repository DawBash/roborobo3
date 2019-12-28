/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#ifndef SANDBOXCONTROLLER_H
#define SANDBOXCONTROLLER_H

#include "TemplateEE/include/TemplateEEController.h"
#include "Utilities/Packet.h"

using namespace Neural;

class RobotWorldModel;

class SandboxController : public TemplateEEController
{
    public:
    
        SandboxController(RobotWorldModel *wm);
        ~SandboxController();
    
        double getFitness() override;

    protected:
    
        void initController() override;
        void initEvolution() override;
    
        void stepController() override;
        void stepEvolution() override;
    
        void performSelection() override;
        void performVariation() override;
    
        void broadcastGenome() override;
    
        bool sendGenome( TemplateEEController* __targetRobotController ) override;
        bool receiveGenome( Packet* p ) override;
    
        void resetFitness() override;
        void updateFitness() override;

        void logCurrentState() override;
};


#endif

