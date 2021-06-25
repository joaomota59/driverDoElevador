# Driver do elevador

## Compilacao do driver
```shell
make -C /lib/modules/5.9.14/build M=`pwd` modules
```
## Carregando driver
```shell
insmod elevador.ko
```
## Verifica se o modulo foi carregado
```shell
lsmod|grep elevador
```
