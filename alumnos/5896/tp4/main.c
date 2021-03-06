#include "header.h"


int main(int argc, char * const *argv)
{
	int opcion;
	char *ruta;
	//char buffer[800];
	//memset(buffer,0,sizeof buffer);

	// parseo de ruta y de puerto
	char *archivoConfig = NULL;
	int puerto;

	while ((opcion = getopt (argc, argv, "d:")) >= 0){
		switch (opcion){

			case 'd':  // Abrir archivo .conf
				nombre_archivo(&archivoConfig);	
				break;

		} // cierre switch
	} // cierre while opcion

	if (!archivoConfig){ // default
		archivoConfig = "archivo.conf";
	}

	//fin de parseo de ruta y de puerto


	//declaro puerto y ruta
	puerto = parser_puerto(archivoConfig); //funciones.c devuelve el puerto
	ruta = parser_ruta(archivoConfig);//funciones.c devuelve la ruta
	//fin de declarar puerto y ruta



	//int fdsocket;
	//int opt = 1, optlen = sizeof(opt);
	//socklen_t addrlen;
	//struct sockaddr_in srv_addr;
	//struct sockaddr_in cli_addr;


	/* creación y configuración del socket */

	int sfd;
	struct sockaddr_in direccion = {};

	printf ("Servidor HTTP\n");
	printf ("Puerto: %d\n", puerto);
	
	signal(SIGCHLD, SIG_IGN);
	sfd = socket (AF_INET,SOCK_STREAM,0);

	if (sfd < -1){
		perror("Error en la creacion del socket\n");
		return -1;
	}

	direccion.sin_family=AF_INET;
	direccion.sin_port = htons(puerto);
	direccion.sin_addr.s_addr = INADDR_ANY;

	if (bind(sfd,(struct sockaddr*) &direccion,sizeof(direccion))==-1){
		perror("Error en bind\n");
		return -1;
	}

	if(listen(sfd,10)<0){
		perror("Error en listen\n");
		return -1;
	}
	//fin de creacion y configuracion de socket

	//CREACION DEL SERVIDOR
	struct sockaddr_in  direccion_cli={};
	socklen_t direccion_cli_len = sizeof direccion_cli;	
	int acceptfd;



	while( (acceptfd = accept(sfd, (struct sockaddr *)&direccion_cli,&direccion_cli_len)) > 0) {
		switch (fork()) {
			case 0: // hijo
				close(sfd);
				//http_worker(sd_conn, (struct sockaddr *) &cli_addr);
				printf ("Direccion del nuevo cliente: %s en el puerto %d \n",inet_ntoa((struct in_addr) direccion_cli.sin_addr),puerto);
				http_worker(acceptfd,ruta);
				close(acceptfd);
				return 0;

			case -1: // error
				perror("Error en la creacion de fork\n");
				return -1;
				break;

			default: // padre
				//close(acceptfd);
				break;
		}
		close(acceptfd);
	}
	//FIN DEL SERVIDOR
	close(sfd);
	return 0;
}
