/*----------------------------------------------------------------------------*/
/* File: tem.c                                                             */
/* Date: 13/03/2006                                                           */
/* Author: Zhiyi Huang                                                       */
/* Version: 0.1                                                               */
/*----------------------------------------------------------------------------*/
 
/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/aio.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/string.h>
#include <linux/sort.h>
//#include "math.h"
//#include "/usr/include/stdlib.h"

//Macros utilizadas para especificar informações relacionadas ao modulo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhiyi Huang");
MODULE_DESCRIPTION("A elevadorlate module");


/* Valor major igual a zero quando for alocar um major dinamicamente */
int major=0;
//Especifica parâmetros do módulo. variável, tipo e permissões usadas no sysfs
//S_IRUGO - Permissão de leitura para todos os usuários
module_param(major, int, S_IRUGO);
//MODULE_PARM_DESC é utilizado para documentar uma variável
MODULE_PARM_DESC(major, "device major number");

#define MAX_DSIZE	3071
struct my_dev {
        char data[MAX_DSIZE+1];
        size_t size;              
        struct semaphore sem;     /* Para implementar exclusão mútua */
        struct cdev cdev;
	struct class *class;
	struct device *device;
} *elevador_dev;



int elevador_open (struct inode *inode, struct file *filp)
{
	return 0;
}

int elevador_release (struct inode *inode, struct file *filp)
{
	return 0;
}



int cmpfunc (const void * a, const void * b) {//função para auxilar no qsort
   return ( *(int*)a - *(int*)b );
}

static int compare(const void *lhs, const void *rhs) {
    int lhs_integer = *(const int *)(lhs);
    int rhs_integer = *(const int *)(rhs);

    if (lhs_integer < rhs_integer) return -1;
    if (lhs_integer > rhs_integer) return 1;
    return 0;
}


int algoritmoDoElevador(char *arr){
    int bordaDoDisco,quantElemento,elementoInicial,posDoElementoInicial,soma;
    int flagBordaDoDisco;
    int tamanhoString,i;
    flagBordaDoDisco = 0;
    quantElemento = 0;//quantidade de elementos que foram passados
    tamanhoString = strlen(arr);//tamanho da string de entrada

	for(i=0;i< tamanhoString;i++){
		if(arr[i]==' '){
            quantElemento++;//quantidade de numeros no vetor menos a primeira entrada
          }
	}
   int vetorElementos[MAX_DSIZE+1];
   
   int contador;//contador para o vetor de elementos
   int auxToken;
   contador = -1;
   
   //Split de todos numeros passados
   char * token = strsep(&arr, " ");//split da string passada, strsep, funcao do kernel, separa a string em tokens
   
   char *ptr;
   bordaDoDisco = (int)simple_strtol(token,&ptr,10);//pega o primeiro numero que é a borda do disco
   
   
   while( contador < quantElemento ) {//while necessario para fazer o split
      //printf( " %s\n", token ); //printing each token
      
      
      auxToken = simple_strtol(token,&ptr,10);
      if(auxToken>bordaDoDisco && contador!=-1){//se algum elemento for maior que a borda do disco...
		flagBordaDoDisco = 1;
		break;  
      }
      if(contador!=-1){
      	vetorElementos[contador] = auxToken;//pega todos elementos menos o primeiro
      }
	
      if(token!=NULL){//so separa os tokens so se ainda tiver ainda como separar 
	token = strsep(&arr, " ");
      }	
	contador = contador + 1;
      
	/*  
      	
	*/

   }
   
 
   
   if(flagBordaDoDisco == 1){ //entao é porque a entrada passada é maior que a borda do disco entao não pode
		return -1;
   }
   
   
   elementoInicial = vetorElementos[0];//primeiro elemento, antes de ordenar
   
   sort(vetorElementos, quantElemento, sizeof(int), &compare,NULL);//função que ordena a entrada em ordem crescente
   
   for(i=0;i<quantElemento;i++){
   	
		   if(vetorElementos[i]==elementoInicial){
			posDoElementoInicial = i; //indice do elemento inicial no vetor ordenado
            break;
	   }
   }
   

   soma = 0;
   for (i=posDoElementoInicial;i<quantElemento-1;i++){//todos elementos a frente do elemento inicial no vetor ordenado
		  soma+=abs(vetorElementos[i+1]-vetorElementos[i]);//ou seja vai no sentido mais externo do disco
   }
  
      
   for (i=posDoElementoInicial;i>0;i--){
		  if (i==posDoElementoInicial){//condicao para pegar o ultimo elemento do loop anterior e subtrair com o anterior do primeiro elemento
			 soma += abs(vetorElementos[quantElemento-1] - vetorElementos[i-1]);
		  }
		  else{
			  soma += abs(vetorElementos[i]-vetorElementos[i-1]);
		  }
   }

	return soma;
	
	
	
}







// função para leitura do arquivo de device
//filp - referencia para o arquivo de device
//buf - area no espaço do usuário onde o dado lido será colocado
//count - quantidade de dados lidos
//f_pos - posição atual de leitura
ssize_t elevador_read (struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
/*    if (copy_to_user(buf, elevador_dev->data, 1) != 0)
        return -EFAULT;
    else
        return 1;
*/
	int rv=0;

printk(KERN_WARNING " f_pos: %lld; count: %zu;\n", *f_pos, count);



	if (down_interruptible (&elevador_dev->sem))//Inicio da regiao critica
		return -ERESTARTSYS;
	if (*f_pos > MAX_DSIZE)//verifica se já leu tudo do buffer do device  
		goto wrap_up;     
	if (*f_pos + count > MAX_DSIZE)//Verifica se ainda tem dados para ler e se é maior que o permitido
		count = MAX_DSIZE - *f_pos;//Ajusta count 
    //Função para copiar dados do espaço do kernel(elevador_dev->data) para
	//o espaço do usuário(buf). Count  é a quantidade de dados a ser copiado.
	
	int resultado;
	resultado = algoritmoDoElevador(elevador_dev->data);//passa para funcao a string recebida com echo
	
	char resultadoEmString[MAX_DSIZE+1];
	
	
	if (resultado == -1){//deu errado, algum elemento passado eh maior que a borda do disco
		strcpy(elevador_dev->data,"Impossivel Calcular- Existe um elemento maior que a borda do disco!\n");
		strcpy(resultadoEmString,"Impossivel Calcular- Existe um elemento maior que a borda do disco!\n");
	}
	else{//se der certo, entao joga o resultado obtido da funcao do elevador
		snprintf(resultadoEmString,MAX_DSIZE+1,"%d",resultado);//converte o resultado de inteiro para string
		strcat(resultadoEmString,"\n");
		strcpy(elevador_dev->data,resultadoEmString);
	}
	  
	if (copy_to_user (buf, elevador_dev->data+*f_pos, strlen(resultadoEmString))) {
		rv = -EFAULT;
		goto wrap_up;
	}
	up (&elevador_dev->sem);
	*f_pos += count;//Avança a posição atual de leitura
	return count;

wrap_up:
	up (&elevador_dev->sem);
	return rv;

}

ssize_t elevador_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int count1=count, rv=count;

	if (down_interruptible (&elevador_dev->sem))
		return -ERESTARTSYS;
    //verifica se f_pos ultrapassa o limite máximo, se ultrapassar, encerra
	if (*f_pos > MAX_DSIZE)
		goto wrap_up;
	//verifica se a quantidade de dados a ser escrita somada à posição atual do offset
  	//ultrapassa o limite máximo
	if (*f_pos + count > MAX_DSIZE)
		//Se ultrapassar, utiliza count1 para gravar apenas o que não ultrapasse esse limite
		count1 = MAX_DSIZE - *f_pos;


	if (copy_from_user (elevador_dev->data+*f_pos, buf, count1)) {
		rv = -EFAULT;
		goto wrap_up;
	}
	up (&elevador_dev->sem);
	*f_pos += count1;
	return count;

wrap_up:
	up (&elevador_dev->sem);
	return rv;
}

long elevador_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{

	return 0;
}

loff_t elevador_llseek (struct file *filp, loff_t off, int whence)
{
        long newpos;

        switch(whence) {
        case SEEK_SET:
                newpos = off;
                break;

        case SEEK_CUR:
                newpos = filp->f_pos + off;
                break;

        case SEEK_END:
                newpos = elevador_dev->size + off;
                break;

        default: /* can't happen */
                return -EINVAL;
        }
        if (newpos<0 || newpos>MAX_DSIZE) return -EINVAL;
        filp->f_pos = newpos;
        return newpos;
}

struct file_operations elevador_fops = {
        .owner =     THIS_MODULE,
	.llseek =    elevador_llseek,
        .read =      elevador_read,
        .write =     elevador_write,
        .unlocked_ioctl = elevador_ioctl,
        .open =      elevador_open,
        .release =   elevador_release,
};


/**
 * Inicializa o módulo e cria o dispositivo
 */
int __init elevador_init_module(void){
	int rv;
	//Macro para converter major e minor number do tipo dev_t
	dev_t devno = MKDEV(major, 0);

	if(major) {
		//Registra o device com major devno 
		//e 1 é a quantidade de devices, 
		//elevador é o nome do device
		rv = register_chrdev_region(devno, 1, "elevador");
		if(rv < 0){
			printk(KERN_WARNING "Can't use the major number %d; try atomatic allocation...\n", major);
			rv = alloc_chrdev_region(&devno, 0, 1, "elevador");
			major = MAJOR(devno);
		}
	}
	else {
        //Aloca um major dinamicamente para o device e registra-o. 
		//0 - minor number 1 - numero de devices
		rv = alloc_chrdev_region(&devno, 0, 1, "elevador");
		major = MAJOR(devno);
	}

	if(rv < 0) return rv;
    //Alocação de memória fisica para colocar a estrutura do device. 
	//GFP_KERNEL bloqueia o processo se a memória 
	//não está imediatamente disponível
	elevador_dev = kmalloc(sizeof(struct my_dev), GFP_KERNEL);
	if(elevador_dev == NULL){
		rv = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return rv;
	}
    //Inicializa a memória alocada com kmalloc
	memset(elevador_dev, 0, sizeof(struct my_dev));
	//cdev - Estrutura para dispositivos de caractere
	//Inicializa a estrutura cdev do dispositivo
	//elevador_fops - especifica as funções que implementam as
	//operações sobre o arquivo de device a serem registradas no VFS
	cdev_init(&elevador_dev->cdev, &elevador_fops);
	elevador_dev->cdev.owner = THIS_MODULE;
	elevador_dev->size = MAX_DSIZE;
	sema_init (&elevador_dev->sem, 1);
	//Entrega a estrutura cdev ao VFS
	rv = cdev_add (&elevador_dev->cdev, devno, 1);
	if (rv) printk(KERN_WARNING "Error %d adding device elevador", rv);
    //UDEV- Deamon responsável por criar o arquivo de device 
	//a partir das informações fornecidas
	
	//Cria uma classe para uso em device_create
	elevador_dev->class = class_create(THIS_MODULE, "elevador");
	if(IS_ERR(elevador_dev->class)) {
		cdev_del(&elevador_dev->cdev);
		unregister_chrdev_region(devno, 1);
		printk(KERN_WARNING "%s: can't create udev class\n", "elevador");
		rv = -ENOMEM;
		return rv;
	}
    //Cria arquivo de device em /dev automaticamente
	elevador_dev->device = device_create(elevador_dev->class, NULL,
					MKDEV(major, 0), "%s", "elevador");
	if(IS_ERR(elevador_dev->device)){
		class_destroy(elevador_dev->class);
		cdev_del(&elevador_dev->cdev);
		unregister_chrdev_region(devno, 1);
		printk(KERN_WARNING "%s: can't create udev device\n", "elevador");
		rv = -ENOMEM;
		return rv;
	}

	printk(KERN_WARNING "Hello world from elevadorlate Module\n");
	printk(KERN_WARNING "elevador device MAJOR is %d, dev addr: %lx\n", major, (unsigned long)elevador_dev);

  return 0;
}


/**
 * Finaliza o módulo. Desfaz tudo que foi feito na inicalização.
 */
void __exit elevador_exit_module(void){
	device_destroy(elevador_dev->class, MKDEV(major, 0));
	class_destroy(elevador_dev->class);
	cdev_del(&elevador_dev->cdev); 
	kfree(elevador_dev);
	unregister_chrdev_region(MKDEV(major, 0), 1);
	printk(KERN_WARNING "Good bye from late Module\n");
}

//Funções de callback chamadas na inicialização e no desligamento do módulo.
module_init(elevador_init_module);
module_exit(elevador_exit_module);


