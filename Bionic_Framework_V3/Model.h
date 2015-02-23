/*
REMINDER - Keep the Interface and Implementation separate!!!

-- Would I ever want different types of Models? --
Probably.  One could have different pipelines it switches between.
Another could relate to several Model+View combos running simultaneously.
How does this last one work?  Maybe you just have several of these objects!  Each with their own unique parts
*/


/*
Perhaps I define this one to manage each Model?
Separating the Model and Pipeline allows me to have
several models running simultaneously, and potentially
do superposition...
*/
class ModelManager;

/*
class Model {
	public:
		Model();
		virtual ~Model(); // Item 7 in Effective C++

		void update();

	private:
		// Need to define this object
		MPUData modelInput;

		// Vector for my Pipelines
		// Need to define this object too
		Pipe[] pipes;

};
*/

/*
INPUT = unprocessed data - what does this look like? ranges? number of inputs?
OUTPUT = processed data - what does this look like? ranges? number of outputs?
A Pipeline is COMPOSED of various filters

PIPELINE IS ABSTRACT!

Should I use a factory here?
*/
class Pipe {
	public:
		// Pipe();
		// virtual ~Pipe(); // Item 7 in Effective C++

		virtual void update( long _deltaMs ) = 0;

	private:
		// Eventually...
		// How do these attach to one another?
		// Filter[] filters;

		// I'm not sure if I want this...
		// virtual void attachInputs() = 0;
};




// Deriving this sets the global interface
// I start to implement more specifics here: specify the inputs, implement the filters, etc...
// Should also start to define how to access the outputs here...
class Pipe_MagnitudeAcceleration : public Pipe {
	public:
		Pipe_MagnitudeAcceleration( VectorInt16 *_accel );
		// ~Pipe_MagnitudeAcceleration();
		void update( long _deltaMs );

	private:
		// Do I want this?
		// void attachInputs( VectorInt16 &_accel ) { accel = &_accel; }

		// Can I make this a constant reference?
		VectorInt16 *accel;

		// What type of object structure will I use here?
		// Filter_MovingAverage filter;
		// I don't know if I want to instantiate these here...
		float filterValue = 0;
		float filterAlpha = 0.9;
};


Pipe_MagnitudeAcceleration::Pipe_MagnitudeAcceleration( VectorInt16 *_accel ) {
	accel = _accel;
}


void Pipe_MagnitudeAcceleration::update( long _deltaMs ) {
	// Condition data before passing to filter
	float currentMagnitude = accel->getMagnitude();

	// Feed the filter
	// filter.update( magnitude );

	// But for now...
	filterValue = (filterAlpha*_deltaMs) * filterValue  +  ((1-filterAlpha)*_deltaMs) * currentMagnitude;
}




// Deriving this sets the global interface
// I start to implement more specifics here: specify the inputs, implement the filters, etc...
// Should also start to define how to access the outputs here...
class Pipe_LPFRoll : public Pipe {
	public:
		Pipe_LPFRoll( const float _ypr[3] );
		// ~Pipe_LPFRoll();
		void update( long _deltaMs );
		// float output();

	private:
		const float *ypr[3];

		float filterValue = 0;
		float filterAlpha = 0.5;
};


Pipe_LPFRoll::Pipe_LPFRoll( const float _ypr[3] ) {
	for ( int i=0; i<3; ++i ) ypr[i] = (&_ypr)[i];
}


void Pipe_LPFRoll::update( long _deltaMs ) {

	// But for now...
	filterValue = (filterAlpha*_deltaMs) * filterValue  +  ((1-filterAlpha)*_deltaMs) * (*ypr[2]);
}
