# Use the Nvidia PyTorch base image
FROM nvcr.io/nvidia/pytorch:24.05-py3

# Copy the cudnn tarball file to the Docker image
COPY cudnn-linux-x86_64-8.9.7.29_cuda12-archive.tar.xz /tmp/

# Extract the tarball
RUN tar -xvf /tmp/cudnn-linux-x86_64-8.9.7.29_cuda12-archive.tar.xz -C /tmp/

# Copy the necessary files to their respective directories
RUN cp /tmp/cudnn-*-archive/include/cudnn*.h /usr/local/cuda/include && \
    cp -P /tmp/cudnn-*-archive/lib/libcudnn* /usr/local/cuda/lib64 && \
    chmod a+r /usr/local/cuda/include/cudnn*.h /usr/local/cuda/lib64/libcudnn*

# Clean up
RUN rm -rf /tmp/cudnn-*-archive /tmp/cudnn-linux-x86_64-8.9.7.29_cuda12-archive.tar.xz

# Set the LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/cuda/lib64

RUN apt-get update && apt-get install -y \
    curl \
    && apt-get clean


ADD --chmod=755 https://astral.sh/uv/install.sh /install.sh
RUN /install.sh && rm /install.sh

# Set uv environment path
ENV PATH=${PATH}:/root/.cargo/bin

# Install ONNX Runtime GPU with Azure
RUN uv pip install --system onnxruntime-gpu --extra-index-url https://aiinfra.pkgs.visualstudio.com/PublicPackages/_packaging/onnxruntime-cuda-12/pypi/simple/

# Install dependencies from requirements.txt file with uv
COPY requirements.txt /tmp/
RUN uv pip install --system -r /tmp/requirements.txt --no-deps

# Set the working directory back to the root
WORKDIR /workspace