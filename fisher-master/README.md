# 构建说明

## 依赖

* SDL2
* libqrencode

```bash
sudo apt install libsdl2-dev libqrencode-dev
```

## 构建

```bash
cd build
make
```

> 如果无法正常构建请尝试通过 [makemake](https://github.com/hubenchang0515/makemake) 重新生成 `Makefile`

```bash
cd build
makemake
make
```