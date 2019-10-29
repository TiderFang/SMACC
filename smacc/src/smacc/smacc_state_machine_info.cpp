#include <smacc/smacc_state_machine_info.h>
#include <smacc/smacc_state_info.h>
#include <smacc/smacc_state_machine.h>

namespace smacc
{
void SmaccStateMachineInfo::printAllStates(ISmaccStateMachine *sm)
{
    ROS_INFO("----------- PRINT ALL STATES -------------------");
    stateMsgs.clear();
    for (auto &val : this->states)
    {
        smacc_msgs::SmaccState stateMsg;
        stateMsg.index = stateMsgs.size();
        auto state = val.second;

        std::stringstream ss;
        ss << "**** State: " << demangleSymbol(val.first.c_str()) << std::endl;

        stateMsg.name = state->demangledStateName;
        stateMsg.level = (int)state->getStateLevel();

        ss << "**** State: " << stateMsg.name << std::endl;
        ss << "StateLevel: " << stateMsg.level << std::endl;

        ss << " Childstates:" << std::endl;

        for (auto &child : state->children_)
        {
            auto childStateName = child->demangledStateName;
            stateMsg.children_states.push_back(childStateName);

            ss << " - " << childStateName << std::endl;
        }

        ss << " Transitions:" << std::endl;

        for (auto &transition : state->transitions_)
        {
            smacc_msgs::SmaccTransition transitionMsg;

            transitionMsg.index = transition.index;
            transitionMsg.event.event_type = transition.eventInfo->getEventTypeName();;
            transitionMsg.destiny_state_name = transition.destinyState->demangledStateName;

            transitionMsg.transition_tag = transition.transitionTag;
            transitionMsg.event.event_source = transition.eventInfo->getEventSourceName();
            transitionMsg.event.event_object_tag = transition.eventInfo->getObjectTagName();
            transitionMsg.event.label = transition.eventInfo->label;

            ss << " - Transition.  " << std::endl;
            ss << "      - Index: " << transitionMsg.index << std::endl;
            ss << "      - Event Type :" << transitionMsg.event.event_type << std::endl;
            ss << "      - Event Source: " << transitionMsg.event.event_source << std::endl;
            ss << "      - Event ObjectTag: " << transitionMsg.event.event_object_tag << std::endl;
            ss << "      - Event Label: " << transitionMsg.event.label << std::endl;
            ss << "      - Destiny State: " << transitionMsg.destiny_state_name << std::endl;
            ss << "      - Transition Tag: " << transitionMsg.transition_tag << std::endl;
            ss << "      - Owner State: " << transitionMsg.destiny_state_name << std::endl;

            stateMsg.transitions.push_back(transitionMsg);
        }

        const std::type_info *statetid = state->tid_;

        std::map<const std::type_info *, std::vector<smacc::StateBehaviorInfoEntry *>> smaccBehaviorInfoMappingByOrthogonalType;

        ss << " Orthogonals:" << std::endl;
        if (SmaccStateInfo::staticBehaviorInfo.count(statetid) > 0)
        {
            for (auto &bhinfo : SmaccStateInfo::staticBehaviorInfo[statetid])
            {
                if (smaccBehaviorInfoMappingByOrthogonalType.count(bhinfo.orthogonalType) == 0)
                {
                    smaccBehaviorInfoMappingByOrthogonalType[bhinfo.orthogonalType] = std::vector<smacc::StateBehaviorInfoEntry *>();
                }

                smaccBehaviorInfoMappingByOrthogonalType[bhinfo.orthogonalType].push_back(&bhinfo);
            }
        }

        auto &runtimeOrthogonals = sm->getOrthogonals();

        for (auto &orthogonal : runtimeOrthogonals)
        {
            smacc_msgs::SmaccOrthogonal orthogonalMsg;

            const auto *orthogonaltid = &typeid(*(orthogonal.second));
            orthogonalMsg.name = demangleSymbol(orthogonaltid->name());

            ss << " - orthogonal: " << orthogonalMsg.name << std::endl;

            if (smaccBehaviorInfoMappingByOrthogonalType[orthogonaltid].size() > 0)
            {
                auto &behaviors = smaccBehaviorInfoMappingByOrthogonalType[orthogonaltid];
                for (auto &bhinfo : behaviors)
                {
                    auto substateBehaviorName = demangleSymbol(bhinfo->behaviorType->name());
                    orthogonalMsg.substate_behavior_names.push_back(substateBehaviorName);
                    ss << "          - substate behavior: " << substateBehaviorName << std::endl;
                }
            }
            else
            {
                ss << "          - NO SUBSTATE BEHAVIORS -" << std::endl;
            }

            auto &clients = orthogonal.second->getClients();
            if (clients.size() > 0)
            {
                for (auto *client : clients)
                {
                    auto clientTid = &(typeid(*client));
                    auto clientName = demangleSymbol(clientTid->name());
                    orthogonalMsg.client_names.push_back(clientName);
                    ss << "          - client: " << clientName << std::endl;
                }
            }
            else
            {
                ss << "          - NO CLIENTS - " << std::endl;
            }
            stateMsg.orthogonals.push_back(orthogonalMsg);
        }

        ss << " Logic units:" << std::endl;
        if (SmaccStateInfo::logicUnitsInfo.count(statetid) > 0)
        {
            int k = 0;
            for (auto &luinfo : SmaccStateInfo::logicUnitsInfo[statetid])
            {
                smacc_msgs::SmaccLogicUnit logicUnitMsg;
                logicUnitMsg.index = k++;
                logicUnitMsg.type_name = demangleSymbol(luinfo.logicUnitType->name());

                ss << " - logic unit: " << logicUnitMsg.type_name << std::endl;
                if (luinfo.objectTagType != nullptr)
                {
                    logicUnitMsg.object_tag = luinfo.objectTagType->finaltype;
                    ss << "        - object tag: " << logicUnitMsg.object_tag << std::endl;
                }

                for (auto &tev : luinfo.sourceEventTypes)
                {
                    // WE SHOULD CREATE A SMACC_EVENT_INFO TYPE, also using in typewalker transition
                    auto eventTypeName = tev->getEventTypeName();
                    smacc_msgs::SmaccEvent event;

                    ss << "             - triggering event: " << tev->getEventTypeName() << std::endl;
                    event.event_type = eventTypeName;

                    event.event_source = tev->getEventSourceName();
                    ss << "                 - source type: " << event.event_source << std::endl;

                    event.event_object_tag = tev->getObjectTagName();
                    ss << "                 - source object: " << event.event_object_tag << std::endl;
                    
                    event.label  = tev->label;
                    ss << "                 - event label: " << event.label << std::endl;

                    logicUnitMsg.event_sources.push_back(event);
                }

                stateMsg.logic_units.push_back(logicUnitMsg);
            }
        }
        else
        {
            ss << "- NO LOGIC UNITS - " << std::endl;
        }

        ROS_INFO_STREAM(ss.str());
        stateMsgs.push_back(stateMsg);
    }
}
} // namespace smacc