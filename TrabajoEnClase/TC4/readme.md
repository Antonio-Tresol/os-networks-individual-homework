Read.Me Semana 5

[Ejercicio propuesto: 2022/Set/16; entrega 2022/Set/23]

Objetivos
   Completar la clase para intercambio de mensajes entre computadoras que no comparten memoria
   Agregar la funcionalidad del servidor

Introducción
   Vamos a construir un servidor que sea capaz de atender cada solicitud por medio de un proceso separado (fork),
   es posible generar hilos también

Funcionamiento

   En esta entrega, vamos a crear los métodos de una clase "Socket" para lograr que un proceso servidor pueda
   atender solicitudes de los clientes.  Para que ambos procesos puedan "comunicarse" va a ser necesario
   definir un protocolo que ambos entiendan.  En nuestro caso, este protocolo será muy sencillo.

   Paso de mensajes por medio de sistemas Linux, descripción lógica

      ________________                                ________________
     |                |                              |                |
     |   -----------  |                              |   -----------  |
     |  |           | |                              |  |           | |
     |  | Proceso C | |                              |  | Proceso S | |
     |  |           | |                              |  |           | |
     |   -----------  |                              |   -----------  |
     |                |                              |                |
     |  ============  |                              |  ============  |
     |  +          +  |         /~~~~~~~~~~\         |  +          +  |
     |  + Sist. Op.+  |         |  Red de  |         |  + Sist. Op.+  |
     |  +  (Linux) +  |<------->|  comunic.|<------->|  + (Linux)  +  |
     |  ============  |         \~~~~~~~~~~/         |  ============  |
     |   Máquina A    |                              |   Máquina B    |
     |________________|                              |________________|



Tareas

   - Completar la clase "Socket" para poder intercambiar mensajes entre procesos que no comparten memoria.
     Se facilita la interfaz "Socket.h" y la parte básica de la implantación "Socket.cc".
     Para esta etapa deben completar:
        Accept(), acepta una solicitud de conexión TCP y crea un nuevo "handle" para intercambiar información, para
           ello deben utilizar el llamado al sistema "accept" y hacer que devuelva una nueva instancia de la clase
           "Socket"
        Socket( int ), otro constructor que recibe un entero como parámetro
        Listen( int ), marca el socket como pasivo, es decir que va a recibir conexiones por medio de Accept (listen),
           el parámetro establece el tamaño de la cola (backlog)
        Bind( int ), para relacionar este socket con el proceso (bind), el parámetro determina el # de puerto a utilizar.
           Para establecer que direcciones pueden conectarse a nuestro servidor hay que indicar ese rango en el parámetro
           "address" utilizando esta estructura para IPv4:

               struct sockaddr_in server_addr;
               ...
               server_addr.sin_family = AF_INET;	// Definimos la familia para IPv4
               server_addr.sin_addr.s_addr = htonl( INADDR_ANY );	// Establecemos cualquier dirección
               server_addr.sin_port = htons( Port );	// El puerto asociado al servicio
               ...
               len = sizeof( server_addr );
               ...
               bind( id, (const sockaddr *) & server_addr, len );

        Shutdown( int ), cierra parcialmente el Socket de acuerdo con el parámetro especificado

   - Los ejemplos "MirrorServerTest.cc" y "MirrorClientTest.cc" deben funcionar correctamente

Referencias
   https://os.ecci.ucr.ac.cr/ci0123/material/sockets-course.ppt
   https://os.ecci.ucr.ac.cr/ci1321/lab4.html