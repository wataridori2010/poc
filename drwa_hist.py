import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

# http://www.lifewithpython.com/2014/01/python-flatten-nested-lists.html
def flattenf(nested_list):
    """2重のリストをフラットにする関数"""
    return [e for inner_list in nested_list for e in inner_list]

data=pd.read_csv('hist.txt')

df=data.as_matrix()
df2=flattenf(df)
plt.hist(df2,bins=50)