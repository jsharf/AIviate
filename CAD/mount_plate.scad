 module halfcircle(h)
{
	difference()
	{
		cylinder(h, r=35, center=true);
		translate([0, -35, 0])
			cube([70, 35, h+1], center=true);
	}
}

module horseshoe(height)
{
	difference()
	{
		halfcircle(height);
		union()
		{
		   translate([0, -20, 0])
			cube([50, 60, height+1], center=true);
		   translate([0, 5, 0])
			cylinder(height+10, r=25, center=true);
		}
	}
}

module mount(height)
{
	difference()
	{
		scale([0.92, 0.92, 1])
		horseshoe(height);
		translate([0, -10, 0])
		cube([56.5, 1.8, 100], center=true);		
	}
}


mount(3);



