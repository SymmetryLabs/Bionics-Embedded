int variable = 0;

void setup()
{
	Serial.begin(115200);
}

void loop()
{
	function( &variable );
	Serial.println(variable);
}

void function( int *x )
{
	*x++;
}