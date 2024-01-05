# Conditional Branch Predictor Based on Weightless Neural Networks

Code and algorithms used in my Master's Thesis.

## Versions and technologies:
  - C++14
  - Python 3.9.4  

## Datasets:

  - The "Dataset_pc_decimal" folder provides the 6 datasets used in this work. 
  - In each file, the first column represents the PC in decimal and the second column represents the real outcome of the actual branch

## In order to run experiments:

  - In terminal make the apropriate compilation: $ sh compiler.sh
  - Then, execute in tterminal using command line arguments: $ ./bpu-wisard Dataset_pc_decimal/[name of dataset] [n] [a] [b] [c] [d0] [d1] [d2] [d3] [d4] [e]
    where:
      - [name of dataset]: is the full name of one of the files located in the "Dataset_pc_decimal" folder
      - [n]: is the size of the n-tuple
      - [a], [b], [c], [d0], [d1], [d2], [d3], [d4], [e]: are the parameters discussed in the dissertation
  - For example, after compiling, execute: $ ./bpu-wisard Dataset_pc_decimal/M1.txt 32 2 2 2 2 2 3 4 1 1
    The terminal will show something like this:
       
      ----- Results ------
      Predicted  branches: 282678
      Not predicted branches: 17322
      Accuracy: 94.226000
     
      ------ Size of ntuple (address_size): 32 -----

      ------ Size of each input: 348 -----
  - The  "Specialized-preditors" directory collets the results of the specialized predictor for each dataset
