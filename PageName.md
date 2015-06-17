# 系统组成 #

记录仪无线通讯系统，由\*无线终端**、**无线基站\*和管理计算机组成。
**无线通讯单元\*连接到记录仪RS232通信接口组成\*无线终端**，并通过Zigbee协议和无线基站通信。
无线基站通过Zigbee协议从无线终端读取数据并存储到FLASH文件系统中。
管理计算机和无线基站模块通过以太网链接，可以从无线基站中下载数据。


# 工作流程 #

当载有记录仪的车辆进入无线基站的通讯范围后，无线通讯单元自动和基站建立链接，基站向无线通讯单元发出数据读取指令，无线通讯单元接收指令后，通过串口读取记录仪的数据，并将数据转发给基站。基站读取完整数据后将数据保存为USB磁盘交换文件格式。

对于多台车辆的情况，基站采用排队方式，依次分别读取各记录仪数据。

# 无线通讯单元 #

## 硬件介绍 ##

无线通信单元两种，一种用于记录仪，一种用于基站。他们在硬件上完全相同，只是加载不同的程序。

无线通信单元USB电缆提供电源，通过RS232接口和其他设备交换数据。

除电源LED指示灯外，另外有三个状态指示LED，正常工作时1、3号指示灯亮。

此外还有两个按钮，1号按钮用于人工启动网络接入。2号按钮暂时未定义。

## 软件编译 ##

  * 将zstack解压，在工程文件在ZStack\Projects\zstack\Utilities\SerialApp\CC2430DB目录下

  * 使用IAR FOR MS51 7.3B打开工程文件，选择Workspace->EndDevice编译,生成记录仪终端模块无线通信单元的代码

  * 使用IAR FOR MS51 7.3B打开工程文件，选择Workspace->Cordnator编译,生成基站模块无线通信单元的代码

## 烧写 ##

  * 连接JTAG仿真器，USB接PC，另一端连接目标板

  * 在打开的IAR软件中，切换到需要烧写模块的Workspace, 然后选择Debug, 当软件提示下载完成后，即可终止调试。这时目标板已经烧写成功。

  * 也可以使用TI提供的CC24xx系列的烧写工具进行

# 基站模块 #

基站模块分两部分，一部分是基站控制单元，另一部分是无线通信单元。无线通信单元的程序编译烧制如前所述，参见无线终端的描述。这里主要介绍控制单元的硬件和软件设置

## 硬件 ##

控制单元硬件为一ARM嵌入式系统，通过RS232串口和无线通信单元连接，通过USB接口为通信单元供电。

## 软件 ##

### 交叉编译环境配置 ###

**准备Linux操作系统**

建议安装Ubuntu发行版的Linux系统，具体参考ubuntu网站，或其他资料
网站地址  http://www.ubuntu.org.cn/
下载光盘镜像文件（ISO）后刻录到光盘上安装

**在Linux系统中安装交叉编译器**

基本操作是将编译器包解压缩到自选的目录，一般建议解压缩到/opt目录下
在此项目中使用的版本是arm-none-linux-gnueabi-gcc 4.3.2版

**对于arm-linux-gcc-4.3.2.tgz编译器压缩包,将压缩包拷贝到/tmp目录下，执行以下命令解压缩
```
cd /
sudo tar -xvzf /tmp/arm-linux-gcc-4.3.2.tgz
```**

### 烧写控制单元操作系统 ###

开发版提供的已经烧写好的系统不能满足需要，需要烧写自己定制的操作系统和文件系统才能满足功能要求。

#### 烧写前准备 ####

下载安装SAM-BA软件(Windows 和 Linux均可)，及解压缩文件到自选目录

##### 环境配置 #####
如果在Linux环境下，可能需要如下配置
```
sudo bash
rmmod usbserial
modprobe usbserial vendor=0x03eb product=0x6124
exit
```
连接USB后可使用以下命令验证是否连接成功
```
lsusb -d 03eb:6124
    Bus 004 Device 006: ID 03eb:6124 Atmel Corp
dmesg
    ...
    kernel: usb 4-2: new full speed USB device using uhci_hcd and address 5
    kernel: usb 4-2: configuration #1 chosen from 1 choice
    kernel: usbserial_generic 4-2:1.0: generic converter detected
    kernel: usbserial_generic: probe of 4-2:1.0 failed with error -5
    kernel: usbserial_generic 4-2:1.1: generic converter detected
    kernel: usb 4-2: generic converter now attached to ttyUSBx

```

#### 烧写步骤 ####

1  将核心版SW1，SW2置于OFF位，链接USB电缆

2  启动SAM-BA软件，选择board type(at91sam9261ek)和端口(/dev/ttyUSB1)

3  选择Nandflash

4  选择Enable Nandflash script并执行

5  选项 执行 Erase All script

6  选择 SendBootFile, 并执行, 选择 BootStrap文件

7  发送U-Boot bin 文件到 Nandflash 地址 0x20000

8  发送linux核心uImage bin文件到 nandflase 地址 0xA0000

9  传送r根文件系统ram-disk image(gziped)到地址address 0x400000

10 SW1 on,选择Dataflash标签

11 执行EnableDataflash脚本

12 (optional)执行EraseAll脚本

13 执行SendBootFile脚本

14 传送U-Boot bin文件到dataflash, 地址0x8400

15 传送linux uImage bin文件到data-flash 地址 0x42000

16 输入 exit 退出SAM-BA

17 重启核心板系统

18 登入核心板系统，安装nandflash盘，将ram盘拷贝到nandflash。
```
mount -t yaffs2 /dev/mtdblock1 /mnt
/bin/init-mnt-fs.sh
```

19 重新启动核心板系统，修改U-boot配置，从nandflash启动

```
setenv bootcmd cp.b 0xC0042000 0x20400000 0x300000\; bootm 0x20400000
setenv bootargs mem=64M console=ttyS0,115200 root=/dev/mtdblock1 rw rootfstype=yaffs2
setenv bootdelay 0
saveenv
```

以上烧写步骤也可以使用脚本完成。



### 软件编译 ###

**编译控制软件**

解压缩控制源程序到目录。
进入QueenBee子目录，运行
```
arm-none-linux-gnueabi-g++ -o qb *.cpp
```
生成可执行文件qb

**上传控制软件**

将开发板接入网络，缺省地址是192.168.1.77，用户root/000000
执行下面命令将可执行文件拷贝到控制单元
```
scp qb root@192.168.1.77:/
```

**执行程序
```
/qb /dev/ttyS2
```**


# 系统演示 #

## 准备工作 ##

  1. 确认已经将qb拷贝到控制单元
  1. 确认基站控制单元连接到/dev/ttyS2端口

## 操作步骤 ##

  1. 从记录仪下载数据
> > 通过ssh连接到基站模块，在linux下执行以下命令
```
ssh root@192.168.1.77
```
> > 在windows环境下，可下载ssh客户端[putty](http://the.earth.li/~sgtatham/putty/latest/x86/putty.exe)软件连接
  1. 登录系统后，运行
```
/qb /dev/ttyS2 1 0
```

> qb程序的参数
```
/qb <通讯端口设备路径，/dev/ttyS2>  <是否使用空中协议，1：使用，0：不用> <调试信息输出，0最多>
```
> 程序执行结束后，应该在根目录下生成TEST.IBB文件
  1. 在Windows系统下，安装WinScp程序，启动WinScp后将IBB文件拷贝到Windows系统，就可以象读U盘一样导入数据了。