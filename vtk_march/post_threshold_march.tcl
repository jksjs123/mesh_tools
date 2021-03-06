
set NAME post
set STUDY ./pgm/$NAME.pgm
set COLUMNS 640
set ROWS 640
set TISSUE 255
set START_SLICE 0
set END_SLICE 689
# set VOI "0 639 0 639 $START_SLICE $END_SLICE"
set VOI "20 619 20 619 1 688"

set SLICE_ORDER rotzrotyis
set PIXEL_SIZE 0.01
set SPACING 0.01
set WORLD_ORIGIN_X 0
set WORLD_ORIGIN_Y 0
set IMAGE_ORIGIN_X 0
set IMAGE_ORIGIN_Y 0
set VALUE 254.9
set SAMPLE_RATE "1 1 1"
set GAUSSIAN_2D_STANDARD_DEVIATION "1 1"
set BLUR_THRESHOLD 255
set GAUSSIAN_STANDARD_DEVIATION "1 1 1"
set DECIMATE_REDUCTION .6
set DECIMATE_ITERATIONS 5
set DECIMATE_ASPECT_RATIO 6
set DECIMATE_ERROR .0002
set DECIMATE_ERROR_INCREMENT .0002
set SMOOTH_ITERATIONS 100
set SMOOTH_FACTOR .05
set FEATURE_ANGLE 60

source threshold_marching_cubes.tcl
