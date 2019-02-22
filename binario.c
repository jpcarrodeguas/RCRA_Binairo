#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int *init_vector(int dimension) {
    return malloc(dimension*dimension*sizeof(int));
}

void clasp_solve(int dimension){
	FILE *fp;
	char c;
	char out[4096];
	char *token;
	const char s[2] = " ";
	int i;

	fp = popen("clasp binairo.cnf --verbose=0", "r");

	c = getc(fp);
	for (i=0; c!=EOF; i++){
		out[i]=c;
		c = getc(fp);
	}
	
	pclose(fp);
	
	fp = fopen("binairo.txt", "w+");
	
	token = strtok(out, s);
	while( token != NULL ) {
		if(!strcmp(token, "v")){
			token = strtok(NULL, s);
			continue;
		}
		if(!strcmp(token, "s")){
			token = strtok(NULL, s);
			continue;
		}
		if(!strcmp(token, "SATISFIABLE")){
			token = strtok(NULL, s);
			continue;
		}
		i=atoi(token);
		if(i > 0){
			fprintf(fp, "%d", 1);
		}
		else if(i <  0){
			fprintf(fp, "%d", 0);
			i=-i;
		}
		if(((i-1) % dimension) == dimension-1){
			fprintf(fp, "\n");
		}
		token = strtok(NULL, s);
	}

	
	
	
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

int same_number_of_each_rule(FILE *file, int n, int *vector) {
    // El array 'vector' contiene la configuración de fichas
    int count = 0;
    int i, j, aux;

    for (i = 0; i < n; i++) {
        // Rule for row
        for (j = 0; j < n; j++) {
            aux = (j+1+n*i) * vector[j];
            fprintf(file, "%d ", aux);
        }
        fprintf(file, "0\n");
        count += 1;
        // Rule for column
        for (j = 0; j < n; j++) {
            aux = (i+j*n+1) * vector[j];
            fprintf(file, "%d ", aux);
        }
        fprintf(file, "0\n");
        count +=1;
    }

    return count;
}

void gen_combinations(int counter, int *vector, int length) {
    int i;

    if (counter > 0) {
        i = length-counter-2;
        if (i >= 0) {
            if (vector[i] == 0 && vector[i+1] == 0) {
                vector[length-counter] = 1;
                gen_combinations(counter-1, vector, length);
                return;
            } else if (vector[i] == 1 && vector[i+1] == 1) {
                vector[length-counter] = 0;
                gen_combinations(counter-1, vector, length);
                return;
            }
        }
        vector[length-counter] = 0;
        gen_combinations(counter-1, vector, length);
        vector[length-counter] = 1;
        gen_combinations(counter-1, vector, length);
    } else {
        int sum = 0;
        for (i = 0; i < length; i++) {
            sum += vector[i];
        }
        if (sum == (length/2)+1 || sum == (length/2)-1) {
            for (i = 0; i < length; i++) {
                printf(" %d ", vector[i]);
            }
            printf("\n");
        }
    }
}

void write_rules(int *vector, int dimension){
	FILE *file, *tmpfile;
	int rulenum = 0;
	char c;
    int i;
	
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

    int *vector2 = malloc(sizeof(int)*dimension); // para testear 'same_number_of_each_rule'

    //rulenum += same_number_of_each_rule(tmpfile, dimension, vector2);
	
    //gen_combinations(dimension, vector2, dimension);
	
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
    free(vector2);

	clasp_solve(dimension);
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
