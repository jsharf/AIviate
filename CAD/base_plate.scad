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

difference()
{
	cube([55, 90, 2], center=true);
	translate([0, -12.5, 0])
	rotate([90, 0, 0])
	mount(3.5);
	translate([0, 34.0, 0])
	rotate([90, 0, 0])
	mount(3.5);
	translate([0, 38, 0])
	cube([30, 16, 3], center=true);
}