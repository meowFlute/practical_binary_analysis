#include <stdio.h>
#include <stdlib.h>

unsigned char process_cmd_input();
float multiply();
float add();
float divide();

float x = 0.0f;
float y = 0.0f;

int main(int argc, char * argv[])
{
	process_cmd_input(argc, argv);
	printf("response = %f\n", divide());
	printf("response = %f\n", add());
	printf("response = %f\n", multiply());	
	return 0;
}

unsigned char process_cmd_input(int argc, char* argv[])
{
	int i;
	char ans = 0;
	if(argc == 1)
	{
		printf("just put some args in for fun plz\n");
		ans = 1;
	}
	else if(argc == 3)
	{
		x = atof(argv[1]);
		y = atof(argv[2]);
	}
	else
	{
		printf("args:\n");
		for(i = 0; i < argc; i++)
		{
			printf("\targ %d = %s\n", i, argv[i]);
		}
	}
	return ans;
}

float multiply()
{
	float ans = x * y;
	printf("divide %f * %f = %f\n", x, y, ans);
	return ans;
}

float add()
{
	float ans = x + y;
	printf("divide %f + %f = %f\n", x, y, ans);
	return ans;
}

float divide()
{
	float ans = x / y;
	printf("divide %f / %f = %f\n", x, y, ans);
	return ans;
}
