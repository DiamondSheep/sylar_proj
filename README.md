# A CPP Project

## Introduction
Following this [video](https://www.bilibili.com/video/BV184411s7qF?from=search&seid=13149288556532331318&spm_id_from=333.337.0.0&vd_source=465f813b30d56d4ab8ca252aa30ca3f5) to develop a C++ server framework.

Thanks to Sylar.

## How to build

```
# install required libs
sudo apt update
sudo apt install libboost-dev libyaml-cpp-dev

# load code from github
git clone https://github.com/DiamondSheep/sylar_proj.git
cd sylar_proj

# build and run test (you can modify CMakeLists.txt for testing different module)
sh run.sh
```
## How to use
### 1. Logger
Logger is a basic component of this project to print information in different level.
```
// Initialize (or customized)
sylar::Logger g_logger = SYLAR_LOG_ROOT();
// Print
SYLAR_LOG_INFO(g_logger) << "log information";
```

## Planning
* [x] Log
* [x] Config
* [x] Thread