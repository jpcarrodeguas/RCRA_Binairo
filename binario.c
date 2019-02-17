#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int *init_vector(int dimension) {
    return malloc(dimension*dimension*sizeof(int));
}

void clasp_solve(){
	execv("clasp --verbose=0 binairo.cnf > claspout.txt", NULL);
}

int three_consecutive_rule(FILE *file, int n) {
    int count = 0;
    int i, j, aux;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n-2; j++) {
            aux = i*n+j;
            fprintf(file, "%d %d %d 0\n", aux+1, aux+2, aux+3);
            fprintf(file, "-%d -%d -%d 0\n", aux+1, aux+2, aux+3);
            fprintf(file, "%d %d %d 0\n", j*n+i+1, (j+1)*n+i+1, (j+2)*n+i+1);
            fprintf(file, "-%d -%d -%d 0\n", j*n+i+1, (j+1)*n+i+1, (j+2)*n+i+1);
            count += 4;
        }
    }

    return count;
}

void write_rules(int *vector, int dimension){
	FILE *file, *tmpfile;
	int rulenum = 0;
	char c;
    int i, j;
	
	tmpfile = fopen("tmp.cnf", "w+");
	
	for(i=0; i<dimension*dimension; i++){
		switch (vector[i]){
			case 1:
				fprintf(tmpfile, "%d 0\n", (i+1));
				rulenum++;
				break;
			case 0:
				fprintf(tmpfile, "-%d 0\n", (i+1));
				rulenum++;
				break;
			default: 
				break;
		}
	}
    
    rulenum += three_consecutive_rule(tmpfile, dimension);
	
	file = fopen ("binairo.cnf", "w+");
	fprintf(file, "p cnf %d %d\n", (dimension*dimension), rulenum);
	
	fseek(tmpfile, 0, SEEK_SET);
	
	c = fgetc(tmpfile); 
    while (c != EOF) 
    { 
        fputc(c, file); 
        c = fgetc(tmpfile); 
    } 
	
	fclose(tmpfile);
	remove("tmp.cnf");
	fclose(file);

	clasp_solve();
}

int read_file(char *filepath) {
    int c, dimension;
    FILE *file;
    file = fopen(filepath, "r");
    int *vector;

    if (file) {		
		fscanf (file, "%d", &dimension);  		//Leemos el primer caracter para saber cual es la dimensión del problema
		
        vector = init_vector(dimension);		//Inicializamos el vector

        int i = 0;

        while ((c = getc(file)) != EOF) {		// Transcribimos la situación inicial 1 = 1; 0 = 0; Vacio = -1 
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
        
        /*
        for (i = 0; i < (dimension*dimension); i++){
            printf("%3d", vector[i]);
            if ((i % dimension) == 5) {
                printf("\n");
            }
            //putchar(c);
        }
        */
        
        write_rules(vector, dimension);
        
        fclose(file);
        free(vector);
    }
    return dimension;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("*** Usage: binario <filename>\n");
        return 1;
    }

    printf("*** Input file: %s\n", argv[1]);

    int i = read_file(argv[1]);

    printf("Dimension: %d\n", i);

    return 0;
}
