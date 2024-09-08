import sys
import os
import yaml
import torch
import ml_collections

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from networks import RoTransNet, rot_cfg
from networks import UNet_plusplus

def read_yaml(base_dir, file_path):
    with open(os.path.join(base_dir, file_path, "config.yml"), 'r') as file:
        return yaml.safe_load(file)

def prepare_model(config, base_dir, file_path, version): 
    if config.network == 'RoTransNet':
        model_config = rot_cfg()
        model_config.num_classes = config.num_classes
        model_config.img_size = config.img_size
        model_config.num_layers = config.num_layers
        model_config.img_size = config.img_size
        model_config.num_classes = config.num_classes
        model = RoTransNet(config=model_config)
        model.load_state_dict(torch.load(os.path.join(base_dir, file_path, "RoTransNet.pt")))
        input = torch.rand(config.batch_size, 1, config.img_size, config.img_size).to(torch.float32)
        export_onnx(base_dir, model, input, f"RoTransNet_{config.dataset}_{config.img_size}", version)
        
    elif config.network == "UNet_plusplus":
        model = UNet_plusplus(config.num_classes)
        model.load_state_dict(torch.load(os.path.join(base_dir, file_path, "UNet_plusplus.pt")))
        input = torch.rand(config.batch_size, 1, config.img_size, config.img_size).to(torch.float32)
        export_onnx(base_dir, model, input, f"UNet_plusplus_{config.dataset}_{config.img_size}", version)
    

def export_onnx(base_dir, model, input, name_model, version):
    save_path = os.path.join(base_dir, f"deployment/model_repository/{name_model}/{version}")
    os.makedirs(save_path)
    model.eval()
    torch.onnx.export(
        model, input, os.path.join(save_path, "model.onnx"),
        export_params=True, opset_version=10,
        input_names=["input"], output_names=["output"],
        # dynamic_axes={"input": {0: "batch_size"}, "output": {0: "batch_size"}}
    )
        
        
if __name__ == "__main__":
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    file_path = "outputs/RoTransNet/MSBrain/bs24_optSGD_res128"
    config = read_yaml(base_dir, file_path)
    config_dict = ml_collections.ConfigDict(config)
    prepare_model(config_dict, base_dir, file_path, 1)