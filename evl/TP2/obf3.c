#include <stdio.h>

int main () {

		int v = 7*6;

		v = (v/6)*5*6;
		if ((v/6)<2*6) v = (v/6)-7*6; else v = (v/6)+7*6;
		while ((v/6)<50*6) {
				v++;
		}
		printf("v vaut %i.\n",(v/6)/6);

		return 0;
}
