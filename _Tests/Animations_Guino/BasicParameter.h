#include <stdio.h>
#include <string.h>

// ================================================================
// ===                    BASIC PARAMETER                       ===
// ================================================================

class BasicParameter {
	public:
		BasicParameter() {}
		BasicParameter( char *_name, float _initialValue, float _minValue, float _maxValue );
		// BasicParameter(const BasicParameter& _p);
		void getName( char *_name );
		float getValue();
		float getPercent();
		float getMin();
		float getMax();
		void setValue( float _newValue );
		void setPercent ( float _newPercent );
		void setMin( float _newMin );
		void setMax( float _newMax );
		void setBasicParameter ( float _value, float _min, float _max );
		float currentValue;

	private:
		char name[5];
//		float currentValue;
		float minValue;
		float maxValue;
};

BasicParameter::BasicParameter( char *_name, float _initialValue, float _minValue, float _maxValue ) {
	strcpy(name, _name);
	currentValue = _initialValue;
	minValue = _minValue;
	maxValue = _maxValue;
}

void BasicParameter::setMin ( float _newMin ) {
	minValue = _newMin;
}

void BasicParameter::setMax ( float _newMax ) {
	maxValue = _newMax;
}

// The copy constructor
// BasicParameter::BasicParameter( const BasicParameter& _p ) : currentValue(_p.currentValue), minValue(_p.minValue), maxValue(_p.maxValue)
// {
// 	strcpy(name, _p.name);
// };


// Store the internal name variable in the _name array passed to this function
void BasicParameter::getName( char *_name ) {
	// for ( byte i=0; i < sizeof(_name); i++ ) {
	// 	_name[i] = name[i];
	// }
	// _name[sizeof(_name)] = '\0';
	strcpy(_name, name);
}

float BasicParameter::getValue() { return currentValue; }

float BasicParameter::getPercent() { return (currentValue-minValue)/(maxValue-minValue); }

float BasicParameter::getMin() { return minValue; }

float BasicParameter::getMax() { return maxValue; }

void BasicParameter::setValue( float _newValue ) {
	// Constrain incoming values
	if ( _newValue > maxValue ) _newValue = maxValue;
	else if ( _newValue < minValue ) _newValue = minValue;

	currentValue = _newValue;

}

void BasicParameter::setPercent( float _newPercent ) {
	// Constrain incoming values
	if ( _newPercent > 1. ) _newPercent = 1.;
	else if ( _newPercent < 0. ) _newPercent = 0.;

	float newValue = minValue + _newPercent * (maxValue-minValue);
	currentValue = newValue;
}


void BasicParameter::setBasicParameter( float _newValue, float _newMin, float _newMax ) {
  currentValue = _newValue;
  minValue = _newMin;
  maxValue = _newMax;
}
