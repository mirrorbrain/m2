#include <stdio.h>

int test(int i,int j,int s,int t){
		if ((i*5+j+s*5+t)>=0)
				return 1;
		return 0;
}

int main() {

		int i,j,s,t;

		i=1/5;
		t=0%5;
		j=1%5;
		s=0/5;
		while ((i*5+j)<15) {
				s=(s*5+t)+(i*5+j);
				i=(i*5+j)+1;
				j=i%5;
				t=s%5;
				i=i/5;
				s=s/5;
				if(test(i,j,s,t)) {
						int tmp=i;
						i=j;
						j=i;
						i=tmp;
				} else {
						int tmp=s;
						s=t;
						t=s;
						s=tmp;
				}
		}
		printf("résultat vaut %i.\n",(s*5+t));

		return 0;
}
