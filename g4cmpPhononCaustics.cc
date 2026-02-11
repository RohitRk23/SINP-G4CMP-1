/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file Phonon_Caustics/g4cmpPhononCaustics.cc
/// \brief Main program of the G4CMP/Phonon_Caustics example
//
// $Id$
//
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

#include "G4CMPPhysicsList.hh"
#include "G4CMPConfigManager.hh"
#include "Caustic_PhononActionInitialization.hh"
#include "Caustic_PhononConfigManager.hh"
#include "Caustic_PhononDetectorConstruction.hh"

#include "G4LatticePhysical.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BIC_HP.hh"
#include "G4CMPPhysics.hh"

int main(int argc,char** argv)
{
//--------checkers-----------
//std::cout<<G4LatticeLogical::GetBasis(0);
/*
G4ThreeVector norm= (0*GetBasis(0)+1*GetBasis(1)+0*GetBasis(2)).unit();

fOrient = G4RotationMatrix::IDENTITY;
fOrient.rotateZ(rot).rotateY(norm.theta()).rotateZ(norm.phi());
fInverse = fOrient.inverse();
*/
//----------------------------------
  // Construct the run manager
  //
  G4RunManager * runManager = G4RunManagerFactory::CreateRunManager();

  

  // Set mandatory initialization classes
  //
  Caustic_PhononDetectorConstruction* detector = new Caustic_PhononDetectorConstruction();
  runManager->SetUserInitialization(detector);
  
  G4VUserPhysicsList* physics = new G4CMPPhysicsList();
  physics->SetCuts();
  runManager->SetUserInitialization(physics);

  FTFP_BERT* physics2 = new FTFP_BERT;
  physics2->RegisterPhysics(new G4CMPPhysics);
  physics2->SetCuts();
  runManager->SetUserInitialization(physics2);

  QGSP_BIC_HP* physics3 = new QGSP_BIC_HP;
  physics3->RegisterPhysics(new G4CMPPhysics);
  physics3->SetCuts();
  runManager->SetUserInitialization(physics3);

  // // Use a Modular Physics List (FTFP_BERT is one)
  // G4VModularPhysicsList* physicsList = new FTFP_BERT();

  // // Register the G4CMP Physics modules into the main list
  // physicsList->RegisterPhysics(new G4CMPPhysics());
  
  // IMPORTANT: You also need the Lattice Physics for propagation
  //physicsList->RegisterPhysics(new G4CMPLatticePhysics());

  //runManager->SetUserInitialization(physicsList);
  // Set user action classes (different for Geant4 10.0)
  //
  runManager->SetUserInitialization(new Caustic_PhononActionInitialization);
  
  // Create configuration managers to ensure macro commands exist
  G4CMPConfigManager::Instance();
  Caustic_PhononConfigManager::Instance();
  
  // Visualization manager
  //
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  


  // This Part is to run the Macros
  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if (argc==1) {   // Define UI session for interactive mode
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
    ui->SessionStart();
    delete ui;
  } else {           // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }

  delete visManager;
  delete runManager;
  //std::cout<<"The file is finish"<<std::endl;
  return 0;
  //std::cout<<"The file is finish"<<std::endl;
  
}
