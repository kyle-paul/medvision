## Run the container
docker run -it --gpus all -v /var/run/docker.sock:/var/run/docker.sock -v d/Documents/GitHub/MY-REPO/Mediv/deployment/model_analyzer:/workspace/model_analyzer --net=host nvcr.io/nvidia/tritonserver:24.06-py3-sdk


## Run model analyzer
model-analyzer profile \
    --model-repository /workspace/model_analyzer/ \
    --profile-models UNet_plusplus_MMWHS_128 --triton-launch-mode=remote \
    --output-model-repository-path /workspace/model_analyzer/UNet_plusplus_MMWHS_128/output \
    --export-path /workspace/model_analyzer/UNet_plusplus_MMWHS_128/profile_results \
    --override-output-model-repository


model-analyzer profile --model-repository /workspace/model_analyzer/ --profile-models UNet_MMWHS_128 --triton-launch-mode=remote --output-model-repository-path /workspace/model_analyzer/UNet_MMWHS_128/output --export-path /workspace/model_analyzer/UNet_MMWHS_128/profile_results --override-output-model-repository