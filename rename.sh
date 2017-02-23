random_string() { 
  echo "$(date +%s%N)$RANDOM" | md5sum | awk '{print $1}' 
}

find ./imgs3/ -type f | while read FILE; do 
  EXTENSION=${FILE##*.}
  mv "$FILE" "$(dirname "$FILE")/$(random_string).$EXTENSION"
done
