#include "server.h"

static uint32_t CRC32(const uint8_t data[], size_t data_length) {
	uint32_t crc32 = 0xFFFFFFFFu;
	
	for (size_t i = 0; i < data_length; i++) {
		const uint32_t lookupIndex = (crc32 ^ data[i]) & 0xff;
		crc32 = (crc32 >> 8) ^ CRCTable[lookupIndex];
	}
	
	crc32 ^= 0xFFFFFFFFu;
	return crc32;
}

static uint8_t checksum(uint8_t *buff, size_t size) {
    uint64_t sum = 0;

    while (size > 1) {
        sum += *buff++;
        size -= 2;
    }

    if (size > 0)
        sum += *buff;

    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (uint8_t)sum;
}


void server_init(server *s, const char *source_ip, uint32_t source_port)
{
	pack_id = 0;
	s->wait = 1; 
	strcpy(s->source_ip, source_ip);
	s->source_port = source_port;

	memset(s->dest_ip, 0, sizeof(s->dest_ip));
	s->dest_port = 0;

	s->socket = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(s->socket == -1) {
		perror("Failed to create socket");
		exit(1);
	}

	server_change_protocol(s, RAW_IP);
}

static int parce_msg(server *s, uint8_t *msg, size_t size)
{
	s->dgram = msg;



	struct iphdr *iph = s->dgram;

	s->protocol = iph->protocol;

	switch(s->protocol) {
		case (RAW): {
			s->msg = msg;
			s->hdr = msg;
			s->size_msg = size;
			s->hdr = 0;
			break
		}
		case (RAW_IP || TCP): {
			if (iph->check == checksum(s->dgram, sizeof(struct iphdr))) {
				s->msg = s->dgram + sizeof(struct iphdr);
				s->hdr = s->dgram;
				s->size_msg = iph->tot_len;
				s->size_hdr = sizeof(struct iphdr);
			} else {
				s->msg = NULL;
				s->hdr = NULL;
				s->size_msg = 0;
				s->size_hdr = 0;
				return -1;
			}

			if (s->protocol == RAW_IP)
				break;
		}

		case TCP: {
			struct tcphdr *tcph = s->dgram + sizeof(struct ip);

			if (tcph->check == checksum(s->dgram, iph->tot_len)) {
				s->msg = s->dgram + sizeof(struct iphdr) + sizeof(struct tcphdr);
				s->size_hdr = sizeof(struct iphdr) + + sizeof(struct tcphdr);
			} else {
				s->msg = NULL;
				s->hdr = NULL;
				s->size_msg = 0;
				s->size_hdr = 0;
				return -1;
			}
		}
	}
	return 0;	

}

void server_change_protocol(server *s, enum PROTO p)
{
	s->protocol = p;
	memset(s->dgram, 0, sizeof(s->dgram));
	
	switch(s->protocol) {
		case (RAW_IP || TCP): {
			struct iphdr *iph = s->dgram;

			iph->ihl = 5;
			iph->version = 4;
			iph->tos = 0;
			iph->tot_len = 0;
			iph->frag_off = 0;
			iph->ttl = 255;
			iph->protocol = RAW_IP;
			iph->check = 0;
			iph->saddr = inet_addr(s->source_ip);
			if (s->protocol == RAW_IP) break;
		}

		case TCP: {
			struct tcphdr *tcph = s->dgram + sizeof(struct ip);
			
			iph->protocol = TCP;
			tcph->source = htons(s->source_port);
			tcph->seq = 0;
			tcph->ack_seq = 0;
			tcph->doff = 5;
			tcph->fin=0;
			tcph->syn=1;
			tcph->rst=0;
			tcph->psh=0;
			tcph->ack=0;
			tcph->urg=0;
			tcph->window = htons(5840);
			tcph->check = 0;
			tcph->urg_ptr = 0;
		}
	}
}

void server_send(server *s, uint8_t *data, size_t size)
{
	struct iphdr *iph = s->dgram;
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_port = htons(source_port);
	sin.sin_addr.s_addr = inet_addr(source_ip);

	switch(s->protocol) {
		case (RAW): {
		}
		case (RAW_IP || TCP): {
			iph->tot_len = sizeof(struct iphdr) + size;
			iph->id = htonl(s->pack_id);
			iph->daddr = sin.sin_addr.s_addr;
			iph->check = checksum(s->dgram, sizeof(struct iphdr));

			if (s->protocol == RAW_IP) {
				memcpy(s->dgram + sizeof(struct iphdr), data, size);
				break;
			}
		}

		case TCP: {
			struct tcphdr *tcph = s->dgram + sizeof(struct ip);

			iph->tot_len =+ sizeof(struct tcphdr);
			tcph->dest = htons(s->dest_port);
			tcph->check = checksum(s->dgram, iph->tot_len);
			memcpy(s->dgram + sizeof(struct iphdr) + sizeof(struct tcphdr), data, size);
		}
	}	

	if (sendto (s->socket, s->dgram, iph->tot_len, 0, &sin, sizeof(sin)) < 0) {
		perror("sendto failed");
	}
}

void server_recv(server *s, uint8_t *buff, size_t size)
{
	size_t recv_size = recvfrom(s->socket, buff, size, 0, NULL, NULL); 
	uint8_t *msg = parce_msg(s, buff, recv_size);
	if (msg == NULL) {
		printf("bad msg\n");
	} else {
		for (int i = 0; i < size)
	}
}