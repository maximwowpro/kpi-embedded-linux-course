clear
<<<<<<< HEAD
<<<<<<< HEAD
sudo dmesg -C
sudo insmod ./lab3_module.ko num_threads=5 thread_inc_iterations=10000
=======
sudo insmod ./firstmode.ko num_threads=5 thread_inc_iterations=100
>>>>>>> af4ec21... lab3: MT and list
=======
sudo dmesg -C
sudo insmod ./lab3_module.ko num_threads=5 thread_inc_iterations=10000
>>>>>>> 6c878c9... Added lab 3 for the first review.
sleep 2s
dmesg
echo 
echo 
echo 
sudo rmmod lab3_module
dmesg
