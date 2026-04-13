# flashelectronsim

## Applicator selection (10 cm / 5 cm / 2 cm)

You can now choose applicator inner diameter from a macro before `/run/initialize`:

```tcl
/flash/setApplicatorIDcm 10   # allowed: 10, 5, 2
```

The geometry uses these fixed specs:

- 10 cm applicator: ID=100 mm, OD=116 mm, L=428 mm
- 5 cm applicator:  ID=50 mm,  OD=66 mm,  L=428 mm
- 2 cm applicator:  ID=20 mm,  OD=36 mm,  L=428 mm

`macros/beamdiag_template.mac`, `macros/pddpossib.mac`, and `macros/runpossib.mac`
are now directly runnable with safe defaults (`10 cm`, default source, default beamOn),
and also include optional commented replacement tokens for batch scripts
(`SOURCE_MAC`, `APPLICATOR_CM`, `BEAMON`).

## Multithreading (Geant4 MT)

`main.cc` now uses Geant4's default run manager type, so MT is enabled automatically
when your Geant4 installation is built with multithreading support.

You can set threads by either:

- environment variable: `G4FORCENUMBEROFTHREADS=<N>`
- command line: `./FlashElectronSim <macro.mac> <N>`

If both are provided, command-line `<N>` is used.
