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
middle_radius = (jar_mouth + led_depth - 2)/2 - 3;
height = 158;
wall_thickness = 2.4;
cut_thickness = 81.5;
support_angle = 20.0;
ring_support_height = height/7;
ring_support_translate = sqrt(0.5*(radius-1)^2) - 0.18;
board_guide_translate = ring_support_translate - 3.1;
pcb_board_thickness = 1.8;


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

    //MAIN BOARD GUIDES
    difference(){
        union(){
            // main board guide one
            translate([board_guide_translate, board_guide_translate, height/2 - ring_support_height*1.25])
            rotate([75, 0, -45])
            cube(size = [wall_thickness*2.6, wall_thickness*3.0, ring_support_height/3], center = true);

            // main board guide two
            translate([-board_guide_translate, -board_guide_translate, height/2 - ring_support_height*1.25])
            rotate([-75, 0, -45])
            cube(size = [wall_thickness*2.6, wall_thickness*3.0, ring_support_height/3], center = true);
        }
        //guide center cut
        translate([0, 0, 0])
        rotate([0, 0, -45])
        cube(size = [pcb_board_thickness, radius*2.1, height], center = true);
        
        //guide top cut
        translate([0, 0, height/2 - ring_support_height*1.08])
        rotate([0, 0, -45])
        cube(size = [wall_thickness*6.0, radius*2.1, wall_thickness*1.2], center = true);
        
        //guide interior cuts
        translate([board_guide_translate - 2.0, board_guide_translate - 2.0, height/2 - ring_support_height*1.35])
        rotate([-50, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*4.0, wall_thickness*2.0], center = true);

        translate([-board_guide_translate + 2.0, -board_guide_translate + 2.0, height/2 - ring_support_height*1.35])
        rotate([50, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*4.0, wall_thickness*2.0], center = true);        
       
    }
  

    
    //MAIN BOARD STOPS
    translate([0, 0, -height/1.7])
    difference(){
        union(){
            // main board guide one
            translate([board_guide_translate, board_guide_translate, height/2 - ring_support_height*1.25])
            rotate([75, 0, -45])
            cube(size = [wall_thickness*2.6, wall_thickness*3.0, ring_support_height/3], center = true);

            // main board guide two
            translate([-board_guide_translate, -board_guide_translate, height/2 - ring_support_height*1.25])
            rotate([-75, 0, -45])
            cube(size = [wall_thickness*2.6, wall_thickness*3.0, ring_support_height/3], center = true);
        }
        //guide center cut
        translate([0, 0, height/1.205])
        rotate([0, 0, -45])
        cube(size = [pcb_board_thickness, radius*2.1, height], center = true);
        
        //guide top cut
        translate([0, 0, height/2 - ring_support_height*1.08])
        rotate([0, 0, -45])
        cube(size = [wall_thickness*6.0, radius*2.1, wall_thickness*1.2], center = true);
        
        //guide interior cuts
        translate([board_guide_translate - 2.0, board_guide_translate - 2.0, height/2 - ring_support_height*1.35])
        rotate([-50, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*4.0, wall_thickness*2.0], center = true);

        translate([-board_guide_translate + 2.0, -board_guide_translate + 2.0, height/2 - ring_support_height*1.35])
        rotate([50, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*4.0, wall_thickness*2.0], center = true);
        
        //guide exterior cuts
        translate([board_guide_translate + 2.6, board_guide_translate + 2.6, height/2 - ring_support_height*1.3])
        rotate([0, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*2.0, wall_thickness*4.0], center = true);
        
        translate([-board_guide_translate - 2.6, -board_guide_translate - 2.6, height/2 - ring_support_height*1.3])
        rotate([0, 0, -45])
        cube(size = [wall_thickness*4.0, wall_thickness*2.0, wall_thickness*4.0], center = true); 
       
    }
    

    
    //SIDE SUPPORT ONE
    //union cylinder code
    difference(){
        union(){
            //top cylinder
            translate([0, 0, height/2 - ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r = radius, center = true);
            
            //top slope
            translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height/2, r1 = middle_radius, r2 = radius, center = true);
            
            //middle cylinder
            translate([0, 0, -ring_support_height/4])
            rotate([0, 0, 0])
            cylinder(h = height - ring_support_height - ring_support_height/2, r = middle_radius, center = true);
                  
            //bottom slope, (no bottom cylinder)
            translate([0, 0, -height/2 + ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r1 = radius, r2 = middle_radius, center = true);
            
        }
        
        //center interior cylinder cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = middle_radius - wall_thickness, center = true);
        
        //top interior cylinder cut
        translate([0, 0, height/2 - ring_support_height/2])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height + 0.01, r = radius - wall_thickness, center = true);
        
        //top interior slope cut
        translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = middle_radius - wall_thickness, r2 = radius - wall_thickness, center = true);
        
        //bottom interior slope cut
        translate([0, 0, -height/2 + ring_support_height/4])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = radius - wall_thickness, r2 = middle_radius - wall_thickness, center = true);

        //side support one cuts
        //side cut one
        translate([0, -45, 0])
        rotate([support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
    }//end side support one



    //SIDE SUPPORT TWO
    //union cylinder code
    difference(){
        union(){
            //top cylinder
            translate([0, 0, height/2 - ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r = radius, center = true);
            
            //top slope
            translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height/2, r1 = middle_radius, r2 = radius, center = true);
            
            //middle cylinder
            translate([0, 0, -ring_support_height/4])
            rotate([0, 0, 0])
            cylinder(h = height - ring_support_height - ring_support_height/2, r = middle_radius, center = true);
                  
            //bottom slope, (no bottom cylinder)
            translate([0, 0, -height/2 + ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r1 = radius, r2 = middle_radius, center = true);
            
        }
        
        //center interior cylinder cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = middle_radius - wall_thickness, center = true);
        
        //top interior cylinder cut
        translate([0, 0, height/2 - ring_support_height/2])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height + 0.01, r = radius - wall_thickness, center = true);
        
        //top interior slope cut
        translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = middle_radius - wall_thickness, r2 = radius - wall_thickness, center = true);
        
        //bottom interior slope cut
        translate([0, 0, -height/2 + ring_support_height/4])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = radius - wall_thickness, r2 = middle_radius - wall_thickness, center = true);

        //side support two cuts
        //side cut one
        translate([0, -45, 0])
        rotate([-support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([0, 45, 0])
        rotate([-support_angle, 0, 0])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);

    }//end side support two    
    

    //SIDE SUPPORT THREE
    //union cylinder code
    difference(){
        union(){
            //top cylinder
            translate([0, 0, height/2 - ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r = radius, center = true);
            
            //top slope
            translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height/2, r1 = middle_radius, r2 = radius, center = true);
            
            //middle cylinder
            translate([0, 0, -ring_support_height/4])
            rotate([0, 0, 0])
            cylinder(h = height - ring_support_height - ring_support_height/2, r = middle_radius, center = true);
                  
            //bottom slope, (no bottom cylinder)
            translate([0, 0, -height/2 + ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r1 = radius, r2 = middle_radius, center = true);
            
        }
        
        //center interior cylinder cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = middle_radius - wall_thickness, center = true);
        
        //top interior cylinder cut
        translate([0, 0, height/2 - ring_support_height/2])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height + 0.01, r = radius - wall_thickness, center = true);
        
        //top interior slope cut
        translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = middle_radius - wall_thickness, r2 = radius - wall_thickness, center = true);
        
        //bottom interior slope cut
        translate([0, 0, -height/2 + ring_support_height/4])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = radius - wall_thickness, r2 = middle_radius - wall_thickness, center = true);

        //side support three cuts
        //side cut one
        translate([-45, 0, 0])
        rotate([support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);

    }//end side support three



    //SIDE SUPPORT FOUR
    //union cylinder code
    difference(){
        union(){
            //top cylinder
            translate([0, 0, height/2 - ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r = radius, center = true);
            
            //top slope
            translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height/2, r1 = middle_radius, r2 = radius, center = true);
            
            //middle cylinder
            translate([0, 0, -ring_support_height/4])
            rotate([0, 0, 0])
            cylinder(h = height - ring_support_height - ring_support_height/2, r = middle_radius, center = true);
                  
            //bottom slope, (no bottom cylinder)
            translate([0, 0, -height/2 + ring_support_height/2])
            rotate([0, 0, 0])
            cylinder(h = ring_support_height, r1 = radius, r2 = middle_radius, center = true);
            
        }
        
        //center interior cylinder cut
        translate([0, 0, 0])
        rotate([0, 0, 0])
        cylinder(h = height + 0.01, r = middle_radius - wall_thickness, center = true);
        
        //top interior cylinder cut
        translate([0, 0, height/2 - ring_support_height/2])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height + 0.01, r = radius - wall_thickness, center = true);
        
        //top interior slope cut
        translate([0, 0, height/2 - ring_support_height/4 - ring_support_height + 0.05])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = middle_radius - wall_thickness, r2 = radius - wall_thickness, center = true);
        
        //bottom interior slope cut
        translate([0, 0, -height/2 + ring_support_height/4])
        rotate([0, 0, 0])
        cylinder(h = ring_support_height/2 + 0.01, r1 = radius - wall_thickness, r2 = middle_radius - wall_thickness, center = true);

        //side support four cuts
        //side cut one
        translate([-45, 0, 0])
        rotate([-support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);
        
        //side cut two
        translate([45, 0, 0])
        rotate([-support_angle, 0, 90])
        cube(size = [2.05*(radius + wall_thickness), cut_thickness, 2*height], center = true);

    }//end side support four
    

    
    //top ring supports
    translate([ring_support_translate, ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, -45])
    cube(size = [wall_thickness*1.5, wall_thickness, ring_support_height], center = true);
    
    translate([-ring_support_translate, -ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, -45])
    cube(size = [wall_thickness*1.5, wall_thickness, ring_support_height], center = true);
    
    translate([ring_support_translate, -ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, 45])
    cube(size = [wall_thickness*1.5, wall_thickness, ring_support_height], center = true);
    
    translate([-ring_support_translate, ring_support_translate, height/2 - ring_support_height/2])
    rotate([0, 0, 45])
    cube(size = [wall_thickness*1.5, wall_thickness, ring_support_height], center = true);  
    
}//end scale