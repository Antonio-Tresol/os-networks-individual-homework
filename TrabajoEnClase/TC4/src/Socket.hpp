

/**
 *  Establece la definición de la clase Socket para efectuar la comunicación
 *  de procesos que no comparten memoria, utilizando un esquema de memoria
 *  distribuida.  El desarrollo de esta clase se hará en varias etapas, primero
 *  los métodos necesarios para los clientes, en la otras etapas los métodos para el servidor,
 *  manejo de IPv6, conexiones seguras y otros
 *
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2023-i
 *  Grupos: 2 y 3
 *
 * (versión Ubuntu)
 *
 **/

#ifndef Socket_hpp
#define Socket_hpp

class Socket {

    public:
        Socket( char type, bool IPv6 = false );
        Socket( int );				// int = socket descriptor
        ~Socket();
        int Connect( const char * host, int port );
        int Connect( const char * host, const char * service );
        void Close();
        int Read( void * buffer, int bufferSize );
        int Write( const void * buffer, int bufferSize );
        int Write( const char * buffer );	// size is calculated with strlen
        int Listen( int backlog );
        int Bind( int port );
        Socket * Accept();
        int Shutdown( int mode );		// mode = { SHUT_RD, SHUT_WR, SHUT_RDWR }
        void SetIDSocket( int newId );
        int sendTo( const void * message, int length, const void * other );
        int recvFrom( void * buffer, int length, void * other );
        int SSLConnect(const char * host, int port);
        int SSLConnect(const char * host, const char * service );
        int SSLRead(void * buffer, int bufferSize );
        int SSLWrite(const void * buffer, int bufferSize );
        int InitSSLContext();
        int InitSSL();
        // int ConnectIPV6(const char* host, const char* port);
        // int ConnectIPV6(const char* host, int port);
        
    private:
        int idSocket;
        int port;
        bool ipv6;
        void * SSLContext;	// SSL context
	    void * SSLStruct;	// SSL BIO basis input output

};

#endif
