#include "BasicParameter.h"

using namespace std;
// Initialize two BasicParameters

// Option 1 - THIS DOES NOT WORK
// BasicParameter param1;
// BasicParameter param2;
// param1 = new BasicParameter("leve", 0.5, 0.0, 1.0);
// param2 = new BasicParameter("leve", 1.5, 1.0, 2.0);

// Option 2 - THIS DOES NOT WORK
// BasicParameter param1 = new BasicParameter("leve", 0.5, 0.0, 1.0);
// BasicParameter param2 = new BasicParameter("leve", 1.5, 1.0, 2.0);

// Option 3 = THIS WORKS
BasicParameter param1("leve", 0.5, 0.0, 1.0);
BasicParameter param2("leve", 1.5, 1.0, 2.0);

// Array of BasicParameters

// BasicParameter *params[3];
// params[0] = new BasicParameter();
// BasicParameter *params[3] = { &param1, &param1, &param1 };
// params[0] = param2;
// *params[1] = param2;
// *params[2] = param1;

// BasicParameter 

// // BasicParameter params[2] = { param1, param2 };
// BasicParameter * params[3] = { nullptr, nullptr, nullptr };
// // BasicParameter params[3] = { std::nullptr, std::nullptr, std::nullptr };
// params[0] = param1;
// params[1] = param2;


void setup()
{

	Serial.begin(115200);
	while (!Serial);
	delay(1000);
	Serial.println("Begin BasicParameter Test");
	Serial.println("-----");

}

void loop()
{

	// Serial.println("Print by reference");
	// printParameter( param1 );
	// printParameter( param2 );
	// Serial.println();

	// Serial.println("Print by pointer");
	// printParameterP( &param1 );
	// printParameterP( &param2 );
	// Serial.println();

	// Change the value by passing the object
	// setParameter( param1 );

	// Change the value by passing a pointer
	// setParameterP( &param1 );

	printByPassParameter1( &param1 );

	Serial.println("-----");
	delay(2000);

}


// Write the BasicParameter
// Pass one to a write function
void printParameterP( BasicParameter *_p) {
	char name[5];
	_p->getName(name);
	// for ( byte i = 0; i < sizeof(name); i++ ) Serial.println(name[i], HEX);
	Serial.print("Name: "); Serial.println(name);
	Serial.print("Val: "); Serial.println(_p->getValue());
	Serial.print("Min: "); Serial.println(_p->getMin());
	Serial.print("Max: "); Serial.println(_p->getMax());
}

void printParameter( BasicParameter &_p ) { // Should the argument be a const?  Won't compile...
	char name[5];
	_p.getName(name);
	// for ( byte i = 0; i < sizeof(name); i++ ) Serial.println(name[i], HEX);
	Serial.print("Name: "); Serial.println(name);
	Serial.print("Val: "); Serial.println(_p.getValue());
	Serial.print("Min: "); Serial.println(_p.getMin());
	Serial.print("Max: "); Serial.println(_p.getMax());
}

// Works
void setParameter( BasicParameter &_p ) { // Should the argument be a const?  Won't compile...
	_p.setValue(1.0);
}

// Works
void setParameterP( BasicParameter *_p ) { // Should the argument be a const?  Won't compile...
	_p->setValue(1.0);
}

void printByPassParameter1 ( BasicParameter *_p ) {
	// printParameterP( _p );
	printByPassParameter2 ( _p );
}

void printByPassParameter2 ( BasicParameter *_p ) {
	printParameterP( _p );
}