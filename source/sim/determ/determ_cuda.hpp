#ifndef SIM_DETERM_DETERM_CUDA_HPP
#define SIM_DETERM_DETERM_CUDA_HPP

#include "core/param_set.hpp"
#include "core/model.hpp"
#include "sim/cell_param.hpp"
#include "core/reaction.hpp"
#include "baby_cl_cuda.hpp"
#include "determ.hpp"
#include <vector>
#include <array>
using namespace std;

#define check(RESULT) do {                      \
      check(RESULT, __FILE__, __LINE__);          \
    } while(0)

namespace { const char *strerrno(int) { return strerror(errno); } }

template<class T, T Success, const char *(ErrorStr)(T t)>
struct ErrorInfoBase {
    static constexpr bool isSuccess(T t) { return t == Success; }
      static const char *getErrorStr(T t) { return ErrorStr(t); }
};
template<class T> struct ErrorInfo;
template <> struct ErrorInfo<cudaError_t> :
  ErrorInfoBase<cudaError_t, cudaSuccess, cudaGetErrorString> {};
template <> struct ErrorInfo<int> :
  ErrorInfoBase<int, 0, strerrno> {};

namespace {
  template<class T>
    static void (check)(T result, const char *file, unsigned line) {
        if (ErrorInfo<T>::isSuccess(result)) return;
          std::cerr << file << ":"
                        << line << ": "
                                    << ErrorInfo<T>::getErrorStr(result) << "\n";
            exit(-1);
    }
}

class simulation_cuda: public simulation_determ {
  public:
    class Context : public ContextBase {
        //FIXME - want to make this private at some point
      protected:
        int _cell;
        simulation_cuda& _simulation;
        double _avg;

      public:
        typedef CPUGPU_TempArray<RATETYPE, NUM_SPECIES> SpecieRates;
        CPUGPU_FUNC
        Context(simulation_cuda& sim, int cell) : _simulation(sim),_cell(cell) { }
        CPUGPU_FUNC
        RATETYPE calculateNeighborAvg(specie_id sp, int delay = 1) const;
        CPUGPU_FUNC
        void updateCon(const SpecieRates& rates);
        CPUGPU_FUNC
        const SpecieRates calculateRatesOfChange();
        CPUGPU_FUNC
        virtual RATETYPE getCon(specie_id sp) const final {
          return getCon(sp, 1);
        }
        CPUGPU_FUNC
        RATETYPE getCon(specie_id sp, int delay) const {
            return _simulation._baby_cl_cuda[sp][1 - delay][_cell];
        }
        CPUGPU_FUNC
        RATETYPE getCritVal(critspecie_id rcritsp) const {
            return _simulation._cellParams[rcritsp + NUM_REACTIONS + NUM_DELAY_REACTIONS][_cell];
        }
        CPUGPU_FUNC
        RATETYPE getRate(reaction_id reaction) const {
            return _simulation._cellParams[reaction][_cell];
        }
        CPUGPU_FUNC
        int getDelay(delay_reaction_id delay_reaction) const{
            return _simulation._cellParams[delay_reaction + NUM_REACTIONS][_cell];
        }
        CPUGPU_FUNC
        virtual void advance() final { ++_cell; }
        CPUGPU_FUNC
        virtual void set(int c) final {_cell = c;}
        CPUGPU_FUNC
        virtual bool isValid() const final { return _cell >= 0 && _cell < _simulation._cells_total; }
    };

    baby_cl_cuda _baby_cl_cuda;
    CPUGPU_TempArray<int, 6>* _old_neighbors;
    int* _old_numNeighbors;
    RATETYPE* _old_cellParams;
    int* _old_intDelays;
    RATETYPE* _old_crits;
    void initialize();

    CPUGPU_FUNC
    void execute_one(int k) { 
        // Iterate through each extant cell or context
        //if (_width_current == _width_total || k % _width_total <= 10) { // Compute only existing (i.e. already grown)cells
            // Calculate the cell indices at the start of each mRNA and protein's dela
            Context c(*this, k);

            // Perform biological calculations
            //_baby_cl_cuda[ph11][0][0] = 5.0; works
            //c.calculateRatesOfChange();
            c.updateCon(c.calculateRatesOfChange());
        //}
        if (k==0){
            _j++;
            _baby_cl_cuda.advance();
        }
    }
    
    void simulate_cuda();
    simulation_cuda(const model& m, const param_set& ps, int cells_total, int width_total, RATETYPE step_size, RATETYPE analysis_interval, RATETYPE sim_time) :
        simulation_determ(m,ps,NULL,NULL, cells_total, width_total,step_size, analysis_interval, sim_time), _baby_cl_cuda(*this) {
          _old_neighbors = _neighbors;
          check(cudaMallocManaged(&_neighbors, sizeof(CPUGPU_TempArray<int, 6>)*_cells_total));
          _old_numNeighbors = _numNeighbors;
          check(cudaMallocManaged(&_numNeighbors, sizeof(int)*_cells_total));
          _old_cellParams = _cellParams._array;
          check(cudaMallocManaged(&(_cellParams._array), sizeof(RATETYPE)*_cells_total*NUM_PARAMS));
          _old_intDelays = _intDelays._array;
          check(cudaMallocManaged(&(_intDelays._array), sizeof(RATETYPE)*_cells_total*NUM_DELAY_REACTIONS));
        }
    ~simulation_cuda() {
      cudaFree(_intDelays._array);
      _intDelays._array = _old_intDelays;
      cudaFree(_cellParams._array);
      _cellParams._array = _old_cellParams;
      cudaFree(_neighbors);
      _neighbors = _old_neighbors;
      cudaFree(_numNeighbors);
      _numNeighbors = _old_numNeighbors;
    }
};
#endif

