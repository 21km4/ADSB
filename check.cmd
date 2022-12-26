@echo off

if not exist check_G9.exe gcc -o check_G9 evaluate.c
check_G9 out1.txt all/testcase1/answer
check_G9 out2.txt all/testcase2/answer
check_G9 out3.txt all/testcase3/answer
check_G9 out4.txt all/testcase4/answer
check_G9 out5.txt all/testcase5/answer
check_G9 out6.txt all/testcase6/answer
check_G9 out7.txt all/testcase7/answer
check_G9 out8.txt all/testcase8/answer
check_G9 out9.txt all/testcase9/answer
check_G9 out10.txt all/testcase10/answer