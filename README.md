# packet_mmap
packet mmap

  利用packet mmap抓包分析，逐层提取数据 ethernet->ip->tcp->app
单独输出应用层16进制字符串方便解析，展示完整帧信息.

1. make clean && make && make install

2. cd bin

3. ./capture eth0

![image](https://github.com/yanfanc/packet_mmap/blob/master/test.PNG)

