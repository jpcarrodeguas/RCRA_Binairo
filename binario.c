#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int *init_vector(int dimension) {
    return malloc(dimension*dimension*sizeof(int));
}

int read_file(char *filepath) {
    int c, ret;
    FILE *file;
    file = fopen(filepath, "r");
    int *vector;

    if (file) {
        ret = getc(file);

        vector = init_vector(ret);

        int i = 0;

        while ((c = getc(file)) != EOF) {
            switch (c) {
                case '.':
                    printf("Punto\n");
                    vector[i] = -1;
                    i++;
                    break;
                case '0':
                    printf("Cero\n");
                    vector[i] = 0;
                    i++;
                    break;
                case '1':
                    printf("Uno\n");
                    vector[i] = 1;
                    i++;
                    break;
                default:
                    printf("afa");
                    break;
            }
        }
        for (i = 0; i < (ret*ret); i++)
            printf("hl %c", vector[i]);
            if ((i % ret) == 5) {
                printf("\n");
            }
            //putchar(c);
        fclose(file);
        free(vector);
    }
    return ret;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("*** Usage: binario <filename>\n");
        return 1;
    }

    printf("*** Input file: %s\n", argv[1]);

    int i = read_file(argv[1]);

    printf("Dimension: %c\n", i);

    return 0;

}