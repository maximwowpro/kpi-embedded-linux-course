clear
sudo insmod ./firstmode.ko num_threads=5 thread_inc_iterations=100
sleep 2s
sudo rmmod firstmode
