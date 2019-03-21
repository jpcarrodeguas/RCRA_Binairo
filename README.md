Binairo_RCRA
============

Programa que resuelve puzles Binairo utilizando clasp y clingo.

## Compilación:

Toda la compilación puede hacerse utilizando el comando make, esto compilará
tanto la versión que resuelve el problema con SAT como la versión que lo
resuelve con ASP. De querer solo compilar una versión del problema,
puede utilizarse el comando make para compilar la versión correspondiente:
	
```bash
$ make 	   #compila las dos versiones
$ make sat #compila la versión sat, el ejecutable se llama binairo
$ make asp #compila la versión asp, el ejecutable se llama binairoasp 	
```

## Uso

Ambas versiones reciben la situación inicial como argumento, esta situación
inicial debe tener la dimensión del tablero en la primera linea del fichero,
seguida de una representación de la situación inicial con un caracter por celda
del tablero: '0' para representar una ficha blanca, '1' para representar una
ficha negra, '.' para representar una celda vacía.

```bash
$ binairo <inputfile>
$ binairoasp <inputfile>
```

Un ejemplo de fichero que el programa toma como situación inicial sería:

| 6 <br> ...... <br> .11.0. <br> 01.... <br> ....00 <br> .1.... <br> 0..1.. | ![alt text](https://www.dc.fi.udc.es/~cabalar/kr/current/initial.png "Situación inicial") |
|---------------------------------------------------------------------------|-------------------------------------------------------------------------------------------|

## Resultados
Tiempo que ambas versiones tardan en resolver los puzles utilizados como
benchmark (obtenidos con el comando time):

| Dimensión     | Binairo SAT   | Binairo ASP  |
| ------------- |--------------:| ------------:|
| 6*6	        |		0m0,013s|      0m0,017s|
| 8*8           |       0m0,018s|      0m0,021s|
| 10*10         |       0m0,022s|      0m0,027s|
| 14*14         |       0m0,116s|      0m0,050s|
| 20*20         |       0m2,674s|      0m0,106s|
| 30*30         |       1m9,422s|      0m0,336s|