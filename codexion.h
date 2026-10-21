#ifndef	CODEXION
#define	CODEXION
 #include <pthread.h>

typedef struct	Data{
	int	number_coders;
	int time_compile;
	int	time_burnout;
	int	time_debug;
	int	time_refactor;
	int	number_compiles;
	int	dongle_cooldown;
	char	*scheduler;
}	t_data;

typedef struct Coder{
	int number_compiles;
	int id;
	long int last_compilation;
	pthread_t thread;
}	t_coder;
#endif
