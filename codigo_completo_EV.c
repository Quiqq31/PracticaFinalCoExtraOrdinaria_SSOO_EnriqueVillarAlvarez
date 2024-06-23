#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

// Declaracion y Definicion de Variables y/o Estructuras de Datos

typedef struct {
    int id;
    char cliente[50];
    char fecha_inicio[11];
    char fecha_fin[11];
    int habitacion;
} Reserva;

typedef struct Nodo {
    Reserva reserva;
    struct Nodo* siguiente;
} Nodo;

sem_t semaforo;

void* reservas;

#define MAX_RESERVAS 100

// Declaracion y Definicion de Funciones e Implementacion

/* -------------------------- FUNCIONES: (CRVD) - GESTION DE RESERVAS -------------------------- */

/* --- Segunda Funcion de Gestión de Reservas: Añadir Reservas ---
Agregar las reservas nuevas a la "lista" de reservas  
*/    
void añadir_reserva(Nodo** head, Nodo* reservaNueva) {
    reservaNueva->siguiente = *head;
    *head = reservaNueva;
}

/* --- Primera Funcion de Gestión de Reservas: CREAR Reservas --- */ 
Nodo* nueva_reserva(int id, const char* cliente, const char* fecha_inicio, const char* fecha_fin, int habitacion) {
    Nodo* reservaNueva = (Nodo*)malloc(sizeof(Nodo));
    reservaNueva->reserva.id = id;
    strcpy(reservaNueva->reserva.cliente, cliente);
    strcpy(reservaNueva->reserva.fecha_inicio, fecha_inicio);
    strcpy(reservaNueva->reserva.fecha_fin, fecha_fin);
    reservaNueva->reserva.habitacion = habitacion;
    reservaNueva->siguiente = NULL;
    return reservaNueva;
}

/* --- Tercera Funcion de Gestion de Reservas: Buscador de Reservas--- 
Como parameteros de inicio tengo el nodo head y 
el id de la reserva. Para poder hacer una busqueda 
basado en parametros y criterios especificos.
*/ 
Nodo* buscar_reserva(Nodo* head, int id, const char* cliente, const char* fechaIni, const char* fechaFin, int numHabitacion, int opcionBusqueda) {
    if (head == NULL) {
        printf("La lista de reservas está vacía.\n");
        return NULL;
    }

    Nodo* actual = head;
    printf("Buscando reserva ...\n");

    switch (opcionBusqueda) {
        case 1:
            while (actual != NULL && actual->reserva.id != id) {
                actual = actual->siguiente;
            }
            break;
        case 2:
            while (actual != NULL && strcmp(actual->reserva.cliente, cliente) != 0) {
                actual = actual->siguiente;
            }
            break;
        case 3:
            while (actual != NULL && strcmp(actual->reserva.fecha_inicio, fechaIni) != 0) {
                actual = actual->siguiente;
            }
            break;
        case 4:
            while (actual != NULL && strcmp(actual->reserva.fecha_fin, fechaFin) != 0) {
                actual = actual->siguiente;
            }
            break;
        case 5:
            while (actual != NULL && actual->reserva.habitacion != numHabitacion) {
                actual = actual->siguiente;
            }
            break;
        default:
            printf("\nError en la seleccion de criterios de busqueda.");
            break;
    }

    if (actual == NULL) {
        printf("Reserva no encontrada.\n");
    } else {
        printf("Reserva encontrada: ID - %d, Cliente - %s\n       Más Datos:  Fehca Inicio - %s, Fecha Fin - %s, Num de Habitación - %d\n", actual->reserva.id, actual->reserva.cliente, actual->reserva.fecha_inicio, actual->reserva.fecha_fin, actual->reserva.habitacion);
    }

    return actual;
}

/* --- Cuarta Funcion de Gestión de Reservas: Modificar Reserva --- 
Como parameteros de inicio tengo el nodo head y 
el id de la reserva, y el nuevo cliente. Para 
poder modificar una reserva y cambiar el cliente
atribuido a la reserva.
*/ 
void modificar_reserva(Nodo* head, int id, const char* clienteNuevo) {
    int opcionBusqueda = 0;
    int numHabitacion = 0;
    char cliente[50];
    char fechaIni[11];
    char fechaFin[11];

    printf("\n\n Busqueda de Reservas por Criterios: \n");
    printf("    [Selecciona el criterio de busqueda de entre los siguientes] \n");
    printf(" '1' - Buscar por id \n");
    printf(" '2' - Buscar por nombre del cliente \n");
    printf(" '3' - Buscar por fecha de inicio \n");
    printf(" '4' - Buscar por fecha de fin \n");
    printf(" '5' - Buscar por habitación \n --> ");

    scanf("%d", &opcionBusqueda);

    switch (opcionBusqueda) {
        case 1:
            break;
        case 2:
            printf(" Introduce el nombre del Cliente con la Reserva: ");
            scanf("%59s", cliente);
            break;
        case 3:
            printf(" Introduce la fecha de inicio de la Reserva: ");
            scanf("%10s", fechaIni);
            break;
        case 4:
            printf(" Introduce la fecha de fin de la Reserva: ");
            scanf("%10s", fechaFin);
            break;
        case 5:
            printf(" Introduce el numero de habitacion de la Reserva: ");
            scanf("%d", &numHabitacion);
            break;
        default:
            break;
    }
    
    Nodo* reserva = buscar_reserva(head, id, cliente, fechaIni, fechaFin, numHabitacion, opcionBusqueda);

    if (reserva != NULL) {
        strcpy(reserva->reserva.cliente, clienteNuevo);
        
    } else {
        printf("\n Reserva no encontrada, por favor compruebe los datos introducidos \n");
    }
}


/* --- Quinta Funcion de Gestión de Reservas: Eliminar Reserva --- 
Como parameteros de inicio tengo un puntero al 
nodo head y el id de la reserva, y el nuevo cliente.
Para poder eleminar una reserva y liberar la de la lista
de reservas.

El funcionamiento comienza con seleccionar el nodo actual y
el nodo previo, que es lo mismo que seleccionar la reserva actual
y la reserva anterior. Y basicamente lo que hacemos es "reemplazar"
la reserva previa, por la reserva actual. De manera que el nodo 'previo'
se ahora es un duplicado del nodo 'actual'. Luego liberamos el nodo actual.
*/ 
void eliminar_reserva(Nodo** head, int id) {
    Nodo* actual = *head;
    Nodo* anterior = NULL;
    while (actual != NULL && actual->reserva.id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual != NULL) {
        if (anterior == NULL) {
            *head = actual->siguiente;
        } else {
            anterior->siguiente = actual->siguiente;
        }
        free(actual);
    } else {
        printf("\nReserva no encontrada");
    }
}

/* --- Sexta Funcion de Gestión de Reservas: Mostrar un Listado de las Reservas --- 
Como parameteros de inicio tengo el nodo head. Para
poder mostrar la lista de reservas en el sistema.

El funcionamiento comienza con seleccionar el nodo head,
para ir recorriendo la lista de reservas. Utilizamos un bucle while
que como condicion compruebe si el apuntador no tiene valor de (NULL), 
es decir comprueba si el apuntador esta apuntando a un nodo que no sea el ultimo.
*/ 

void listar_reservas(Nodo* head) {
    if (head == NULL) {
        printf("No hay reservas para listar.\n");
        return;
    }

    Nodo* actual = head;
    while (actual != NULL) {
        printf("\n  -> ID: %d, Cliente: %s, Fecha Inicio: %s, Fecha Fin: %s, Habitación: %d",
               actual->reserva.id, actual->reserva.cliente, actual->reserva.fecha_inicio,
               actual->reserva.fecha_fin, actual->reserva.habitacion);
        actual = actual->siguiente;
    }
}



void crear_proceso_reserva() {
    pid_t pid = fork();
    if (pid == 0) { // Aseguramos que el hijo se encargue de manejar la solicitud de reserva.
        printf("\n Manejando la solicitud de reserva... \n");
        exit(0);
    } else if (pid > 0) { // Bloqueamos el proceso padre con un wait.
        wait(NULL);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}


//  ------------------------------------------------------ AQUI EMPIEZA EL CODIGO DE PRUEBAS -------------------------------
/*
void simular_solicitudes_reserva(int numSolicitudes) {
    for (int i = 0; i < numSolicitudes; i++) {
        if (fork() == 0) {
            printf("\nManejando la solicitud de reserva...\n");
            exit(0);
        }
    }
    for (int i = 0; i < numSolicitudes; i++) {
        wait(NULL);
    }
}

void* consultar_reservas(void* arg) {
    printf("\nConsultando reservas...\n");

    return NULL;
}

void* actualizar_reservas(void* arg) {
    printf("\nActualizando reservas...\n");
    return NULL;
}

void crear_hilos() {
    pthread_t hilo1, hilo2;
    pthread_create(&hilo1, NULL, consultar_reservas, NULL);
    pthread_create(&hilo2, NULL, actualizar_reservas, NULL);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
}
*/
//  ------------------------------------------------------ HASTA AQUI EL CODIGO DE PRUEBAS -------------------------------


//  ------------------------------- Funciones -------------------------------

void* consultar_reserva_sincornizado(void* arg) {
    sem_wait(&semaforo);
    printf("\n\n   *** Consultando reservas con semaforos *** \n");
    listar_reservas((Nodo*)arg);
    sem_post(&semaforo);
    return NULL;
}

void* actualizar_reserva_sincronizado(void* arg) {
    int id;
    char clienteActualizado[50];
    sem_wait(&semaforo);
    printf("\n\n   *** Actualizando/Modificar reservas usando semaforos *** \n");
    printf("\nIntroduzca el id de la reserva: ");
    scanf("%d", &id);
    printf("\nIntroduzca el nombre del nuevo cliente de la reserva: ");
    scanf("%49s", clienteActualizado);

    modificar_reserva((Nodo*)arg, id, clienteActualizado);
    sem_post(&semaforo);
    return NULL;
}

void crear_hilos_sincronizados(Nodo* head) {
    pthread_t hilo1, hilo2;
    pthread_create(&hilo1, NULL, consultar_reserva_sincornizado, head);
    pthread_create(&hilo2, NULL, actualizar_reserva_sincronizado, head);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
}



// ----------------------------------------------------------------------------------------
//    Memoria Compartida
// ----------------------------------------------------------------------------------------

void memoria_compartida() {
    int shm_id = shmget(IPC_PRIVATE, sizeof(Reserva) * MAX_RESERVAS, IPC_CREAT | 0666);
    reservas = shmat(shm_id, NULL, 0);
}

void* consultar_reservas_memoria(void* arg) {
    printf("\nConsultar reservas memoria compartida\n");
    Reserva* reservas = (Reserva*)arg;

    for (int i = 0; i < MAX_RESERVAS; i++) {
        if (reservas[i].id != 0) {
            printf("ID: %d, Cliente: %s, Fecha Inicio: %s, Fecha Fin: %s, Habitación: %d\n",
                   reservas[i].id, reservas[i].cliente, reservas[i].fecha_inicio, reservas[i].fecha_fin, reservas[i].habitacion);
        }
    }
    return NULL;
}

void* actualizar_reservas_memoria(void* arg) {
    printf("\nActualizar reservas memoria compartida\n");
    Reserva* reservas = (Reserva*)arg;
    int id, habitacion;
    char cliente[50], fecha_inicio[11], fecha_fin[11];

    printf("Introduzca el ID de la reserva: ");
    scanf("%d", &id);
    printf("Introduzca el nombre del cliente: ");
    getchar();  // Limpiar el buffer de entrada
    fgets(cliente, sizeof(cliente), stdin);
    // Eliminar el salto de línea residual
    cliente[strcspn(cliente, "\n")] = 0;
    printf("Introduzca la fecha de inicio (YYYY-MM-DD): ");
    fgets(fecha_inicio, sizeof(fecha_inicio), stdin);
    // Eliminar el salto de línea residual
    fecha_inicio[strcspn(fecha_inicio, "\n")] = 0;
    printf("Introduzca la fecha de fin (YYYY-MM-DD): ");
    fgets(fecha_fin, sizeof(fecha_fin), stdin);
    // Eliminar el salto de línea residual
    fecha_fin[strcspn(fecha_fin, "\n")] = 0;
    printf("Introduzca el número de habitación: ");
    scanf("%d", &habitacion);

    for (int i = 0; i < MAX_RESERVAS; i++) {
        if (reservas[i].id == 0) {
            reservas[i].id = id;
            strcpy(reservas[i].cliente, cliente);
            strcpy(reservas[i].fecha_inicio, fecha_inicio);
            strcpy(reservas[i].fecha_fin, fecha_fin);
            reservas[i].habitacion = habitacion;
            break;
        }
    }

    return NULL;
}


void crear_hilos_memoria() {
    pthread_t hilo1, hilo2;
    pthread_create(&hilo1, NULL, consultar_reservas_memoria, NULL);
    pthread_create(&hilo2, NULL, actualizar_reservas_memoria, NULL);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
}

void comunicar_con_pipe() {
    int pipe_fd[2];
    char buffer[100];
    pipe(pipe_fd);
    if (fork() == 0) {
        close(pipe_fd[1]);
        read(pipe_fd[0], buffer, sizeof(buffer));
        printf("\nProceso hijo leyó: %s\n", buffer);
        close(pipe_fd[0]);
        exit(0);
    } else {
        close(pipe_fd[0]);
        char* mensaje = "\nMensaje desde el proceso padre";
        write(pipe_fd[1], mensaje, strlen(mensaje) + 1);
        close(pipe_fd[1]);
        wait(NULL);
    }
}

// --------------------------------- FUNCIONES DE ESCRIBIR Y LEER DE FICHEROS/BASE DE DATOS ---------------------------------

void guardar_reservas(Nodo* head) {
    FILE* file = fopen("reservas.txt", "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    while (head) {
        fprintf(file, "%d,%s,%s,%s,%d\n",
                head->reserva.id, head->reserva.cliente,
                head->reserva.fecha_inicio, head->reserva.fecha_fin,
                head->reserva.habitacion);
        head = head->siguiente;
    }
    fclose(file);
}

Nodo* cargar_reservas() {
    FILE* file = fopen("reservas.txt", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    Nodo* head = NULL;
    Reserva r;
    while (fscanf(file, "%d,%49[^,],%10[^,],%10[^,],%d\n",
                  &r.id, r.cliente, r.fecha_inicio, r.fecha_fin, &r.habitacion) != EOF) {
        Nodo* nuevo = nueva_reserva(r.id, r.cliente, r.fecha_inicio, r.fecha_fin, r.habitacion);
        añadir_reserva(&head, nuevo);
    }
    fclose(file);
    return head;
}

int main() {
    Nodo* head = NULL;
    sem_init(&semaforo, 0, 1);
    memoria_compartida();

    printf("\n ---- Creamos Nuevas Reservas ---- ");
    // Ejemplo de uso 1 cliente
    Nodo* reserva1 = nueva_reserva(1, "Carlos Alvarez", "2024-05-01", "2024-05-20", 101);
    añadir_reserva(&head, reserva1);
    // Ejemplo de uso 2 cliente
    Nodo* reserva2 = nueva_reserva(2, "Quique Villar", "2024-06-02", "2024-06-15", 102);
    añadir_reserva(&head, reserva2);
    // Ejemplo de uso 3 cliente
    Nodo* reserva3 = nueva_reserva(3, "Manuel Jimenez", "2024-06-28", "2024-07-05", 103);
    añadir_reserva(&head, reserva3);
    
    printf("\n Listando reservas después de añadir: ");
    listar_reservas(head);
    
    // Guardar y cargar reservas desde archivo
    guardar_reservas(head);
    head = cargar_reservas();

    printf("\n Listando reservas después de cargar desde el archivo:");
    listar_reservas(head);
    
    // Manejar procesos y hilos
    crear_proceso_reserva();
    crear_hilos_sincronizados(head);

    // Guardar y cargar las reservas modificadas desde archivo
    guardar_reservas(head);
    head = cargar_reservas();
    
    // Comunicación entre procesos
    comunicar_con_pipe();

    sem_destroy(&semaforo);

    return 0;
}