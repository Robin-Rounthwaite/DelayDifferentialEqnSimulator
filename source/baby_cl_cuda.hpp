#ifndef BABY_CL_CUDA_HPP
#define BABY_CL_CUDA_HPP
#include "specie.hpp"
#include "model.hpp"
#include "baby_cl.hpp"
#include <cuda/cuda.h>
#define WRAP(x, y) ((x) + (y)) % (y)
#define MAX(x, y) ((x) < (y) ? (y) : (x))
using namespace std;

#include <cstddef>

class simulation_cuda;

class baby_cl_cuda : public baby_cl {
  public:
    //RATETYPE *_darray;
    
    baby_cl_cuda(simulation& sim) : baby_cl(sim) { }
    
    baby_cl_cuda(int length, int width, simulation& sim) : baby_cl(length,width,sim) {}
    
    
protected:
    void dealloc_array(){
        if (_cuda){
            (cudaFree(_darray));
            _darray = NULL;
        }
    }
    
    void allocate_array(){
        if (_total_length >0){
            int size = _total_length * sizeof(RATETYPE);
            (cudaMalloc((void**)&_darray, size));
            //_array= new RATETYPE[_total_length];
            //if (_array == NULL){std::cout<<"ERROR"<<std::endl; exit(EXIT_MEMORY_ERROR);}
        }
        else{
            _darray= NULL;
        }
    }
    
};


#endif

