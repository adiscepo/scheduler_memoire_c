demo_dev=$1
folder=/Users/attilio/Desktop/Code/rp2040_kernel

echo "Run C version"
echo "" > $folder/plot/output_rt_demo.log
# screen -d -m minicom -b 115200 -o -D $demo_dev -C $folder/plot/output_rt_demo.log
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program $folder/build/kernel.elf verify reset exit"
source $folder/venv/bin/activate
python3 $folder/plot/plot_act.py $folder/plot/output_rt_c.log &> /dev/null
echo "C version launched ($$)"