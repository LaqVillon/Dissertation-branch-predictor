import sys
import random
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def main():
    # ****************************************************************************************
    # ************************************ Reading data **************************************
    # ****************************************************************************************

    original_dataset_name = ['I1', 'I2', 'S1', 'S2', 'M1', 'M2']
    for name_dataset in original_dataset_name:
        list_percent_classfiers = [0.5, 0.6, 0.7, 0.8, 0.9]
        list_blocks_sizes = ['1k', '2k', '5k', '10k']
        dict_acc = {0.5: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}, 
                    0.6: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0},
                    0.7: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}, 
                    0.8: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0},
                    0.9: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}}
        dict_std = {0.5: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}, 
                    0.6: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0},
                    0.7: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}, 
                    0.8: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0},
                    0.9: {'1k': 0.0, '2k': 0.0, '5k': 0.0, '10k': 0.0}}
        # print(dict_acc)
        # for m in list_datasets:
        for l in list_percent_classfiers:
            for k in list_blocks_sizes: 
                path = 'Results/' + name_dataset + '/' + str(int(l * 100)) + '%/'
                name_file = k + '.csv'
                df = pd.read_csv(path + name_file)
                # print(df)
                acc = round(df['Accuracy'].mean(), 4)
                std = round(df['Accuracy'].std(), 4)
                # print(acc, std)
                dict_acc[l][k] = acc
                dict_std[l][k] = std
        print(dict_acc)
        print(dict_std)

        list_1k = [dict_acc[i]['1k'] for i in dict_acc]
        list_2k = [dict_acc[i]['2k'] for i in dict_acc]
        list_5k = [dict_acc[i]['5k'] for i in dict_acc]
        list_10k = [dict_acc[i]['10k'] for i in dict_acc]
        print(list_1k)
        print(list_2k)
        print(list_5k)
        print(list_10k)
        

        # ****************************************************************************************
        # ********************************** Building figures ************************************
        # ****************************************************************************************
        esann_acc = {'I1':99.7948, 'I2':99.9749, 'M1':96.0540, 'M2':86.4968, 'S1':96.0651, 'S2':97.8504}
        best_predictors_acc = {'I1':99.8067, 'I2':99.9786, 'M1':96.1322, 'M2':88.1783, 'S1':96.8521, 'S2':98.0946}
        dict_ylimits = {'I1': {'bottom': 99.7940, 'top': 99.8070}, 
                        'I2': {'bottom': 99.9740, 'top': 99.9800}, 
                        'M1': {'bottom': 95.70, 'top': 96.20},
                        'M2': {'bottom': 86.40, 'top': 88.20},
                        'S1': {'bottom': 96.00, 'top': 96.90},
                        'S2': {'bottom': 97.40, 'top': 98.10}}
        # Limits of the y axis        
        plt.ylim(dict_ylimits[name_dataset]['bottom'], dict_ylimits[name_dataset]['top'])
        
        # The ESANN first and the best predictor results
        plt.axhline(y = esann_acc[name_dataset], color = 'purple', linestyle = '-', linewidth=1.8, label="WNN") 
        plt.axhline(y = best_predictors_acc[name_dataset], color = 'black', linestyle = '-', linewidth=1.8, label="Best") 
        
        # set width of bar
        barWidth = 0.20

        # Set position of bar on X axis
        br1 = np.arange(len(list_1k))
        br2 = [x + barWidth for x in br1]
        br3 = [x + barWidth for x in br2]
        br4 = [x + barWidth for x in br3]
    
        # Make the plot
        plt.bar(br1, list_1k, color ='r', width = barWidth, edgecolor ='grey', label ='1k')
        plt.bar(br2, list_2k, color ='g', width = barWidth, edgecolor ='grey', label ='2k')
        plt.bar(br3, list_5k, color ='b', width = barWidth, edgecolor ='grey', label ='5k')
        plt.bar(br4, list_10k, color ='grey', width = barWidth, edgecolor ='grey', label ='10k')

        # Adding Xticks
        plt.xlabel('Classifier precision (%)', fontsize = 14)
        # plt.ylabel('Accuracy (%)', fontsize = 10)
        plt.xticks([r + barWidth*1.5 for r in range(len(list_1k))], ['50', '60', '70', '80', '90'], fontsize = 15)
        plt.yticks(fontsize = 15)

        # Legend
        plt.legend(loc='lower right')

        # Save figure
        plt.savefig(name_dataset + "-classifier.jpg", bbox_inches='tight')
        plt.clf() 


if __name__ == "__main__":
    sys.exit(main())
