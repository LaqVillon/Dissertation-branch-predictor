import sys
import random
import numpy as np

NUM_BRANCH_PER_BLK = 10000
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
        with open(lst_fineName[0]) as f:
            i1P = f.read().splitlines()
        i1P = list(map(int,i1P))
        npTemp = np.array(i1P)
        brancDataSetI1_np = np.append(brancDataSetI1_np, [npTemp], axis=0)
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

    lst_fineName = ['I1-10k-10rows.csv', 'I1-10k-10rows.csv', 'I1-10k-10rows.csv', 'I1-10k-10rows.csv', 'I1-10k-10rows.csv', 'I1-10k-10rows.csv']
    numPreditores = len(lst_fineName)
    brancDataSetI1_np, numBlks = loadTest(lst_fineName)

    
    
    #Test I1 50%
    l_test_i1 = randTest(numBlks, IDPI1, numPreditores, 0.5) 
    random.shuffle(l_test_i1)
    #print(l_test_i1)

    totalHits = 0
    for i in range(numBlks):
        predUsed = (l_test_i1[i] + IDPI1)%numPreditores
        totalHits = totalHits + brancDataSetI1_np[predUsed][i]
    print('Total Hits', totalHits)

    #Uso para teste random
    print(randTest(numBlks, IDPI1, numPreditores, 0.5))
    print(randTest(numBlks, IDPI2, numPreditores, 0.5))
    print(randTest(numBlks, IDPS1, numPreditores, 0.5))
    print(randTest(numBlks, IDPS2, numPreditores, 0.5))
    print(randTest(numBlks, IDPM1, numPreditores, 0.5))
    print(randTest(numBlks, IDPM2, numPreditores, 0.5))
#END - main

if __name__ == "__main__":
    sys.exit(main())