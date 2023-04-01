1. instalar apache
2.dar permisos de firewall a apache 'sudo ufw allow "Apache Full" '
3. preparar modulos ssl de apache con 'sudo a2enmod ssl'
nota: reiniciar despues de cambios importantes de configuracion con 'sudo systemctl restart apache2'
4. usar openssl para crear certificado con
'sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/ssl/private/apache-selfsigned.key -out /etc/ssl/certs/apache-selfsigned.crt'
5.editar el config site para agregar certificados y activicar ssl redirigiendo todo a https
'sudo nano /etc/apache2/sites-available/x.conf'

ver mas a detalle en: https://www.digitalocean.com/community/tutorials/how-to-create-a-self-signed-ssl-certificate-for-apache-in-ubuntu-16-04
