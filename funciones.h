#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SH_DIR "/bin/sh"

int txt_to_pipe(char *nombre_archivo, char *comando);
int stdin_to_pipe(char *comando);
void stdin_stdout(char *comando);
int txt_to_txt(char *nombre_archivo_1, char *nombre_archivo_2, char *comando);
int txt_to_stdout(char *nombre_archivo, char *comando);
int stdin_to_txt(char *nombre_archivo, char *comando);
int pipe_to_stdout(int pipe_in, char *comando);
int pipe_to_txt(int pipe_in, char *nombre_archivo, char *comando);
int pipe_to_pipe(int pipe_in, char *comando);

int txt_to_pipe(char *nombre_archivo, char *comando){
	int p[2];
	pipe(p);
	int c;
	int archivo = open(nombre_archivo, O_RDONLY);

	//printf("Open abrio el archivo con numero %d\n", archivo);
	//printf("los identificadores del pipe son %d y %d\n", p[0], p[1]);

	if(archivo < 0){
		printf("Oh el archivo no existe\n");
		return -1;
	}

	if(fork() == 0){

		close(p[0]);//cierro la entrada de la tuberia

		close(0); // cierro stdin
		dup(archivo); // Redirecciono la entrada al archivo abierto

		close(1); // cierro la salida estandar stdout
		dup(p[1]); // redirecciono la salida estandar a la salida del pipe

		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		//close(p[0]);
		wait(NULL);
		close(archivo);
		close(p[1]);

		return p[0];
	}
}

int stdin_to_pipe(char *comando){
	int p[2];
	pipe(p);

	//printf("Los identificadores del pipe son %d y %d\n", p[0], p[1]);
	if(fork() == 0){
		close(p[0]); // Cierro la entrada de la tuberia

		close(1); // Cierro stdout
		dup(p[1]); // Redirecciono la salida a la tuberia

		execl(SH_DIR, SH_DIR, "-c", comando, 0);

		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);
		close(p[1]);

		return p[0]; // Regreso descriptor de archivo a la tuberia
	}
}

void stdin_stdout(char *comando){
	if(fork() == 0){
		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);
	}
}

int txt_to_txt(char *nombre_archivo_1, char *nombre_archivo_2, char *comando){
	printf("txt_to_txt\n");
	int archivo1, archivo2;

	archivo1 = open(nombre_archivo_1, O_RDONLY);
	archivo2 = open(nombre_archivo_2, O_CREAT|O_TRUNC|O_WRONLY);

	if(archivo1 < 0){
		printf("No se pudo abrir %s\n", nombre_archivo_1);
		return -1;
	}
	if(archivo2 < 0){
		printf("No se pudo crear %s\n", nombre_archivo_2);
		return -1;
	}

	if(fork() == 0){
		close(0); // Cierro stdin
		dup(archivo1); // Asigno archivo 1 a la entrada

		close(1); // Cierro stdout
		dup(archivo2); // Asigno archivo 2 a la salida

		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);
		close(archivo1);
		close(archivo2);

		return EXIT_SUCCESS;
	}
}

int txt_to_stdout(char *nombre_archivo, char *comando){
	printf("txt_to_stdout\n");
	int archivo;
	archivo = open(nombre_archivo, O_RDONLY);
	if(archivo < 0){
		printf("No se pudo abrir %s\n", nombre_archivo);
		return -1;
	}

	if(fork() == 0){
		close(0); // Cierra stdin
		dup(archivo); // Remplaza la entrada por el archivo

		execl(SH_DIR, SH_DIR, "-c", comando);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);

		close(archivo);
		return EXIT_SUCCESS;
	}
}

int stdin_to_txt(char *nombre_archivo, char *comando){
	printf("stdin_to_txt\n");
	int archivo;
	archivo = open(nombre_archivo, O_CREAT|O_TRUNC|O_WRONLY);

	if(archivo < 0){
		printf("No se pudo crear %s\n", nombre_archivo);
		return -1;
	}

	if(fork() == 0){
		close(1); // Cierra stdout
		dup(archivo); // Asigna la salida al archivo

		close(2);
		dup(archivo);

		execl(SH_DIR, SH_DIR, "-c", comando, 0);

		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);

		close(archivo);
		return EXIT_SUCCESS;
	}
}

int pipe_to_stdout(int pipe_in, char *comando){
	printf("pipe_to_stdout\n");

	if(pipe_in < 3){
		printf("El descriptor de la tuberia no es valido\n");
		return -1;
	}

	if(fork() == 0){
		close(0); // Cierrra stdin
		dup(pipe_in); // redirecciona stdin con el contenido de la tuberia

		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);

		close(pipe_in);

		return EXIT_SUCCESS;
	}
}

int pipe_to_txt(int pipe_in, char *nombre_archivo, char *comando){
	printf("pipe_to_txt\n");

	if(pipe_in < 3){
		printf("Descriptor de la tuberia no valido\n");
		return -1;
	}

	int archivo = open(nombre_archivo, O_CREAT|O_TRUNC|O_WRONLY);

	if(fork() == 0){
		close(0);
		dup(pipe_in);

		close(1);
		dup(archivo);

		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);

		close(pipe_in);
		close(archivo);

		return EXIT_SUCCESS;
	}
}

int pipe_to_pipe(int pipe_in, char *comando){
	printf("pipe_to_pipe\n");

	int p[2];
	pipe(p);

	if(pipe_in < 3){
		printf("Descriptor de tuberia invalido\n");
		return -1;
	}

	if(fork() == 0){
		close(p[0]); // Cierra entrada de tuberia recien creada
		close(0); // Cierra stdin
		dup(pipe_in); // Lo remplaza por el pipe de entrada

		close(1); // Cierra stdout
		dup(p[1]); // Lo asigna a la salida de la tuberia recien creada

		execl(SH_DIR, SH_DIR, "-c", comando, 0);
		_exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);

		close(pipe_in);
		close(p[1]);

		return p[0];
	}
}