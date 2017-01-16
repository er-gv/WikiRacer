#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>


#include <netdb.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "error_codes.h"
#define FAIL    -1
#define WIKI_HOST "en.wikipedia.org"
#define SSL_PORT  "443"


#include "wiki_socket.h"
#define DEBUG_MODE 1
typedef struct wiki_socket_t {
    int sockfd;
    struct addrinfo *servinfo;
    SSL_CTX* ssl_ctx;
    SSL* sslConnection;
    
}WikiSocket;


static WikiSocket wikiSocket;
static void logSSLError(int ret);
static SSL_CTX* ssl_ctx = NULL;



static int initSocket(){
    
    int httpStatus;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = 0;              // fill in my IP for me
    
    struct addrinfo** addr_list = &wikiSocket.servinfo;
    
    httpStatus = getaddrinfo(WIKI_HOST, SSL_PORT, &hints, &wikiSocket.servinfo); 
    if(httpStatus !=0){
        //return httpStatus
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(httpStatus));
        exit(1);
    }
        
    wikiSocket.sockfd = socket((*addr_list)->ai_family, (*addr_list)->ai_socktype, (*addr_list)->ai_protocol);
    if(wikiSocket.sockfd ==-1){
        //return httpStatus
        fprintf(stderr, "soket() call failed.\n");
        exit(1);
    }
    
    httpStatus = connect(wikiSocket.sockfd, (*addr_list)->ai_addr, (*addr_list)->ai_addrlen);
    if(httpStatus <0){
        //return httpStatus
        fprintf(stderr, "wikiSocket did not connect to %s.\nProgram now aborts.\n", WIKI_HOST);
        close(wikiSocket.sockfd); 
        exit(1);
    }
    
    return httpStatus;
}

static void initSSL(){
    static int flag = 1;
    if(flag>0){
        flag--;
        SSL_load_error_strings ();
        SSL_library_init ();
        ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    }
}

    
int setupSSLConnection(){
    initSSL();
    wikiSocket.ssl_ctx = ssl_ctx;
    wikiSocket.sslConnection = SSL_new(wikiSocket.ssl_ctx);
    //bind the ssl to the socket
    SSL_set_connect_state(wikiSocket.sslConnection);
    SSL_set_fd(wikiSocket.sslConnection, wikiSocket.sockfd);
    return SSL_connect(wikiSocket.sslConnection);  
}


void ShowCerts(){
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(wikiSocket.sslConnection);	/* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);							/* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);							/* free the malloc'ed string */
        X509_free(cert);					/* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

int openWikiConnection(){
    
    int status =0;
    wikiSocket.sockfd = -1;
    wikiSocket.servinfo = NULL;
    wikiSocket.ssl_ctx = NULL;
    wikiSocket.sslConnection = NULL;
    
    status = initSocket();
    //printf("open socket status: %d.\n", status);
    if(status ==0){
        //puts("Got TCP Socket. init SSL connection");
        status = setupSSLConnection();
        if(status == 1 && DEBUG_MODE){
            //ShowCerts();
        }
    }
    return status;
}

int fetchWikiPage(const char* search_term){
    int msgLen;
    char msg[1024];
   
    int bytesWritten, bytesLeft;
    int i=0;
    sprintf(msg, "GET /wiki/%s HTTP/1.0\r\nHost: %s\r\n\r\n", search_term, WIKI_HOST);
    msgLen = strlen(msg);
    //printf("Let\'s send %s \n", msg);
    
    bytesWritten =0;
    bytesLeft= msgLen;
    //SSL_set_connect_state(wikiSocket.sslConnection);
    //SSL_set_fd(wikiSocket.sslConnection, wikiSocket.sockfd);
    
    do{
        int bytes = SSL_write(wikiSocket.sslConnection, msg, bytesLeft); 
        i++;
        //printf("called SSL_write %d times in order to send request %s\n", i, msg);
        if(bytes< 0){
            if(SSL_get_error(wikiSocket.sslConnection,bytes)!=SSL_ERROR_WANT_READ){
                logSSLError(bytes);
                return -1;
            }
            else
                continue;
        }
        bytesWritten+=bytes;
        bytesLeft -= bytes;
    }while(bytesLeft>0);
    
    return (bytesWritten==msgLen?1:0);
}

int readFragmentOfWikiPage(char* buffer, const unsigned int buffSize){
    
    
    int bytesRead = 0;
    int bytesLeft = buffSize; 
    //SSL_set_connect_state(wikiSocket.sslConnection);
    do{
        int bytes = SSL_read(wikiSocket.sslConnection, buffer, bytesLeft); 
       // printf("read %d bytes from socket.\n", bytes);
        if(bytes<= 0){
             logSSLError(bytes);
            return -1;
        }
        //puts(buffer);
        bytesRead+=bytes;
        bytesLeft -= bytes;
    }while(bytesLeft>0);
    
    return bytesRead;
}


static void logSSLError(int ret){
    switch (SSL_get_error(wikiSocket.sslConnection, ret)){
        case SSL_ERROR_NONE:
            puts("no SSL errors here. everything is OK");
            break;
            
        case SSL_ERROR_ZERO_RETURN:
            puts("The TLS/SSL connection has been closed");
            break;
            
        case SSL_ERROR_WANT_READ:
            puts("The operation did not complete; the same TLS/SSL Read function should be called again later.\nIf, by then, the underlying BIO has data available for reading then some TLS/SSL protocol progress will take place,\n i.e. at least part of an TLS/SSL record will be read.");
            break;
            
        case SSL_ERROR_WANT_WRITE:
            puts("The operation did not complete; the same TLS/SSL Write function should be called again later.\nIf, by then, the underlying BIO has data available for writing then some TLS/SSL protocol progress will take place,\n i.e. at least part of an TLS/SSL record will be written.");
            break;
            
        case SSL_ERROR_WANT_CONNECT:
            puts("The underlying BIO was not connected yet to the peer and the call would block in connect()/accept()");
            break;
            
        case SSL_ERROR_WANT_ACCEPT:
            puts("The underlying BIO was not connected yet to the peer and the call would block in connect()/accept()");
            break;
            
        case SSL_ERROR_WANT_X509_LOOKUP:
            puts("The operation did not complete because an application callback set by SSL_CTX_set_client_cert_cb()\nhas asked to be called again. The TLS/SSL I/O function should be called again later. Details depend on the application.");
            break;
            
        case SSL_ERROR_SYSCALL:
            puts("Some I/O error occurred.");
            break;
            
        case SSL_ERROR_SSL:
            puts("A failure in the SSL library occurred, usually a protocol error.\nThe OpenSSL error queue contains more information on the error.");
            break;
        
        default:
            puts("Unknown error occured.");
    }
}

void closeWikiConnection(){
    
    
    SSL_free(wikiSocket.sslConnection);
    //SSL_CTX_free(wikiSocket.ssl_ctx);
    //freeaddrinfo(wikiSocket.servinfo); // free the linked-list
    
}

void freeTCPResources(){
    
    
    SSL_CTX_free(wikiSocket.ssl_ctx);
    freeaddrinfo(wikiSocket.servinfo); // free the linked-list
    close(wikiSocket.sockfd); 
}


