 #include <stdio.h>

int encode (int i) {
		return i*6;
}

int decode (int i) {
		return i/6;
}

int main () {

		int x = encode(7);
		int v = encode(6);
		v = decode(x) * decode(v);
		x = v + decode(x);
		printf("x vaut %i.\n",x);

		return 0;
}
