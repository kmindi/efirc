#include <ircsocket.h>
#include <pthread.h>

/* create an ircsocket - our interface for the irc protocol */
IRCSocket ircsocket(6667, "chat.freenode.net", "efirc-dev101",
	"efirc-dev101", "efirc-dev101", "pass",
	"", 2);
//	"efirc-dev101", "efirc-dev101", "pass",
//	fopen("cppirc.log", "w"));

void*
listen(void* arg)
{
	ircsocket.recv_raw();

	return 0;
}

void
check(const irc_msg_data* msg_data, void* irc_socket)
{
	IRCSocket* irc = (IRCSocket*)irc_socket;

	//printf("%s\n", msg_data->params);
	irc->send_privmsg("#efirc", msg_data->params);
}

void
pong(const irc_msg_data* msg_data, void* irc_socket)
{
	IRCSocket* irc = (IRCSocket*)irc_socket;

	printf("%s\n", msg_data->params);
	//irc->send_privmsg("#efirc", msg_data->params);
	irc->send_pong(msg_data->params);
}

int
main(int argc, const char* argv[])
{
	pthread_t ti;

	ircsocket.connect_server();
	ircsocket.auth();

	/* MAIN!!!1einself11! */
	pthread_create(&ti, NULL, &listen, NULL);

	/* some client features examples 
	ircsocket.add_link("PRIVMSG", &check);
	ircsocket.add_link("332", &check);
	ircsocket.add_link("353", &check);
	ircsocket.add_link("NICK", &check);
	ircsocket.add_link("PING", &pong); */

	/* connect to a server
	ircsocket.send_join("#efirc");
	ircsocket.send_privmsg("#efirc", "hi"); */

	ircsocket.call_cmd();
	pthread_join(ti, NULL);

	return 0;
}

