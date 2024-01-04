// C++ program for the Branch Preditor Unit

/*
 *  Date: Set/2023
 *  Author: Luis Villon
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h> 
#include <time.h> 
#include "wisard.hpp"
#include <vector> 
#include <iostream>
#include <string>
#include <sstream>  
#include <inttypes.h>


namespace wp = wisardpkg;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::to_string;


FILE *stream;
char *buf = NULL;
size_t len = 0;


int read_branch(uint32_t *pc, uint8_t *outcome);
void pc_binary(unsigned n, vector<uint8_t> &pc_bits);
void pc_binary_lower(vector<uint8_t> *pc_bits_lower, const vector<uint8_t> *pc_bits, const unsigned n);
void xor_pc_ghr(vector<uint8_t> *xor_pc_ghr, vector<uint8_t> *pc_bits, vector<uint8_t> *ghr, const unsigned n);
void xor_pc_ghr_distribuited(vector<uint8_t> *xor_pc_ghr, vector<uint8_t> *pc_bits, vector<uint8_t> *ghr);
void add_input(vector<uint8_t> *train_data, vector<uint8_t> *new_data);
void add_input_block(vector<uint8_t> *train_data, vector<uint8_t> *new_data, const unsigned n);
void add_input_lhr(vector<uint8_t> *train_data, const vector<uint8_t> *pc_bits, vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, unsigned int *lhr_sum);
void add_input_block_lhr(vector<uint8_t> *train_data, const vector<uint8_t> *pc_bits, vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, unsigned int *lhr_sum, const unsigned n);
void local_history_init(vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, const unsigned int *lhr_index);
void global_address_init(vector<uint8_t> *ga, const unsigned lower, const unsigned branches);
void global_history_update(vector<uint8_t> *ghr, uint8_t const *outcome);
void lhr_update(vector<vector<uint8_t>> &lhr, unsigned int *lhr_sum, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, uint8_t const *outcome);
void ga_update(vector<uint8_t> *ga, const vector<uint8_t> *pc_bits, const unsigned lower);
void display_final_results(uint32_t *num_branches, uint32_t *num_branches_predicted, int *input_size, const uint8_t *address_size);
void save_accuracy(uint32_t *num_branches, uint32_t *num_branches_predicted, char *name);
void save_results_classification(char result, char *name);


int main(int argc, char *argv[])
{
    // Readinf file
    stream = stdin;
    if (argc != 12) 
    {
        printf("Please, write the file/path_to_file correctly!\n");
        exit(0);
    }
    else 
    {
        // Use as input file
        stream = fopen(argv[1], "r");
        if (stream == NULL)
        {
            printf("Can't open file\n");
        }
    }
    
    // Reading parameters from command line arguments
    unsigned int parameter1 = (unsigned int)atof(argv[2]);  // ntuple_size
    unsigned int parameter2 = (unsigned int)atof(argv[3]);  // PCl times
    unsigned int parameter3 = (unsigned int)atof(argv[4]);  // GHR times
    unsigned int parameter4 = (unsigned int)atof(argv[5]);  // PC xor GHR times    
    unsigned int parameter5 = (unsigned int)atof(argv[6]);  // LHR1 times
    unsigned int parameter6 = (unsigned int)atof(argv[7]);  // LHR2 times
    unsigned int parameter7 = (unsigned int)atof(argv[8]);  // LHR3 times
    unsigned int parameter8 = (unsigned int)atof(argv[9]);  // LHR4 times
    unsigned int parameter9 = (unsigned int)atof(argv[10]);  // LHR5 times
    unsigned int parameter12 = (unsigned int)atof(argv[11]);  // GAs
    
    uint32_t pc = 0;
    uint8_t outcome = 0;
    uint32_t num_branches = 0;
    uint32_t num_branches_predicted = 0;
    vector<uint8_t> pc_bits {};  // PC(32)
    vector<uint8_t> pc_bits_lower_24 {};  // PC(n), n higher most significcant bits
    vector<uint8_t> ghr_size_24 (24, 0);  // ghr of 24 bits
    vector<uint8_t> xor_pc_ghr24 {};  // xoring: PC(16least) xor ghr of 24 bits
    
    
    // LHR 1
    const unsigned lhr_lenght = 24; // Size of each LHR register
    const unsigned lhr_bits_pc = 12; // Pc lower bit for indexing each LHR register (base -> 12bits)
    const unsigned int lhr_index = 1 << lhr_bits_pc;
    vector<vector<uint8_t>> lhr (lhr_index);  // Local history register
    unsigned int lhr_sum = 0;  
    local_history_init(lhr, &lhr_lenght, &lhr_bits_pc, &lhr_index);

    // LHR 2
    const unsigned lhr_lenght2 = 16; // Size of each LHR register
    const unsigned lhr_bits_pc2 = 10; // Pc lower bit for indexing each LHR register (base -> 12bits)
    const unsigned int lhr_index2 = 1 << lhr_bits_pc2;
    vector<vector<uint8_t>> lhr2 (lhr_index2);  // Local history register
    unsigned int lhr_sum2 = 0;  
    local_history_init(lhr2, &lhr_lenght2, &lhr_bits_pc2, &lhr_index2);

    // LHR 3
    const unsigned lhr_lenght3 = 9; // Size of each LHR register
    const unsigned lhr_bits_pc3 = 9; // Pc lower bit for indexing each LHR register (base -> 12bits)
    const unsigned int lhr_index3 = 1 << lhr_bits_pc3;
    vector<vector<uint8_t>> lhr3 (lhr_index3);  // Local history register
    unsigned int lhr_sum3 = 0;  
    local_history_init(lhr3, &lhr_lenght3, &lhr_bits_pc3, &lhr_index3);

    // LHR 4
    const unsigned lhr_lenght4 = 7; // Size of each LHR register
    const unsigned lhr_bits_pc4 = 7; // Pc lower bit for indexing each LHR register (base -> 12bits)
    const unsigned int lhr_index4 = 1 << lhr_bits_pc4;
    vector<vector<uint8_t>> lhr4 (lhr_index4);  // Local history register
    unsigned int lhr_sum4 = 0;  
    local_history_init(lhr4, &lhr_lenght4, &lhr_bits_pc4, &lhr_index4);

    // LHR 5
    const unsigned lhr_lenght5 = 5; // Size of each LHR register
    const unsigned lhr_bits_pc5 = 5; // Pc lower bit for indexing each LHR register (base -> 12bits)
    const unsigned int lhr_index5 = 1 << lhr_bits_pc5;
    vector<vector<uint8_t>> lhr5 (lhr_index5);  // Local history register
    unsigned int lhr_sum5 = 0;  
    local_history_init(lhr5, &lhr_lenght5, &lhr_bits_pc5, &lhr_index5);

    // Information for GAs
    vector<uint8_t> ga {};  // global address
    const unsigned lower = 8;
    const unsigned branches = 8;
    global_address_init(&ga, lower, branches);
    
    const int interval = 10000;  // Intervals for printing predictions
    const uint8_t address_size = parameter1;
    int input_size = (parameter2 * 24) + (parameter3 * 24) + 
                     (parameter4 * 24) + (parameter5 * lhr_lenght) + 
                     (parameter6 * lhr_lenght2) + (parameter7 * lhr_lenght3) + 
                     (parameter8 * lhr_lenght4) + (parameter9 * lhr_lenght5) + 
                     (parameter12 * ga.size());

    wp::Wisard w(address_size, input_size);
    cout << "Input size : " << input_size << endl;
    vector<uint8_t> train_data {};
    uint8_t result {};

    while (read_branch(&pc, &outcome)) 
    {
        num_branches++;
        
        // ------ Binaryzing and preparing train data - input --------
        pc_binary(pc, pc_bits);  // PC(32)
        pc_binary_lower(&pc_bits_lower_24, &pc_bits, 24);  // PC(24l) lowe  
        xor_pc_ghr(&xor_pc_ghr24, &pc_bits_lower_24, &ghr_size_24, 24);  // 

        // =======================================================
        //                 Adding training data - input 
        // =======================================================
        
        // ============= pc bits =============
        add_input_block(&train_data, &pc_bits_lower_24, parameter2);  // Adding: several pc_bits_lower_16 
        
        // =========== Global histories ======       
        add_input_block(&train_data, &ghr_size_24, parameter3);  // Adding: several ghr_size_24         
        
        // =========== xored data PC GHRS =======
        add_input_block(&train_data, &xor_pc_ghr24, parameter4);  // Adding: several xor_pc_ghr24
        
        // =========== local histories =======
        add_input_block_lhr(&train_data, &pc_bits, lhr, &lhr_lenght, &lhr_bits_pc, &lhr_sum, parameter5);  // Adding: several lhr1          
        add_input_block_lhr(&train_data, &pc_bits, lhr2, &lhr_lenght2, &lhr_bits_pc2, &lhr_sum2, parameter6);  // Adding: several lhr2          
        add_input_block_lhr(&train_data, &pc_bits, lhr3, &lhr_lenght3, &lhr_bits_pc3, &lhr_sum3, parameter7);  // Adding: several lhr3          
        add_input_block_lhr(&train_data, &pc_bits, lhr4, &lhr_lenght4, &lhr_bits_pc4, &lhr_sum4, parameter8);  // Adding: several lhr4          
        add_input_block_lhr(&train_data, &pc_bits, lhr5, &lhr_lenght5, &lhr_bits_pc5, &lhr_sum5, parameter9);  // Adding: several lhr5          
    
        // =========== GAs =======
        add_input_block(&train_data, &ga, parameter12);  // Adding: several ga
        result = w.classify(train_data);
        if(result == outcome)  //train_label 
        { 
            num_branches_predicted++;
        }
        if (num_branches % interval == 0)
        {
            double accuracy = ((double)num_branches_predicted/(double)num_branches) * 100;
            printf("branch number: %d\n", num_branches);
            printf("----- Partial Accuracy: %f\n\n", accuracy);
        }
        
        // ------- Train the predictor ---------
        w.train(train_data, outcome);
        
        // -------- Updating tables/registers
        global_history_update(&ghr_size_24, &outcome);
        lhr_update(lhr, &lhr_sum, &lhr_lenght, &lhr_bits_pc, &outcome);
        lhr_update(lhr2, &lhr_sum2, &lhr_lenght2, &lhr_bits_pc2, &outcome);
        lhr_update(lhr3, &lhr_sum3, &lhr_lenght3, &lhr_bits_pc3, &outcome);
        lhr_update(lhr4, &lhr_sum4, &lhr_lenght4, &lhr_bits_pc4, &outcome);
        lhr_update(lhr5, &lhr_sum5, &lhr_lenght5, &lhr_bits_pc5, &outcome);
        ga_update(&ga, &pc_bits, lower);

        // -------- Cleaning current branch information
        train_data.clear();
        pc_bits.clear();
        pc_bits_lower_24.clear();
        xor_pc_ghr24.clear();
    }
    
    display_final_results(&num_branches, &num_branches_predicted, &input_size, &address_size);
    save_accuracy(&num_branches, &num_branches_predicted, argv[1]);    
    fclose(stream);
    free(buf);
    return 0;
}


// Reads a line from the input stream and extracts the PC and Outcome of a branch
// Returns True if Successful
int read_branch(uint32_t *pc, uint8_t *outcome)
{
    if (getline(&buf, &len, stream) == -1) 
    {
        return 0;
    }
    uint32_t tmp;
    sscanf(buf,"%u %u\n", pc, &tmp);
    *outcome = tmp;
    //printf("Branch: %u  --- outcome: %u\n", *pc, *outcome);
    return 1;
}


// binary digits of an int number
void pc_binary(unsigned n, vector<uint8_t> &pc_bits)
{
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
    {
        (n & i) ? pc_bits.push_back(1) : pc_bits.push_back(0);
        // pc_bits.erase(pc_bits.begin());
    }
}


// Display a vector
void display(const vector<int> *const v)
{
    for (auto str: *v)  // v is not a vector, *v is the vector to be referenced
    {
        cout << str << " ";
    }
    cout << endl;
}


// Adding new input to train_data
void add_input(vector<uint8_t> *train_data, vector<uint8_t> *new_data)
{
    for (size_t i = 0; i < (*new_data).size(); i++)
    {
        (*train_data).push_back((*new_data)[i]);
    }
}


// Adding block of inputs
void add_input_block(vector<uint8_t> *train_data, vector<uint8_t> *new_data, const unsigned n)
{
    for (unsigned i = 0; i < n; i++)
    {
        add_input(train_data, new_data);
    }
    
}


// Adding new input, from lhr to train_data
void add_input_lhr(vector<uint8_t> *train_data, const vector<uint8_t> *pc_bits, vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, unsigned int *lhr_sum)
{
    unsigned int sum_32 = 0;
    int pc_position_initial = (*pc_bits).size() - (*lhr_bits_pc);
    for (size_t i = 0; i < (*lhr_bits_pc); i++)
    {
        int pc_position = pc_position_initial + i;
        int aux_32 = (*pc_bits)[pc_position];
        sum_32 = sum_32 + (aux_32 << ((*lhr_bits_pc) - 1 - i));
    }
    //cout << "\n" << sum_32 <<endl;
    for (size_t j = 0; j < (*lhr_lenght); j++)
    {
        (*train_data).push_back(lhr[sum_32][j]);
    }
    *lhr_sum = sum_32;
}


// Adding block of inputs of lhr
void add_input_block_lhr(vector<uint8_t> *train_data, const vector<uint8_t> *pc_bits, vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, unsigned int *lhr_sum, const unsigned n)
{
    for (size_t i = 0; i < n; i++)
    {
        add_input_lhr(train_data, pc_bits, lhr, lhr_lenght, lhr_bits_pc, lhr_sum);
    }
}


// Initialization global addresses
void global_address_init(vector<uint8_t> *ga, const unsigned lower, const unsigned branches)
{
    for (size_t i = 0; i < branches; i++)
    {
        for (size_t j = 0; j < lower; j++)
        {
            (*ga).push_back(0);
        }  
    }
}


// Initialization of local history
void local_history_init(vector<vector<uint8_t>> &lhr, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, const unsigned int *lhr_index)
{
    for(size_t i = 0; i < (*lhr_index); i++)
    {
        for(size_t j = 0; j < (*lhr_lenght); j++)
        {
            lhr[i].push_back(0);
        }
    }
}


// Update of a the 16 ghr
void global_history_update(vector<uint8_t> *ghr, uint8_t const *outcome)
{
    int temp = (int)*outcome;
    (*ghr).push_back(temp);
    (*ghr).erase((*ghr).begin());
}


// Update global address
void ga_update(vector<uint8_t> *ga, const vector<uint8_t> *pc_bits, const unsigned lower)
{
    int pc_position_initial = (*pc_bits).size() - lower;
    for (size_t i = 0; i < lower; i++)
    {
        int pc_position = pc_position_initial + i;
        (*ga).push_back((*pc_bits)[pc_position]);
        (*ga).erase((*ga).begin());
    }
}


// Update of a the LHR
void lhr_update(vector<vector<uint8_t>> &lhr, unsigned int *lhr_sum, const unsigned *lhr_lenght, const unsigned *lhr_bits_pc, uint8_t const *outcome)
{
    int temp = (int)(*outcome);
    lhr[(*lhr_sum)].push_back(temp);
    lhr[(*lhr_sum)].erase(lhr[(*lhr_sum)].begin());
    (*lhr_sum) = 0;
}


// Display the final input
void display_final_results(uint32_t *num_branches, uint32_t *num_branches_predicted, int *input_size, const uint8_t *address_size)
{
    double accuracy = ((double)(*num_branches_predicted)/(double)(*num_branches)) * 100;
    printf(" ----- Results ------\n");
    printf("Predicted  branches: %d\n", *num_branches_predicted);
    printf("Not predicted branches: %d\n", *num_branches - *num_branches_predicted);
    printf("Accuracy: %f\n", accuracy);
    printf("\n------ Size of ntuple (address_size): %d -----\n", *address_size);
    printf("\n------ Size of each input: %d -----\n", *input_size);
}

// lower bits of PC, least significant
void pc_binary_lower(vector<uint8_t> *pc_bits_lower, const vector<uint8_t> *pc_bits, const unsigned n)
{
    int pc_position_initial = (*pc_bits).size() - n;
    for (size_t i = 0; i < n; i++)
    {
        int pc_position = pc_position_initial + i;
        (*pc_bits_lower).push_back((*pc_bits)[pc_position]);
    }
}

// xoring two vectors (bits from PC and ghr of size n)
void xor_pc_ghr(vector<uint8_t> *xor_pc_ghr, vector<uint8_t> *pc_bits, vector<uint8_t> *ghr, const unsigned n)
{
    for (size_t i = 0; i < n; i++)
    {
        (*xor_pc_ghr).push_back((*pc_bits)[i] ^ (*ghr)[i]);
    }
}


// xoring two vectors, acoording to size of relçation of the 2 vectors
void xor_pc_ghr_distribuited(vector<uint8_t> *xor_pc_ghr, vector<uint8_t> *pc_bits, vector<uint8_t> *ghr)
{    
    // Second attemp
    size_t interval = (size_t)((*ghr).size())/(size_t)((*pc_bits).size());
    for (size_t i = 0; i < interval; i++)
    {
        int index = (*pc_bits).size() * i;
        for (size_t j = 0; j < (*pc_bits).size(); j++)
        {
            (*xor_pc_ghr).push_back((*pc_bits)[j] ^ (*ghr)[index + j]);
        }
    }
}


// Saving accuracies
void save_accuracy(uint32_t *num_branches, uint32_t *num_branches_predicted, char *name)
{
    double accuracy = ((double)(*num_branches_predicted)/(double)(*num_branches)) * 100;
    char path[] = "-accuracy.csv";
    char path_file[strlen(path) + strlen(name) + 1];
    snprintf(path_file, sizeof(path_file), "%s%s", name, path);
    FILE *f1;
    f1 = fopen(path_file,"a+");
    if (!f1)
    {
        printf("Can't open file\n");
    }
    fprintf(f1,"%5.4f", accuracy);
    fprintf(f1,"\n");
    fclose(f1);
}


void save_results_classification(char result, char *name)
{
    char path[] = "-results-classification.csv";
    char path_file[strlen(path) + strlen(name) + 1];
    snprintf(path_file, sizeof(path_file), "%s%s", name, path);
    FILE *f1;
    f1 = fopen(path_file,"a+");
    if (!f1)
    {
        printf("Can't open file\n");
    }
    fprintf(f1,"%c", result);
    fprintf(f1,"\n");
    fclose(f1);
}
