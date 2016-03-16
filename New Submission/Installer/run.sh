#!/bin/bash
# usage: ./run.sh <page img>
# set working dir to where the file is located!
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
  # check if bb.txt is empty. if so, return...
  if [! -s bb.txt ]
    then
    return
  fi
  # this should also have written the coordinates of the bounding rect to bb.txt (its actually a contour, not rect)
  # ocr / label processing will be done on input image
  ../../text_seg "$1" < bb.txt
  # now batman comes to work for x_label detection
  ../../x_label_batman "x_label.jpg"
  # now batman comes to work for y_label detection
  ../../y_label_batman "y_label.jpg"
  # now run scale detection
  ../../scale
  # color processing will be done on the cropped image
  # make a new file for input to gen-table
  cp bb.txt gen.txt
  # run legend removal
  ../../legend-box "$img_cropped" "nolegend-$img_cropped"
  # run uncoloring
  ../../unColoring "nolegend-$img_cropped" "o.png"
  # run legend detection (new one)
  ../../legendDetection2 "o.png" "nolegend-$img_cropped" "p.png"
  # ../../legendDetection "$img_cropped"
  # color segmentation
  ../../color-segmentation "$img_cropped" "bin" "p.png" >> gen.txt
  # run granularity detction. actually fick this
  # ../../xaxis-granularity "$1" < bb.txt >> gen.txt
  # fingers crossed
  tablexml="$2"
  ../../gen-table scale.xml bin $tablexml < gen.txt
}

function incrementDone {
  if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  # hopefully don't need mutex....
  value="`cat \"$doneFileLoc\"`"
  ((value+=$1))
  echo "$value" > "$doneFileLoc"
}
function printDone {
  if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  value="`cat \"$doneFileLoc\"`"
  echo $value $1
}
function pageFn {
  # this function should be thread safe.
  # assuming this is called from the build directory only.
  # make a temp directory for the page
  if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  myshare=$2
  basename=`basename $1 .png`
  dirname=$basename"-dir"
  rm -rf $dirname
  mkdir $dirname
  cd $dirname
  # lots of shit happens in tmp
  mkdir tmp
  # fix filename
  img="../$1"
  # TODO: should write on a new file, sicne original is needed by make-pdf
  ../../orientation "$img" "$basename.png" &> /dev/null
  img="$basename.png"
  # make the notext image
  img_notext="notext-$basename.png"
  printDone "Processing"
  ../../remove-text $img $img_notext   &> /dev/null
  incrementDone $(($myshare/10))
  # run graph-candidates to detect boxes
  graph_basename="$basename-graphs"
  printDone "Processing"
  ../../graph-candidates $img $img_notext $graph_basename  &> /dev/null
  incrementDone $(($myshare/10))
  # xml file name is passed as 2nd parameter
  # all graphs of this page will be stored in 1 table only
  tablexml="../$basename-table.xml"
  touch $tablexml
  numgraphs=$(ls | grep $graph_basename | wc -l)
  for file in $(ls | grep $graph_basename)
  do
    # echo "calling graphfn on image $file"
    printDone "Processing"
    graphFn $file $tablexml  &> /dev/null
    incrementDone $(($myshare*8/$numgraphs/10))
  done
  cd ..
}

function pdfFn {
  # print the output file name first
  doneFileLoc=" "
  if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit
  fi
  # get number of pages in pdf
  # image_tmp= new Imagick();
  # $image_tmp->pingImage("$1");
  # numpages=$image_tmp->getNumberImages();
  numpages=`identify -format %n "$1"`
  basename="`basename $1 .pdf`"
  folder="$basename-dir"
  # print 0 in the begninng because of qt problem. then print output file name
  echo "$numpages "`pwd`"/$folder/out.pdf"
  # commenting these out for now
  rm -rf "$folder"
  mkdir "$folder"
  doneFileLoc=`pwd`"/doneFile.txt"
  echo '0' > "$doneFileLoc"
  printDone "Converting pdf to images"
  convert -density 300 $1 "$folder/scan.png" &> /dev/null
  # if only 1 page, then rename the file to scan-0.png
  incrementDone "10"
  cd "$folder"
  if [ "$numpages" == "1" ]
    then
    mv "scan.png" "scan-0.png"
  fi
  cnt=$(ls | grep .png | wc -l)
  for file in $(ls | grep .png)
  do
    # correct orientation
    # echo "calling pageFn on image $file"
    pageFn "$file" $((90/cnt)) &
  done
  wait
  printDone "Rendering PDF"
  pth=`pwd`"/scan"
  outname="out"
  echo "$cnt\n" | ../make-pdf "$pth" "$outname" &> /dev/null
  echo "100 Done."
  exit
}

pdfFn "$1"
# cd scan0004-dir/scan-3-dir
# graphFn $1 "temp_nis.xml"