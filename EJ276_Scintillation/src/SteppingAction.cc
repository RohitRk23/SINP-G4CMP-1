//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file optical/OpNovice2/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "HistoManager.hh"
#include "Run.hh"
#include "TrackInformation.hh"

#include "G4Cerenkov.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Scintillation.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4LogicalVolume.hh"
//To Store the Energy Deposit per event 
#include "EventAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
SteppingAction::SteppingAction()
  : G4UserSteppingAction()
  , fVerbose(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
SteppingAction::~SteppingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void SteppingAction::UserSteppingAction(const G4Step* step)
{
  static G4ParticleDefinition* opticalphoton =
    G4OpticalPhoton::OpticalPhotonDefinition();

  G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();
  Run* run =
    static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  G4Track* track          = step->GetTrack();
  G4StepPoint* endPoint   = step->GetPostStepPoint();
  G4StepPoint* startPoint = step->GetPreStepPoint();

// 1. Fetch the scoring volume pointer (only happens once per run)
    if (!fScoringVolume) {
        const DetectorConstruction* detConstruction = static_cast<const DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume = detConstruction->GetScoringVolume();
    }

    // 2. Get the logical volume where this step started
    G4LogicalVolume* currentVolume = startPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    // 3. Only proceed if the step occurred inside your crystal
    if (currentVolume == fScoringVolume) 
    {


        // Filter: Only look at the primary particle (Track ID == 1)
        if (track->GetTrackID() == 1) 
        {

            const G4VProcess* process = endPoint->GetProcessDefinedStep();
        
            if (process != nullptr) 
            {
                G4String processName = process->GetProcessName();
                  if (processName != "Transportation")

              {
                G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
                // G4int processTypeID = process->GetProcessType();
                // G4int subTypeID = process->GetProcessSubType();
            
                // G4int stepNumber = track->GetCurrentStepNumber();
                G4double kinEnergy = endPoint->GetKineticEnergy(); // Energy after step
                // G4double eDep = step->GetTotalEnergyDeposit();
                // G4cout << "Step #" << stepNumber 
                //        << " | Process: " << processName 
                //        << " | Type ID: " << processTypeID 
                //        << " | SubType ID: " << subTypeID
                //        << " | Neutron KinE: " << kinEnergy / MeV << " MeV" 
                //        << G4endl;
            
                analysisMan->FillNtupleIColumn(2, 0, eventID);
                // analysisMan->FillNtupleIColumn(2, 1, stepNumber);
                analysisMan->FillNtupleSColumn(2, 1, processName);
                // analysisMan->FillNtupleIColumn(2, 2, processTypeID);
                // analysisMan->FillNtupleIColumn(2, 2, subTypeID);
                analysisMan->FillNtupleDColumn(2, 3, kinEnergy);
                // analysisMan->FillNtupleDColumn(2, 3, eDep);
            
                analysisMan->AddNtupleRow(2);

              }
           }
        }
        // // 4. Extract any secondaries born during this specific step
        // const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();

        // if (secondaries != nullptr && !secondaries->empty()) 
        // {
        //     G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        //     G4int parentTrackID = track->GetTrackID(); // ID of the particle that caused the interaction

        //     // Loop through all the newly created particles
        //     for (size_t i = 0; i < secondaries->size(); ++i) 
        //     {
        //         const G4Track* secTrack = (*secondaries)[i];
        //         G4String secName = secTrack->GetDefinition()->GetParticleName();

        //         // OPTIONAL: Ignore optical photons to keep your ROOT file size manageable
        //         // if (secName == "opticalphoton") continue; 

        //         G4int secTrackID = secTrack->GetTrackID();
        //         G4double secKinE = secTrack->GetKineticEnergy();
                
        //         G4String creatorProcess = "";
        //         if (secTrack->GetCreatorProcess() != nullptr) {
        //             creatorProcess = secTrack->GetCreatorProcess()->GetProcessName();
        //         }

        //         // 5. Save to a new Ntuple (ID = 3)
        //         analysisMan->FillNtupleIColumn(3, 0, eventID);
        //         analysisMan->FillNtupleIColumn(3, 1, parentTrackID);
        //         analysisMan->FillNtupleIColumn(3, 2, secTrackID);
        //         analysisMan->FillNtupleSColumn(3, 3, secName);
        //         analysisMan->FillNtupleDColumn(3, 4, secKinE);
        //         analysisMan->FillNtupleSColumn(3, 5, creatorProcess);
                
        //         analysisMan->AddNtupleRow(3);
        //     }
        // }
      // }
      //------------To check the details of the optical photons only (within Crystal) ------------------//
      //  Extract any secondaries born during this specific step
        const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();

        if (secondaries != nullptr && !secondaries->empty()) 
        {
            G4int numPhotons = 0;
            G4String photonCreatorProcess = "Unknown";

            // Loop through all newly created particles in this step
            for (size_t i = 0; i < secondaries->size(); ++i) 
            {
                const G4Track* secTrack = (*secondaries)[i];
                
                // We ONLY care if the secondary is an optical photon
                if (secTrack->GetDefinition()->GetParticleName() == "opticalphoton") 
                {
                    numPhotons++;
                    
                    // We only need to grab the process name once, since all photons 
                    // born in this exact step will have the exact same creator process.
                    if (numPhotons == 1 && secTrack->GetCreatorProcess() != nullptr) {
                        photonCreatorProcess = secTrack->GetCreatorProcess()->GetProcessName();
                    }
                }
            }

            // 5. If this step generated optical photons, save the data!
            if (numPhotons > 0) 
            {
                // G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
                
                // track->GetTrackID() belongs to the particle currently taking the step.
                // Because this particle just created these photons, it is the Parent!
                // G4int parentTrackID = track->GetTrackID(); 
                G4String parentName = track->GetDefinition()->GetParticleName();
                // G4int parentPDG = track->GetDefinition()->GetPDGEncoding();
                // Save to Ntuple (ID = 3)
                // analysisMan->FillNtupleIColumn(3, 0, eventID);
                // analysisMan->FillNtupleIColumn(2, 1, parentTrackID);
                // analysisMan->FillNtupleSColumn(3, 0, photonCreatorProcess);
                // analysisMan->FillNtupleIColumn(3, 2, numPhotons); // Highly recommended to keep this!
                analysisMan->FillNtupleSColumn(3, 0, parentName);
                // analysisMan->FillNtupleIColumn(3, 1, parentPDG);  
                analysisMan->AddNtupleRow(3);
            }
        }
    } // End of Scoring Volume check
      //------------To check the details of the optical photons only (within Crystal) ------------------//
  //  }
  const G4DynamicParticle* theParticle = track->GetDynamicParticle();
  const G4ParticleDefinition* particleDef =
    theParticle->GetParticleDefinition();

  TrackInformation* trackInfo =
    (TrackInformation*) (track->GetUserInformation());

  if(particleDef == opticalphoton)
  {
    //------------optical photon in PMT------------------//
     G4String preVol  = "";
     G4String postVol = "";

    if(startPoint->GetPhysicalVolume())
        preVol = startPoint->GetPhysicalVolume()->GetName();

    if(endPoint->GetPhysicalVolume())
        postVol = endPoint->GetPhysicalVolume()->GetName();

    // Count when photon ENTERS PMT
    if(preVol != "PMT_PV" && postVol == "PMT_PV")
    {
      G4double photonEn = track->GetTotalEnergy();

      // --- Extract Timing Information ---
      G4double arrivalTime  = track->GetGlobalTime();  // Final time (when it hit PMT)
      G4double transitTime  = track->GetLocalTime();   // Time spent traveling (Final - Initial)

      auto analysisMan = G4AnalysisManager::Instance();
      if (photonEn>0)
      {
      // Fill Ntuple 1 (Photons)
      analysisMan->FillNtupleDColumn(1, 0,photonEn); // (NtupleID, ColumnID, Value)
      // --- Add times to the Ntuple ---
      analysisMan->FillNtupleDColumn(1, 1, arrivalTime); 
      // analysisMan->FillNtupleDColumn(1, 2, transitTime);

      analysisMan->AddNtupleRow(1);                   // Tell it to save a row in Ntuple 1
     }
      
      // Get the EventAction pointer
     EventAction* eventAction = (EventAction*)G4EventManager::GetEventManager()->GetUserEventAction();
    
        if(eventAction) {
          eventAction->AddPMTHit();
          eventAction->AddPMTEnergy(track->GetTotalEnergy());
          // eventAction->PMTphotonEnergy(track->GetTotalEnergy());
    }
         analysisMan->FillH1(25,1);   // histogram ID 25

        // avoid double counting
        track->SetTrackStatus(fStopAndKill);
    }
    
    //------------optical photon in PMT------------------//
    const G4VProcess* pds = endPoint->GetProcessDefinedStep();
    G4String procname     = pds->GetProcessName();
    if(procname.compare("OpAbsorption") == 0)
    {
      run->AddOpAbsorption();
      if(trackInfo->GetIsFirstTankX())
      {
        run->AddOpAbsorptionPrior();
      }
    }
    else if(procname.compare("OpRayleigh") == 0)
    {
      run->AddRayleigh();
    }
    else if(procname.compare("OpWLS") == 0)
    {
      G4double en = track->GetKineticEnergy();
      run->AddWLSAbsorption();
      run->AddWLSAbsorptionEnergy(en);
      analysisMan->FillH1(4, en / eV);  // absorption energy
      // loop over secondaries, create statistics
      // const std::vector<const G4Track*>* secondaries =
      auto secondaries = step->GetSecondaryInCurrentStep();
      for(auto sec : *secondaries)
      {
        en = sec->GetKineticEnergy();
        run->AddWLSEmission();
        run->AddWLSEmissionEnergy(en);
        analysisMan->FillH1(5, en / eV);  // emission energy
        G4double time = sec->GetGlobalTime();
        analysisMan->FillH1(6, time / ns);
      }
    }
    else if(procname.compare("OpWLS2") == 0)
    {
      G4double en = track->GetKineticEnergy();
      run->AddWLS2Absorption();
      run->AddWLS2AbsorptionEnergy(en);
      analysisMan->FillH1(7, en / eV);  // absorption energy
      // loop over secondaries, create statistics
      // const std::vector<const G4Track*>* secondaries =
      auto secondaries = step->GetSecondaryInCurrentStep();
      for(auto sec : *secondaries)
      {
        en = sec->GetKineticEnergy();
        run->AddWLS2Emission();
        run->AddWLS2EmissionEnergy(en);
        analysisMan->FillH1(8, en / eV);  // emission energy
        G4double time = sec->GetGlobalTime();
        analysisMan->FillH1(9, time / ns);
      }
    }

    // optical process has endpt on bdry,
    if(endPoint->GetStepStatus() == fGeomBoundary)
    {
      G4ThreeVector m0 = startPoint->GetMomentumDirection();
      G4ThreeVector m1 = endPoint->GetMomentumDirection();

      G4OpBoundaryProcessStatus theStatus = Undefined;

      G4ProcessManager* OpManager = opticalphoton->GetProcessManager();
      G4ProcessVector* postStepDoItVector =
        OpManager->GetPostStepProcessVector(typeDoIt);
      G4int n_proc = postStepDoItVector->entries();

      if(trackInfo->GetIsFirstTankX())
      {
        G4ThreeVector momdir = endPoint->GetMomentumDirection();
        G4double px1         = momdir.x();
        G4double py1         = momdir.y();
        G4double pz1         = momdir.z();
        if(px1 < 0.)
        {
          analysisMan->FillH1(11, px1);
          analysisMan->FillH1(12, py1);
          analysisMan->FillH1(13, pz1);
        }
        else
        {
          analysisMan->FillH1(14, px1);
          analysisMan->FillH1(15, py1);
          analysisMan->FillH1(16, pz1);
        }

        trackInfo->SetIsFirstTankX(false);
        run->AddTotalSurface();

        for(G4int i = 0; i < n_proc; ++i)
        {
          G4VProcess* currentProcess = (*postStepDoItVector)[i];

          G4OpBoundaryProcess* opProc =
            dynamic_cast<G4OpBoundaryProcess*>(currentProcess);
          if(opProc)
          {
            G4double angle = std::acos(startPoint->GetMomentumDirection().x());
            theStatus      = opProc->GetStatus();
            analysisMan->FillH1(10, theStatus);
            switch(theStatus)
            {
              case Transmission:
                run->AddTransmission();
                break;
              case FresnelRefraction:
                run->AddFresnelRefraction();
                analysisMan->FillH1(17, px1);
                analysisMan->FillH1(18, py1);
                analysisMan->FillH1(19, pz1);

                // transmission
                analysisMan->FillH1(20, angle / deg);
                break;
              case FresnelReflection:
                run->AddFresnelReflection();
                analysisMan->FillH1(21, angle / deg);
                break;
              case TotalInternalReflection:
                run->AddTotalInternalReflection();
                analysisMan->FillH1(21, angle / deg);
                break;
              case LambertianReflection:
                run->AddLambertianReflection();
                break;
              case LobeReflection:
                run->AddLobeReflection();
                break;
              case SpikeReflection:
                run->AddSpikeReflection();
                break;
              case BackScattering:
                run->AddBackScattering();
                break;
              case Absorption:
                run->AddAbsorption();
                break;
              case Detection:
                run->AddDetection();
                break;
              case NotAtBoundary:
                run->AddNotAtBoundary();
                break;
              case SameMaterial:
                run->AddSameMaterial();
                break;
              case StepTooSmall:
                run->AddStepTooSmall();
                break;
              case NoRINDEX:
                run->AddNoRINDEX();
                break;
              case PolishedLumirrorAirReflection:
                run->AddPolishedLumirrorAirReflection();
                break;
              case PolishedLumirrorGlueReflection:
                run->AddPolishedLumirrorGlueReflection();
                break;
              case PolishedAirReflection:
                run->AddPolishedAirReflection();
                break;
              case PolishedTeflonAirReflection:
                run->AddPolishedTeflonAirReflection();
                break;
              case PolishedTiOAirReflection:
                run->AddPolishedTiOAirReflection();
                break;
              case PolishedTyvekAirReflection:
                run->AddPolishedTyvekAirReflection();
                break;
              case PolishedVM2000AirReflection:
                run->AddPolishedVM2000AirReflection();
                break;
              case PolishedVM2000GlueReflection:
                run->AddPolishedVM2000AirReflection();
                break;
              case EtchedLumirrorAirReflection:
                run->AddEtchedLumirrorAirReflection();
                break;
              case EtchedLumirrorGlueReflection:
                run->AddEtchedLumirrorGlueReflection();
                break;
              case EtchedAirReflection:
                run->AddEtchedAirReflection();
                break;
              case EtchedTeflonAirReflection:
                run->AddEtchedTeflonAirReflection();
                break;
              case EtchedTiOAirReflection:
                run->AddEtchedTiOAirReflection();
                break;
              case EtchedTyvekAirReflection:
                run->AddEtchedTyvekAirReflection();
                break;
              case EtchedVM2000AirReflection:
                run->AddEtchedVM2000AirReflection();
                break;
              case EtchedVM2000GlueReflection:
                run->AddEtchedVM2000AirReflection();
                break;
              case GroundLumirrorAirReflection:
                run->AddGroundLumirrorAirReflection();
                break;
              case GroundLumirrorGlueReflection:
                run->AddGroundLumirrorGlueReflection();
                break;
              case GroundAirReflection:
                run->AddGroundAirReflection();
                break;
              case GroundTeflonAirReflection:
                run->AddGroundTeflonAirReflection();
                break;
              case GroundTiOAirReflection:
                run->AddGroundTiOAirReflection();
                break;
              case GroundTyvekAirReflection:
                run->AddGroundTyvekAirReflection();
                break;
              case GroundVM2000AirReflection:
                run->AddGroundVM2000AirReflection();
                break;
              case GroundVM2000GlueReflection:
                run->AddGroundVM2000AirReflection();
                break;
              case Dichroic:
                run->AddDichroic();
                break;
              default:
                G4cout << "theStatus: " << theStatus
                       << " was none of the above." << G4endl;
                break;
            }
          }
        }
      }
    }
  }

  else
  {  // particle != opticalphoton
    // print how many Cerenkov and scint photons produced this step
    // this demonstrates use of GetNumPhotons()
    auto proc_man =
      track->GetDynamicParticle()->GetParticleDefinition()->GetProcessManager();
    G4ProcessVector* proc_vec = proc_man->GetPostStepProcessVector(typeDoIt);
    G4int n_proc              = proc_vec->entries();

    G4int n_scint = 0;
    G4int n_cer   = 0;
    for(G4int i = 0; i < n_proc; ++i)
    {
      G4String proc_name = (*proc_vec)[i]->GetProcessName();
      if(proc_name.compare("Cerenkov") == 0)
      {
        auto cer = (G4Cerenkov*) (*proc_vec)[i];
        n_cer    = cer->GetNumPhotons();
      }
      else if(proc_name.compare("Scintillation") == 0)
      {
        auto scint = (G4Scintillation*) (*proc_vec)[i];
        n_scint    = scint->GetNumPhotons();
      }
    }
    if(fVerbose > 0)
    {
      if(n_cer > 0 || n_scint > 0)
      {
        G4cout << "In this step, " << n_cer << " Cerenkov and " << n_scint
               << " scintillation photons were produced." << G4endl;
      }
    }

    // loop over secondaries, create statistics
    const std::vector<const G4Track*>* secondaries =
      step->GetSecondaryInCurrentStep();

    for(auto sec : *secondaries)
    {
      if(sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton)
      {
        G4String creator_process = sec->GetCreatorProcess()->GetProcessName();
        if(creator_process.compare("Cerenkov") == 0)
        {
          G4double en = sec->GetKineticEnergy();
          run->AddCerenkovEnergy(en);
          run->AddCerenkov();
          analysisMan->FillH1(1, en / eV);
        }
        else if(creator_process.compare("Scintillation") == 0)
        {
          G4double en = sec->GetKineticEnergy();
          run->AddScintillationEnergy(en);
          run->AddScintillation();
          analysisMan->FillH1(2, en / eV);

          G4double time = sec->GetGlobalTime();
          analysisMan->FillH1(3, time / ns);
        }
      }
    }
  }

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
