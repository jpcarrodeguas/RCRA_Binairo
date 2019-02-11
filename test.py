#!/usr/bin/python3

# CUIDADO CON MEMORIA, EN VEZ DE GUARDAR TO EN MEMORIA, IR ESCRIBIENDO

import sys, subprocess

def rules(filename):

    f = open(filename, "r")
    datos = f.readlines()
    f.close()

    n = int(datos[0].rstrip())
    
    vector = []
    aux = ""
    for i in range(1,len(datos)):
        s = datos[i].rstrip()
        aux += s

    # Situación inicial
    for i in range(len(aux)):
        if aux[i] == '0':
            vector.append(str(-1*(i+1)) + " 0\n")
        elif aux[i] == '1':
            vector.append(str(i+1) + " 0\n")


    # Norma no más de dos por filas
    for i in range(n):
        for j in range(n-2):
            va = i*n+j
            vector.append(str(va+1) + " " + str(va+2) + " " + str(va+3) + " 0\n")
            vector.append(str(-1*(va+1)) + " " + str(-1*(va+2)) + " " + str(-1*(va+3)) + " 0\n")

    # Norma no más de dos por columnas
    for j in range(n):
        for i in range(n-2):
            vector.append(str(i*n+j+1) + " " + str((i+1)*n+j+1) + " " + str((i+2)*n+j+1) + " 0\n")
            vector.append(str(-1*(i*n+j+1)) + " " + str(-1*((i+1)*n+j+1)) + " " + str(-1*((i+2)*n+j+1)) + " 0\n")

    # Norma mismo número de elementos
    myformat = "{0:{fill}" + str(n) + "b}"
    for i in range(pow(2,n)):
        aux = list(map(int, myformat.format(i, fill=0)))
        if no_three_in_a_row(aux, 1) and no_three_in_a_row(aux, 0):
            if sum(aux) != n/2:
                # añadir norma con esta configuración
                aux2 = [1 if j == 0 else -1 for j in aux]
                # Norma en filas
                for j in range(n):
                    aux3 = [x+6*j for x in list(range(1,n+1))]
                    rule_vector = [aux2[k] * aux3[k] for k in range(n)]
                    s = ""
                    for e in rule_vector:
                        s += str(e) + " "
                    s += "0 \n"
                    vector.append(s)
                # Norma en columnas
                for j in range(1,n+1):
                    aux3 = list(range(j, n*n+1, n))
                    rule_vector = [aux2[k] * aux3[k] for k in range(n)]
                    s = ""
                    for e in rule_vector:
                        s += str(e) + " "
                    s += "0 \n"
                    vector.append(s)

    # Linea inicial
    vector.insert(0, "p cnf " + str(n*n) + " " + str(len(vector)) + "\n")

    filecnf = filename.split(".")[0] + ".cnf"

    f = open(filecnf, "w")
    f.writelines(vector)
    f.close()

    do_clasp(n, filecnf)

def no_three_in_a_row(vector, value):

    count = 0
    for e in vector:
        if value == e:
            count += 1
            if count > 2:
                return False
        else:
            count = 0
    return True

def do_clasp(n, filecnf):

    a = subprocess.run(["clasp","--verbose=0", filecnf], stdout=subprocess.PIPE)
    s = a.stdout.decode('utf-8').split("\n")
    print_output(n, s)

def print_output(n, datos):

    vector = []

    for i in range(len(datos)):
        aux = datos[i].rstrip().split(" ")
        if aux[0] == 'v':
            for j in range(1, len(aux)):
                vector.append(int(aux[j]))

    vector2 = [0 if i < 0 else 1 for i in vector]

    for i in range(len(vector2) - 1):
        print(vector2[i], end='', flush=True)
        if i % n == (n-1):
            print("")

def main(filename):
    rules(filename)

if __name__ == "__main__":
    main(sys.argv[1])
