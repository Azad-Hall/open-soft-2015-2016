#!/bin/bash
# usage: ./run.sh <page img>

function graphFn {
  if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  # make the cropped image
  basename=`basename $1 .png`
  # do skew removal
  ../../skew-detection "$1" "$1"
  # not actually cropped, just everything outside the graph region is white.
  img_cropped="$basename-cropped.png"
  ../../graph-box $1 $img_cropped > bb.txt
  # this should also have written the coordinates of the bounding rect to bb.txt (its actually a contour, not rect)
  # ocr / label processing will be done on input image
  ../../text_seg "$1" < bb.txt
  # now run scale detection
  ../../scale
  # color processing will be done on the cropped image
  # make a new file for input to gen-table
  cp bb.txt gen.txt
  ../../color-segmentation $img_cropped "bin" >> gen.txt
  # run granularity detction
  ../../xaxis-granularity "$1" < bb.txt >> gen.txt
  # fingers crossed
  tablexml="$2"
  ../../gen-table scale.xml bin $tablexml < gen.txt
}

function pageFn {
  # this function should be thread safe.
  # assuming this is called from the build directory only.
  # make a temp directory for the page
  if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit
  fi
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
  ../../remove-text $img $img_notext > /dev/null

  # run graph-candidates to detect boxes
  graph_basename="$basename-graphs"
  ../../graph-candidates $img $img_notext $graph_basename > /dev/null

  # xml file name is passed as 2nd parameter
  # all graphs of this page will be stored in 1 table only
  tablexml="../$basename-table.xml"
  touch $tablexml
  for file in $(ls | grep $graph_basename)
  do
    echo "calling graphfn on image $file"
    graphFn $file $tablexml
    # mv "$tablexml" "../$2"
  done
  cd ..
}

function pdfFn {
  if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  basename=`basename $1 .pdf`
  folder="$basename-dir"
  # commenting these out for now
  rm -rf "$folder"
  mkdir "$folder"
  convert -density 300 $1 "$folder/scan.png"
  cd "$folder"
  $cnt=0
  for file in $(ls | grep .png)
  do
    # correct orientation
    ../orientation "$file" "$file"
    echo "calling pageFn on image $file"
    pageFn $file
    ((cnt=cnt+1))
  done
  $pth=`pwd`"$basename"
  $outname="$basename-out"
  echo "$cnt\n" | ../make-pdf $pth $outname
  exit
}


pdfFn $1
# cd scan0004-dir/scan-3-dir
# graphFn $1
