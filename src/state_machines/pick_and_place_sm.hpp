#ifndef PICK_PLACE_SM_HPP
#define PICK_PLACE_SM_HPP

#include <iostream>
#include <string>
#include <vector>
#include "tinyfsm.hpp"

using namespace std;

namespace pick_place_sm
{

  //   ___             _
  //  | __|_ _____ _ _| |_ ___
  //  | _|\ V / -_) ' \  _(_-<
  //  |___|\_/\___|_||_\__/__/

  // Event declarations
  // declare the events triggering the state changes
  // as structures inheriting from tinyfsm::Event

  enum EventTypes
  {
    NOTSET = 0,
    START = 1,
    SCANNING = 2,
    PLANNING = 3,
    PICK = 4,
    PLACE = 5,
    FAIL = 6,
    RECOVER = 7
  };

  struct BaseEvent : tinyfsm::Event
  {
    int type;
    BaseEvent() : type(NOTSET){};
  };

  // the lidar sensor outputs a scan event only when an object is detected in its field of view
  struct ScanEvent : BaseEvent
  {
    ScanEvent()
    {
      type = SCANNING;
    };
  };

  // a scan event ALWAYS triggers a planning event
  // so the latter inherits from the scan event
  struct PlanningEvent : BaseEvent
  {
    PlanningEvent()
    {
      type = PLANNING;
    };
  };

  // a pick event is a planning event but it adds information on the pre and post grasp operations. Pre-grasp informs the robot to keep a particular pose at a particular distance from the object to pick; post-grasp informs the robot to retreat the object in a particular way before any other motion
  struct PickEvent : PlanningEvent
  {
    PickEvent()
    {
      type = PICK;
    };
  };

  // very similar to the pick event; the pre-place approach instructs the robot to stop at a precise pose and position near the final one; the post-place tells the robot how to retire from the placing point after the object placing
  struct PlaceEvent : PlanningEvent
  {
    PlaceEvent()
    {
      type = PLACE;
    };
  };

  struct FailEvent : BaseEvent
  {
    FailEvent()
    {
      type = FAIL;
    };
  };

  struct StartEvent : BaseEvent
  {
    StartEvent()
    {
      type = START;
    };
  };

  struct RecoverEvent : BaseEvent
  {
    RecoverEvent()
    {
      type = RECOVER;
    };
  };

  //   ___ _        _         __  __         _    _
  //  / __| |_ __ _| |_ ___  |  \/  |__ _ __| |_ (_)_ _  ___
  //  \__ \  _/ _` |  _/ -_) | |\/| / _` / _| ' \| | ' \/ -_)
  //  |___/\__\__,_|\__\___| |_|  |_\__,_\__|_||_|_|_||_\___|

  // State machine declaration

  // declare the base state machine class, which inherits from tinyfsm::Fsm and overrides the method to react to events (react) and the methods to manage state changes (entry, exit)

  class PickPlaceSM : public tinyfsm::Fsm<PickPlaceSM>
  {
  public:
    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &){};
    void react(BaseEvent const &){};

    virtual void react(StartEvent const &);
    virtual void react(ScanEvent const &);
    virtual void react(PlanningEvent const &);
    virtual void react(PickEvent const &);
    virtual void react(PlaceEvent const &);
    virtual void react(FailEvent const &);
    virtual void react(RecoverEvent const &);

    virtual void entry(void){}; /* entry actions in some states */
    void exit(void){};          /* no exit actions at all */

  protected:
  };

  //   ___ _        _
  //  / __| |_ __ _| |_ ___ ___
  //  \__ \  _/ _` |  _/ -_|_-<
  //  |___/\__\__,_|\__\___/__/

  // State declarations

  class PreIdle : public PickPlaceSM
  {
    void entry() override;
    void react(PlaceEvent const &) override;
    void react(StartEvent const &) override;
  };

  class Idle : public PickPlaceSM
  {
    void entry() override;
    void react(StartEvent const &) override;
  };

  class Scanning : public PickPlaceSM
  {
    void react(ScanEvent const &) override;
    void react(FailEvent const &) override;
  };

  class PickPlanning : public PickPlaceSM
  {
    void react(PlanningEvent const &) override;
    void react(FailEvent const &) override;
  };

  class Picking : public PickPlaceSM
  {
    void react(PickEvent const &) override;
    void react(ScanEvent const &) override;
    void react(FailEvent const &) override;
  };

  class PlacePlanning : public PickPlaceSM
  {
    void react(PlanningEvent const &) override;
    void react(FailEvent const &) override;
  };

  class Placing : public PickPlaceSM
  {
    void react(PlaceEvent const &) override;
    void react(ScanEvent const &) override;
    void react(FailEvent const &) override;
  };

  class Panic : public PickPlaceSM
  {
    void entry() override;
    void react(StartEvent const &) override;
    void react(RecoverEvent const &) override;
  };

} // namespace pick_place_sm

#endif