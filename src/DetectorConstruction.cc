// src/DetectorConstruction.cc
#include "DetectorConstruction.hh"

#include "BeamlineGeometry.hh"
#include "PhantomGeometry.hh"
#include "ExitPlane.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "G4GenericMessenger.hh"
#include <cmath>

DetectorConstruction::DetectorConstruction()
{
  fMessenger = std::make_unique<G4GenericMessenger>(this, "/flash/", "Flash electron beam setup");

  auto& appCmd = fMessenger->DeclareMethod("setApplicatorIDcm",
                                           &DetectorConstruction::SetApplicatorIDcm,
                                           "Select applicator ID in cm (allowed: 10, 5, 2).");
  appCmd.SetParameterName("idCm", false);
  appCmd.SetStates(G4State_PreInit);
}

void DetectorConstruction::SetApplicatorIDcm(G4double idCm)
{
  const auto idMm = idCm*cm;
  if (std::abs(idMm - 100.0*mm) < 1e-6*mm ||
      std::abs(idMm - 50.0*mm)  < 1e-6*mm ||
      std::abs(idMm - 20.0*mm)  < 1e-6*mm) {
    fApplicatorIDmm = idMm;
    return;
  }

  G4cout << "[DetectorConstruction] Unsupported applicator ID=" << idCm
         << " cm. Using default 10 cm." << G4endl;
  fApplicatorIDmm = 100.0*mm;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto nist = G4NistManager::Instance();
  auto air  = nist->FindOrBuildMaterial("G4_AIR");

  // -------- World --------
  const auto worldSize = 2.0*m;
  auto solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
  auto logicWorld = new G4LogicalVolume(solidWorld, air, "WorldLV");
  auto physWorld  = new G4PVPlacement(nullptr, {}, logicWorld, "WorldPV",
                                      nullptr, false, 0, true);

  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  // -------- Beamline (selectable applicator) --------
  const auto beam = BeamlineGeometry::BuildBeamline(logicWorld, fApplicatorIDmm);

  // -------- Exit plane (just after applicator exit) --------
  // Place 0.5 mm downstream so it does not overlap the water phantom front face.
  const auto exitPlaneZ = beam.zAppExit - 0.5*mm;   // upstream of water
  
  // NOTE: ExitPlaneGeometry defaults are in mm, so pass plain numbers (not *mm).
  //ExitPlaneGeometry::BuildExitPlane(logicWorld, exitPlaneZ, 60.0, 0.1);
  //ExitPlaneGeometry::BuildExitPlane(logicWorld, exitPlaneZ, 49.0*mm, 0.1*mm);
  ExitPlaneGeometry::BuildExitPlane(logicWorld, exitPlaneZ, beam.appOuterR + 1.0*mm, 0.1*mm);

  // -------- Water phantom --------
  // Your measurement: applicator exit → water surface = 0 mm (flush).
  const auto phantomFrontZ = beam.zAppExit;
  // PhantomGeometry defaults are in mm, so pass mm numbers (30 cm = 300 mm).
  const auto phantom = PhantomGeometry::BuildWaterBox(logicWorld, phantomFrontZ, 300.0, 300.0);

  // -------- Print geometry landmarks (prints in terminal) --------
  G4cout << "==============================" << G4endl;
  G4cout << "Applicator exit Z = " << beam.zAppExit/mm << " mm" << G4endl;
  G4cout << "Phantom front Z   = " << phantomFrontZ/mm << " mm" << G4endl;
  G4cout << "Phantom center Z  = " << phantom.zPhantomCenter/mm << " mm" << G4endl;
  G4cout << "Exit plane Z      = " << exitPlaneZ/mm << " mm" << G4endl;
  G4cout << "==============================" << G4endl;

  return physWorld;
}

