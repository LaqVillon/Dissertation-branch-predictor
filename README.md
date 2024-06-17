# Conditional Branch Predictor Based on Weightless Neural Networks

Code and algorithms used in my Master's Thesis. You can found the final text here https://www.pesc.coppe.ufrj.br/index.php/en/publicacoes-pesquisa/details/20/3137

## Requirements:
  - C++14
  - Python 3.9.4  

## Datasets:

  - The "Dataset_pc_decimal" folder provides the 6 datasets used in this work. 
  - In each file, the first column represents the PC in decimal and the second column represents the real outcome of the actual branch

## In order to run experiments:

  - In terminal make the apropriate compilation:
    ```
    $ sh compiler.sh
    ```
  - Then, execute in tterminal using command line arguments:
    ```
    $ ./bpu-wisard Dataset_pc_decimal/[name of dataset] [n] [a] [b] [c] [d0] [d1] [d2] [d3] [d4] [e]
    ```
    where:
      - [name of dataset]: is the full name of one of the files located in the "Dataset_pc_decimal" folder
      - [n]: is the size of the n-tuple
      - [a], [b], [c], [d0], [d1], [d2], [d3], [d4], [e]: are the parameters discussed in final text of the dissertation
  
    For example, after compiling, execute:
    ```
    $ ./bpu-wisard Dataset_pc_decimal/M1.txt 32 2 2 2 2 2 3 4 1 1
    ```
    The terminal will show something similar to this:
    ```   
      ----- Results ------
    
      Predicted  branches: 282678
    
      Not predicted branches: 17322
    
      Accuracy: 94.226000
     
      ------ Size of ntuple (address_size): 32 -----

      ------ Size of each input: 348 -----
    ```
  - The  "Specialized-preditors" directory collets the results of the specialized predictor for each dataset. Execute the python scripts to print the corresponding bar plots.

## Articles

Some of this work was done in collaboration with researchers from other institutions. Our efforts have been recognized in two publications:
- [Neurocomputing](https://www.sciencedirect.com/science/article/abs/pii/S0925231223007609)
- [ESANN](https://www.esann.org/proceedings/2022)

## License

[MIT License](LICENSE)
