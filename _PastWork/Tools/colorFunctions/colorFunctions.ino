int hsb[3];
uint32_t color;

void setup() {
  Serial.begin(9600);
  
//  hsb[0] = 255;
//  hsb[1] = 255;
//  hsb[2] = 255;
//  HSBtoRGB(hsb, &color);

//  color = Color(0, 0, 255);
//  RGBtoHSB(color,hsb);
//  Serial.print("Hue ");
//  Serial.println(hsb[0]);
//  Serial.print("Sat ");
//  Serial.println(hsb[1]);
//  Serial.print("Val ");
//  Serial.println(hsb[2]);
}

void loop() {
}




uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}




void HSBtoRGB(int hsb[], uint32_t *c) { 
  
  int r;
  int g;
  int b;
  int base;
  int hue;
  int sat;
  int val;
  uint32_t color;
  hue = hsb[0];
  sat = hsb[1];
  val = hsb[2];
  
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
  
    r   = val;
    g = val;
    b  = val; 
   
  } else  { 
    
    base = ((255 - sat) * val)>>8;
  
    switch(hue/60) {
	case 0:
		r = val;
		g = (((val-base)*hue)/60)+base;
		b = base;
	break;
	
	case 1:
		r = (((val-base)*(60-(hue%60)))/60)+base;
		g = val;
		b = base;
	break;
	
	case 2:
		r = base;
		g = val;
		b = (((val-base)*(hue%60))/60)+base;
	break;
	
	case 3:
		r = base;
		g = (((val-base)*(60-(hue%60)))/60)+base;
		b = val;
	break;
	
	case 4:
		r = (((val-base)*(hue%60))/60)+base;
		g = base;
		b = val;
	break;
	
	case 5:
		r = val;
		g = base;
		b = (((val-base)*(60-(hue%60)))/60)+base;
	break;
    }
      
    color = r;
    color <<= 8;
    color |= g;
    color <<= 8;
    color |= b;
  }   
  *c = color;
}




void RGBtoHSB(uint32_t c, int hsb[]) {
  byte r;
  byte g;
  byte b;
  byte minRGB;
  byte maxRGB;
  byte d;
  byte h;
  int hue;
  byte sat;
  byte val;
  
  b = color&0xFF;
  color >>= 8;
  g = color&0xFF;
  color >>= 8;
  r = color&0xFF;
  
//  Serial.print("red ");
//  Serial.println(r);
//  Serial.print("green ");
//  Serial.println(g);
//  Serial.print("blue ");
//  Serial.println(b);
  
  minRGB = min( r, min(g,b) );
  maxRGB = max( r, max(g,b) );
  
//  Serial.print("minRGB ");
//  Serial.println(minRGB);
//  Serial.print("maxRGB ");
//  Serial.println(maxRGB);
  
  if( minRGB==maxRGB ) {
    hue = 0;
    sat = 0;
    val = minRGB;
  }
  else {
    d = (r==minRGB) ? g-b : ( (b==minRGB) ? r-g : b-r );
//    Serial.print("d ");
//    Serial.println(d);
    h = (r==minRGB) ? 3 : ( (b==minRGB) ? 1 : 5 );
//    Serial.print("h ");
//    Serial.println(h);
    hue = 60*h - (60*d)/(maxRGB-minRGB);
    sat = 255*float(maxRGB-minRGB)/maxRGB;
    val = maxRGB;
  }
  
  hsb[0] = hue;
  hsb[1] = sat;
  hsb[2] = val;
}
