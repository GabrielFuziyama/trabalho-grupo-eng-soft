#include <iostream>
//#include <cassert>
#include <assert.h>
#include <cmath>

using namespace std;


float divisao( float numerador, float denominador){
	return numerador/denominador;
}


int main(){

	float resultado = divisao(10, 2);
	
	//if ( resultado == 5 ) 
	//	cout << "NAO FUDEU" << endl;
	//else
	//	cout << "FUDEU" << endl;

	assert( resultado == 5 );
	
	
	resultado = divisao(10, 3);
	assert( fabs(resultado - 3.33) < 0.0001);
	
	return 0;
}











