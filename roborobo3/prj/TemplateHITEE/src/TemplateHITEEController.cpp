/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "TemplateHITEE/include/TemplateHITEEController.h"
#include "TemplateHITEE/include/TemplateHITEESharedData.h"

#include "World/World.h"
#include "RoboroboMain/roborobo.h"
#include "WorldModels/RobotWorldModel.h"

using namespace Neural;

bool debug = true;



TemplateHITEEController::TemplateHITEEController( RobotWorldModel *wm ) : TemplateEEController( wm )
{
    // superclass constructor called before this baseclass constructor.
    
    _minValue = -10.0;
    _maxValue = 10.0;
    
    _maturationDelay = TemplateHITEESharedData::maturationDelayDefaultValue;
    
    rewards = new SlidingWindow(TemplateHITEESharedData::memorySlidingWindowSize);
    rewards->setAlwaysPositiveSum(true);
    distances = new SlidingWindow(TemplateHITEESharedData::memorySlidingWindowSize);
    distances->setAlwaysPositiveSum(true);
    xCoords = new SlidingWindow(TemplateHITEESharedData::coordSlidingWindowSize);
    yCoords = new SlidingWindow(TemplateHITEESharedData::coordSlidingWindowSize);
    resetFitness();
    
    if ( TemplateHITEESharedData::transferVolume == 0.0 )
        packetLength_HIT = 1; // default minimal value
    else
        if ( TemplateHITEESharedData::transferVolume == 1.0 )
            packetLength_HIT = (int)_currentGenome.size();
        else
            packetLength_HIT = (int)_currentGenome.size() * TemplateHITEESharedData::transferVolume;
    
    if ( _wm->getId() == 0 && debug == true )
        std::cout << "[Info] genome size = " << (int)_currentGenome.size() << "\n";
}

TemplateHITEEController::~TemplateHITEEController()
{
    // superclass destructor automatically called after this baseclass destructor.
}

void TemplateHITEEController::stepController()
{
    TemplateEEController::stepController();
}

void TemplateHITEEController::initController()
{
    TemplateEEController::initController();
}

void TemplateHITEEController::initEvolution()
{
    TemplateEEController::initEvolution();
}

void TemplateHITEEController::performSelection()
{
    TemplateEEController::performSelection();
}

void TemplateHITEEController::performVariation()
{
    TemplateEEController::performVariation();
}

void TemplateHITEEController::broadcastGenome()
{
    TemplateEEController::broadcastGenome();
}

double TemplateHITEEController::getFitness()
{
    //distances->displayInfo(); // DEBUG
    return _wm->_fitnessValue;
}

void TemplateHITEEController::resetFitness()
{
    //TemplateEEController::resetFitness();
    _dSumTravelled = 0;
    rewards->reset(0);
    distances->reset(0);
    xCoords->reset(_wm->getXReal());
    yCoords->reset(_wm->getYReal());
}

void TemplateHITEEController::updateFitness()
{
    // nothing to do -- updating is performed in TemplateHITEEController::AgentObserver and uses updateFitness2
    // note that updateFitness() is called automatically at each iteration (which is not the case of updateFitness2, which is called when needed)
}

void TemplateHITEEController::updateFitness2( int value )
{
    // Fitness 1
    rewards->push( value );
    
    // fitness 2
    
    xCoords->push(_wm->getXReal());
    yCoords->push(_wm->getYReal());
    
    double dist = getEuclideanDistance(
                    xCoords->getPrev( 0 ), // current position (same as _wm->getXreal()
                    yCoords->getPrev( 0 ),
                    xCoords->getPrev( -TemplateHITEESharedData::coordSlidingWindowSize+1 ), // oldest recorded position
                    yCoords->getPrev( -TemplateHITEESharedData::coordSlidingWindowSize+1 )
                    );

    if ( dist < 0 ) // debug
    {
        std::cout << "[ERROR] dist<0\n";
        std::cout << "[DEBUG] dist:" << (double)dist << " from: (" << _wm->getXReal() << "," << _wm->getYReal() << ")<=>(" << xCoords->getPrev( 0 ) << "," << yCoords->getPrev( 0 ) << ") vs (" << xCoords->getPrev(-TemplateHITEESharedData::coordSlidingWindowSize+1) << "," << yCoords->getPrev(-TemplateHITEESharedData::coordSlidingWindowSize+1) << ")\n";
        exit (-2);
    }
    
    distances->push ( dist );
    
    //if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true )
    //    std::cout << "[DEBUG] dist:" << (double)dist << " from: (" << _wm->getXReal() << "," << _wm->getYReal() << ")<=>(" << xCoords->getPrev( 0 ) << "," << yCoords->getPrev( 0 ) << ") vs (" << xCoords->getPrev(-coordSlidingWindowSize+1) << "," << yCoords->getPrev(-coordSlidingWindowSize+1) << ")\n";

    // update "official" fitness
    
    switch ( TemplateHITEESharedData::fitnessFunction )
    {
        case 0:
            _wm->_fitnessValue = rewards->getSum();
            break;
        case 1:
            _wm->_fitnessValue = distances->getSum(); //distances->getPrev(0);
            break;
        default:
            std::cout << "[ERROR] unknown fitness function ("<<TemplateHITEESharedData::fitnessFunction<<"). Exit.\n";
    }
    //if ( gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 )
    //    std::cout << "[DEBUG] updateFitness2("<<value<<")\n";
    
    if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true && gWorld->getIterations()%400 == 0 )
        std::cout << "[DEBUG] dist:" << (double)dist << " from: (" << _wm->getXReal() << "," << _wm->getYReal() << ")<=>(" << xCoords->getPrev( 0 ) << "," << yCoords->getPrev( 0 ) << ") vs (" << xCoords->getPrev(-TemplateHITEESharedData::coordSlidingWindowSize+1) << "," << yCoords->getPrev(-TemplateHITEESharedData::coordSlidingWindowSize+1) << ") -- fitness()=" << getFitness() << "\n";
    
}

void TemplateHITEEController::logCurrentState()
{
    TemplateEEController::logCurrentState();
}

// -------
// -------
// -------

void TemplateHITEEController::stepEvolution()
{
    if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true && gWorld->getIterations()%400 == 0 )
        std::cout << "[DEBUG] [" << gWorld->getIterations()<< "] focus is on robot #" << _wm->getId() << " ( __fit=" << getFitness() <<"__ , matDelay=" << _maturationDelay << " )\n";
    
    if ( getNewGenomeStatus() ) // check for new NN parameters
    {
        mapGenotypeToPhenotype();
        setNewGenomeStatus(false);
        _maturationDelay = TemplateHITEESharedData::maturationDelayDefaultValue;
        resetFitness();
    }
    else
    {
        if ( _maturationDelay > 0 )
            _maturationDelay--;
    }
    
    //if ( gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 )
    //    std::cout << "[DEBUG] agent #" << gRobotIndexFocus << " : " << _wm->_fitnessValue << "\n";
    
    // Do NOT call TemplateEEController::stepEvolution()
    
    clearReservoir(); // [!n] check when should be done (possibly inbetween world iteration? if done here: not synced. though async is ok.
    
    if ( _wm->isAlive() == true ) // in HIT-EE, always true.
    {
        // send genome to neighbours
        broadcastGenome();

        // with a (very) small probability, apply uniform mutation to one gene
        if ( random01() < TemplateHITEESharedData::P_mutation )
        {
            if ( TemplateHITEESharedData::mutationVolume == 0.0 )
            {
                // only one gene mutates
                int randomIndex = randint() % _currentGenome.size();

                switch ( TemplateHITEESharedData::mutationOp )
                {
                    case 0:
                        _currentGenome[randomIndex] = random01() * ( _maxValue - _minValue )  + _minValue;
                        break;
                    case 1:
                        _currentGenome[randomIndex] = getBoundedGaussianMutatedValue( _currentGenome[randomIndex], _currentSigma, _minValue, _maxValue );
                        break;
                default:
                    std::cout << "[ERROR] Mutation operator incorrect. Exit.\n";
                    exit (-1);
                }
            }
            else
            {
                if ( TemplateHITEESharedData::mutationVolume == 1.0 )
                {
                    // all genes mutate
                    switch ( TemplateHITEESharedData::mutationOp )
                    {
                        case 0:
                            for ( unsigned int i = 0 ; i < _currentGenome.size() ; i++ )
                                _currentGenome[i] = random01() * ( _maxValue - _minValue )  + _minValue;
                            break;
                        case 1:
                            for ( unsigned int i = 0 ; i < _currentGenome.size() ; i++ )
                                _currentGenome[i] = getBoundedGaussianMutatedValue( _currentGenome[i], _currentSigma, _minValue, _maxValue );
                            break;
                        default:
                            std::cout << "[ERROR] Mutation operator incorrect. Exit.\n";
                            exit (-1);
                    }
                }
                else
                {
                    // a sub-part of the genes mutate (more than 1, less than all)
                    
                    int nbGenesToMutate = TemplateHITEESharedData::mutationOp * _currentGenome.size() + 0.5;
                    
                    std::vector<int> indexes;
                    
                    // set some values:
                    for ( int i = 0 ; i < _currentGenome.size(); i++ )
                        indexes.push_back(i); // 0 1 ... size-1
                    
                    std::random_shuffle ( indexes.begin(), indexes.end() ); // use built-in random generator
                    //std::shuffle ( indexes.begin(), indexes.end(), engine ); // use MT
                    
                    switch ( TemplateHITEESharedData::mutationOp )
                    {
                        case 0:
                            for ( unsigned int i = 0 ; i < nbGenesToMutate ; i++ )
                                _currentGenome[indexes[i]] = random01() * ( _maxValue - _minValue )  + _minValue;
                            break;
                        case 1:
                            for ( unsigned int i = 0 ; i < nbGenesToMutate ; i++ )
                                _currentGenome[indexes[i]] = getBoundedGaussianMutatedValue( _currentGenome[indexes[i]], _currentSigma, _minValue, _maxValue );
                            break;
                        default:
                            std::cout << "[ERROR] Mutation operator incorrect. Exit.\n";
                            exit (-1);
                    }
                }
            }

            mapGenotypeToPhenotype(); // genome must be reloaded to phenotype

            if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true  )
            {
                std::cout << "[DEBUG] [" << gWorld->getIterations()<< "] robot #" << _wm->getId() << " mutates";
                if ( TemplateHITEESharedData::resetOnMutation == true )
                    std::cout << "(reset: true)\n";
                else
                    std::cout << "(reset: false)\n";
            }
            
            if ( TemplateHITEESharedData::resetOnMutation == true )
                setNewGenomeStatus(true);
        }
    }
    
    // logging
    _dSumTravelled = _dSumTravelled + getEuclideanDistance( _wm->getXReal(), _wm->getYReal(), _Xinit, _Yinit ); //remark: incl. squareroot.

}

// -----
// -----
// -----

bool TemplateHITEEController::sendGenome( TemplateEEController* __targetRobotController )
{
    // future implementations: conditional send.

    if ( _maturationDelay > 0 || getNewGenomeStatus() == true )
    {
        // this agent is either (1) still maturing or (2) already accepted incoming information, and waiting to be updated. Do not communicate.
        return false;
    }
    
    Packet* p = new Packet();
    p->senderId = std::make_pair(_wm->getId(), _birthdate);
    p->genome = _currentGenome;
    p->fitness = getFitness();
    //p->sigma = _currentSigma;
    
    bool retValue = ((TemplateHITEEController*)__targetRobotController)->receiveGenome(p);
    
    delete p;
    
    return retValue;
}

// -----
// -----
// -----

// Remark: receiveGenome is called only if robot's listening mode is on (ie. _isListening == true).
bool TemplateHITEEController::receiveGenome( Packet* p )
{
    // future implementations: conditional receive&integrate.
    
    if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true )
    {
        // std::cout << "[DEBUG] robot #" << _wm->getId() << " ( __fit=" << getFitness() << "__ , matDelay=" << _maturationDelay << " ) receives information from robot #" << p->senderId.first << " ( __fit=" << p->fitness << "__ ) --> " << getFitness() << " vs. " << p->fitness << "\n";
        //rewards->displayInfo();
        if ( getFitness() < 0 )
        {
            std::cout << "[ERROR] [" << gWorld->getIterations()<< "] robot #" << _wm->getId() << "'s sum_of_rewards is negative. Stop.\n";
            exit(-1);
        }
    }
    
    if ( getNewGenomeStatus() == true ) // already accepted incoming information, waiting to be updated.
        return false;
    
    std::map<std::pair<int,int>, std::vector<double> >::const_iterator it = _genomesList.find(p->senderId);
    
    if ( it == _genomesList.end() ) // check never met since last listening reset (same robot, same generation)
    {
        if ( _maturationDelay > 0 )
        {
            //if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true  )
                //std::cout << "[DEBUG] [" << gWorld->getIterations()<< "] robot #" << _wm->getId()  << " ( __fit=" << getFitness() << "__ , matDelay=" << _maturationDelay << " ) is NOT mature\n";
                //std::cout << ".";
            return false; // this agent is still maturing. Ignored.
        }
        
        // register this robot (used to consider each sender only once per iteration)
        _genomesList[p->senderId] = p->genome;
        //_sigmaList[p->senderId] = p->sigma;
        //_fitnessValueList[p->senderId] = p->fitness;

        if ( ( TemplateHITEESharedData::strictlySuperiorSelection == true and p->fitness > getFitness() ) or ( TemplateHITEESharedData::strictlySuperiorSelection == false and p->fitness >= getFitness() )  )
        {
            std::vector<int> indexes;
            
            // set some values:
            for ( int i = 0 ; i < _currentGenome.size(); i++ )
                indexes.push_back(i); // 0 1 ... size-1
            
            if ( TemplateHITEESharedData::transferVolume < 1.0 ) // not necessary if transmit full genome
                std::random_shuffle ( indexes.begin(), indexes.end() ); // use built-in random generator
                //std::shuffle ( indexes.begin(), indexes.end(), engine ); // use MT
            
            for ( int i = 0 ; i < packetLength_HIT ; i++ )
              _currentGenome[indexes[i]] = p->genome[indexes[i]];
            
            setNewGenomeStatus(true);
            if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true  )
                std::cout << "[DEBUG] [" << gWorld->getIterations()<< "] robot #" << _wm->getId() << " ACCEPTS incoming material from robot #" << p->senderId.first << " --> " << getFitness() << " vs. " << p->fitness << " [___________________________]\n";
            return true;
        }
        else
        {
            if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true )
                std::cout << "[DEBUG] [" << gWorld->getIterations()<< "] robot #" << _wm->getId() << " REJECTS incoming material from robot #" << p->senderId.first << " --> " << getFitness() << " vs. " << p->fitness << "\n";
            return false; // other was not better. Ignored.
        }
    }
    else
    {
        //if ( gRobotDisplayFocus == true && gRobotIndexFocus == _wm->getId() && gDisplayMode != 2 && debug == true  )
            //std::cout << "[DEBUG] robot #" << _wm->getId() << " rejects communication (already interacted at this iteration)\n";
        return false; // already met. Ignored.
    }
}
