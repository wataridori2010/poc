import pandas as pd
import numpy as np
import math
import random
import matplotlib.pyplot as plt
from scipy.interpolate import UnivariateSpline
from copy import deepcopy
import math


%matplotlib inline
#df=pd.read_csv('VID_20180303_185221_path.csv')
df=pd.read_csv('c-phone/gyro/20180311_222134.csv')
#df=pd.read_csv('c-phone/gyro/20180311_221622.csv')
#df=pd.read_csv('c-phone/gyro/20180310_180246.csv')



frame=np.array(df.iloc[:,[0]]).reshape(-1,1)
x=np.array(df.iloc[:,[1]]).reshape(-1,1)
#x=x[0:2000]
totallen=len(x)

offset=196#64

x_all1    =[]
x_mod1=x[offset]
x_mod1_pre=x[offset]
th1        =0.08#0.043
rec1      =1#0.995#95#0.995

x_all2    =[]
x_mod2=0
rec2=0.997

bias_old=0
bias_all=[]
rec_cur=rec1
rec_seq=rec1
v_old=0
rate=0.5

plt.figure(figsize=(16, 8))

length=len(x)
#for i in range(offset, totallen-offset):
y_pred_1_pre=0
y_pred_1_curr=0

cur_off=64

loop_num=10#5
beta = 1
alpha = 0.25#15

old_num=16#16#32
new_num=4
x2_rec           =np.zeros(old_num+new_num)
x2_rec_tmp  =np.zeros(old_num+new_num)
x2_rec_old    =np.zeros(old_num+new_num)


for i in range(offset, length-offset):

    
    # smoothed x
    x_=np.mean(x[i-old_num:i+new_num])
    
    if i==offset:
        x2_rec=np.copy(x[i-old_num:i+new_num])
        x2_rec_old=np.copy(x[i-old_num:i+new_num])
    
    # newest sample
    #x2_rec[old_num+new_num-1] = np.copy(x[i+new_num-1])    
    
    #off
    off_v=0
    
    # optimization
    for s in range(1, loop_num):
        
        # target val
        for j in range(-old_num, new_num):
            v_sm = x2_rec[j+old_num]
            v_rl = x[i+j]
            sumv = np.copy(v_rl) # raw value
            sumw = 1            
            #filtering each pos
            for k in range(-old_num, new_num):
                if j != k:
                    # original
                    weightl = math.exp( -float(j - k)*float(j - k)/(100.0) )                
                    weight = math.exp( -(x2_rec[j+old_num] - x2_rec[k+old_num])*(x2_rec[j+old_num] - x2_rec[k+old_num])/(3*th1*th1) )
                    weight = weight * weightl
                    # modified 
                    #weight = (th1/(th1+abs(x2_rec[j+old_num] - x2_rec[k+old_num])) )* (th1/(th1+abs(x2_rec[j+old_num] - x2_rec[k+old_num])) )      
                    sumv=sumv+x2_rec[k+old_num]*weight*alpha
                    sumw=sumw+weight*alpha

            x2_rec_tmp[j+old_num]=  sumv/sumw + beta*(x2_rec_old[j+old_num]-x2_rec[j+old_num])
            
            '''
            # limitation
            if( x2_rec_tmp[j+old_num]-x[i+j] >th1):
                x2_rec_tmp[j+old_num] = x[i+j] +th1  
                off_v = -x2_rec_tmp[j+old_num] + th1
                
            elif(x[i+j]-x2_rec_tmp[j+old_num] >th1):        
                x2_rec_tmp[j+old_num]=x[i+j] -th1
                off_v = x2_rec_tmp[j+old_num] - th1

            '''
        x2_rec=np.copy(x2_rec_tmp) - off_v
        
    x_mod1=x2_rec[old_num+2]
    
    
    for j in range(1, old_num+new_num):
        x2_rec[j-1]=np.copy(x2_rec[j])

    
    x2_rec_old = np.copy(x2_rec)
    
    '''
    if( x_mod1-x[i] >th1):
        
        x_mod1=x[i] +th1
        
    elif(x[i]-x_mod1 >th1):
        
        x_mod1=x[i] -th1

    '''
        
    #x_mod1_pre=x_mod1
    x_all1.append(x_mod1)

    
x_all1=np.array(x_all1)
plt.plot(x_all1)
plt.plot(x[offset:length-offset])