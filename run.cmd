@echo off

gcc adsb_G9.c -o run_G9 -lm -O2
timeout 10 ./run_G9 all/testcase1/idata out1.txt all/testcase1/answer
timeout 10 ./run_G9 all/testcase2/idata out2.txt all/testcase2/answer
timeout 10 ./run_G9 all/testcase3/idata out3.txt all/testcase3/answer
timeout 10 ./run_G9 all/testcase4/idata out4.txt all/testcase4/answer
timeout 10 ./run_G9 all/testcase5/idata out5.txt all/testcase5/answer
timeout 10 ./run_G9 all/testcase6/idata out6.txt all/testcase6/answer
timeout 10 ./run_G9 all/testcase7/idata out7.txt all/testcase7/answer
timeout 10 ./run_G9 all/testcase8/idata out8.txt all/testcase8/answer
timeout 10 ./run_G9 all/testcase9/idata out9.txt all/testcase9/answer
timeout 10 ./run_G9 all/testcase10/idata out10.txt all/testcase10/answer