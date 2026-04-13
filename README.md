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
support `APPLICATOR_CM` placeholder replacement for batch jobs.
