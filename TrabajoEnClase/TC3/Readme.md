Read.Me

[Ejercicio propuesto: 2023/Mar/21; entrega 2023/Mar/21]

Objetivo
   Completar su clase "Socket" en C++ para que funcione con SSL en IPv4

Tareas

   - Completar la clase C++ "Socket" para poder intercambiar mensajes de manera cifrada entre procesos que no
     comparten memoria

     Se facilita el ejemplo "ssl-cli.c" para analizar el funcionamiento de un programa que hace una conexión SSL

     Se facilita la interfaz "Socket.h".  Para esta etapa deben completar:
        Agregar un método para inicializar el modo SSL (InitSSL)
           InitSSLContext, para construir un contexto SSL (SSL_CTX_new)
           InitSSL, para inicializar las estructuras SSL (SSL_new)

	Agregar un método para conectarse de manera segura SSLConnect (SSL_connect)

	SSLRead, para lectura de datos por el canal SSL (SSL_read)

	SSLWrite, para la escritura de datos por el canal SSL (SSL_write)

   - Instancias nuevas de la clase:
	void * SSLContext;	// SSL context
	void * SSLStruct;	// SSL BIO basis input output

   - El ejemplo "ipv4-ssl-test.cc" debe funcionar correctamente

Descripción detallada
   - Clase Socket
	void Socket::InitSSLContext();
           // We must create a method to define our context
           const SSL_Method * method = TLS_client_method();
           // Check for errors
           SSL_CTX * context = SSL_CTX_new( method );
           // Check for errors
           this->SSLContext = (void *) context;

	void Socket::InitSSL();	// Create a SSL socket, a new context must be created before
           this->InitSSLContext();
           SSL * ssl = SSL_new( (SSL_CTX *) this->context );
           // Check for errors
           this->SSLStruct = (void *) ssl;

	int SSLConnect( char *, int );
           Connect( host, port );	// Establish a non SSL connection first
           SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
           st = SSL_connect( (SSL *) this-SSLStruct );
           // check for errors

	int SSLConnect( char *, char * );
           // Similar as above

	int SSLRead( void *, int );
           // Use SSL_read

	int SSLWrite( void *, int );
           // Use SSL_write


Ubuntu
	Debe instalar "libssl-dev" para las bibliotecas SSL
	
Referencias
   https://os.ecci.ucr.ac.cr/ci0123/material/sockets-course.ppt
   https://www.openssl.org/docs/man3.0/man7/ssl.html
   El manual del sistema operativo Unix