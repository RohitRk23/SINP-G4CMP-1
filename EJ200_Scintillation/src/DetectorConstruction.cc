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
/// \file optical/OpNovice2/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// This is to check pc push
#include "DetectorConstruction.hh"

#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
//-------------For PMT------------//
#include "DMXPmtSD.hh"
#include "G4SDManager.hh"
//-------------For PMT------------//

#include <iostream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction()
  , fDetectorMessenger(nullptr)
{
  fExpHall_x = fExpHall_y = fExpHall_z = 10.0 * m;
  fTank_x = fTank_y = fTank_z = 1.0 * m;

  fTank = nullptr;

  fTankMPT    = new G4MaterialPropertiesTable();
  fWorldMPT   = new G4MaterialPropertiesTable();
  fSurfaceMPT = new G4MaterialPropertiesTable();
  // fMirrorSurfaceMPT = new G4MaterialPropertiesTable(); // For Reflecting surface

  fSurface = new G4OpticalSurface("Surface");
  fSurface->SetType(dielectric_dielectric);
  fSurface->SetFinish(ground);
  fSurface->SetModel(unified);
  fSurface->SetSigmaAlpha(0.2);
  // fSurface->SetMaterialPropertiesTable(fSurfaceMPT);

  // // For Reflecting Surface
  // fMirrorSurface = new G4OpticalSurface("MirrorSurface");
  // fMirrorSurface->SetType(dielectric_metal);
  // fMirrorSurface->SetFinish(polished);
  // fMirrorSurface->SetModel(unified);
  // fMirrorSurface->SetMaterialPropertiesTable(fMirrorSurfaceMPT);

  fTank_LV  = nullptr;
  fWorld_LV = nullptr;

  // fTankMaterial  = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  // fTankMaterial  = G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO");
  // fWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  fCopyWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"); // For Reflecting Surface
  fWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  //For Crystal
  //
  G4NistManager* nistManager = G4NistManager::Instance();
 // Define elements
    G4Element* H = nistManager->FindOrBuildElement("H");
    G4Element* C = nistManager->FindOrBuildElement("C");

    // EJ-200 plastic scintillator
    // Chemical composition: (C_9 H_10)_n  — polyvinyltoluene based
    // Density: 1.023 g/cm³
    fTankMaterial = new G4Material("EJ200", 1.023*g/cm3, 2);
    fTankMaterial->AddElement(H, 8.5*perCent);   // by mass fraction
    fTankMaterial->AddElement(C, 91.5*perCent);
  // fWorldMaterial = nistManager->FindOrBuildMaterial("G4_AIR");

  // G4Element *elCa = nistManager->FindOrBuildElement("Ca");
  // G4Element *elW = nistManager->FindOrBuildElement("W");
  // G4Element *elO = nistManager->FindOrBuildElement("O");
  // G4double density = 6.1*g/cm3;
  // fTankMaterial = new G4Material("CalciumTungstate", density, 3);
  // fTankMaterial->AddElement(elCa, 1);
  // fTankMaterial->AddElement(elW, 1);
  // fTankMaterial->AddElement(elO, 4);

  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
  delete fTankMPT;
  delete fWorldMPT;
  delete fSurfaceMPT;
  // delete fMirrorSurfaceMPT;
  delete fSurface;
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
  fTankMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

  fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);

  // ------------- Volumes --------------
  // The experimental Hall

// Additional Volume for making the world refelcting
    //-----------------------------------------------------------------//
    G4Box* copy_world_box = new G4Box("CopyWorld", 8.01*cm,8.01*cm,8.01*cm);

   fCopyWorld_LV = new G4LogicalVolume(copy_world_box, fCopyWorldMaterial, "CopyWorld", 0, 0, 0);

  G4VPhysicalVolume* Copyworld_PV =
    new G4PVPlacement(0, G4ThreeVector(), fCopyWorld_LV, "CopyWorld", 0, false, 0);
    //-----------------------------------------------------------------//

  // G4Box* world_box = new G4Box("World", fExpHall_x, fExpHall_y, fExpHall_z);
  G4Box* world_box = new G4Box("World", 8.*cm, 8.*cm, 8.*cm);

  fWorld_LV = new G4LogicalVolume(world_box, fWorldMaterial, "World", 0, 0, 0);

  G4VPhysicalVolume* world_PV =
    new G4PVPlacement(0, G4ThreeVector(), fWorld_LV, "World", fCopyWorld_LV, false, 0);

  
  // // The tank
  // G4Box* tank_box = new G4Box("Tank", fTank_x, fTank_y, fTank_z);
  G4VSolid* tank_box = new G4Tubs("Tank", 0.*cm, 2.0*cm, 2.0*cm, 0.*deg, 360.*deg);

  fTank_LV = new G4LogicalVolume(tank_box, fTankMaterial, "Tank", 0, 0, 0);
  fScoringVolume = fTank_LV; // To make the scoring volume accessible to the SteppingAction class

  fTank = new G4PVPlacement(0, G4ThreeVector(), fTank_LV, "Tank", fWorld_LV, false, 0);

//----------------------------------PMT Addition Part-----------------------------------------//
  //For PMT
  // 1. Define the PMT Material (Quartz or Glass is common)
  G4Material* pmt_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_GLASS_PLATE");

  // 2. Create the PMT geometry (e.g., a thin disk on the wall)
  G4double pmt_radius = 2.0*cm;
  G4double pmt_thickness = 1.*mm;
  G4Tubs* pmt_solid = new G4Tubs("PMT_Solid", 0.*cm, pmt_radius, pmt_thickness, 0.*deg, 360.*deg);

  G4LogicalVolume* pmt_log = new G4LogicalVolume(pmt_solid, pmt_mat, "PMT_LV");

  // 3. Place the PMT on the inside surface of the World box (at 10m)
  // Note: World is 6cm half-width, so we place at 5.90cm
  G4RotationMatrix* rotY = new G4RotationMatrix();
  rotY->rotateX(90.*deg);
  // G4VPhysicalVolume* pmt_phys = new G4PVPlacement(rotY, G4ThreeVector(0, 4.0*cm, 0), 
  //                                   pmt_log, "PMT_PV", fWorld_LV, false, 0);
    G4VPhysicalVolume* pmt_phys = new G4PVPlacement(0, G4ThreeVector(0,0, 4.0*cm), 
                                       pmt_log, "PMT_PV", fWorld_LV, false, 0);
// ----------------------------------PMT Addition Part-----------------------------------------//
  //Crystal Volume
  //G4_AIR
//   G4VSolid* tank_box = new G4Tubs("Tank", 0.*m, 0.5*m, 0.5*m, 0.*deg, 360.*deg);
//    fTank_LV =
//     new G4LogicalVolume(tank_box,fTankMaterial,"Tank",0,0,0);
// //    new G4LogicalVolume(SubstrateSolid,fCrystalMaterial,"SubstrateLogical1");
//   fTank =
//     new G4PVPlacement(0,G4ThreeVector(),fTank_LV,"Tank",
//                       fWorld_LV,false,0);


// ------------ Generate & Add Material Properties Table for BGO ------------
  //
  std::vector<G4double> photonEnergy = {
    2.4848*eV, 2.5098*eV, 2.5355*eV, 2.5616*eV, 2.5883*eV, 2.6155*eV, 
    2.6434*eV, 2.6718*eV, 2.7008*eV, 2.7305*eV, 2.7608*eV, 2.7919*eV, 
    2.8236*eV, 2.8560*eV, 2.8892*eV, 2.9232*eV, 2.9580*eV, 2.9937*eV, 
    3.0302*eV, 3.0676*eV, 3.1059*eV
  };

  // Water
  //
  std::vector<G4double> refractiveIndex1 = {
   1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
   1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58
  };

  std::vector<G4double> absorption = {
    380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 
    380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm,
    380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm, 380.0 * cm,
    380.0 * cm, 380.0 * cm, 380.0 * cm
  };

  std::vector<G4double> scintilComponent1 = {
    0.0493, 0.0654, 0.0923, 0.1219, 0.1639, 0.2198, 0.2959, 0.3737, 0.4252,
    0.4771, 0.5451, 0.6385, 0.7462, 0.8663, 0.9511, 0.9948, 0.8678, 0.5406, 
    0.2586, 0.0784, 0.0050
  };


  G4MaterialPropertiesTable* EJ200MPT = new G4MaterialPropertiesTable();

  EJ200MPT->AddProperty("RINDEX", photonEnergy, refractiveIndex1)
    ->SetSpline(true);
  EJ200MPT->AddProperty("ABSLENGTH", photonEnergy, absorption)->SetSpline(true);
  EJ200MPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintilComponent1)
    ->SetSpline(true);

  EJ200MPT->AddConstProperty("SCINTILLATIONYIELD", 8000. / MeV);
  EJ200MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
  EJ200MPT->AddConstProperty("SCINTILLATIONRISETIME1", 0.1*ns);
  EJ200MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1*ns);
  EJ200MPT->AddConstProperty("SCINTILLATIONYIELD1",1.0);
  
  G4cout << "EJ200 CRSTAL G4MaterialPropertiesTable:" << G4endl;
  EJ200MPT->DumpTable();
  fTankMaterial->SetMaterialPropertiesTable(EJ200MPT);
  G4cout << "EJ200 CRSTAL G4MaterialPropertiesTable IS ADDED :" << G4endl;


  // ------------- Surface --------------

  G4LogicalBorderSurface* surface =
    new G4LogicalBorderSurface("Surface", fTank, world_PV, fSurface);

   std::vector<G4double> specularlobe = {
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
  };

   std::vector<G4double> specularspike = {
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
  };

   std::vector<G4double> backscatter = {
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
    0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
  };

  std::vector<G4double> rindex2 = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  };

  // std::vector<G4double> BGOreflectivity = {
  //   0.11955223, 0.11969612, 0.11984993, 0.12001467, 0.12019145, 0.12038156, 0.12058644,
  //   0.12080779, 0.12104753, 0.12130791, 0.12159155, 0.12190152, 0.12224144, 0.12261563,
  //   0.12302925, 0.12348854, 0.12400111, 0.12457636, 0.12522596, 0.12596467, 0.12681139
  // };

  std::vector<G4double> EJ200transmitivity = {
    0.913025, 0.914053, 0.914335, 0.912220, 0.911012, 0.911250, 0.911322, 0.910044, 
    0.909788, 0.909967, 0.908454, 0.909891, 0.906906, 0.905072, 0.907509, 0.905625, 
    0.896146, 0.876886, 0.808671, 0.542997, 0.205974

  };

  // std::vector<G4double> BGOefficiency = {
  //   0.17608955, 0.17606078, 0.17603001, 0.17599707, 0.17596171, 0.17592369, 0.17588271,
  //   0.17583844, 0.17579049, 0.17573842, 0.17568169, 0.1756197, 0.17555171, 0.17547687,
  //   0.17539415, 0.17530229, 0.17519978, 0.17508473, 0.17495481, 0.17480707, 0.17463772
  // };

  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();

  // SMPT->AddProperty("RINDEX", photonEnergy, rindex2);
  SMPT->AddProperty("SPECULARLOBECONSTANT", photonEnergy, specularlobe);
  SMPT->AddProperty("SPECULARSPIKECONSTANT", photonEnergy, specularspike);
  SMPT->AddProperty("BACKSCATTERCONSTANT", photonEnergy, backscatter);
  // SMPT->AddProperty("REFLECTIVITY", photonEnergy, EJ200reflectivity);
  SMPT->AddProperty("TRANSMITTANCE", photonEnergy, EJ200transmitivity);
  // SMPT->AddProperty("EFFICIENCY", photonEnergy, EJ200efficiency);

  G4cout << "EJ200 G4MaterialPropertiesTable:" << G4endl;
  SMPT->DumpTable();

  fSurface->SetMaterialPropertiesTable(SMPT);
  G4cout << "EJ200 G4MaterialPropertiesTable FOR SURFACE IS ADDED:" << G4endl;

  G4OpticalSurface* opticalSurface = dynamic_cast<G4OpticalSurface*>(
    surface->GetSurface(fTank, world_PV)->GetSurfaceProperty());
  G4cout << "******  opticalSurface->DumpInfo:" << G4endl;
  if(opticalSurface)
  {
    opticalSurface->DumpInfo();
  }
  G4cout << "******  end of opticalSurface->DumpInfo" << G4endl;

  
  // new G4LogicalSkinSurface("MirrorSurface", fWorld_LV, fMirrorSurface); // For Reflecting Surface
// 1. Create a new Optical Surface for the World Boundary
G4OpticalSurface* worldSkin = new G4OpticalSurface("WorldSkin");
worldSkin->SetModel(unified);
// worldSkin->SetType(dielectric_dielectric);
worldSkin->SetType(dielectric_metal);
worldSkin->SetFinish(polishedfrontpainted);
std::cout<<"Set the skin "<<std::endl;
// 2. Define the Reflectivity (1.0 = 100% reflection)
G4double Energies[] = {2.0*eV, 3.5*eV}; // Define range covering your scintillation
G4double Reflectivity[] = {1.0, 1.0};   // 100% reflection
// G4double transmitivity[] = {0.05, 0.05};   // 100% reflection
G4double Efficiency[] = {0.0, 0.0};   // 100% reflection
G4MaterialPropertiesTable* worldSkinMPT = new G4MaterialPropertiesTable();
worldSkinMPT->AddProperty("REFLECTIVITY", Energies, Reflectivity, 2);
// worldSkinMPT->AddProperty("TRANSMITTANCE", energies, transmitivity, 2);
worldSkinMPT->AddProperty("EFFICIENCY", Energies, Efficiency, 2);
std::cout<<"properties are added "<<std::endl;
worldSkin->SetMaterialPropertiesTable(worldSkinMPT);

// 3. Wrap the World Logical Volume in this surface
// new G4LogicalSkinSurface("WorldSkinSurface", fWorld_LV, worldSkin);
new G4LogicalBorderSurface("WorldBorder", world_PV, Copyworld_PV, worldSkin);
new G4LogicalBorderSurface("CopyWorldBorder", Copyworld_PV, world_PV, worldSkin);
std::cout<<"Surface is defined successfully "<<std::endl;



  //----------------------------------PMT Addition Part-----------------------------------------//

  // 1. Define the Optical Surface
  G4OpticalSurface* pmtSurface = new G4OpticalSurface("PmtSurface");
  pmtSurface->SetType(dielectric_dielectric); // Interaction between two dielectrics (Air and Glass)
  pmtSurface->SetFinish(polished);            // Smooth interface
  pmtSurface->SetModel(unified);

  // 2. Link to a Property Table (You can reuse fSurfaceMPT from the example)
  // pmtSurface->SetMaterialPropertiesTable(fSurfaceMPT); 

  // 3. Define the Boundary
  // world_PV is your World Physical Volume, pmt_phys is your PMT Physical Volume
  new G4LogicalBorderSurface("WorldToPmt", world_PV, pmt_phys, pmtSurface);
  // 2. Define the Reflectivity (1.0 = 100% reflection)
  G4double energies[] = {2.0*eV, 3.5*eV}; // Define range covering your scintillation
  G4double refractiveIndex[] = {1.51, 1.65}; 
  G4double reflectivity[] = {0.0, 0.0};   // 00% reflection
  G4double efficiency[] = {1.0, 1.0};   
  G4MaterialPropertiesTable* PMTMP = new G4MaterialPropertiesTable();
  PMTMP->AddProperty("RINDEX", energies, refractiveIndex, 2);
  PMTMP->AddProperty("REFLECTIVITY", energies, reflectivity, 2);
  PMTMP->AddProperty("EFFICIENCY", energies, efficiency, 2);
  std::cout<<"properties are added "<<std::endl;
  pmtSurface->SetMaterialPropertiesTable(PMTMP);
  pmt_mat->SetMaterialPropertiesTable(PMTMP); // Set the same properties to the PMT material for proper refraction  
//----------------------------------PMT Addition Part-----------------------------------------//
  
  return Copyworld_PV;
}

//....oooOO0OOooo........oooOO0OOooo....PMT....oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructSDandField()
{
  // Create the SD
  G4String pmtSDName = "DMXPmtSD";
  DMXPmtSD* pmtSD = new DMXPmtSD(pmtSDName);
  
  // Register with Manager
  G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
  
  // Attach to the Logical Volume
  SetSensitiveDetector("PMT_LV", pmtSD);
}
//....oooOO0OOooo........oooOO0OOooo.....PMT...oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetSurfaceSigmaAlpha(G4double v)
{
  fSurface->SetSigmaAlpha(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  G4cout << "Surface sigma alpha set to: " << fSurface->GetSigmaAlpha()
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetSurfacePolish(G4double v)
{
  fSurface->SetPolish(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  G4cout << "Surface polish set to: " << fSurface->GetPolish() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddTankMPV(const G4String& prop,
                                      G4MaterialPropertyVector* mpv)
{
  fTankMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the box is now: " << G4endl;
  fTankMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddWorldMPV(const G4String& prop,
                                       G4MaterialPropertyVector* mpv)
{
  fWorldMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the world is now: " << G4endl;
  fWorldMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddSurfaceMPV(const G4String& prop,
                                         G4MaterialPropertyVector* mpv)
{
  fSurfaceMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the surface is now: " << G4endl;
  fSurfaceMPT->DumpTable();
  G4cout << "............." << G4endl;
}
// This is for Reflecting Surface
//------------------------------------------------------------------------------//
// void DetectorConstruction::AddMirrorProperty(const G4String& prop, 
//                                             G4MaterialPropertyVector* mpv)
// {
//   fMirrorSurfaceMPT->AddProperty(prop, mpv);
// }
//------------------------------------------------------------------------------//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddTankMPC(const G4String& prop, G4double v)
{
  fTankMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the box is now: " << G4endl;
  fTankMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddWorldMPC(const G4String& prop, G4double v)
{
  fWorldMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the world is now: " << G4endl;
  fWorldMPT->DumpTable();
  G4cout << "............." << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddSurfaceMPC(const G4String& prop, G4double v)
{
  fSurfaceMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the surface is now: " << G4endl;
  fSurfaceMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetWorldMaterial(const G4String& mat)
{
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);
  if(pmat && fWorldMaterial != pmat)
  {
    fWorldMaterial = pmat;
    if(fWorld_LV)
    {
      fWorld_LV->SetMaterial(fWorldMaterial);
      fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4cout << "World material set to " << fWorldMaterial->GetName() << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetTankMaterial(const G4String& mat)
{
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);
  if(pmat && fTankMaterial != pmat)
  {
    fTankMaterial = pmat;
    if(fTank_LV)
    {
      fTank_LV->SetMaterial(fTankMaterial);
      fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
      fTankMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4cout << "Tank material set to " << fTankMaterial->GetName() << G4endl;
  }
}
