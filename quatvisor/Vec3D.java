import java.lang.Math;

// Vec3D - simple 3D vector class
// processing.unlekker.net



class Vec3D {
  public static final float PI = 3.1415926535f;
  
  float x, y, z;

  Vec3D(float _x, float _y, float _z) {
    x=_x;
    y=_y;
    z=_z;
  }

  Vec3D(Vec3D v) {
    x=v.x;
    y=v.y;
    z=v.z;
  }

  void set(float _x, float _y, float _z) {
    x=_x;
    y=_y;
    z=_z;
  }

  void set(Vec3D v) {
    x=v.x;
    y=v.y;
    z=v.z;
  }

  void add(float _x, float _y, float _z) {
    x+=_x;
    y+=_y;
    z+=_z;
  }

  void add(Vec3D v) {
    x+=v.x;
    y+=v.y;
    z+=v.z;
  }

  void sub(float _x, float _y, float _z) {
    x-=_x;
    y-=_y;
    z-=_z;
  }

  void sub(Vec3D v) {
    x-=v.x;
    y-=v.y;
    z-=v.z;
  }

  void mult(float m) {
    x*=m;
    y*=m;
    z*=m;
  }

  void div(float m) {
    x/=m;
    y/=m;
    z/=m;
  }
  
  Vec3D scale(float m)
  {
     return new Vec3D(x*m, y*m, z*m);
  }

  float length() {
    return ((float)Math.sqrt(x*x+y*y+z*z));
  }

  void normalise() {
    float l=length();
    if (l!=0) {
      x/=l;
      y/=l;
      z/=l;
    }
  }

  void rotateX(float val) {
    // Floats are not precise enough, so doubles are used for the calculations
    double cosval=Math.cos(val);
    double sinval=Math.sin(val);
    double tmp1=y*cosval - z*sinval;
    double tmp2=y*sinval + z*cosval;

    y=(float)tmp1;
    z=(float)tmp2;
  }

  void rotateY(float val) {
    // Floats are not precise enough, so doubles are used for the calculations
    double cosval=Math.cos(val);
    double sinval=Math.sin(val);
    double tmp1=x*cosval - z*sinval;
    double tmp2=x*sinval + z*cosval;

    x=(float)tmp1;
    z=(float)tmp2;
  }

  void rotateZ(float val) {
    // Floats are not precise enough, so doubles are used for the calculations
    double cosval=Math.cos(val);
    double sinval=Math.sin(val);
    double tmp1=x*cosval - y*sinval;
    double tmp2=x*sinval + y*cosval;

    x=(float)tmp1;
    y=(float)tmp2;
  }
}

class Quaternion {
  public  float W, X, Y, Z;      // components of a quaternion
  public static final float PI = 3.1415926535f;

  // default constructor
  public Quaternion() {
    W = 1.0f;
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
  }

  // initialized constructor

  public Quaternion(float w, float x, float y, float z) {
    W = w;
    X = x;
    Y = y;
    Z = z;
  }

  // quaternion multiplication
  public Quaternion mult (Quaternion q) {
    float w = W*q.W - (X*q.X + Y*q.Y + Z*q.Z);

    float x = W*q.X + q.W*X + Y*q.Z - Z*q.Y;
    float y = W*q.Y + q.W*Y + Z*q.X - X*q.Z;
    float z = W*q.Z + q.W*Z + X*q.Y - Y*q.X;

    W = w;
    X = x;
    Y = y;
    Z = z;
    return this;
  }

  // conjugates the quaternion
  public Quaternion conjugate () {
    X = -X;
    Y = -Y;
    Z = -Z;
    return this;
  }

  // inverts the quaternion
  public Quaternion reciprical () {
    float norme = (float)Math.sqrt(W*W + X*X + Y*Y + Z*Z);
    if (norme == 0.0f)
      norme = 1.0f;

    float recip = 1.0f / norme;

    W =  W * recip;
    X = -X * recip;
    Y = -Y * recip;
    Z = -Z * recip;

    return this;
  }

  // sets to unit quaternion
  public Quaternion normalize() {
    float norme = (float) Math.sqrt(W*W + X*X + Y*Y + Z*Z);
    if (norme == 0.0)
    {
      W = 1.0f; 
      X = Y = Z = 0.0f;
    } else
    {
      float recip = 1.0f/norme;

      W *= recip;
      X *= recip;
      Y *= recip;
      Z *= recip;
    }
    return this;
  }

  // Makes quaternion from axis
  public Quaternion fromAxis(float Angle, float x, float y, float z) { 
    float omega, s, c;
    int i;

    s = (float) Math.sqrt(x*x + y*y + z*z);

    if (Math.abs(s) > Float.MIN_VALUE)
    {
      c = 1.0f/s;

      x *= c;
      y *= c;
      z *= c;

      omega = -0.5f * Angle;
      s = (float)Math.sin(omega);

      X = s*x;
      Y = s*y;
      Z = s*z;
      W = (float)Math.cos(omega);
    } else
    {
      X = Y = 0.0f;
      Z = 0.0f;
      W = 1.0f;
    }
    normalize();
    return this;
  }

  // Rotates towards other quaternion
  public void slerp(Quaternion a, Quaternion b, float t)
  {
    float omega, cosom, sinom, sclp, sclq;
    int i;


    cosom = a.X*b.X + a.Y*b.Y + a.Z*b.Z + a.W*b.W;


    if ((1.0f+cosom) > Float.MIN_VALUE)
    {
      if ((1.0f-cosom) > Float.MIN_VALUE)
      {
        omega = (float) Math.acos(cosom);
        sinom = (float) Math.sin(omega);
        sclp = (float) Math.sin((1.0f-t)*omega) / sinom;
        sclq = (float) Math.sin(t*omega) / sinom;
      } else
      {
        sclp = 1.0f - t;
        sclq = t;
      }

      X = sclp*a.X + sclq*b.X;
      Y = sclp*a.Y + sclq*b.Y;
      Z = sclp*a.Z + sclq*b.Z;
      W = sclp*a.W + sclq*b.W;
    } else
    {
      X =-a.Y;
      Y = a.X;
      Z =-a.W;
      W = a.Z;

      sclp = (float) Math.sin((1.0f-t) * PI * 0.5);
      sclq = (float) Math.sin(t * PI * 0.5);

      X = sclp*a.X + sclq*b.X;
      Y = sclp*a.Y + sclq*b.Y;
      Z = sclp*a.Z + sclq*b.Z;
    }
  }

  public Quaternion exp()
  {                               
    float Mul;
    float Length = (float) Math.sqrt(X*X + Y*Y + Z*Z);

    if (Length > 1.0e-4)
      Mul = (float) Math.sin(Length)/Length;
    else
      Mul = 1.0f;

    W = (float) Math.cos(Length);

    X *= Mul;
    Y *= Mul;
    Z *= Mul; 

    return this;
  }

  public Quaternion log()
  {
    float Length;

    Length = (float) Math.sqrt(X*X + Y*Y + Z*Z);
    Length = (float) Math.atan(Length/W);

    W = 0.0f;

    X *= Length;
    Y *= Length;
    Z *= Length;

    return this;
  }
  
  public Matrix4x4 getMatrix()
  {
      Quaternion norm = this.normalize();
      double qx = norm.X;
      double qy = norm.Y;
      double qz = norm.Z;
      double qw = norm.W;
      
      Matrix4x4 transformation = new Matrix4x4(1.0f - 2.0f*qy*qy - 2.0f*qz*qz, 2.0f*qx*qy - 2.0f*qz*qw, 2.0f*qx*qz + 2.0f*qy*qw, 0.0f,
          2.0f*qx*qy + 2.0f*qz*qw, 1.0f - 2.0f*qx*qx - 2.0f*qz*qz, 2.0f*qy*qz - 2.0f*qx*qw, 0.0f,
          2.0f*qx*qz - 2.0f*qy*qw, 2.0f*qy*qz + 2.0f*qx*qw, 1.0f - 2.0f*qx*qx - 2.0f*qy*qy, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f);
      return transformation;
  }
}

