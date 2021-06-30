# Driver do elevador

## Primeiramente entrar no modo de superusuário
```shell
sudo su
```

## Compilacao do driver
```shell
make -C /lib/modules/5.9.14/build M=`pwd` modules
```
> Obs: Onde tem 5.9.14 coloque a versao do seu kernel!

## Carregando driver
```shell
insmod elevador.ko
```
## Verifica se o módulo foi carregado
```shell
lsmod|grep elevador
```
ou
```shell
ls /dev/elevador
```

## Envio de entrada para o driver
```shell
echo "199 53 98 183 37 122 14 124 65 67">/dev/elevador
```
## Obtendo resultado do driver
```shell
cat /dev/elevador
```
## Caso queira apagar arquivo de device criado
```shell
rmmod elevador
```
## Sobre a entrada
A entrada é composta por uma série de números inteiros, indicando primeiro o número do último cilindro no disco (os cilindros variam de 0 até este número), o cilindro sobre o qual a cabeça de leitura está inicialmente posicionada e a sequência de requisições de acesso.
