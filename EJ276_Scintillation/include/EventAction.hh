#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction {
public:
    void AddPMTHit() { fHitCount++; }
    void AddPMTEnergy(G4double edep) { fTotalEnergy += edep; }
    // void PMTphotonEnergy(G4double edep) { fPhotonEnergies.push_back(edep); }
    void SetInitialEnergy(G4double energy) { fInitialEnergy = energy; }
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

private:
    G4int fHitCount = 0;
    G4double fTotalEnergy = 0.0;
    // std::vector<G4double> fPhotonEnergies; // Vector to store each hit
    G4double fInitialEnergy = 0.0;
};

#endif