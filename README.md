# leap_sims

How to run magnet_sims
  set the environment:
  source /cvmfs/sft.cern.ch/lcg/releases/LCG_97/Geant4/10.06.p01/x86_64-centos7-gcc8-opt/Geant4-env.sh
  source /cvmfs/sft.cern.ch/lcg/releases/LCG_97/Geant4/10.06.p01/x86_64-centos7-gcc8-opt/bin/geant4.sh

  Then in the same folder as the macro (luxe_e_gamma.mac)
  /cvmfs/sft.cern.ch/lcg/releases/LCG_97/CMake/3.14.3/x86_64-centos7-gcc8-opt/bin/cmake ./
  make
  ./lxbeamsim luxe_e_gamma.mac 1 > filename.txt

  1 means one thread, not tested for more
  The output will be directed to a .txt file called filename

How to run leap_sims_from_basic
  set the environment:
  source /cvmfs/sft.cern.ch/lcg/releases/LCG_97/Geant4/10.06.p01/x86_64-centos7-gcc8-opt/Geant4-env.sh
  source /cvmfs/sft.cern.ch/lcg/releases/LCG_97/Geant4/10.06.p01/x86_64-centos7-gcc8-opt/bin/geant4.sh

  make a build directory and change to the build directory
  mkdir build
  cd build

  /cvmfs/sft.cern.ch/lcg/releases/LCG_97/CMake/3.14.3/x86_64-centos7-gcc8-opt/bin/cmake ../
  make
  ./leap_sims -m test_run.mac -t 4 >output.txt

  -m test_run.mac will run in batch mode und use macro test_run.mac
  -t 4 sets the number of threads to 4
  >output.txt redirects the output to a textfile called output
