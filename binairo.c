#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define PBSTR "############################################################"
#define PBWIDTH 60

void print_progress (double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
} /* by razzak [https://stackoverflow.com/questions/14539867/] */

void clasp_solve(int dimension){
	FILE *fp;
	char c;
	char out[4096];
	char *token;
	const char s[2] = " ";
	int i;
    int count = 0;

	fp = popen("clasp binairo.cnf --verbose=0", "r");

	c = getc(fp);
	for (i=0; c!=EOF; i++){
		out[i]=c;
		c = getc(fp);
        if (i == 4095) {
            break;
        }
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
            printf("%d", 1);
            count++;
		}
		else if(i <  0){
			fprintf(fp, "%d", 0);
            printf("%d", 0);
            count++;
			i=-i;
		}
		if(((i-1) % dimension) == dimension-1){
			fprintf(fp, "\n");
            printf("\n");
		}
        if (count == dimension*dimension) {
            break;
        }
		token = strtok(NULL, s);
	}

}

int three_consecutive_rule(FILE *file, int n, int fprogress) {
    int count = 0;
    int i, j, aux;

    for (i = 0; i < n; i++) {
        if (fprogress) {
            print_progress(((double) i+1) / n);
        }
        for (j = 0; j < n-2; j++) {
            // Rules for rows
            aux = i*n+j;
            fprintf(file, "%d %d %d 0\n", aux+1, aux+2, aux+3);
            fprintf(file, "-%d -%d -%d 0\n", aux+1, aux+2, aux+3);
            // Rules for columns
            fprintf(file, "%d %d %d 0\n", j*n+i+1, (j+1)*n+i+1, (j+2)*n+i+1);
            fprintf(file, "-%d -%d -%d 0\n", j*n+i+1, (j+1)*n+i+1, (j+2)*n+i+1);
            // Increment rule counter
            count += 4;
        }
    }
    if (fprogress) {
        printf("\n");
    }

    return count;
}

int same_config_rule(FILE *file, int n, int *rulenum, int fprogress) {
    int count = 0;
    int z = (n*n);
    int i, j, k;
    int p, q;
    int buffer[n+n];

    for (i = 0; i < n; i++) {
        if (fprogress) {
            print_progress(((double) i+1) / n);
        }
        for (j = i+1; j < n; j++) {
            for (k = 0; k < n; k++) {
                // Rules for rows
                p = k+i*n+1;
                q = k+j*n+1;
                z++;
                buffer[k+k] = z;

                fprintf(file, "-%d %d %d 0\n", z, p, q);
                fprintf(file, "-%d -%d -%d 0\n", z, p, q);
                fprintf(file, "%d -%d %d 0\n", z, p, q);
                fprintf(file, "%d %d -%d 0\n", z, p, q);

                // Rules for columns
                p = k*n+i+1;
                q = k*n+j+1;
                z++;
                buffer[k+k+1] = z;

                fprintf(file, "-%d %d %d 0\n", z, p, q);
                fprintf(file, "-%d -%d -%d 0\n", z, p, q);
                fprintf(file, "%d -%d %d 0\n", z, p, q);
                fprintf(file, "%d %d -%d 0\n", z, p, q);

                // Increment extravariables count and number of rules
                count += 2;
                (*rulenum) += 8;
            }
            // Extravariables rows
            for (k = 0; k < n+n; k+=2) {
                fprintf(file, "%d ", buffer[k]);
            }
            fprintf(file, "0\n");
            // Extravariables columns
            for (k = 1; k < n+n; k+=2) {
                fprintf(file, "%d ", buffer[k]);
            }
            fprintf(file, "0\n");
            (*rulenum) += 2;
        }
    }
    if (fprogress) {
        printf("\n");
    }

    return count;
}

int same_number_of_each_rule(FILE *file, int n, int *vector, int type, int fprogress, int *initialconf) {
    // El array 'vector' contiene la configuración de fichas, type para indicar que ficha nos interesa
    int count = 0;
    int i, j, aux;
    char buffer[80];
    int aux2;

    for (i = 0; i < n; i++) {
        if (fprogress) {
            print_progress(((double) i+1) / n);
        }
        // Rule for row
        sprintf(buffer, "\r");
        aux2 = 1;
        for (j = 0; j < n; j++) {
            if (vector[j] == type) {
                aux = (j+n*i);// * vector[j];
                //printf("%d %d ", aux, initialconf[aux]);
                if (initialconf[aux] == 0 || initialconf[aux] != vector[j]) {
          //          printf("%d %d %d ", aux, initialconf[aux], vector[j]);
                    aux = (aux+1) * vector[j];
                    sprintf(buffer + strlen(buffer), "%d ", aux);
                } else {
                    aux2 = 0;
                    break;
                }
               // fprintf(file, "%d ", aux);
            }
        }
      // printf("\n");
       // fprintf(file, "0\n");
       if (aux2) {
            fprintf(file, "%s0\n", buffer);
            count++;
        }
        // Rule for column
        sprintf(buffer, "\r");
        aux2 = 1;
        for (j = 0; j < n; j++) {
            if (vector[j] == type) {
                aux = (i+j*n);// * vector[j];
                if (initialconf[aux] == 0 || initialconf[aux] != vector[j]) {
                    aux = (aux+1) * vector[j];
                    sprintf(buffer + strlen(buffer), "%d ", aux);
                } else {
                    aux2 = 0;
                    break;
                }
               // fprintf(file, "%d ", aux);
            }
        }
       // fprintf(file, "0\n");
        if (aux2) {
            fprintf(file, "%s0\n", buffer);
            count++;
        }
    }
   // printf("\n");
    

    return count;
}

void gen_combinations(int counter, int *vector, int length, int *rulenum, FILE *file, int fprogress, int *initialconf) {
    // El 0 es -1
    int i, sum;

    if (counter > 0) {
        i = length-counter-2;
        if (i >= 0) {
            if (vector[i] == -1 && vector[i+1] == -1) {
                vector[length-counter] = 1;
                gen_combinations(counter-1, vector, length, rulenum, file, fprogress, initialconf);
                return;
            } else if (vector[i] == 1 && vector[i+1] == 1) {
                vector[length-counter] = -1;
                gen_combinations(counter-1, vector, length, rulenum, file, fprogress, initialconf);
                return;
            }
        }
        vector[length-counter] = -1;
        gen_combinations(counter-1, vector, length, rulenum, file, fprogress, initialconf);
        vector[length-counter] = 1;
        gen_combinations(counter-1, vector, length, rulenum, file, fprogress, initialconf);
    } else {
        sum = 0;
        for (i = 0; i < length; i++) {
            sum += vector[i];
        }
        if (sum == 2 || sum == -2) {
            (*rulenum) += same_number_of_each_rule(file, length, vector, (sum/2), fprogress, initialconf);
        }
    }
}

void write_rules(int *vector, int dimension, int fprogress){
	FILE *file, *tmpfile;
	int rulenum = 0;
	char c;
    int i, extravariables;
	
	tmpfile = fopen("tmp.cnf", "w+");

    printf("Generating initial configuration...\n");
	
	for(i=0; i<dimension*dimension; i++){
		switch (vector[i]){
			case 1:
				fprintf(tmpfile, "%d 0\n", (i+1));
				rulenum++;
				break;
			case -1:
				fprintf(tmpfile, "-%d 0\n", (i+1));
				rulenum++;
				break;
			default: 
				break;
		}
	}

    printf("Generating rule for three consecutive...\n");
    
    rulenum += three_consecutive_rule(tmpfile, dimension, fprogress);

    printf("Generating rule for same configuration...\n");

    extravariables = same_config_rule(tmpfile, dimension, &rulenum, fprogress);

    int *vector2 = malloc(sizeof(int)*dimension);

    printf("Generating rule for same number of elements...\n");

    gen_combinations(dimension, vector2, dimension, &rulenum, tmpfile, fprogress, vector);

    if (fprogress) {
        printf("\n");
    }

    printf("Writing rules into 'binairo.cnf'...\n\n");

	file = fopen ("binairo.cnf", "w+");
	fprintf(file, "p cnf %d %d\n", (dimension*dimension+extravariables), rulenum);
	
	fseek(tmpfile, 0, SEEK_SET);
	
	/*c = fgetc(tmpfile); 
    while (c != EOF) 
    { 
        fputc(c, file); 
        c = fgetc(tmpfile); 
    }*/

    char buffer[BUFSIZ];
    size_t bytes;

    while (0 < (bytes = fread(buffer, 1, sizeof(buffer), tmpfile)))
        fwrite(buffer, 1, bytes, file);
	
	fclose(tmpfile);
	remove("tmp.cnf");
	fclose(file);
    free(vector2);

    printf("Solving...\n");

	clasp_solve(dimension);

    printf("\nDone! (solution also in 'binairo.txt')\n");
}

void read_file(char *filepath) {
    int c, dimension;
    FILE *file;
    file = fopen(filepath, "r");
    int *vector;
    int i;
    int fprogress = 1; // 1 -> print progress bar, 0 -> no printing

    if (file) {		
		//Leemos el primer caracter para saber cual es la dimensión del problema
		fscanf (file, "%d", &dimension);
        // Inicializamos el vector
        vector = malloc(dimension*dimension*sizeof(int));

        i = 0;

        // Transcribimos la situación inicial 1=1; 0=-1; Vacio = 0
        while ((c = getc(file)) != EOF) { 
            switch (c) {
                case '.':
                    vector[i] = 0;
                    i++;
                    break;
                case '0':
                    vector[i] = -1;
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

        if (dimension > 20) {
            printf("WARNING! Execution may take a while!\n");
            printf("Desactivating progress bar, all power going to generation...\n\n");
            fprogress = 0;
        }

        printf("Starting rule generation...\n");
        
        write_rules(vector, dimension, fprogress);
        
        fclose(file);
        free(vector);
    }
}

int main(int argc, char **argv) {

    printf("Binairo SAT - RCRA P1\n\n");

    if (argc != 2) {
        printf("*** Usage: binario <filename>\n");
        return 1;
    }

    printf("Input file: %s\n\n", argv[1]);

    read_file(argv[1]);

    return 0;
}
