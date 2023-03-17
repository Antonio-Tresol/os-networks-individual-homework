[Ejercicio propuesto: 2023/Mar/17, entrega 2023/Mar/17]

Objetivo
   Completar la clase "Socket" para lograr el intercambio de mensajes utilizando el protocolo UDP en IPv4,
   para lograr la transferencia de datagramas.
   En el intercambio de datagramas no es necesario establecer una conexión.
   Para este trabajo, será necesario construir el servidor, quien recibirá los mensajes, identificará el emisor
   y nos devolverá una tira, y el cliente quién se encargará de enviar el primer datagrama

Procedimiento

1) Funcionamiento UDP
   - Completar la clase "Socket" para poder intercambiar mensajes UDP
      - Se facilita la interfaz "Socket.h"
      - Programar los métodos: "sendTo" y "recvFrom", de manera que los procesos puedan intercambiar
        mensajes utilizando UDP

   - Archivos facilitados:
      (clase Socket de las asignaciones anteriores)
      ipv4-udp-client.cc
      ipv4-udp-server.cc

Referencias
   Acceso al manual de Unix: "man sendto" o "man recvfrom"

   https://os.ecci.ucr.ac.cr/ci0123/material/sockets-course.ppt