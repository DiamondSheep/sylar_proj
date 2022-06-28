# A CPP Project

## Introduction
Following this [video](https://www.bilibili.com/video/BV184411s7qF?from=search&seid=13149288556532331318&spm_id_from=333.337.0.0&vd_source=465f813b30d56d4ab8ca252aa30ca3f5) to develop a C++ server framework.

Thanks to Sylar.

## How to build

```
# install required libs
sudo apt update
sudo apt install libboost-dev libyaml-cpp-dev

git clone https://github.com/DiamondSheep/sylar_proj.git
cd sylar_proj

# build and run
sh run.sh
```
## How to use
```
sylar::Logger g_logger = 
sylar::LoggerManager::GetInstance()->getLogger(name);

SYLAR_LOG_INFO(g_logger) << "log information";
```

## Planning
* [x] Log
* [ ] Config