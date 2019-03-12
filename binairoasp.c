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
	char out[dimension*dimension];
	int i;
	
	char command[64];
	
	snprintf(command, sizeof(command), "clingo -c dim=%d Binairo_EDB.txt Binairo_KB.txt --verbose=0", dimension);
	
	fp = popen(command, "r");

	c = getc(fp);
	for (i=0; c!=EOF; i++){
		if (c == '('){ 
			row = getc(fp); 	//fila
			getc(fp);			//ignorar coma
			col = getc(fp); 	//columna
			getc(fp);			//ignorar coma
			int posit = pos((row - '0'), (col - '0'), dimension);
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
	printf("\n");
}


void write_init(int *vector, int dimension){
	FILE *file;
	int i;
	
	file = fopen("Binairo_EDB.txt", "w+");
	
	for(i=0; i<dimension*dimension; i++){
		switch (vector[i]){
			case 1:
				fprintf(file, "pos(%d,%d,black). \n", row(i, dimension), col(i, dimension));
				break;
			case 0:
				fprintf(file, "pos(%d,%d,white). \n", row(i, dimension), col(i, dimension));
				break;
			default: 
				break;
		}
	}
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
                default:
                    break;
            }
        }
        
        fclose(file);
        write_init(vector, dim);
        free(vector);
        clingo_solve(dim);
	}
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("*** Usage: binarioasp <filename>\n");
        return 1;
    }
	
	read_file(argv[1]);
}
