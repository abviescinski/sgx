#include <iostream>

using namespace std;

bool testing(int n){

	if (n == 10){
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
	
}

int main (){
	
	int num; 
	bool ret;
	cout << "Digite um numero: \n";
	cin >>  num;
	
	ret = testing(num);
	
	cout << "O retorno foi "<< ret <<endl;

}
