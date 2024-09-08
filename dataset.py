import torch
from torch.utils.data import Dataset
import numpy as np
from scipy.ndimage import zoom

class CustomDataset(Dataset):
    def __init__(self, num_classes, image_paths, label_paths, img_size):
        '''
        Args:
            num_classes (int): Number of classes.
            image_paths (str): Image file paths.
            label_paths (str): Label file paths.
            img_size    (int): Training image size
        
        Note:
            Make sure to process the data into this structures
            <dataset name>
            ├── p_images
            |   ├── 0001_0001.npy
            │   ├── 0001_0002.npy
            │   ├── 0001_0003.npy
            │   ├── ...
            |
            └── p_labels
                ├── 0001_0001.npy
                ├── 0001_0002.npy
                ├── 0001_0003.npy
                ├── ...     
        '''
        self.num_classes = num_classes
        self.image_paths = image_paths
        self.label_paths = label_paths
        self.img_size = img_size
        self.length = len(image_paths)
        
    def __len__(self):
        return self.length
    
    def __getitem__(self, index):
        image = np.load(self.image_paths[index])
        label = np.load(self.label_paths[index])
        
        x, y = image.shape
        if x != self.img_size and y != self.img_size:
            image = zoom(image, (self.img_size / x, self.img_size / y), order=0)
            label = zoom(label, (self.img_size / x, self.img_size / y), order=0)
        
        encoded_label = np.zeros( (self.num_classes, ) + label.shape)
        for i in range(self.num_classes): 
            encoded_label[i][label == i] = 1
        
        return torch.tensor(image).unsqueeze(0), torch.tensor(encoded_label)