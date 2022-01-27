#include "state_machines/pick_and_place_sm.hpp"

namespace pick_place_sm
{

  // States definitions

  // define the machine states as classes inheriting from the base state machine class

  //   ___    _ _
  //  |_ _|__| | |___
  //   | |/ _` | / -_)
  //  |___\__,_|_\___|

  void Idle::entry()
  {
    std::cout << "[IDLE] waiting for manual start!\n";
  }

  void Idle::react(StartEvent const &e)
  {
    std::cout << "[IDLE] requested manual start. Transition to Scanning\n";
    transit<Scanning>();
  }

  //   ___           _
  //  | _ \__ _ _ _ (_)__
  //  |  _/ _` | ' \| / _|
  //  |_| \__,_|_||_|_\__|

  void Panic::RecoverFromError10(void)
  {
    std::cout << "[PANIC] Fixing error 10 ... FIXED!\n";
    transit<PreIdle>();
  }

  void Panic::react(FailEvent const &e)
  {
    std::cout << "[PANIC] Received a failure with code: " << e.error_code << ".\n";
    switch (e.error_code)
    {
    case 10:
      std::cout << "[PANIC] recoverable failure, trying recover..\n";
      RecoverFromError10();
      break;
    case 20:
    default:
      std::cout << "[PANIC] hard failure, forwarding error event\n";
      std::cout << "[PANIC] transition to PreIdle state\n";
      transit<PreIdle>();
      this->current_state_ptr->dispatch(e);
      break;
    }
  }

  //   ___         ___    _ _
  //  | _ \_ _ ___|_ _|__| | |___
  //  |  _/ '_/ -_)| |/ _` | / -_)
  //  |_| |_| \___|___\__,_|_\___|

  void PreIdle::entry()
  {
    std::cout << "[PRE-IDLE] entry, doing sanity check..\n";
    std::cout << "[PRE-IDLE] OK! moving to Idle\n";
    transit<Idle>();
  }

  void PreIdle::react(FailEvent const &e)
  {
    std::cout << "[PRE-IDLE] received hard failure event: " << e.error_code << "\n";
    error_list.push_back(e);
    std::cout << "[PRE-IDLE] list updated, keeping the state.\n";
  }

  void PreIdle::react(PlaceEvent const &e)
  {
    std::cout << "[PRE-IDLE] pick & place completed! Moving to Idle\n";
    transit<Idle>();
  }

  void PreIdle::react(StartEvent const &e)
  {
    std::cout << "[PRE-IDLE] received start request, integrity check..\n";

    if (error_list.empty())
    {
      std::cout << "[PRE-IDLE] everything looks good, transition to Idle and forwarding the start request.\n";
      transit<Idle>();
    }
    else
    {
      std::cout << "[PRE-IDLE] error list is non-empty! Fix all the errors before requesting start.\n";
      print_error_list();
    }
  }

  void PreIdle::print_error_list()
  {
    std::cout << "Error list:\n";
    for (auto e : error_list)
      std::cout << e.error_code << "\n";
  }

  //   ___                    _
  //  / __| __ __ _ _ _  _ _ (_)_ _  __ _
  //  \__ \/ _/ _` | ' \| ' \| | ' \/ _` |
  //  |___/\__\__,_|_||_|_||_|_|_||_\__, |
  //                                |___/

  void Scanning::react(ScanEvent const &e)
  {
    std::cout << "[SCANNING] received a ScanEvent, copying the message\n";

    std::cout << "[SCANNING] generating the planning request\n";
    std::cout << "[SCANNING] transition to Pick Planning\n";
    transit<PickPlanning>();
  }

  //   ___ _    _   ___ _                _
  //  | _ (_)__| |_| _ \ |__ _ _ _  _ _ (_)_ _  __ _
  //  |  _/ / _| / /  _/ / _` | ' \| ' \| | ' \/ _` |
  //  |_| |_\__|_\_\_| |_\__,_|_||_|_||_|_|_||_\__, |
  //                                           |___/

  void PickPlanning::react(PlanningEvent const &e)
  {
    std::cout << "[PICK PLANNING] received a planning request\n";
    // pick_request.scan_msg = e.scan_msg;
    pick_request.point_of_interest = "poi position";
    pick_request.pre_grasp = "pre grasp position";
    pick_request.post_grasp = "post grasp position";
    std::cout << "[PICK PLANNING] transition to Picking\n";
    transit<Picking>();
  }

  void PickPlanning::react(FailEvent const &e)
  {
    transit<Panic>();
  }

  //   ___ _    _   _
  //  | _ (_)__| |_(_)_ _  __ _
  //  |  _/ / _| / / | ' \/ _` |
  //  |_| |_\__|_\_\_|_||_\__, |
  //                      |___/

  void Picking::react(PickEvent const &e)
  {
    std::cout << "[PICKING] received a picking plan, executing it\n";
    std::cout << "[PICKING] pick operation complete, transition to place planning\n";
    transit<PlacePlanning>();
  }

  void Picking::react(ScanEvent const &e)
  {
    std::cout << "[PICKING] received a new scan event while picking, replanning\n";
    transit<PickPlanning>();
  }

  //   ___ _             ___ _                _
  //  | _ \ |__ _ __ ___| _ \ |__ _ _ _  _ _ (_)_ _  __ _
  //  |  _/ / _` / _/ -_)  _/ / _` | ' \| ' \| | ' \/ _` |
  //  |_| |_\__,_\__\___|_| |_\__,_|_||_|_||_|_|_||_\__, |
  //                                                |___/

  void PlacePlanning::react(PlanningEvent const &e)
  {
    std::cout << "[PLACE PLANNING] received a planning request\n";
    // place_request.scan_msg = e.scan_msg;
    place_request.point_of_interest = e.point_of_interest;
    place_request.occupancy_grid = e.occupancy_grid;
    place_request.pre_place = "pre placing position";
    place_request.post_place = "post placing position";
    std::cout << "[PLACE PLANNING] transition to Placing\n";
    transit<Placing>();
  }

  //   ___ _         _
  //  | _ \ |__ _ __(_)_ _  __ _
  //  |  _/ / _` / _| | ' \/ _` |
  //  |_| |_\__,_\__|_|_||_\__, |
  //                       |___/

  void Placing::react(PlaceEvent const &e)
  {
    std::cout << "[PLACING] received a placing plan, executing place operation\n";
    std::cout << "[PLACING] place operation complete, going to PreIdle and sending a Completion event\n";
    transit<PreIdle>();
  }

  void Placing::react(ScanEvent const &e)
  {
    std::cout << "[PLACING] received a new scan event while placing, replanning\n";
    transit<PlacePlanning>();
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
  void PickPlaceSM::react(CompletionEvent const &)
  {
    std::cout << "Ignored completion event\n";
  }

  // TO BE REMOVED!
  // This function enforces the transit to panic just to test it
  // it is wrong to perform other operations after the transit to another state
  void Scanning::react(FailEvent const &e)
  {
    transit<Panic>();
    this->current_state_ptr->dispatch(e);
  }

} // namespace pick_place_sm

FSM_INITIAL_STATE(pick_place_sm::PickPlaceSM, pick_place_sm::PreIdle)