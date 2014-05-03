module switch()
{
	union()
	{
	translate([0, 0, 19.2/2])
	difference()
	{
		cube([20.8,33.9, 1],true);
		cube([18, 28.5, 1.5], true);
	}
	difference()
	{
		cube([20.8, 33.9, 19.2], true);
		cube([18.8, 31.9, 21], true);
	}
	}
}

switch();