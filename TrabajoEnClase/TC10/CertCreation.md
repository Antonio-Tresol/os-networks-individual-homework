## Create a Self-Signed Certificate using OpenSSL on Fedora

To create a self-signed certificate using OpenSSL and save it to a file called "ci0123.pem". Here are the detailed instructions:

1. **Open the terminal**: Press `Ctrl` + `Alt` + `T` or search for "Terminal" in the applications menu.

2. **Install OpenSSL** if it's not already installed on your system. Run the following command:

if using a distribution based on Red Hat Enterprise Linux, such as Fedora, CentOS, or Red Hat Enterprise Linux itself, run the following command:
```bash
sudo dnf install openssl
```
if you are using Debian-based distro, run the following command:

```bash
sudo apt install openssl
```

3. **Create a new directory** to store the certificate and private key. Replace `/path/to/directory` with your desired directory path:

```bash
mkdir /path/to/directory
cd /path/to/directory
```

4. **Create a self-signed certificate and private key** using OpenSSL with the following command:

```bash
openssl req -x509 -newkey rsa:4096 -keyout ci0123.key -out ci0123.crt -days 365
```

This command will generate a 4096-bit RSA private key and a self-signed X.509 certificate that will be valid for 365 days. Replace "365" with the desired number of days for the certificate's validity.

This will ask for a passphrase for the private key. 

5. During the certificate generation process, you'll be prompted to enter a **passphrase for the private key** and to provide information for the **certificate's subject fields**. Fill in the required information:

- Country Name (2 letter code)
- State or Province Name (full name)
- Locality Name (city)
- Organization Name (company)
- Organizational Unit Name (section)
- Common Name (domain name)
- Email Address

6. **Combine the private key and certificate** into a single .pem file:

```bash
cat ci0123.key ci0123.crt > ci0123.pem
```

1. If needed, **remove the passphrase from the private key** to avoid entering it every time you use the key:

```bash
openssl rsa -in ci0123.key -out ci0123_nopass.key
```

Replace "ci0123.key" in your applications with "ci0123_nopass.key" if you choose to use the key without a passphrase.

After this steps, self-signed certificate will be stored in the "ci0123.pem" file in the specified directory.