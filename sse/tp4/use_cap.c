/* 
use_cap.c 

Code d√©riv√© du "Linux Capability Exploration Lab" de Wenliang Du, Syracuse University.

compilation
gcc -o use_cap -lcap use_cap.c


La capacit√© CAP_DAC_READ_SEARCH permet d'outrepasser les contr√¥les de permissions lors 
de la lecture de fichiers ou l'identification du contenu (recherche) de r√©pertoires

*/

#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/capability.h>
#include <sys/capability.h>


/*
extraits de la page man(3) de cap_set_flag

int cap_set_flag(cap_t cap_p, cap_flag_t flag, int ncap, 
                 const cap_value_t *caps, cap_flag_value_t value);

-cap_value_t      : identifies a capability, such as CAP_CHOWN.
-cap_flag_t       : identifies one of the three flags associated with a
                    capability (CAP_EFFECTIVE, CAP_INHERITABLE or CAP_PERMITTED).
-cap_flag_value_t : identifies the setting of a particular capability
                    flag (i.e, the value of a capability in a set).
                    Valid values for this type are CAP_CLEAR or CAP_SET.

cap_p est une structrure de donnÈes contenant un Ètat des capacitÈs
caps est un tableau de capacitÈs, ncap indique le nombre d'ÈlÈments du tableau

*/

int cap_disable(cap_value_t capacity_to_disable)
{
	cap_t process_caps;
	process_caps = cap_get_proc();
	if (process_caps == NULL)
		return -1;

	if (cap_set_flag(process_caps, CAP_EFFECTIVE, 1, &capacity_to_disable, CAP_CLEAR) != 0)
		return -1;

	if (cap_set_proc(process_caps) != 0)
		return -1;
	else
		return 0;
}

int cap_enable(cap_value_t capacity_to_enable)
{
	cap_t process_caps;
	process_caps = cap_get_proc();
	if (process_caps == NULL)
		return -1;

	if (cap_set_flag(process_caps, CAP_EFFECTIVE, 1, &capacity_to_enable, CAP_SET) != 0)
		return -1;

	if (cap_set_proc(process_caps) != 0)
		return -1;
	else
		return 0;
}

int cap_drop(cap_value_t capacity_to_drop)
{
	cap_t process_caps;
	process_caps = cap_get_proc();
	if (process_caps == NULL)
		return -1;

	if (cap_set_flag(process_caps, CAP_EFFECTIVE, 1, &capacity_to_drop, CAP_CLEAR) != 0)
		return -1;
	if (cap_set_flag(process_caps, CAP_PERMITTED, 1, &capacity_to_drop, CAP_CLEAR) != 0)
		return -1;

	if (cap_set_proc(process_caps) != 0)
		return -1;
	else
		return 0;
}


int main(void)
{
	pid_t proc_pid;

	proc_pid = getpid();
	printf("Le PID du processus courant est %d\n\n", proc_pid);

	if (open ("/etc/shadow", O_RDONLY) < 0)
		printf("(a) Echec de l'ouverture\n");

	if (cap_disable(CAP_DAC_READ_SEARCH) < 0) return -1;

	if (open ("/etc/shadow", O_RDONLY) < 0)
		printf("(b) Echec de l'ouverture\n");

	if (cap_enable(CAP_DAC_READ_SEARCH) < 0) {
		printf("Echec d'attribution de capacitÈ. Interruption du programme.\n");
		return -1;
	}

	if (open ("/etc/shadow", O_RDONLY) < 0)
		printf("(c) Echec de l'ouverture\n");

	if (cap_drop(CAP_DAC_READ_SEARCH) < 0) return -1;

	if (open ("/etc/shadow", O_RDONLY) < 0)
		printf("(d) Echec de l'ouverture\n");

	if (cap_enable(CAP_DAC_READ_SEARCH) == 0) return -1;

	if (open ("/etc/shadow", O_RDONLY) < 0)
		printf("(e) Echec de l'ouverture\n");
}
