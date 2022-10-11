# ADCS source
All source files for ADCS that will be on OBC, not including any test code

## SGP4 Implementation

Source files come from repository https://github.com/aholinch/sgp4.git

# Building Testing Code
Run this at the top-level directory (OBC-firmware):

```bash
make -f adcs/Makefile # Build
./adcs/build/adcs_demo
```

To remove the build files, run the following:
```bash
make -f adcs/Makefile clean
```