/*
 * This file is part of the SYMPLER package.
 * https://github.com/kauzlari/sympler
 *
 * Copyright 2002-2013, 
 * David Kauzlaric <david.kauzlaric@frias.uni-freiburg.de>,
 * and others authors stated in the AUTHORS file in the top-level 
 * source directory.
 *
 * SYMPLER is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SYMPLER is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SYMPLER.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Please cite the research papers on SYMPLER in your own publications. 
 * Check out the PUBLICATIONS file in the top-level source directory.
 *
 * You are very welcome to contribute extensions to the code. Please do 
 * so by making a pull request on https://github.com/kauzlari/sympler
 * 
 */



#include "thermostat_peters.h"


#include "phase.h"
#include "threads.h"
#include "simulation.h"
#include "manager_cell.h"
#include "random.h"
// #include "node.h"

using namespace std;


#define M_SIMULATION ((Simulation*) m_parent)
#define M_CONTROLLER M_SIMULATION->controller()
#define M_PHASE M_SIMULATION->phase()
#define M_MANAGER M_PHASE->manager()

ThermostatPeters::ThermostatPeters(Simulation* sim)
  : ThermostatWithRng(sim)
{
  init();
}


void ThermostatPeters::init()
{
  m_properties.setClassName("ThermostatPeters");

  m_properties.setDescription(
    "Parnet class for Peters thermostats. See: E. A. J. F. Peters, " 
      "Europhys. Lett. 66 (3), pp. 311.");

  STRINGPC
    (species1, m_species.first,
     "First species this thermostat should work on.");

  STRINGPC
    (species2, m_species.second,
     "Second species this thermostat should work on.");

  FUNCTIONPAIRPC
      (dissipation, m_dissipation/*, 0*/,
      "Scalar dissipation function. Type some nonsense to \n"
          "obtain an incomplete list of possible variables and constants.\n"
          "The expression may contain vectors and tensors,"
          " but as a whole it must represent a scalar.");

  STRINGPC
    (weightingFunction, m_weighting_function,
     "Defines the weighting function to be used.");

  m_species.first = "UNDEF";
  m_species.second = "UNDEF";
  m_weighting_function = "default";
}


void ThermostatPeters::setup()
{

  MSG_DEBUG("ThermostatPeters::setup", "CALLED");

  ThermostatWithRng::setup();

  m_dt = M_CONTROLLER->dt();

  m_wf = M_SIMULATION->findWeightingFunction(m_weighting_function);
  m_cutoff = m_wf->cutoff();
  m_rc = 1/m_cutoff;

  m_cp = M_MANAGER->cp(M_MANAGER->getColour(m_species.first), M_MANAGER->getColour(m_species.second)/*m_species*/);
  
  m_cp->setCutoff(m_cutoff);
  m_cp->setNeedPairs(true);
  
  
  m_dissipation.setReturnType(Variant::SCALAR);
  m_dissipation.setColourPair(m_cp);
//   m_dissipation.setWeightF(m_wf);
  
}
