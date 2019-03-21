#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int pos(int row, int col, int dim){
	return ((row-1)*dim+(col-1));
}

int row(int pos, int dim){
	return (pos/dim)+1;
}

int col(int pos, int dim){
	return (pos%dim)+1;
}

void clingo_solve(int dimension){
	FILE *fp;
	char c, row, col, color;
	char *out=malloc(dimension*dimension*sizeof(char));
	int i;
	char command[64];
	char rowchars[3];
	char colchars[3];
	
	
	snprintf(command, sizeof(command), "clingo -c dim=%d Binairo_KB.txt Binairo_EDB.txt --verbose=0", dimension);
	
	fp = popen(command, "r");

	c = getc(fp);
	for (i=0; c!=EOF; i++){
		if (c == '('){ 
			rowchars[0] = getc(fp); 	//primer posible numero de fila
			c = getc(fp);
			
			if (c != ','){				//si leo una coma, el numero es de 1 sola cifra
				rowchars[1] = c;		//he leido otro numero, lo guardo en el array
				colchars[2] = '\0';		//null terminated string para que el atoi lea solo hasta donde debe
				row=atoi(rowchars);		//genero número de fila con los dos numeros
				getc(fp);				//como máximo, va a haber 2 numeros, puedo ignorar la coma
			}else{						//he leido una coma, el numero es una sola cifra
				rowchars[1] = '\0';		//null terminated string para que el atoi lea solo hasta donde debe
				row=atoi(rowchars);		//genero número de fila con el numero que lei al principio
			}

			colchars[0] = getc(fp); 	//primer posible numero de columna
			c = getc(fp);
			
			if (c != ','){				//si leo una coma, el numero es de 1 sola cifra
				colchars[1] = c;		//he leido otro numero, lo guardo en el array
				colchars[2] = '\0';		//null terminated string para que el atoi lea solo hasta donde debe
				col=atoi(colchars);		//genero número de columna con los dos numeros
				getc(fp);				//como máximo, va a haber 2 numeros, puedo ignorar la coma
			}else{						//he leido una coma, el numero es una sola cifra
				colchars[1] = '\0';		//null terminated string para que el atoi lea solo hasta donde debe
				col=atoi(colchars);		//genero número de columna con el numero que lei al principio
			}

			int posit = pos(row , col, dimension);
			
			color = getc(fp);
			switch (color){
				case 'b' :
					out[posit]=1;
					break;
				case 'w':
					out[posit]=0;
					break;
				default:
					break;
			}
		}
		c = getc(fp);
	}
	
	pclose(fp);
	
	fp = fopen("binairoasp.txt", "w+");
	for (i=0; i<dimension*dimension; i++){
		fprintf(fp, "%d", out[i]);
		printf("%d", out[i]);
		if (((i) % dimension) == dimension-1){
			fprintf(fp, "\n");
            printf("\n");
		}
	}
	
	free(out);
}


void write_init(int *vector, int dimension){
	FILE *file;
	int i;
	
	file = fopen("Binairo_EDB.txt", "w+");
	
	for(i=0; i<dimension*dimension; i++){
		switch (vector[i]){
			case 1:
				fprintf(file, "pos(%d,%d,black).\n", row(i, dimension), col(i, dimension));
				break;
			case 0:
				fprintf(file, "pos(%d,%d,white).\n", row(i, dimension), col(i, dimension));
				break;
			default: 
				break;
		}
	}

    fclose(file);
}

void read_file(char *filepath){
	int dim, c, i=0;
	FILE *file = NULL;
	int *vector;
	
	file = fopen(filepath, "r");
	
	if(file){
		
		fscanf (file, "%d", &dim);
		
		vector = malloc(dim*dim*sizeof(int));
		
		while ((c = getc(file)) != EOF) {
            switch (c) {
                case '.':
                    vector[i] = -1;
                    i++;
                    break;
                case '0':
                    vector[i] = 0;
                    i++;
                    break;
                case '1':
                    vector[i] = 1;
                    i++;
                    break;
                case '\n':
                    break;
                default:
                    printf("Error!\n");
                    return;
            }
        }
        
        fclose(file);

        printf("Generating initial configuration...\n");
        write_init(vector, dim);
        free(vector);

        printf("Solving...\n");
        clingo_solve(dim);
        printf("\nDone! (solution also in 'binairoasp.txt')\n");
	}
}

int main(int argc, char **argv) {

    printf("Binairo ASP - RCRA P1\n\n");

    if (argc != 2) {
        printf("*** Usage: binarioasp <filename>\n");
        return 1;
    }

    printf("Input file: %s\n\n", argv[1]);
	
	read_file(argv[1]);

    return 0;
}
