#pragma once

#include <smacc/smacc.h>
namespace sm_moveit_screw_loop
{
// STATE DECLARATION
struct StFinalRaiseHandsUp : smacc::SmaccState<StFinalRaiseHandsUp, SmFetchSixTablePickNSort1>
{
    using SmaccState::SmaccState;

    // TRANSITION TABLE
    typedef mpl::list<
        
        Transition<EvCbSuccess<CbMoveKnownState, OrArm>, StInitialPosture, SUCCESS> ,
        Transition<EvCbFailure<CbMoveKnownState, OrArm>, StFinalRaiseHandsUp, ABORT>
        >
        reactions;

    // STATE FUNCTIONS
    static void staticConfigure()
    {
        configure_orthogonal<OrArm, CbMoveKnownState>("sm_moveit_screw_loop", "config/manipulation/known_states/final_raise_hands_up.yaml");
    }

    void runtimeConfigure()
    {
        // to avoid looping very fast if it aborts
        ros::Duration(1).sleep();
    }

    void onExit(ABORT)
    {
        ros::Duration(1).sleep();
    }
};
} // namespace sm_moveit_screw_loop