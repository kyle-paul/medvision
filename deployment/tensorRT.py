import os
import numpy as np
import onnxruntime as ort
import SimpleITK as sitk
import nibabel as nib
from skimage.transform import resize

def preprocessing(data: np.ndarray):
    max_data = np.max(data)
    min_data = np.min(data)    
    data = (data - min_data) / (max_data - min_data)
    return np.float16(data)

def iter_inference(batch_size: int, inputs: np.ndarray, 
                   engine_path: str, model_path:str):
    
    if not os.path.exists(engine_path):
        os.makedirs(engine_path)
        
    providers = [
        ('TensorrtExecutionProvider', {
            'device_id': 0,
            'trt_max_workspace_size': 2147483648,
            'trt_fp16_enable': True,
            'trt_engine_cache_enable': True,
            'trt_engine_cache_path': engine_path}),
        
        ('CUDAExecutionProvider', {})    
    ]
    
    total_images = inputs.shape[0]
    ort_session = ort.InferenceSession(model_path, providers=providers)
    outputs = np.zeros_like(inputs, dtype=np.int8)
        
    for i in range(0, inputs.shape[0], batch_size):
        end_index = min(i + batch_size, total_images)
        batch_inputs = inputs[i:end_index, np.newaxis, :, :]
        batch_outputs = inference(batch_inputs, ort_session)
        outputs[i:end_index] = batch_outputs
        break

    return outputs
    
def inference(input_batch: np.ndarray, ort_session):
    inp = {ort_session.get_inputs()[0].name: input_batch}
    out = ort_session.run(None, inp)
    output = np.argmax(out[0], axis=1)
    return output

def save_vol(vol: np.ndarray, type: str, path: str):
    vol = np.transpose(vol, (2, 1, 0))
    affine = np.eye(4)
    nifti_file = nib.Nifti1Image(vol.astype(np.int8), affine) if type=='labels' else nib.Nifti1Image(vol, affine)
    nib.save(nifti_file, path)  

def post_process(output, output_path):
    print("Run TensorRT successfully")
    save_vol(output, "labels", output_path)

if __name__ == "__main__":
    
    # User choose the sample data
    img_size = 128
    dataset = "MSBrain"
    network = "RoTransNet"
    model_name = f"{network}_{dataset}_{img_size}"
    
    # Processing at client
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    data_path = os.path.join(base_dir, f"dataset/samples/inputs/{dataset}.nii.gz")
    data = sitk.GetArrayFromImage(sitk.ReadImage(os.path.join(base_dir, data_path)))
    model_path = os.path.join(base_dir, f"deployment/model_repository/{model_name}/1/model.onnx")
    engine_path = os.path.join(base_dir, f"deployment/model_repository/{model_name}/1")
    
    if (data.shape[1] != img_size):
        data = resized_array = resize(data, (data.shape[0], img_size, img_size), mode='reflect', anti_aliasing=True)
        
    # Preprocessing -> AI Server -> Post processing
    inputs = preprocessing(data)
    outputs = iter_inference(
        batch_size=24, 
        inputs = inputs,
        engine_path = engine_path, 
        model_path = model_path
    )
    post_process(outputs, "segmentation.nii.gz")