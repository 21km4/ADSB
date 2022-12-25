gcc adsb_G9.c -o run_G9 -lm -O2
timeout 10 ./run_G9 all/testcase1/idata out.txt all/testcase1/answer