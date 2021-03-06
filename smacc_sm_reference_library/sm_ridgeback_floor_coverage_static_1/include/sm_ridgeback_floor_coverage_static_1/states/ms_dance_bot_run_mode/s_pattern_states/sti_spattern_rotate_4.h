namespace sm_ridgeback_floor_coverage_static_1
{
namespace s_pattern_states
{
// STATE DECLARATION
struct StiSPatternRotate4 : smacc::SmaccState<StiSPatternRotate4, SS>
{
    using SmaccState::SmaccState;

// TRANSITION TABLE
    typedef mpl::list<
    
    Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, StiSPatternForward4>,
    Transition<EvActionAborted<ClMoveBaseZ, OrNavigation>, StiSPatternForward3>
    
    >reactions;

// STATE FUNCTIONS
    static void staticConfigure()
    {
    }

    void runtimeConfigure()
    {
        auto &superstate = this->context<SS>();
        ROS_INFO("[SsrSPatternRotate] SpatternRotate rotate: SS current iteration: %d/%d", superstate.iteration_count, SS::total_iterations());

        float offset = 7;
        float angle = 0;
        if (superstate.direction() == TDirection::LEFT)
            angle = -90 - offset;
        else
            angle = 90 + offset;

        this->configure<OrNavigation, CbRotate>(angle);
        this->configure<OrLED, CbLEDOff>();
    }
};
} // namespace s_pattern_states
} // namespace sm_ridgeback_floor_coverage_static_1