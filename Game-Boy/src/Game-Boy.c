#include "Game-Boy.h"

int main(void)
{
	t_config* config;
	t_log* logger;

	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");

	printf("GAME BOY iniciando ... \n");

	config_destroy(config);
	log_destroy(logger);
	return 0;
}
