// ================================================================
// ===                    BASIC PARAMETER                       ===
// ================================================================

class BasicParameter {
	public:
		BasicParameter( char *_name, float _initialValue, float _minValue, float _maxValue );
		void getName( char *_name );
		float getValue();
		float getPercent();
		float getMin();
		float getMax();
		void setValue( float _newValue );
		void setPercent ( float _newPercent );

	private:
		char name[5];
		float currentValue;
		float minValue;
		float maxValue;
};

BasicParameter::BasicParameter( char *_name, float _initialValue, float _minValue, float _maxValue ) {
	for ( int i=0; i < sizeof(name); i++ ) name[i] = _name[i];
	currentValue = _initialValue;
	minValue = _minValue;
	maxValue = _maxValue;
}

// Store the internal name variable in the _name array passed to this function
void BasicParameter::getName( char *_name ) {
	for ( int i=0; i < sizeof(_name); i++ ) {
		_name[i] = name[i];
	}
}

float BasicParameter::getValue() { return currentValue; }

float BasicParameter::getPercent() { return (currentValue-minValue)/(maxValue-minValue); }

float BasicParameter::getMin() { return minValue; }

float BasicParameter::getMax() { return maxValue; }

void BasicParameter::setValue( float _newValue ) { currentValue = _newValue; }

void BasicParameter::setPercent( float _newPercent ) {
	float newValue = minValue + _newPercent * (maxValue-minValue);
	currentValue = newValue;
}