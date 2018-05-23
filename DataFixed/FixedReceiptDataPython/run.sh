
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-9.0/lib64

#source activate tf16py27 

export CUDA_VISIBLE_DEVICES=0
export GPU_MEMORY_FRACTION=0.4
export NUM_WORKER=1
#export ENGINE_MODEL_ROOT_PATH=/mnt/data1/models
NUM_WORKER=$NUM_WORKER

BIND_ADDR=0.0.0.0:5005;

gunicorn -w $NUM_WORKER --threads 8 -t 120 -b $BIND_ADDR -p gunicorn.pid server:app > engine.log 2>&1 &
