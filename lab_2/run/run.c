#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "server.h"

static void print_buff(uint8_t * buff, size_t size) {

}

int main(int argc, char const *argv[])
{
	server s;
	server_init(&s, "127.0.0.1", 8888);
	char c[128] = {0};
	char buff[2048] = {0};
	int t;
	while (true) {
		scanf("%s",c);
		if (strcmp(c, ":write")) { // new msg 
			printf("Введите текст :");
			scanf("%s",c);
			server_send(&s, buff, strlen(buff));
		} else if (strcmp(c, ":read")) { // read socket
 			server_recv(&s, buff, sizeof(buff));
 			print_buff(s.hdr, s.size_hdr);
 			print_buff(s.msg, s.size_msg);
		} else if (strcmp(c, ":change_proto")) {
			printf("Изменнение протокола : 0 - RAW, 1 - IP, 2 - TCP");
			scanf("%d", t);
			server_change_protocol(&s, t);
		} else if (strcmp(c, ":exit")) {
			printf("end of sesion\n");
			break;
		}
	}
	return 0;	
}