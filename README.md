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
## Sobre o driver...
* O driver permite a escrita dos dados de entrada através do arquivo de device.
* Os dados escritos serão a entrada do algoritmo(sequência de inteiros em que o primeiro número refere-se ao último cilindro, seguido pelo cilindro sobre o qual a cabeça de leitura está inicialmente possicionada e as requisições de acesso). Sempre que lido, o driver deve retornar quantidade total de cilindros percorridos pela cabeça de leitura para atender todas as requisições de acesso ao disco da última entrada enviada ao driver.
