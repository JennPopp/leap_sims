# leap_sims

### How to run leap_sims
1.  set the environment:  
  `source /cvmfs/ilc.desy.de/key4hep/releases/089d775cf2/key4hep-stack/2023-07-24/x86_64-centos7-gcc12.3.0-opt/zs7ye/setup.sh`
    This release uses GEANT 11.1.1 , CAUTION: the current version of leapsims is not compatible with GEANT 11.2, but we're working on it
    
2.  make a build directory and change to the build directory:  
  `mkdir build`  
  `cd build`  

3.build the application:  
  `cmake ../`  
  `make`  

4. edit the config.ini file to your needs
  - available run types are `asymmetry` (starts 2 runs with different polarization configurations) and `single` (starts a single run)
  - if run type `asymmetry` is chosen and the $\xi_3$ of the electron beam is 0 two runs with $\pm \xi_{3,Fe}$ and Bz are started, otherwhise $\xi_{3,Fe}$ stays constant and $\xi_{3,e^-}$ flips
  - available world materials are `Air` and `Galactic`
  - available solenoid types are `TP1` (used for design study) and `TP2` (used for experiment)
  - to remove converter target simply set `convThick` to 0
  - available calorimeter types are `full` using 9 crystals and housing and `crystal`, which places just the wrapped crystals
  - if the full calorimeter is used, always 9 crystals are placed, otherwhise either 9 o 1 are possible
  - distances are in mm, energies in MeV
  - `beamLineStatus 1` uses the experimental setup used at FLARE, `beamLineStatus 2` uses the testbeam setup
  - available output modes are `summary` and `detailed`
  - `polDeg` spezifies the $\xi_3$, longitudinal polarization, of either the material (`[Solenoid]`) , or the initial bema electron (`[GPS]`)
  - `nBunch` is the number of particles that are shot during one event
  - `posType` is by default set to `Beam`, which causes a 2d gaussian profile, but can also be set to `Plane` in order to use a pencil beam disc shape or to `Square` to have a squared shaped constant beam pofile 
  - `spotSize` is equal to the $\sigma$ in x and y if the type is set to beam, otherwhise its the radius of the beam
  - `eneType` can be set to `Gauss`, where `sigmaE` can be set to zero to achieve a monoenergetic beam, or to `User`
  - in the latter case a histogram name `histname` has to be specified
  - make sure to generate unique random seeds for runs once you stopped testing!!!!!
   
5. start simulation with
   `leap_sims [-c configFileName of type string] [-v visualState of type bool]`
