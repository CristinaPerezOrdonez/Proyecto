#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <mysql.h>

typedef struct {
	char nombre[20];
	int socket;
} Conectado;

typedef struct{
	Conectado conectados[100];
	int num;
} ListaConectados;

//La lista se pasa por referencia
int PonConectado (ListaConectados *lista, char nombre[20], int socket){
	//a�ade nuevo conectado y retorna 0 si okey o 0 si la lista ya estaba llena
	if(lista->num ==100)
		return -1;
	else
	{
		strcpy(lista->conectados[lista->num].nombre, nombre);
		lista->conectados[lista->num].socket=socket;
		lista->num++;
		return 0;
	}
}

int EliminaConectado (ListaConectados *lista, char nombre[20]){
	//Retorna 0 si se ha eliminado y -1 si el usuario no esta en la lista
	//lista ya es un puntero a la lista y por ello la pongo tal cual sin &
	int pos=DamePosicion (lista, nombre);
	if(pos==-1)
		return -1;
	else
	{
		int i;
		for(i=pos; i< lista->num-1; i++)
		{
			lista->conectados[i]=lista->conectados[i+1];
			//strcpy(lista->conectados[i].nombre, lista->conectados[i+1].nombre);
			//lista->conectados[i].socket=lista->conectados[i+1].socket;
		}
		lista->num--;
		return 0;
	}
	
}

void DameConectados(ListaConectados *lista, char conectados[300]){
	//Pone en conectados los nombres de todos los conectados separados
	//por /. Primero pone el n�mero de conectados
	//Ejemplo: "3/Pedro/Mayra/Luis"
	sprintf(conectados,"%d", lista->num);
	int i;
	for(i=0;i<lista->num;i++)
		sprintf(conectados, "%s/%s", conectados, lista->conectados[i].nombre);
	
}

int DamePosicion(ListaConectados *lista, char nombre[20]){
	//Devuelve el socket o -1 si no lo ha encontrado en la lista
	//esquema de busqueda
	int i=0;
	int encontrado=0;
	while((i<lista->num)&& (!encontrado))
	{
		if(strcmp(lista->conectados[i].nombre,nombre)==0)
			encontrado=1;
		else
			i++;
	}
	if(encontrado)
		return i;
	else
		return -1;
}

void DameUser(ListaConectados *lista, int socket, char nombre[20]){
	//Devuelve el socket o -1 si no lo ha encontrado en la lista
	//esquema de busqueda
	int i=0;
	int encontrado=0;
	while((i<lista->num)&& (!encontrado))
	{
		if(lista->conectados[i].socket==socket)
			encontrado=1;
		else
			i++;
	}
	if(encontrado)
		sprintf(nombre, "%s", lista->conectados[i].nombre);
	else
		printf("No se ha encontrado ning�n usuario con ese socket");
}


void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s= (int*) socket;
	sock_conn= *s;
	//int socket_conn= * (int *) socket;
	
	char peticion[512];
	char respuesta[512];
	int ret;
	int res;
	ListaConectados miLista;
	int terminar=0;
	//Entramos en un bucle para atender todas las peticiones de este clientes
	//hasta que se desconecte
	while(terminar==0)
	{
		//Ahora recibimos la peticion
		ret=read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido\n");
		
		//Tenemos que a�adirle la marca de fin de string
		//para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		printf("Peticion: %s\n", peticion);
		
		//vamos a ver que nos pide el cliente
		char *p= strtok(peticion,"/");
		int codigo= atoi(p);
		//ya tenemos el codigo de peticion
		char user[20];
		char password[20];
		int err;
		int id;
		char idultimo[10];
		char consulta[80];
		MYSQL *conn;
		MYSQL_RES *resultado;
		MYSQL_ROW row;
		
		
		if((codigo!=0) && (codigo!=6))
		{
			p=strtok(NULL,"/");
			strcpy(user,p);
			//ya tenemos el nombre de usuario
			printf("Codigo: %d, Nombre: %s\n", codigo, user);
			conn=mysql_init(NULL);
			if(conn==NULL)
			{
				printf("Error al crear la conexion: %u %s\n",
					   mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			//inicializar la conexi??n, entrando nuestras claves de acceso y
			//el nombre de la base de datos a la que queremos acceder 
			conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego",0, NULL, 0);
			if (conn==NULL) 
			{
				printf ("Error al inicializar la conexion: %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
		}
		
		if(codigo==0) //peticion de desconexion
		   terminar=1;
		
		else if(codigo==1) //Registra un usuario
		{
			p = strtok (NULL, "/");
			strcpy (password, p);
			
			// construimos la consulta SQL para saber cual es el �ltimo id ocupado
			strcpy (consulta,"SELECT MAX(jugador.id) FROM jugador "); 
			
			// hacemos la consulta 
			err=mysql_query (conn, consulta); 
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "No se ha podido realizar la consulta");
				exit (1);
			}
			//recogemos el resultado de la consulta 
			resultado = mysql_store_result (conn); 
			row = mysql_fetch_row (resultado);
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos la consulta");
			}
			else
				id=atoi(row[0]);
			// El resultado debe ser una matriz con una sola fila
			// y una columna que contiene el id
			
			int id1=id +1;
			sprintf(idultimo, "%d", id1);
			printf("User: %s con password: %s quiere registrarse en la BBDD\n", user, password);
			
			
			
			
			
			
			
			//Me da error el scanf!
			
			
			
			
			/*				err = scanf ("%s, %s, %s",idultimo, user, password);*/
			/*				if (err!=3)*/
			/*				{*/
			/*					printf ("Error al introducir los datos \n");*/
			/*					exit (1);*/
			/*				}*/
			
			
			
			
			
			
			
			// Ahora construimos el string con el comando SQL
			// para insertar la persona en la base. Ese string es:
			// INSERT INTO personas VALUES ('dni', 'nombre', edad); 
			strcpy (consulta, "INSERT INTO Juego.jugador VALUES (");
			//concatenamos el id
			strcat (consulta, idultimo);
			strcat (consulta,",'");
			//concatenamos el user 
			strcat (consulta, user); 
			strcat (consulta, "','");
			//concatenamos el password 
			strcat (consulta, password); 
			strcat (consulta, "');");
			
			
			// Ahora ya podemos realizar la insercion 
			err = mysql_query(conn, consulta);
			if (err!=0) 
			{
				printf ("Error al introducir datos la base %u %s\n", 
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "No se ha podido realizar la consulta");
				exit (1);
			}
			
			//Vemos si se ha a�adido a la tabla jugador para ello la imprimimos
			err=mysql_query (conn, "SELECT * FROM jugador");
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "Error al consultar la base de datos");
				exit (1);
			}
			//recogemos el resultado de la consulta. El resultado de la
			//consulta se devuelve en una variable del tipo puntero a
			//MYSQL_RES tal y como hemos declarado anteriormente.
			//Se trata de una tabla virtual en memoria que es la copia
			//de la tabla real en disco.
			resultado = mysql_store_result (conn);
			// El resultado es una estructura matricial en memoria
			// en la que cada fila contiene los datos de una persona.
			
			// Ahora obtenemos la primera fila que se almacena en una
			// variable de tipo MYSQL_ROW
			row = mysql_fetch_row (resultado);
			// En una fila hay tantas columnas como datos tiene una
			// persona. En nuestro caso hay tres columnas: dni(row[0]),
			// nombre(row[1]) y edad (row[2]).
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos la consulta");
			}
			else
			{
				while (row !=NULL) 
				{
					// la columna 2 contiene una palabra que es la edad
					// la convertimos a entero 
					id = atoi (row[0]);
					// las columnas 0 y 1 contienen DNI y nombre 
					printf ("Los datos del jugador son id: %d, user: %s, password: %s\n", id, row[1], row[2]);
					// obtenemos la siguiente fila
					row = mysql_fetch_row (resultado);
				}
			}
			strcpy(respuesta, "Usuario registrado");
		}
		
		else if(codigo==2) //quieren iniciar sesion
		{
			res= PonConectado(&miLista, user, sock_conn);
			if(res==-1)
				printf("No se ha podido a�adir conectado porque la lista esta llena\n");
			else
				printf("A�adido a la lista de conectados\n");
			char misconectados[300];
			DameConectados(&miLista, misconectados);
			printf("Resultado: %s\n", misconectados);
			p = strtok (NULL, "/");
			strcpy (password, p);
			
			char passwordBBDD[20];
			
			// construimos la consulta SQL
			strcpy (consulta,"SELECT password FROM Juego.jugador WHERE jugador.nombre = '"); 
			strcat (consulta, user);
			strcat (consulta,"'");
			// hacemos la consulta 
			err=mysql_query (conn, consulta); 
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "Error al consultar la base de datos");
				exit (1);
			}
			//recogemos el resultado de la consulta 
			resultado = mysql_store_result (conn); 
			row = mysql_fetch_row (resultado);
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos la consulta");
			}
			else
			{
				// El resultado debe ser una matriz con una sola fila
				// y una columna que contiene el nombre
				
				strcpy(passwordBBDD, row[0]);
				printf ("El password del jugador: %s, es: %s\n", user, passwordBBDD );
			}
			if(strcmp(passwordBBDD,password)==0)
				strcpy(respuesta, "Usuario iniciado");
			else
				strcpy(respuesta, "Datos de inicio de sesi�n incorrectos");
			
		}
		
		else if(codigo==3)//Nos da el jugador que gan� la partida en menor tiempo
		{
			strcpy (consulta,"SELECT jugador.nombre FROM Juego.jugador,Juego.relacion,Juego.partida WHERE partida.tiempo_stop = (SELECT MIN(partida.tiempo_stop) FROM Juego.partida) AND partida.id = relacion.idPartida");
			// hacemos la consulta 
			err=mysql_query (conn, consulta); 
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "Error al consultar la base de datos");
				exit (1);
			}
			//recogemos el resultado de la consulta 
			resultado = mysql_store_result (conn); 
			row = mysql_fetch_row (resultado);
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos en la consulta");
			}
			else
			{
				// El resultado debe ser una matriz con una sola fila
				// y una columna que contiene el nombre
				// El resultado debe ser una matriz con una sola fila
				// y una columna que contiene el nombre
				strcpy(respuesta, "El jugador que gan� la partida en menor tiempo es: "); 
				strcat (respuesta, row[0]);
				strcat (respuesta,".");
			}
		}
		
		else if(codigo==4) //Nos da el menor tiempo en ganar del usuario pedido
		{

			//Construimos la consulta
			strcpy (consulta,"SELECT partida.tiempo_stop FROM Juego.jugador,Juego.relacion,Juego.partida WHERE jugador.nombre = '"); 
			strcat (consulta, user);
			strcat (consulta,"' AND jugador.id=relacion.idJugador AND relacion.idPartida=partida.id");
			// hacemos la consulta 
			err=mysql_query (conn, consulta); 
			if (err!=0) 
			{
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "Error al consultar la base de datos");
				exit (1);
			}
			//recogemos el resultado de la consulta 
			resultado = mysql_store_result (conn); 
			row = mysql_fetch_row (resultado);
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos en la consulta");
			}
			else
			{
				// El resultado debe ser una matriz con una sola fila
				// y una columna que contiene el nombre
				strcpy(respuesta, "El menor tiempo en ganar de user: "); 
				strcat (respuesta, user);
				strcat (respuesta," es: ");
				strcat (respuesta, row[0]);
				strcat (respuesta,".");
			}
		}
		
		else if (codigo==5) //CONSULTA a arreglar
		{
			//Construimos la consulta
			strcpy (consulta,"SELECT * FROM Juego.jugador,Juego.relacion,Juego.partida WHERE jugador.nombre = '"); 
			strcat (consulta, user);
			strcat (consulta,"' AND jugador.id=relacion.idJugador AND relacion.idPartida=partida.id");
			// hacemos la consulta 
			err=mysql_query (conn, consulta); 
			if (err!=0) 
			{
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				strcpy(respuesta, "Error al consultar la base de datos");
				exit (1);
			}
			int cont;
			int cont1;
			//recogemos el resultado de la consulta 
			resultado = mysql_store_result (conn); 
			row = mysql_fetch_row (resultado);
			if (row == NULL)
			{
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(respuesta, "No se han obtenido datos en la consulta");
			}
			else
			{
				while (row !=NULL) 
				{
					cont=0;
					cont=cont++;
					// obtenemos la siguiente fila
					row = mysql_fetch_row (resultado);
				}
				strcpy(respuesta, "El n�mero de partidas jugadas por: "); 
				strcat (respuesta, user);
				strcat (respuesta," es: ");
				sprintf(cont1, "%d", cont);
				strcat (respuesta,".");
			}
		}
		
		else if (codigo==6)//Lista conectados
		{
			char misconectados[300];
			DameConectados(&miLista, misconectados);
			if (misconectados==NULL)
				strcpy(respuesta, "No hay conectados");
			else
			{
				printf("Resultado: %s\n", misconectados);
				sprintf(respuesta, "%s", misconectados);
			}
			
		}
		
		if(codigo!=0)
		{
			printf("Respuesta: %s\n", respuesta);
			//Enviamos respuesta
			write(sock_conn, respuesta, strlen(respuesta));
		}
	}
	char user[20];
	DameUser (&miLista, sock_conn, user);
	res= EliminaConectado(&miLista, user);
	if(res==-1)
		printf("No se ha podido a�adir conectado a la lista");
	else
		printf("A�adido a la lista de conectados");
	//Se acab� el servicio para este cliente
	close(sock_conn);
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	//INICIALIZAMOS
	//abrimos socket
	if((sock_listen =socket(AF_INET, SOCK_STREAM, 0)) <0)
		printf("Error creando el socket");
	//Hacemos el bind al puerto
	
	memset(&serv_adr,0,sizeof(serv_adr)); //inicializamos a cero serv_addr
	serv_adr.sin_family = AF_INET;
	
	//asocia el socket a cualquiera de las ip de la maquina
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	//establecemos el puerto de escucha
	serv_adr.sin_port=htons(9070);
	if(bind(sock_listen,(struct sockaddr *) &serv_adr, sizeof(serv_adr))<0)
		printf("Error en el bind");
	if(listen(sock_listen,3) <0)
		printf("Error en el listen");
	
	int i;
	int sockets[100];
	pthread_t thread;
	i=0;
	//Bucle para atender a clientes
	for(;;){
		printf("Escuchando\n");
		
		sock_conn =accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		
		sockets[i] = sock_conn;
		//sock_conn es eel socket que usaremos para este cliente
		
		//Crear thread y decirle lo que tiene que hacer
		pthread_create (&thread, NULL, AtenderCliente, &sockets[i]);
		i=i+1;
	
	}
	
	//Bucle para atender a 5 clientes
	//for (i=0; i<5; i++)
	//	pthread_join (thread[i], NULL);
}
