# Generate triangle files for medical images

source SliceOrder.tcl

# reader reads slices

set OK 1
# Set defaults for any parameters not defined
if {[info exists NAME] == 0} {
    puts "segmented8.tcl: NAME is not defined"
    set OK 0
}
if {[info exists STUDY] == 0} {
    puts "segmented8.tcl: STUDY is not defined"
    set OK 0
}
if {[info exists END_SLICE] == 0} {
    puts "segmented8.tcl: END_SLICE is not defined"
    set OK 0
}
if {[info exists VALUE] == 0} {
    puts "segmented8.tcl: VALUE is not defined"
    set OK 0
}
if {[info exists TISSUE] == 0} {
    puts "segmented16.tcl: TISSUE is not defined"
    set OK 0
}

if { $OK == 0 } {
    puts "segmented8.tcl: One or more required parameters are missing!"
    exit
}

if {[info exists SLICE_ORDER] == 0} {set SLICE_ORDER si}
if {[info exists HEADER_SIZE] == 0} {set HEADER_SIZE 0}
if {[info exists PIXEL_SIZE] == 0} {set PIXEL_SIZE 1}
if {[info exists START_SLICE] == 0} {set START_SLICE 1}
if {[info exists REDUCTION] == 0} {set REDUCTION 1}
if {[info exists FEATURE_ANGLE] == 0} {set FEATURE_ANGLE 60}
if {[info exists DECIMATE_ANGLE] == 0} {set DECIMATE_ANGLE $FEATURE_ANGLE}
if {[info exists SMOOTH_ANGLE] == 0} {set SMOOTH_ANGLE $DECIMATE_ANGLE}
if {[info exists DECIMATE_ITERATIONS] == 0} {set DECIMATE_ITERATIONS 1}
if {[info exists SMOOTH_ITERATIONS] == 0} {set SMOOTH_ITERATIONS 10}
if {[info exists DECIMATE_REDUCTION] == 0} {set DECIMATE_REDUCTION 1}
if {[info exists DECIMATE_ERROR] == 0} {set DECIMATE_ERROR .0002}
if {[info exists DECIMATE_ERROR_INCREMENT] == 0} {set DECIMATE_ERROR_INCREMENT .0002}
if {[info exists ISLAND_AREA] == 0} {set ISLAND_AREA 4}
if {[info exists ISLAND_REPLACE] == 0} {set ISLAND_REPLACE -1}
if {[info exists SMOOTH_FACTOR] == 0} {set SMOOTH_FACTOR .01}
if {[info exists GAUSSIAN_STANDARD_DEVIATION] == 0} {set GAUSSIAN_STANDARD_DEVIATION "2 2 2"}
if {[info exists GAUSSIAN_RADIUS_FACTORS] == 0} {set GAUSSIAN_RADIUS_FACTORS "1 1 1"}
if {[info exists VOI] == 0} {
    set VOI "0 [expr $COLUMNS - 1] 0 [expr $ROWS - 1] 0 $ZMAX]"
}
if {[info exists SAMPLE_RATE] == 0} {set SAMPLE_RATE "1 1 1"}


#
# 
if {[info exists IMAGE_ORIGIN_X] == 0} {
  set image_originx [expr ( $COLUMNS / 2.0 ) * $PIXEL_SIZE * -1.0]
} else {
  set image_originx [expr $IMAGE_ORIGIN_X]
}

if {[info exists IMAGE_ORIGIN_Y] == 0} {
  set image_originy [expr ( $ROWS / 2.0 ) * $PIXEL_SIZE * -1.0]
} else {
  set image_originy [expr $IMAGE_ORIGIN_Y]
}

if {[info exists WORLD_ORIGIN_X] == 0} {
  set originx [expr $image_originx]
} else {
  set originx [expr $image_originx - $WORLD_ORIGIN_X]
}

if {[info exists WORLD_ORIGIN_Y] == 0} {
  set originy [expr $image_originy]
} else {
  set originy [expr $image_originy - $WORLD_ORIGIN_Y]
}

set minx [lindex $VOI 0]
set maxx [lindex $VOI 1]
set miny [lindex $VOI 2]
set maxy [lindex $VOI 3]
set minz [lindex $VOI 4]
set maxz [lindex $VOI 5]
#
# adjust y bounds for PNM coordinate system
#
set tmp $miny
set miny [expr $ROWS - $maxy -1]
set maxy [expr $ROWS - $tmp -1]

vtkPNMReader reader;
    reader SetFilePrefix $STUDY
    reader SetDataSpacing $PIXEL_SIZE $PIXEL_SIZE $SPACING
    reader SetDataOrigin $originx $originy [expr $START_SLICE * $SPACING]
    eval reader SetDataVOI $minx $maxx $miny $maxy $minz $maxz
    reader SetTransform $SLICE_ORDER
    [reader GetOutput] ReleaseDataFlagOn

set lastConnection reader
if {$ISLAND_REPLACE >= 0} {
  vtkImageIslandRemoval2D islandRemover
      islandRemover SetAreaThreshold $ISLAND_AREA
      islandRemover SetIslandValue $ISLAND_REPLACE
      islandRemover SetReplaceValue $TISSUE
      islandRemover SetInput [$lastConnection GetOutput]
  set lastConnection islandRemover
}

vtkImageThreshold selectTissue
    selectTissue ThresholdBetween $TISSUE $TISSUE
#    selectTissue ThresholdBetween 1 255
    selectTissue SetInValue 255
    selectTissue SetOutValue 0
    selectTissue SetInput [$lastConnection GetOutput]
    set lastConnection selectTissue

vtkImageShrink3D shrinker
     shrinker SetInput [$lastConnection GetOutput]
eval shrinker SetShrinkFactors $SAMPLE_RATE
     shrinker AveragingOn
     set lastConnection shrinker

if {$GAUSSIAN_STANDARD_DEVIATION != "0 0 0"} {
  vtkImageGaussianSmooth gaussian
      eval gaussian SetStandardDeviations $GAUSSIAN_STANDARD_DEVIATION
      eval gaussian SetRadiusFactors $GAUSSIAN_RADIUS_FACTORS
      gaussian SetInput [$lastConnection GetOutput]
      set lastConnection gaussian
}

vtkImageToStructuredPoints toStructuredPoints
    toStructuredPoints SetInput [$lastConnection GetOutput]
    [toStructuredPoints GetOutput] ReleaseDataFlagOn

vtkMarchingCubes mcubes;
    mcubes SetInput [toStructuredPoints GetOutput]
    mcubes ComputeScalarsOff
    mcubes ComputeGradientsOff
    mcubes ComputeNormalsOff
    eval mcubes SetValue 0 $VALUE
    [mcubes GetOutput] ReleaseDataFlagOn

vtkDecimate decimator
    decimator SetInput [mcubes GetOutput]
    eval decimator SetInitialFeatureAngle $DECIMATE_ANGLE
    eval decimator SetMaximumIterations $DECIMATE_ITERATIONS
    decimator SetMaximumSubIterations 0
    decimator PreserveEdgesOn
    decimator SetMaximumError 1
    decimator SetAspectRatio $DECIMATE_ASPECT_RATIO
    decimator SetTargetReduction $DECIMATE_REDUCTION
    eval decimator SetInitialError $DECIMATE_ERROR
    eval decimator SetErrorIncrement $DECIMATE_ERROR_INCREMENT
    [decimator GetOutput] ReleaseDataFlagOn
	
vtkSmoothPolyDataFilter smoother
    smoother SetInput [decimator GetOutput]
    eval smoother SetNumberOfIterations $SMOOTH_ITERATIONS
    eval smoother SetRelaxationFactor $SMOOTH_FACTOR
    eval smoother SetFeatureAngle $SMOOTH_ANGLE
    smoother FeatureEdgeSmoothingOff
    smoother BoundarySmoothingOff;
    smoother SetConvergence 0
    [smoother GetOutput] ReleaseDataFlagOn

vtkPolyDataNormals normals
    normals SetInput [smoother GetOutput]
    eval normals SetFeatureAngle $FEATURE_ANGLE
    [normals GetOutput] ReleaseDataFlagOn

vtkStripper stripper
    stripper SetInput [normals GetOutput]
    [stripper GetOutput] ReleaseDataFlagOn

vtkPolyDataWriter writer
#    writer SetInput [normals GetOutput]
    writer SetInput [smoother GetOutput]
#    writer SetInput [stripper GetOutput]
    eval writer SetFileName $NAME.vtk
    writer SetFileType 1
#    writer SetFileType 2

proc readerStart {} {global NAME; puts -nonewline "$NAME read took:\t"; flush stdout};
reader SetStartMethod readerStart
proc toStructuredPointsStart {} {global NAME; puts -nonewline "$NAME toStructuredPoints took\t"; flush stdout};
toStructuredPoints SetStartMethod toStructuredPointsStart
proc mcubesStart {} {global NAME; puts -nonewline "$NAME mcubes generated\t"; flush stdout};
proc mcubesEnd {} {
    global NAME
    puts -nonewline "[[mcubes GetOutput] GetNumberOfPolys]"
    puts -nonewline " polygons in "
    flush stdout
};
mcubes SetStartMethod mcubesStart
mcubes SetEndMethod mcubesEnd
proc decimatorStart {} {global NAME; puts -nonewline "$NAME decimator generated\t"; flush stdout};
proc decimatorEnd {} {
    global NAME
    puts -nonewline "[[decimator GetOutput] GetNumberOfPolys]"
    puts -nonewline " polygons in "
    flush stdout
};
decimator SetStartMethod decimatorStart
decimator SetEndMethod decimatorEnd
proc smootherStart {} {global NAME; puts -nonewline "$NAME smoother took:\t"; flush stdout};
smoother SetStartMethod smootherStart
proc normalsStart {} {global NAME; puts -nonewline "$NAME normals took:\t"; flush stdout};
normals SetStartMethod normalsStart
proc writerStart {} {global NAME; puts -nonewline "$NAME writer took:\t"; flush stdout};
stripper SetStartMethod stripperStart
proc stripperStart {} {global NAME; puts -nonewline "$NAME stripper took:\t"; flush stdout};

puts "[expr [lindex [time {reader Update;} 1] 0] / 1000000.0] seconds"
puts "[expr [lindex [time {toStructuredPoints Update;} 1] 0] / 1000000.0] seconds"
puts "[expr [lindex [time {mcubes Update;} 1] 0] / 1000000.0] seconds"
#puts "[expr [lindex [time {cleaner Update;} 1] 0] / 1000000.0] seconds"
puts "[expr [lindex [time {decimator Update;} 1] 0] / 1000000.0] seconds"
puts "[expr [lindex [time {smoother Update;} 1] 0] / 1000000.0] seconds"
#puts "[expr [lindex [time {normals Update;} 1] 0] / 1000000.0] seconds"
#puts "[expr [lindex [time {stripper Update;} 1] 0] / 1000000.0] seconds"
writerStart
puts "[expr [lindex [time {writer Update;} 1] 0] / 1000000.0] seconds"

exit
