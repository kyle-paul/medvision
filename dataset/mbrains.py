import os
import numpy as np
import SimpleITK as sitk
from SimpleITK import GetArrayFromImage as arr
from SimpleITK import ReadImage as rd


def refine_classes(vol_label):
    vol_label[vol_label == 4] = 0
    vol_label[vol_label == 5] = 4
    vol_label[vol_label == 6] = 5
    vol_label[vol_label == 7] = 6
    vol_label[vol_label == 8] = 7
    return vol_label

def data_preparation(dir: str, type:str, folder:str):
    
    if not os.path.exists(dir + f'{type}/images'):
        os.makedirs(dir + f'{type}/images')
        
    if not os.path.exists(dir + f'{type}/labels'):
        os.makedirs(dir + f'{type}/labels')
    
    index = 0
    range = 5 if folder == 'TrainData' else 15
    t = 5 if folder == 'TestData' else 0
        
    while index < range:
        vol_image = arr(rd(dir + f'{folder}/' + str(index+1) + '/T1.nii', sitk.sitkFloat32))
        choice = "/LabelsForTraining.nii" if type == "multi" else "/LabelsForTesting.nii" 
        vol_label = arr(rd(dir + f'{folder}/' + str(index+1) + choice))
        
        # if type == "multi" : 
        #     vol_label = refine_classes(vol_label)
        
        assert vol_image.shape == vol_label.shape, "image and label must have the same shape"        
        vol_image = (vol_image - np.min(vol_image)) / (np.max(vol_image) - np.min(vol_image))
        
        for slice, (image, label) in enumerate(zip(vol_image, vol_label)):
            np.save(dir + f'{type}/images/{index+t+1:04d}_{slice+1:04d}.npy', image)
            np.save(dir + f'{type}/labels/{index+t+1:04d}_{slice+1:04d}.npy', label)
            
        index+=1
        
    
if __name__ == "__main__":
    dir = "MBrainS/"
    data_preparation(dir, type="multi", folder="TrainData")
    data_preparation(dir, type="single", folder="TrainData")
    data_preparation(dir, type="single", folder="TestData")