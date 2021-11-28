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
jar_mouth = 58.6;
led_depth = 2.2;
radius = (jar_mouth + led_depth - 2)/2;
height = 158;
wall_thickness = 1.6;
cut_thickness = 81.5;
support_angle = 20.0;
ring_support_height = height/7;
ring_support_translate = sqrt(0.5*(radius-1)^2) + 0.10;


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
        rotate([support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([support_angle, 0, 0])
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
        rotate([-support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([-support_angle, 0, 0])
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
        rotate([support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([support_angle, 0, 90])
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
        rotate([-support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([-support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }
    
    //top ring supports
    translate([ring_support_translate, ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, -45])
    cube(size = [wall_thickness*2, wall_thickness, ring_support_height], center = true);
    
    translate([-ring_support_translate, -ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, -45])
    cube(size = [wall_thickness*2, wall_thickness, ring_support_height], center = true);
    
    translate([ring_support_translate, -ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, 45])
    cube(size = [wall_thickness*2, wall_thickness, ring_support_height], center = true);
    
    translate([-ring_support_translate, ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, 45])
    cube(size = [wall_thickness*2, wall_thickness, ring_support_height], center = true);  
    
}//end scale