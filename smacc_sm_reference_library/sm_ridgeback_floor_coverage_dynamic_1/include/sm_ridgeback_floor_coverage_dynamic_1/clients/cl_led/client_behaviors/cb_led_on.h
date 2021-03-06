#pragma once

#include <smacc/smacc.h>
#include <sm_ridgeback_floor_coverage_dynamic_1/clients/cl_led/cl_led.h>

namespace sm_ridgeback_floor_coverage_dynamic_1
{
namespace cl_led
{
class CbLEDOn : public smacc::SmaccClientBehavior
{
public:
  cl_led::ClLED *ledActionClient_;

  virtual void onEntry() override
  {
    this->requiresClient(ledActionClient_);

    cl_led::ClLED::Goal goal;
    goal.command = cl_led::ClLED::Goal::CMD_ON;
    ledActionClient_->sendGoal(goal);
  }

  virtual void onExit() override
  {
    //ROS_INFO("Entering ToolClientBehavior");
  }
};
} // namespace cl_led
} // namespace sm_ridgeback_floor_coverage_dynamic_1
