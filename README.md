# Compile the mesh partitioning tool

- The libraries "Gmsh" and "Metis" (or "Parmetis") must be in "lib/osx" (or "lib/nic4" on the cluster).
- Create a "build" folder in "Tfe", then "cd build".
- Run "cmake .." puis "make" ("cmake .. -DMPI=ON" for the parallel version).
- The mesh partitioning tool will be found in "Tfe/bin/osx" (or "Tfe/bin/nic4" on the cluster).

# Examples

- "examples/" presents ready-to-use examples

# Files

- All executable files can be found in "ddm/".
