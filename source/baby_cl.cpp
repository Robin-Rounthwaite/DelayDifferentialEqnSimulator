#include "baby_cl.hpp"
#include "simulation.hpp"
using namespace std;

/*
RATETYPE baby_cl::calc_delay(int relatedReactions[]){
    int max;

    for (int i = 0; i <= sizeof(&relatedReactions); i++) {
        int j = relatedReactions[i];

        for (int k = 0; k < _sim.width_total; k++) {
            // Calculate the minimum delay, accounting for the maximum allowable perturbation and gradients
            max = MAX(max, (_sim._parameter_set._delay_sets[j] + (_sim._parameter_set._delay_sets[j] * _sim._model.factors_perturb[j])) * _sim._model.factors_gradient[j][k]);
        }
    }
    return max;
}
*/

/*
void baby_cl::fill_position(){
    for (int i =0; i <= NUM_SPECIES; i++){
    }
}
*/

void baby_cl::initialize(){
    int sum =0;
    int delay =0;
    int specie_size =0;
    int current_pos =0;
    
    for (int i = 0; i < NUM_SPECIES; i++){
        delay = _sim.max_delays[i];
        specie_size = delay + _sim._num_history_steps;
        sum += specie_size;
        _specie_size[i]= specie_size;
        //cout<< specie_size<<endl;
        _position[i] = current_pos;
        current_pos += specie_size * _sim._cells_total;
    }
    _width = _sim._cells_total;
    _total_length = sum * _sim._cells_total;
    cout<<"total_length:" <<_total_length<<endl;
    dealloc_array();
    allocate_array();
    reset();
}

