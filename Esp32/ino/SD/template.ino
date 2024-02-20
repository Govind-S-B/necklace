//main logic for writing wav chunks
int counter = 0;  **
while (1) {       **
  read mic into buffer **
  if (counter == 10000) { **
    file close;
    open new file
    write header
    counter = 0; **
  }
  write buffer  //printbuffer
  counter++;
}