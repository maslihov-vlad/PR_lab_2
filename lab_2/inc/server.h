#include <stdio.h>	
#include <stdint.h>
#include <string.h> 
#include <sys/socket.h>	
#include <stdlib.h> 
#include <errno.h> 
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

enum PROTO 
{
	RAW,
	RAW_IP,
	TCP,
	UDP	
};


struct tcp_header
{
	uint32_t source_address;
	uint32_t dest_address;
	uint8_t placeholder;
	uint8_t protocol;
	uint16_t tcp_length;
};


typedef struct server_
{
	enum PROTO protocol;
	int socket;
	struct sockaddr_in sin;

	uint8_t source_ip[32];
	uint32_t source_port;

	uint8_t dest_ip[32];
	uint32_t dest_port;

	uint8_t dgram[4096];
	
	uint8_t *msg;
	size_t size_msg;

	uint8_t *hdr;
	size_t size_hdr;

	uint32_t wait;
	uint32_t pack_id;
} server;

void server_init(server *s, const char *source_ip, const char *ip, uint32_t port);

void server_change_protocol(server *s);

void server_send(server *s, uint8_t *data, size_t size);

void server_recv(server *s, uint8_t *buff, size_t size);

