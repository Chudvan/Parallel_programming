#include <iostream>
#include <unistd.h>


int main(){
	int N = 5;
	int my_ps_num = 0;

	for(int i=1; i<=N; i++){
		if (!fork())
		{
			my_ps_num = i;
			break;
		}
	}

	if(!my_ps_num)
	{
		std::cout << "i'm main process" << std::endl;
	}
	else
	{
		std::cout << "i'm child process number " << my_ps_num << std::endl;
	}
}
