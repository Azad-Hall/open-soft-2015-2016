#!/bin/bash
# usage: ./run.sh <page img>

function graphFn {
  # make the cropped image
  basename=`basename $1 .png`
  # not actually cropped, just everything outside the graph region is white.
  img_cropped="$basename-cropped.png"
  ../graph-box $1 $img_cropped > bb.txt
  echo "yolo"
  # this should also have written the coordinates of the bounding rect to bb.txt (its actually a contour, not rect)
  # ocr / label processing will be done on input image
  ../text_seg "$1" < bb.txt
  # now run scale detection
  ../scale
  # color processing will be done on the cropped image
  # make a new file for input to gen-table
  cp bb.txt gen.txt
  ../color-segmentation $img_cropped "bin" >> gen.txt
  # run granularity detction
  ../xaxis-granularity "$1" < bb.txt >> gen.txt
  # fingers crossed
  tablexml="$basename-table.xml"
  ../gen-table scale.xml bin $tablexml < gen.txt
  exit
}

function pageFn {
  # this function should be thread safe.
  # assuming this is called from the build directory only.
  # make a temp directory for the page
  basename=`basename $1 .png`
  dirname=$basename"-dir"
  rm -rf $dirname
  mkdir $dirname
  cd $dirname
  pwd
  # fix filename
  img="../$1"
  echo "img = $img"

  # make the notext image
  img_notext="notext-$basename.png"
  ../remove-text $img $img_notext > /dev/null

  # run graph-candidates to detect boxes
  graph_basename="$basename-graphs"
  ../graph-candidates $img $img_notext $graph_basename > /dev/null

  for file in $(ls | grep $graph_basename)
  do
    echo "calling graphfn on image $file"
    graphFn $file
  done
}

function pdfFn {
  exit
}


pageFn $1