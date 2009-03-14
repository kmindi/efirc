#include <ircinterface.h>
#include <pthread.h>
#include <signal.h>

/* create an ircsocket - our interface for the irc protocol */
IRCInterface ircsocket(6667, "chat.freenode.net", "efirc-dev101",
	"efirc-dev101", "efirc-dev101", "pass",
	"", 2);
//	"efirc-dev101", "efirc-dev101", "pass",
//	fopen("cppirc.log", "w"));

pthread_t ti;

void*
listen(void* arg)
{
	ircsocket.irc_receive_messages();

	return 0;
}

void
check(const irc_msg_data *irc_message_data, void *irc_socket)
{
	IRCInterface *irc = (IRCInterface *)irc_socket;

	//printf("%s\n", irc_message_data->params);
	irc->irc_send_privmsg("#efirc", irc_message_data->params);
}

void
pong(const irc_msg_data *irc_message_data, void *irc_socket)
{
	IRCInterface *irc = (IRCInterface *)irc_socket;

	printf("%s\n", irc_message_data->params);
	//irc->irc_send_privmsg("#efirc", irc_message_data->params);
	irc->irc_send_pong(irc_message_data->params);
}

void
print(const irc_msg_data *irc_message_data, void *irc_socket)
{
	printf("%s %s %s\n", irc_message_data->sender, irc_message_data->cmd,
		irc_message_data->params);
}

void
disconnect(int)
{
	ircsocket.irc_disconnect_server();
}

int
main(int argc, const char* argv[])
{
	signal(SIGINT, &disconnect);

	ircsocket.irc_connect_server();
	ircsocket.irc_auth_client();

	/* MAIN!!!1einself11! */
	pthread_create(&ti, NULL, &listen, NULL);

	/* some client features examples
	ircsocket.irc_add_link_queue_entry("PRIVMSG", &check);
	ircsocket.irc_add_link_queue_entry("332", &check);
	ircsocket.irc_add_link_queue_entry("353", &check);
	ircsocket.irc_add_link_queue_entry("NICK", &check);
	ircsocket.irc_add_link_queue_entry("PING", &pong); */

	/* join channel and say hello to the world
	ircsocket.irc_send_join("#efirc");
	ircsocket.irc_send_privmsg("#efirc", "hello world"); */

	//ircsocket.irc_set_default_link_function(&print);

	ircsocket.irc_call_command_queue_entries();
	pthread_join(ti, NULL);

	return 0;
}

