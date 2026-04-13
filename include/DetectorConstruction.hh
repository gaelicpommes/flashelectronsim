// include/DetectorConstruction.hh
#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include <memory>

class G4VPhysicalVolume;
class G4GenericMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;

private:
  void SetApplicatorIDcm(G4double idCm);

  G4double fApplicatorIDmm = 100.0;
  std::unique_ptr<G4GenericMessenger> fMessenger;
};
