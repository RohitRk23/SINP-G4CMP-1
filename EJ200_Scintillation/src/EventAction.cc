#include "EventAction.hh"
#include "DMXPmtHit.hh" //
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "g4analysis.hh" 
#include <iostream>
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include <cstdlib>

// EventAction::EventAction()
//   : G4UserEventAction(), fPMTHCID(-1)
// {}

// EventAction::~EventAction()
// {}
//--------------------FOR ENERGY DEPOSIT AND COUNTS PER EVENT------------
void EventAction::BeginOfEventAction(const G4Event* event) {
    // Reset counters at the start of every event
    fHitCount = 0;
    fTotalEnergy = 0.0;
    // fPhotonEnergies.clear(); // Clear the list for the new event
}

void EventAction::EndOfEventAction(const G4Event* event) {
    auto analysisMan = G4AnalysisManager::Instance();

      // Fill Histogram 22 with the number of hits in THIS event
    analysisMan->FillH1(22, fHitCount);
    // analysisMan->FillH1(24, fInitialEnergy); //For Initial Energy 

    // Fill another histogram (e.g., ID 23) with total energy per event
    if (fHitCount > 0) {
      // Fill Histogram 22 with the number of hits in THIS event
        // analysisMan->FillH1(22, fHitCount);
        analysisMan->FillH1(23, fTotalEnergy); 
    }
    // Fill the columns in the exact order defined in RunAction
    G4int localEventID = event->GetEventID();

    // Read the pre-calculated offset from the bash environment
     G4int offset = 0; 
      if (const char* env_p = std::getenv("EVENT_OFFSET")) {
        offset = std::stoi(env_p);
      }
     G4int globalEventID = localEventID + offset;

    //  Get the Run ID that you injected via sed
    // G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();

    // Calculate the continuous Global Event ID
    // If iteration starts at 0: globalEventID = (runID * eventsPerRun) + localEventID
    // If iteration starts at 1: globalEventID = ((runID - 1) * eventsPerRun) + localEventID
    // G4int globalEventID = (runID * 100) + localEventID;


    analysisMan->FillNtupleIColumn(0,0, globalEventID);
    // analysisMan->FillNtupleIColumn(0,0, event->GetEventID());
    analysisMan->FillNtupleIColumn(0,1, fHitCount);
    analysisMan->FillNtupleDColumn(0,2, fTotalEnergy);
    // analysisMan->FillNtupleDColumn(3, feachphotonEnergy);
    analysisMan->FillNtupleDColumn(0,3, fInitialEnergy); // From SetInitialEnergy call

    // Finalize the row
    analysisMan->AddNtupleRow(0);
    
}







// void EventAction::EndOfEventAction(const G4Event* event)
// {
//   // 1. Get the Hits Collections for this event
//   G4HCofThisEvent* hce = event->GetHCofThisEvent();
//   if (!hce) return;

//   // 2. Find the ID for the pmtCollection if we haven't yet
//   if (fPMTHCID == -1) {
//     fPMTHCID = G4SDManager::GetSDMpointer()->GetCollectionID("pmtCollection");
//   }

//   // 3. Cast the collection to the correct type
//   auto pmtHC = static_cast<DMXPmtHitsCollection*>(hce->GetHC(fPMTHCID));

//   if (pmtHC) {
//     auto analysisManager = G4AnalysisManager::Instance();
//     G4int n_hit = pmtHC->entries();

//     for (G4int i = 0; i < n_hit; i++) {
//       DMXPmtHit* hit = (*pmtHC)[i]; //
      
//       // Fill Ntuple columns (Time and Position)
//       analysisManager->FillNtupleDColumn(0, hit->GetTime());
//       analysisManager->FillNtupleDColumn(1, hit->GetPos().x());
//       analysisManager->FillNtupleDColumn(2, hit->GetPos().y());
//       analysisManager->FillNtupleDColumn(3, hit->GetPos().z());
//       analysisManager->FillNtupleDColumn(4, hit->GetPID());
//       analysisManager->AddNtupleRow();
//       // std::cout<<"Filled Ntuple for hit " << i << ": Time=" << hit->GetTime() 
//       //          << ", Pos=(" << hit->GetPos().x() << ", " << hit->GetPos().y() << ", " << hit->GetPos().z() 
//       //          << "), PID=" << hit->GetPID() << std::endl;
//     }
//   }
// }