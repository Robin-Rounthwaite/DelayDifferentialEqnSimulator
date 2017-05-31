#ifndef CELL_PARAM_HPP
#define CELL_PARAM_HPP

#ifdef __CUDACC__
#define CPUGPU_FUNC __host__ __device__
#else
#define CPUGPU_FUNC
#endif

#include <stdlib.h>
//#include <cuda_runtime_api.h>
//#include <cuda.h>
#include <iostream>
//#include "simulation.hpp"
#include "reaction.hpp"
#include "specie.hpp"

using namespace std;
class simulation;

template<int N>
class cell_param {
    //FIXME - want to make this private at some point
public:
    int   _height, _width;
    bool _cuda;
    RATETYPE *_array;
    const simulation& _sim;
    
    
    class cell{
    public:
        CPUGPU_FUNC
        cell(RATETYPE *row): _array(row) {}
        CPUGPU_FUNC
        RATETYPE& operator[](int k){
            return _array[k];
        }
        CPUGPU_FUNC
        const RATETYPE& operator[](int k) const {
            return _array[k];
        }
        RATETYPE *_array;
    };
    
    CPUGPU_FUNC
    cell_param(const simulation& sim, int ncells)
    :_height(N),_width(ncells),_sim(sim),_cuda(false){
        allocate_array();
    }

    ~cell_param() {
      dealloc_array();
    }
    
    CPUGPU_FUNC
    cell operator[](int i){
        return cell(_array+_width*i);
    }
    
    CPUGPU_FUNC
    const cell operator[](int i) const{
        return cell(_array+_width*i);
    }
    
    void update_rates(const RATETYPE param_data[]);
    int height() const {return _height;}
    int width() const {return _width;}
    inline RATETYPE random_perturbation (RATETYPE perturb) {
        return random_rate(pair<RATETYPE, RATETYPE>(1 - perturb, 1 + perturb));
    }
    RATETYPE random_rate(pair<RATETYPE, RATETYPE> range) {
        return range.first + (range.second - range.first) * rand() / (RAND_MAX + 1.0);
    }
    void initialize();
//protected:
    CPUGPU_FUNC
    void dealloc_array(){
        if (_array){
            delete[] _array;
        }
        _array= NULL;
    }
    
    CPUGPU_FUNC
    void allocate_array(){
        if (_width*_height >0){
            _array= new RATETYPE[_height*_width];
            //if (_array == NULL){std::cout<<"ERROR"<<std::endl; exit(EXIT_MEMORY_ERROR);}
        }
        else{
            _array= NULL;
        }
    }

};


#endif
