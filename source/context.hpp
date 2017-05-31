// A context defines a locale in which reactions take place and species 
//   reside
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "specie.hpp"
#include "concentration_level.hpp"


class Context {
  //FIXME - want to make this private at some point
 public:
  typedef CPUGPU_TempArray<RATETYPE, NUM_SPECIES> SpecieRates;
  const int _cell;
  simulation& _simulation;
  double _avg;
  CPUGPU_FUNC
  Context(simulation& sim, int cell) : _simulation(sim),_cell(cell) { }
  CPUGPU_FUNC
  RATETYPE calculateNeighborAvg(specie_id sp, int delay = 0) const;
  CPUGPU_FUNC
  void updateCon(const SpecieRates& rates);
  CPUGPU_FUNC
  const SpecieRates calculateRatesOfChange();
  CPUGPU_FUNC
  RATETYPE getCon(specie_id sp, int delay = 1) const {
    int modified_step = _simulation._baby_j[sp] + 1 - delay;
    return _simulation._baby_cl[sp][modified_step][_cell];
  }
  CPUGPU_FUNC
  RATETYPE getCritVal(critspecie_id rcritsp) const {
    return _simulation._critValues[rcritsp][_cell];
  }
  CPUGPU_FUNC
  RATETYPE getRate(reaction_id reaction) const {
    return _simulation._rates[reaction][_cell];
  }
  CPUGPU_FUNC
  RATETYPE getDelay(delay_reaction_id delay_reaction) const{
    return _simulation._delays[delay_reaction][_cell]/_simulation._step_size;
  }
};

#endif // CONTEXT_HPP
