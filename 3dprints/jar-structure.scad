/*  File:   jar-structure.scad
    Desc:   Frame to contain jar electronic components.
    Auth:   Chris Saiko
    Email:  chris.saiko@gmail.com
    Copyright (C) 2021 Chris Saiko - All Rights Reserved
*/

//resolution settings
$fa = 1;
$fs = 0.4;

//PARAMETERS
radius = 40;
height = 100;
wall_thickness = 2;
cut_thickness = 67;


//MODEL
scale([1, 1, 1]) {
    
    //top and bottom rings
    difference(){
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height, r = radius, center = true);        
        
        //center cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = radius - wall_thickness, center = true);
        
        //wall cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height - 2*wall_thickness, r = radius + 2*wall_thickness, center = true);
    }

    //side support one
    difference(){
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height, r = radius, center = true);        
        
        //center cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = radius - wall_thickness, center = true);
        
        //side cut one
        translate([0, -45, 0])
        rotate([38, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([38, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }

    //side support two
    difference(){
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height, r = radius, center = true);        
        
        //center cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = radius - wall_thickness, center = true);
        
        //side cut one
        translate([0, -45, 0])
        rotate([-38, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([-38, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }

    //side support three
    difference(){
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height, r = radius, center = true);        
        
        //center cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = radius - wall_thickness, center = true);
        
        //side cut one
        translate([-45, 0, 0])
        rotate([38, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([38, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }

    //side support four
    difference(){
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height, r = radius, center = true);        
        
        //center cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = radius - wall_thickness, center = true);
        
        //side cut one
        translate([-45, 0, 0])
        rotate([-38, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([-38, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }
}//end scale