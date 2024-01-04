import sys
import random
import numpy as np
import pandas as pd


IDPI1 = 0
IDPI2 = 1
IDPS1 = 2
IDPS2 = 3
IDPM1 = 4
IDPM2 = 5


def loadTest(lst_fineName):
    with open(lst_fineName[0]) as f:
        i1P = f.read().splitlines()
    i1P = list(map(int,i1P))
    NUM_BLKS_BRANCH = len(i1P)
    brancDataSetI1_np = np.empty(shape=[0, NUM_BLKS_BRANCH])
    npTemp = np.array(i1P)
    brancDataSetI1_np = np.append(brancDataSetI1_np, [npTemp], axis=0)

    for i in range(1, len(lst_fineName)):
        with open(lst_fineName[i]) as f:
            i1P = f.read().splitlines()
        i1P = list(map(int,i1P))
        npTemp = np.array(i1P)
        brancDataSetI1_np = np.append(brancDataSetI1_np, [npTemp], axis=0)
    # print(brancDataSetI1_np)
    return brancDataSetI1_np, NUM_BLKS_BRANCH
# END - loadTest


def randTest(numBlks, testID, numPreditores, percenOtimo):
    l_test = [testID]*numBlks
    percentOtimo = int(len(l_test)*percenOtimo)
    for i in range(len(l_test)-percentOtimo):
        temp = (random.randrange(1, 6, 1) + testID)%numPreditores
        #print(temp)
        while temp == testID:
            temp = (random.randrange(1, 6, 1) + testID)%numPreditores
            #temp = random.randrange(1, 6, 1)
        l_test[i] = temp
    return l_test
# END - randTest


def main():
    # original_dataset_name = 'S1'  # Dataset
    # percent_otimo = 0.5           # classifier hit precission
    # size_block_experiment = '1k'  # Size of block of instructions
    
    list_datasets = ['I1', 'I2', 'S1', 'S2', 'M1', 'M2']
    # list_percent_classfiers = [0.5]
    # list_percent_classfiers = [0.6, 0.7, 0.8, 0.9]
    list_percent_classfiers = list()
    for i in range(51, 100):
        if(i % 10 != 0):
           list_percent_classfiers.append(i/100)
    # print(list_percent_classfiers)
    # list_blocks_sizes = ['1k', '2k', '5k', '10k']
    list_blocks_sizes = ['1k']

    size_blocks = {'1k': 1000, '2k': 2000, '5k': 5000, '10k': 10000, '20k': 20000,
                            '50k': 50000, '100k': 100000}
    dict_best_predictors = {'I1': IDPI1, 'I2': IDPI2, 'S1': IDPS1, 'S2': IDPS2, 'M1': IDPM1, 'M2': IDPM2}
    
    for m in list_datasets:
        for l in list_percent_classfiers:
            for k in list_blocks_sizes: 
                NUM_BRANCH_PER_BLK = size_blocks[k]
                lst_fineName = ['best-predictor-I1/' + m + '-' + k + '.csv', 
                                'best-predictor-I2/' + m + '-' + k + '.csv', 
                                'best-predictor-S1/' + m + '-' + k + '.csv', 
                                'best-predictor-S2/' + m + '-' + k + '.csv', 
                                'best-predictor-M1/' + m + '-' + k + '.csv', 
                                'best-predictor-M2/' + m + '-' + k + '.csv']
                numPreditores = len(lst_fineName)
                brancDataSetI1_np, numBlks = loadTest(lst_fineName)
                
                list_hits = list()
                list_acc = list()
                for j in range(0, 100):
                    # Test I1 50%
                    
                    l_test_i1 = randTest(numBlks, dict_best_predictors[m], numPreditores, l) 
                    # print(l_test_i1)
                    random.shuffle(l_test_i1)
                    # print(l_test_i1)
                    
                    # Hits calculation
                    totalHits = 0
                    for i in range(numBlks):
                        #predUsed = (l_test_i1[i] + IDPS1)%numPreditores
                        predUsed = (l_test_i1[i])
                        hits_block = brancDataSetI1_np[predUsed][i]
                        print(hits_block, predUsed)
                        totalHits = totalHits + hits_block
                    # print('Total Hits', totalHits)
                    list_hits.append(totalHits)
                    acc = round((totalHits / (NUM_BRANCH_PER_BLK * len(l_test_i1))) * 100, 4)
                    # print('Accuracy', acc)
                    list_acc.append(acc)

                data = {'Hits': list_hits, 'Accuracy': list_acc}
                df = pd.DataFrame(data)
                path = 'Results/' + m + '/' + str(int(l * 100)) + '%/'
                name_file = k + '.csv'
                df.to_csv(path + name_file, index=False)
#END - main


if __name__ == "__main__":
    sys.exit(main())
