import hypermedia.net.*;
import java.nio.*;
import javax.media.opengl.GL2;


PGL gl;
UDP listener;
float step1 = 0;
float step2 = 0;
float step3 = 0;
float ix, iy;
float stepx=0, stepy=0;

float x1=10, y1=0, z1=0, x2=0, y2=10, z2=0;

void setup()
{
  size(500, 500, P3D); 
  background(255);
  listener = new UDP(this, 6008, null);
  listener.listen(true);
  listener.log(true);
}

void draw()
{  
  background(255);
  pushMatrix();
  translate(250, 250, 250);
  rotateZ(step1/50 + stepy/50);
  rotateX(step2/50 + stepx/50);
  rotateY(step3);
  step3 += 0.01;
  translate(-250, -250, -250);
  line(250, 250, 250, 250, 220, 250);
  line(250, 250, 250, 270, 250, 250);
  line(250, 250, 250, 250, 250, 270);
  //Cube(10, 10, 10);
  line(250, 250, 250, 250 + x1, 250 - y1, z1+250);
  pushMatrix();
  translate(x1+250, 250 - y1, z1+250);
    box(2, 2, 2);
  popMatrix();
  line(250, 250, 250, 250 + x2, 250 - y2, 250+z2);
  pushMatrix();
  translate(250+x2, 250-y2, 250+z2);
    sphere(2);
  popMatrix();
  popMatrix();
}

void receive(byte[] message, String ip, int port)
{
    String mystr = new String(message, 0, message.length);
    String[] strs = mystr.split("\t");
    if (strs.length == 6)
    {
      x1 = Float.parseFloat(strs[0]);
      y1 = Float.parseFloat(strs[1]);
      z1 = Float.parseFloat(strs[2]);
      x2 = Float.parseFloat(strs[3]);
      y2 = Float.parseFloat(strs[4]);
      z2 = Float.parseFloat(strs[5]);
      float mag = sqrt(pow(x1, 2) + pow(y1, 2) + pow(z1, 2));
      if (mag != 0)
      {
        x1/=mag; y1/=mag; z1/=mag;
      }
      float mag2 = sqrt(pow(x2, 2) + pow(y2, 2) + pow(z2, 2));
      if (mag2 != 0)
      {
        x2/=mag2; y2/=mag2; z2/=mag2;
      }
      x1*=20; y1*=20; z1*=20; x2*=20; y2*=20; z2*=20;
      println(x1 + " " + y1 + " " + z1 + " " + x2 + " " + y2 + " " + z2);
      println("Magnitude1: " + mag + "\tMagnitude2: " + mag2);
    }
}

boolean lock = false;

void mousePressed()
{
    lock = true;
    ix = mouseX;
    iy = mouseY;
}

void mouseDragged()
{
  if(lock)
  {
   stepx = mouseX - ix;
   stepy = mouseY - iy;
  }
}

void mouseReleased()
{
  lock = false;
  step1 = step1 + stepy;
  step2 = step2 + stepx;
  stepx = 0;
  stepy = 0;
}
