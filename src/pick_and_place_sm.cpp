#include "state_machines/pick_and_place_sm.hpp"

namespace pick_place_sm
{

  // States definitions

  // define the machine states as classes inheriting from the base state machine class


  //   ___           _
  //  | _ \__ _ _ _ (_)__
  //  |  _/ _` | ' \| / _|
  //  |_| \__,_|_||_|_\__|

  void Panic::entry()
  {
    std::cout << "[PANIC] stacca stacca!\n";
  }

  void Panic::react(StartEvent const &e)
  {
    std::cout << "[PANIC] all stopped! Transition to PreIdle\n";
    transit<PreIdle>();
  }

  void Panic::react(RecoverEvent const &e)
  {
    std::cout << "[PANIC] all errors fixed! Recovering previous state..";
    // transit to prev state!
  }

  //   ___         ___    _ _
  //  | _ \_ _ ___|_ _|__| | |___
  //  |  _/ '_/ -_)| |/ _` | / -_)
  //  |_| |_| \___|___\__,_|_\___|

  void PreIdle::entry()
  {
    std::cout << "[PRE-IDLE] sanity check..\n";
  }

  void PreIdle::react(PlaceEvent const &e)
  {
    std::cout << "[PRE-IDLE] pick&place completed! Transition to Idle\n";
    transit<Idle>();
  }

  void PreIdle::react(StartEvent const &e)
  {
    std::cout << "[PRE-IDLE] preliminary procedure completed successfully, transition to Idle\n";
    transit<Idle>();
  }


  //   ___    _ _
  //  |_ _|__| | |___
  //   | |/ _` | / -_)
  //  |___\__,_|_\___|

  void Idle::entry()
  {
    std::cout << "[IDLE] init procedure and waiting manual start!\n";
  }

  void Idle::react(StartEvent const &e)
  {
    std::cout << "[IDLE] requested manual start. Transition to Scanning\n";
    transit<Scanning>();
  }

  
  //   ___                    _
  //  / __| __ __ _ _ _  _ _ (_)_ _  __ _
  //  \__ \/ _/ _` | ' \| ' \| | ' \/ _` |
  //  |___/\__\__,_|_||_|_||_|_|_||_\__, |
  //                                |___/

  void Scanning::react(ScanEvent const &e)
  {
    std::cout << "[SCANNING] received a ScanEvent, transition to Pick Planning\n";
    transit<PickPlanning>();
  }

  void Scanning::react(FailEvent const &e)
  {
    std::cout << "[SCANNING] scan failure! Transition to Panic\n";
    transit<Panic>();
  }

  //   ___ _    _   ___ _                _
  //  | _ (_)__| |_| _ \ |__ _ _ _  _ _ (_)_ _  __ _
  //  |  _/ / _| / /  _/ / _` | ' \| ' \| | ' \/ _` |
  //  |_| |_\__|_\_\_| |_\__,_|_||_|_||_|_|_||_\__, |
  //                                           |___/

  void PickPlanning::react(PlanningEvent const &e)
  {
    std::cout << "[PICK PLANNING] received a planning request, transition to Picking\n";
    transit<Picking>();
  }

  void PickPlanning::react(FailEvent const &e)
  {
    std::cout << "[PICK PLANNING] plan not found! Transition to Panic\n";
    transit<Panic>();
  }

  //   ___ _    _   _
  //  | _ (_)__| |_(_)_ _  __ _
  //  |  _/ / _| / / | ' \/ _` |
  //  |_| |_\__|_\_\_|_||_\__, |
  //                      |___/

  void Picking::react(PickEvent const &e)
  {
    std::cout << "[PICKING] pick successful, transition to Place Planning\n";
    transit<PlacePlanning>();
  }

  void Picking::react(ScanEvent const &e)
  {
    std::cout << "[PICKING] received a new scan event while picking, replanning\n";
    transit<PickPlanning>();
  }

  void Picking::react(FailEvent const &e)
  {
    std::cout << "[PICKING] failed while picking! Transition to Panic\n";
    transit<Panic>();
  }

  //   ___ _             ___ _                _
  //  | _ \ |__ _ __ ___| _ \ |__ _ _ _  _ _ (_)_ _  __ _
  //  |  _/ / _` / _/ -_)  _/ / _` | ' \| ' \| | ' \/ _` |
  //  |_| |_\__,_\__\___|_| |_\__,_|_||_|_||_|_|_||_\__, |
  //                                                |___/

  void PlacePlanning::react(PlanningEvent const &e)
  {
    std::cout << "[PLACE PLANNING] received a planning request, transition to Placing\n";
    transit<Placing>();
  }

  void PlacePlanning::react(FailEvent const &e)
  {
    std::cout << "[PLACE PLANNING] plan not found! Transition to Panic\n";
    transit<Panic>();
  }

  //   ___ _         _
  //  | _ \ |__ _ __(_)_ _  __ _
  //  |  _/ / _` / _| | ' \/ _` |
  //  |_| |_\__,_\__|_|_||_\__, |
  //                       |___/

  void Placing::react(PlaceEvent const &e)
  {
    std::cout << "[PLACING] place successful, transition to PreIdle and sending a Completion event\n";
    transit<PreIdle>();
  }

  void Placing::react(ScanEvent const &e)
  {
    std::cout << "[PLACING] received a new scan event while placing, replanning\n";
    transit<PlacePlanning>();
  }

  void Placing::react(FailEvent const &e)
  {
    std::cout << "[PLACING] placing operation failed! Transition to Panic\n";
    transit<Panic>();
  }



  // Default behavior for base class

  void PickPlaceSM::react(StartEvent const &)
  {
    std::cout << "Ignored start event\n";
  }
  void PickPlaceSM::react(ScanEvent const &)
  {
    std::cout << "Ignored scan event\n";
  }
  void PickPlaceSM::react(PlanningEvent const &)
  {
    std::cout << "Ignored planning event\n";
  }
  void PickPlaceSM::react(PickEvent const &)
  {
    std::cout << "Ignored pick event\n";
  }
  void PickPlaceSM::react(PlaceEvent const &)
  {
    std::cout << "Ignored place event\n";
  }
  void PickPlaceSM::react(FailEvent const &)
  {
    std::cout << "Ignored fail event\n";
  }
  void PickPlaceSM::react(RecoverEvent const &)
  {
    std::cout << "Ignored recover event\n";
  }

} // namespace pick_place_sm

FSM_INITIAL_STATE(pick_place_sm::PickPlaceSM, pick_place_sm::PreIdle)