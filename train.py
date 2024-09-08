import os
import yaml
import ml_collections
from glob import glob
import torch
from dataset import CustomDataset
from sklearn.model_selection import train_test_split
from torch.utils.data import DataLoader
from trainer import Trainer

from networks import RoTransNet, rot_cfg
from networks import RCT_UNet, rct_config
from networks import UNet_plusplus

def read_yaml(file_path: str):
    with open(file_path, 'r') as file:
        return yaml.safe_load(file)
    
def setup():
    config = read_yaml("configs/train_config.yaml")
    config_dict = ml_collections.ConfigDict(config)
    
    print(f"=> Initialize model: {config_dict.network}")
    print(config_dict)
    
    name = f"bs{config_dict.batch_size}_opt{config_dict.optimizer}_res{config_dict.img_size}"
    config_dict.log_dir = f"outputs/{config_dict.network}/{config_dict.dataset}/{name}"
    
    if not os.path.exists(config_dict.log_dir):
        os.makedirs(config_dict.log_dir)
        with open(f"{config_dict.log_dir}/config.yml", "w") as f:
            yaml.dump(config, f)
            
    return config_dict

def prepare_data(config):
    image_paths = glob(f"dataset/{config.dataset}/images/*.npy")
    label_paths = glob(f"dataset/{config.dataset}/labels/*.npy")
    
    if config.range != 'None': 
        image_paths = image_paths[:config.range]
        label_paths = label_paths[:config.range]
    
    train_image_paths, val_image_paths, train_label_paths, val_label_paths = train_test_split(
            image_paths,
            label_paths, 
            test_size=0.2,
            random_state=41,
            shuffle=False,
        )
    train_ds = CustomDataset(config.num_classes, train_image_paths, train_label_paths, img_size=config.img_size)
    val_ds = CustomDataset(config.num_classes, val_image_paths, val_label_paths, img_size=config.img_size)

    train_loader = DataLoader(
        train_ds,
        batch_size=config.batch_size,
        shuffle=False,
        num_workers=config.num_workers,
        drop_last=False,
    )
    
    val_loader = DataLoader(
        val_ds,
        batch_size=config.batch_size,
        shuffle=False,
        num_workers=config.num_workers,
        drop_last=False,
    )
    return train_loader, val_loader 
    

def prepare_model(config): 
    if config.network == 'RoTransNet':
        model_config = rot_cfg()
        model_config.img_size = config.img_size
        model_config.num_classes = config.num_classes
        model = RoTransNet(config=model_config)
        
    elif config.network == "RCT_UNet":
        model_config = rct_config()
        model_config.img_size = config.img_size
        model_config.num_classes = config.num_classes
        model = RCT_UNet(config=model_config)
        
    elif config.network == "UNet_plusplus":
        model = UNet_plusplus(config.num_classes)
                    
    else: 
        raise "Network name is wrong"
    
    return model.cuda()

def prepare_optimizer(config, model):
    
    params = filter(lambda p: p.requires_grad, model.parameters())
    if config.optimizer == 'Adam':
        optimizer = torch.optim.Adam(params, lr=config.base_lr, weight_decay=config.weight_decay)
    elif config.optimizer == 'SGD':
        optimizer = torch.optim.SGD(params, lr=config.base_lr, momentum=config.momentum,
        nesterov=config.nesterov, weight_decay=config.weight_decay)
    elif config.optimizer == 'AdamW':
        optimizer = torch.optim.AdamW(params)
        
    return optimizer

    
if __name__ == "__main__":
    config = setup()
    model = prepare_model(config)
    optimizer = prepare_optimizer(config, model)
    train_loader, val_loader = prepare_data(config)
    
    if config.dataset=="MBrainS/multi":
        cls = ['Cort gray matter', 'Basal ganglia', 'White matter', 'White matter lesions',
               'Cerebrospinal fluid', 'Ventricles', 'Cerebellum', 'Brainstem']
        class_weight=[1., 1., 1., 1., 1., 1., 1., 1.5, 1.75]
        
    elif config.dataset=="MBrainS/single":
        cls = ['Cerebrospinal fluid', 'Gray matter', 'White matter']
        class_weight=[1., 1.5, 1., 1.]
        
    elif config.dataset=="ImageCHD":
        cls = ["Left atrium", "Right atrium", "Left ventricle", "Right ventricle", "Myocardium", "Aorta", "Pulmonary Artery"]
        class_weight = None
        
    elif config.dataset=="MMWHS":
        cls = ["Left ventricle", "Right ventricle", "Left atrium", "Right atrium", "Myocardium of left ventricle", "Ascendind aorta", "Pulmonary Artery"]
        class_weight = None
        
    trainer = Trainer (
        config=config,
        model=model,
        train_loader=train_loader,
        val_loader=val_loader,
        optimizer=optimizer,
        classes=cls,
        class_weight=class_weight,
    )