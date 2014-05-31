import hypermedia.net.*;
import java.nio.*;
import javax.media.opengl.GL2;


PGL gl;
UDP listener;

float x1=10, y1=0, z1=0, x2=0, y2=10, z2=0;

void setup()
{
  size(500, 500, P3D); 
  background(255);
  listener = new UDP(this, 6011, null);
  listener.listen(true);
  listener.log(true);
}

void draw()
{  
  background(255);
  line(250, 250, 250, 250 + x1, 250 - y1, z1+250);
  pushMatrix();
  translate(x1+250, y1+250, z1+250);
    sphere(5);
  popMatrix();
  line(250, 250, 250, 250 + x2, 250 - y2, 250+z2);
  pushMatrix();
  translate(250+x2, 250-y2, 250+z2);
    sphere(5);
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
      if (mag > 0)
      {
        x1/=mag*2; y1/=mag*2; z1/=mag*2;
      }
      float mag2 = sqrt(pow(x2, 2) + pow(y2, 2) + pow(z2, 2));
      if (mag2 > 0)
      {
        x2/=mag2*2; y2/=mag2*2; z2/=mag2*2;
      }
      println(x1 + " " + y1 + " " + z1 + " " + x2 + " " + y2 + " " + z2);
    }
}


