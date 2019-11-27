#include <ros/ros.h>
#include <smacc/smacc.h>

// CLIENTS
#include <sm_threesome/clients/client_1.h>
#include <sm_threesome/clients/client_2.h>

// ORTHOGONALS
#include <sm_threesome/orthogonals/orthogonal_1.h>
#include <sm_threesome/orthogonals/orthogonal_2.h>

//SUBSTATE BEHAVIORS
#include <sm_threesome/substate_behaviors/client_1/substate_behavior_1.h>
#include <sm_threesome/substate_behaviors/client_1/substate_behavior_1b.h>
#include <sm_threesome/substate_behaviors/client_2/substate_behavior_2.h>
#include <sm_threesome/substate_behaviors/client_2/substate_behavior_2b.h>

//LOGIC UNITS
#include <event_aggregator/logic_units/lu_all_events_go.h>

using namespace smacc;

namespace sm_threesome
{
//SUPERSTATES
namespace SS1
{
class Ss1;
}

//STATES
class StState1; // first state specially needs a forward declaration
class StState2;
class StState3;

// STATE MACHINE
struct SmThreeSome
    : public smacc::SmaccStateMachineBase<SmThreeSome, StState1>
{
    using SmaccStateMachineBase::SmaccStateMachineBase;

    virtual void onInitialize() override
    {
        this->createOrthogonal<Orthogonal1>();
        this->createOrthogonal<Orthogonal2>();
    }
};
} // namespace sm_threesome

//STATES
#include <sm_threesome/states/st_state_1.h>
#include <sm_threesome/states/st_state_2.h>
#include <sm_threesome/states/st_state_3.h>

#include <sm_threesome/superstates/ss_superstate_1.h>