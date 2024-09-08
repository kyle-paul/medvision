import numpy as np
import requests

# Define the server URL
url = "http://localhost:8000/v2/models/UNet_ImageCHD_128/infer"

# Create input data (example: an array of zeros)
input_data = np.zeros((24, 1, 128, 128), dtype=np.float32)

# Prepare the data in JSON format
inputs = [
    {
        "name": "input",
        "shape": input_data.shape,
        "datatype": "FP32",
        "data": input_data.tolist()
    }
]

outputs = [
    {
        "name": "output"
    }
]

request_payload = {
    "inputs": inputs,
    "outputs": outputs
}

# Send the request to the Triton server
response = requests.post(url, json=request_payload)

# Check the response status
if response.status_code == 200:
    response_json = response.json()
    print(response_json.keys())
    output_data = np.array(response_json["outputs"][0]["data"]).reshape(response_json["outputs"][0]["shape"])
    print("Output Data: ", output_data.shape)
    
else:
    print("Request failed with status code: ", response.status_code)
    print("Response: ", response.text)