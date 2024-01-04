/*
 * Date: Set/2023
 * Author: Luis Villon
 * This version adapts the wisardpkg (https://github.com/IAZero/wisardpkg) for branch prediction 
 * 
 */

#ifndef WISARDPKG_HPP
#define WISARDPKG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <cstdlib>      //includes functions to convert C-style strings to other types
#include <unordered_map>
#include <algorithm>    // std::random_shuffle
#include <time.h>
#include <exception>
#include <cmath>
#include <cstring>      // memcpy . Inludes many functions that work with C-style strings
#include <stdint.h> 
#include <stdio.h>


namespace wisardpkg {

typedef unsigned long long addr_t;
typedef int content_t;
typedef std::unordered_map<addr_t, content_t> ram_t;


inline int randint(int min, int max){
    std::srand(time(NULL));
    return min + (std::rand() % (int)(max - min + 1));
}

int calculateNumberOfRams(int entrySize, int addressSize){
    int numberOfRAMS = entrySize / addressSize;
    int remain = entrySize % addressSize;
    if(remain > 0)
        numberOfRAMS++;
    return numberOfRAMS;
}


class Exception: public std::exception{
public:
    Exception(const char* msg): msg(msg){
    }
    const char* msg;
    virtual const char* what() const throw()
    {
        return msg;
    }
};


class Bleaching{
public:
    static std::vector<int> make(std::vector<std::vector<int>>& allvotes) {
        std::vector<int> labels(2);
        int bleaching = 1;
        int biggest = 0;
        bool ambiguity = false;
        do{
            for(int i = 0; i < 2; ++i){
                labels[i] = 0;
                for(size_t j = 0; j < allvotes[i].size(); j++){
                    if(allvotes[i][j] >= bleaching){
                        labels[i]++; // Counting all votes greater or equal than bleaching
                    }
                }
            }
            bleaching++;
            // (-- 76 --)
            biggest = 0;
            ambiguity = false;
            for(int i = 0; i < 2; ++i){
                if(labels[i] > biggest){
                    biggest = labels[i];
                    ambiguity = false;
                }
                else if(biggest == labels[i]){
                    ambiguity = true;
                }
            }
        }while((ambiguity == true) && (biggest > 1));
        return labels;
    }

};


class RAM{
public:
    // Constructor
    RAM(){
    }

    // Constructor
    RAM(const std::vector<int> indexes): addresses(indexes){
        checkLimitAddressSize(indexes.size());
    }

    int getVote(const std::vector<uint8_t>& input){
        this->index = getIndex(input);
        auto it = positions.find(this->index);
        if(it == positions.end()){
            return 0;
        }
        else{
            return it->second;
        }
        
    }
    
    void train(const std::vector<uint8_t>& input){
        auto it = positions.find(this->index);
        if(it == positions.end()){
            positions.insert(it,std::pair<addr_t,content_t>(this->index, 1));
        }
        else{
            // Original
            it->second++;
        }
    }

    // Destructor
    // First and second ~RAM() destructor is due to eliminate the temporary RAM object
    ~RAM(){
        addresses.clear();
        positions.clear();
    }
  
    addr_t getIndex(const std::vector<uint8_t>& input) const{
        addr_t index = 0;
        addr_t p = 1;
        for(unsigned int i = 0; i < addresses.size(); i++){
            int bin = input[addresses[i]];
            index += bin*p;
            p *= 2;
        }
        return index;
    }

private:
    std::vector<int> addresses;
    ram_t positions;
    addr_t index;

    void checkLimitAddressSize(int addressSize){
        if(addressSize > 64){
            throw Exception("The base power to addressSize passed the limit of 2^64!");
        }
    }
    
    // IMPORTANT-USE WHEN THE INPUT BITS ARE WELL DEFINED!!
    void checkPos(const int code) const{
        if(code >= 2){
            throw Exception("The input data has a value bigger than base of addresing!");
        }
    }
};


class Discriminator{
public:
  
    Discriminator(): entrySize(0){
    }

    // Costructor
    Discriminator(int addressSize, int entrySize): entrySize(entrySize){
        srand(randint(0,1000000));
        setRAMShuffle(addressSize);
    }
    
    std::vector<int> classify(const std::vector<uint8_t>& input) {
        std::vector<int> votes(rams.size());        
        // Loop for all rams.
        // In bpu rams.size() = inputsiez/addresssize
        for(unsigned int i = 0; i < rams.size(); i++){
            // Applying votation
            votes[i] = rams[i].getVote(input);
        }
        return votes;
    }

    // Training in discriminators
    void train(const std::vector<uint8_t>& input){
        for(unsigned int i=0; i<rams.size(); i++){
            // Training in rams
            rams[i].train(input);
        }
        // END OF TRAINING PROCESS 
    }

    int getNumberOfRAMS() const{
        return rams.size();
    }
    
    ~Discriminator(){
        rams.clear();
    }

    void setRAMShuffle(int addressSize){
        checkAddressSize(entrySize, addressSize);
        int numberOfRAMS = entrySize / addressSize;
        int remain = entrySize % addressSize;
        int indexesSize = entrySize;
        if(remain > 0) {
            numberOfRAMS++;
            indexesSize += addressSize-remain;
        }
        
        // Calling when using the RAM clas for the first time
        rams.resize(numberOfRAMS);
        std::vector<int> indexes(indexesSize);

        for(int i=0; i<entrySize; i++) {
            indexes[i] = i;
        }
        random_shuffle(indexes.begin(), indexes.end());
        // creation of rams according to the number of RAMS (numberOfRAMS)
        for(unsigned int i=0; i<rams.size(); i++){
            std::vector<int> subIndexes(indexes.begin() + (i*addressSize), indexes.begin() + ((i+1)*addressSize));
            rams[i] = RAM(subIndexes);
        }
    }

private:
    void checkEntrySize(const int entry) const {
        if(entrySize != entry){
            throw Exception("The entry size defined on creation of discriminator is different of entry size given as input!");
        }
    }

    void checkAddressSize(const int entrySize, const int addressSize) const{
        if( addressSize < 2){
            throw Exception("The address size cann't be lesser than 2!");
        }
        if( entrySize < 2 ){
            throw Exception("The entry size cann't be lesser than 2!");
        }
        if( entrySize < addressSize){
            throw Exception("The address size cann't be bigger than entry size!");
        }
    }

    void checkListOfIndexes(const std::vector<int>& indexes, const int entrySize) const{
        if((int)indexes.size() != entrySize){
            throw Exception("The list of indexes is not compatible with entry size!");
        }

        std::map<int, int> values;
        for(unsigned int i=0; i<indexes.size(); i++){
            if(indexes[i] >= entrySize){
                throw Exception("The list of indexes has a index out of range of entry!");
            }
            if(values.find(indexes[i]) == values.end()){
                values[indexes[i]] = i;
            }
            else{
                throw Exception("The list of indexes contain repeated indexes!");
            }
        }
    }

    int entrySize;
    std::vector<RAM> rams;
};


/*
 * Top-level class for the WiSARD weightless neural network model
 */
class Wisard{
public:
  
    // Constructor
    Wisard(uint8_t addressSize, int inputsize) 
    {
        this->addressSize = addressSize;
        makeDiscriminator(0, inputsize);
        makeDiscriminator(1, inputsize); 
    }

    // Destructor
    ~Wisard(){
        indexes.clear();
        discriminators.clear();
    }
    
    // Performs a training step for all discriminators
    void train(const std::vector<uint8_t>& input, const uint8_t label){
        discriminators[label].train(input);
    }
    
    // BEGINNING OF THE CLASSIFICATION PROCESS 
    uint8_t classify(const std::vector<uint8_t>& input){
        uint8_t output;
        std::vector<int>candidates = classify2(input);
        // Finding the best dicriminator-label candidate
        if (candidates[0] >= candidates[1]){
            output = 0;
        }
        else{
            output = 1;
        }
        return output;
        // END OF CLASSIFICATION PROCESS 
    }
    
    std::vector<int> classify2(const std::vector<uint8_t>& input){
        std::vector<std::vector<int>> allvotes (2);
        allvotes[0] = discriminators[0].classify(input);
        allvotes[1] = discriminators[1].classify(input);
        return Bleaching::make(allvotes);
    }

    // Creating discrimintors for each label
    void makeDiscriminator(uint8_t label, int entrySize){
        discriminators.resize(2);
        discriminators[size_t(label)] = Discriminator(addressSize, entrySize);
    }

    void checkInputSizes(const int inputSize, const int labelsSize){
        if(inputSize != labelsSize){
            throw Exception("The size of data is not the same of the size of labels!");
        }
    }

    uint8_t addressSize;
    std::vector<int> indexes;
    std::vector<Discriminator> discriminators;
};

}
#endif

