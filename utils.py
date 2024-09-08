import csv
import numpy as np
import nibabel as nib
    
def write_csv(path, data):
    with open(path, mode='a', newline='') as file:
        iteration = csv.writer(file)
        iteration.writerow(data)
    file.close()
    
def rlog(value):
    return round(value, 3)


def save_vol(vol, path, voxel_sizes = (0.958, 0.958, 3.0)):
    vol = np.transpose(vol, (2, 1, 0))
    new_affine = np.diag([voxel_sizes[0], voxel_sizes[1], voxel_sizes[2], 1])
    new_img = nib.Nifti1Image(vol, affine=new_affine)
    nib.save(new_img, path)