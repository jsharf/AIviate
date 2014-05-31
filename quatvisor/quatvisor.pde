import hypermedia.net.*;
import java.nio.*;
import javax.media.opengl.GL2;
import processing.core.PMatrix3D;

PGL gl;
UDP listener;


float x=0, y=0, z=0, w=1;

void setup()
{
  size(500, 500, P3D); 
  listener = new UDP(this, 6010, null);
  listener.listen(true);
  listener.log(true);
}

void draw()
{
  background(255);
  //listener.listen(true);
  float DEGTORAD = PI / 180;

  Quaternion xrotQuat, yrotQuat, zrotQuat, rotQuat;
  Matrix4x4 m;
  float angleX, angleY, angleZ;
  
    gl = beginPGL();

//  angleX = angleDegreesX * DEGTORAD;
//  angleY = angleDegreesY * DEGTORAD;
//  angleZ = angleDegreesZ * DEGTORAD;
//
//  xrot = Vec3D.Z_AXIS.copy().rotateX( angleX );
//  yrot = Vec3D.X_AXIS.copy().rotateY( angleY );
//  zrot = Vec3D.X_AXIS.copy().rotateZ( angleZ );
//
//  xrotQuat = new Quaternion( 1, new Vec3D( 0, 0, 0 ) );
//  xrotQuat.set( xrot.dot( Vec3D.Z_AXIS.copy() ), xrot.cross( Vec3D.Z_AXIS.copy() ) );
//
//  yrotQuat = new Quaternion( 1, new Vec3D( 0, 0, 0 ) );
//  yrotQuat.set( yrot.dot( Vec3D.X_AXIS.copy() ), yrot.cross( Vec3D.X_AXIS.copy() ) );
//
//  zrotQuat = new Quaternion( 1, new Vec3D( 0, 0, 0 ) );
//  zrotQuat.set( zrot.dot( Vec3D.X_AXIS.copy() ), zrot.cross( Vec3D.X_AXIS.copy() ) );

  //rotQuat = new Quaternion(w, x, y, z);
  rotQuat = new Quaternion(w, x, y, z);
  m   = rotQuat.getMatrix();

//  FloatBuffer rotMatrix;
//  rotMatrix = ByteBuffer.allocateDirect(4 * 4 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
//  rotMatrix.put
//    (
//  new float[]
//  {
//    (float)m.matrix[0][0], (float)m.matrix[0][1], (float)m.matrix[0][2], (float)m.matrix[0][3], 
//    (float)m.matrix[1][0], (float)m.matrix[1][1], (float)m.matrix[1][2], (float)m.matrix[1][3], 
//    (float)m.matrix[2][0], (float)m.matrix[2][1], (float)m.matrix[2][2], (float)m.matrix[2][3], 
//    (float)m.matrix[3][0], (float)m.matrix[3][1], (float)m.matrix[3][2], (float)m.matrix[3][3]
//  }
//  );
//  rotMatrix.flip();
  m = m.transpose();
  PMatrix3D rotMatrix = new PMatrix3D(    (float)m.matrix[0][0], (float)m.matrix[0][1], (float)m.matrix[0][2], (float)m.matrix[0][3], 
    (float)m.matrix[1][0], (float)m.matrix[1][1], (float)m.matrix[1][2], (float)m.matrix[1][3], 
    (float)m.matrix[2][0], (float)m.matrix[2][1], (float)m.matrix[2][2], (float)m.matrix[2][3], 
    (float)m.matrix[3][0], (float)m.matrix[3][1], (float)m.matrix[3][2], (float)m.matrix[3][3]);


  pushMatrix();
  
  translate( 250, 250);
  applyMatrix( rotMatrix );
  //translate( -400,-400,0 );
  // draw here.

  rotateX(-PI/2f);
  rotateY(-PI/2f);
  fill(150, 195, 125);
  beginShape(TRIANGLES);
    vertex(-100, -100, -100);
    vertex( 100, -100, -100);
    vertex(   0,    0,  100);
    
    vertex( 100, -100, -100);
    vertex( 100,  100, -100);
    vertex(   0,    0,  100);
    
    vertex( 100, 100, -100);
    vertex(-100, 100, -100);
    vertex(   0,   0,  100);
    
    vertex(-100,  100, -100);
    vertex(-100, -100, -100);
    vertex(   0,    0,  100);
  endShape();

  popMatrix();
}

void receive(byte[] message, String ip, int port)
{
    String mystr = new String(message, 0, message.length);
    String[] strs = mystr.split("\t");
    if (strs.length == 4)
    {
      x = Float.parseFloat(strs[0]);
      y = Float.parseFloat(strs[1]);
      z = Float.parseFloat(strs[2]);
      w = Float.parseFloat(strs[3]);
      println(x + " " + y + " " + z + " " + w);
    }
}


