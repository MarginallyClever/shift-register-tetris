//------------------------------------------------------------------------
// ArduinoTetris - 
// arduino + four shift registers + joystick + two 8x8 LED panels
// dan@marginallyclever.com 2016-12-15
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------

// pins on the shift - to - pins on arduino
#define DATA                4
#define LATCH               3
#define CLK                 2

// built in light on the uno
#define LED                 13

// dimensions of screen
#define WIDTH               8
#define HEIGHT              16

// total pins in the shift sequence
#define MAX_PINS            32
#define MAX_ANODES          (MAX_PINS/2)
#define MAX_CATHODES        (MAX_PINS/2)

// ms to wait when needed to make animations visible
#define WAIT                (2000/MAX_PINS)
#define WAIT2               (100)
#define WAIT3               (5)

#define MOVE_DELAY          (100)  // 100ms = 5 times a second
#define DROP_DELAY          (500)  // 500ms = 2 times a second

// serial speed
#define BAUD                57600

// scrolling message (scrollingText)
#define LETTER_W            (6)
#define MESSAGE_LEN         (4)
#define MESSAGE_W           (MESSAGE_LEN * LETTER_W)
#define LETTER_H            (8)
#define SCROLL_DELAY        (60)

// max size of each tetris piece
#define PIECE_W             (4)
#define PIECE_H             (4)

// how many kinds of pieces
#define NUM_PIECE_TYPES     (7)

#define JOYSTICK_DEAD_ZONE  (45)
#define JOYSTICK_BUTTON     7
#define JOYSTICK_X          A0
#define JOYSTICK_Y          A1
#define X_AXIS_POSITIVE     1  // 1 or -1 to flip control
#define Y_AXIS_POSITIVE     -1  // 1 or -1 to flip control


const char   anodesUnsorted[MAX_ANODES  ] = {        2,    4,       7,      10,   12,      15,16,17,      20,   22,23,24,25,      28,   30,31, };
const char cathodesUnsorted[MAX_CATHODES] = {  0, 1,    3,    5, 6,    8, 9,   11,   13,14,         18,19,   21,            26,27,   29,       };
// fast lookup based on anodesUnsorted
const char   anodesFlag    [MAX_PINS    ] = {  0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, };
// sorted along x
const char   anodes        [MAX_ANODES  ] = {  7, 2,31, 4,24,30,25,28, 15,10,23,12,16,22,17,20, };
// sorted along y
const char cathodes        [MAX_CATHODES] = {  0, 1, 5,29, 6,27,26, 3,  8, 9,13,21,14,19,18,11, };



// 1 color drawings of each piece in each rotation.
// Each piece is max 4 wide, 4 tall, and 4 rotations.
const char piece_I[] = {
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,

  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,

  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
};

const char piece_L1[] = {
  0,1,0,0,
  0,1,0,0,
  0,1,1,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  1,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,1,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
};

const char piece_L2[] = {
  0,1,0,0,
  0,1,0,0,
  1,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,0,0,0,
  1,1,1,0,
  0,0,0,0,
  
  0,1,1,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  0,0,1,0,
  0,0,0,0,
};

const char piece_T[] = {
  0,0,0,0,
  1,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,1,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  0,1,0,0,
  1,1,1,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
};

const char piece_S1[] = {
  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,

  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
};

const char piece_S2[] = {
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
};

const char piece_O[] = {
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
};


// An array of pointers!  
const char *pieces[NUM_PIECE_TYPES] = {
  piece_S1,
  piece_S2,
  piece_L1,
  piece_L2,
  piece_O,
  piece_T,
  piece_I,
};



// scrolling message (scrollingText)
// 1 means on
// 0 means off
// message says "MCR"
const int message[] = {
  1,0,0,0,1,0, 1,1,1,1,1,0, 1,1,1,1,0,0, 0,0,0,0,0,0,
  1,1,0,1,1,0, 1,0,0,0,0,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
  1,0,1,0,1,0, 1,0,0,0,0,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
  1,0,0,0,1,0, 1,0,0,0,0,0, 1,1,1,1,0,0, 0,0,0,0,0,0,
  1,0,0,0,1,0, 1,0,0,0,0,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
  1,0,0,0,1,0, 1,0,0,0,0,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
  1,0,0,0,1,0, 1,0,0,0,0,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
  1,0,0,0,1,0, 1,1,1,1,1,0, 1,0,0,0,1,0, 0,0,0,0,0,0,
};


//------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------


// this is how arduino remembers what the button was doing in the past,
// so arduino can tell when it changes.
int oldButton=0;
// so arduino can tell when user moves sideways
int oldPx = 0;
// so arduino can tell when user tries to turn
int old_iWantToTurn=0;

// this is how arduino remembers the falling piece.
int pieceID;  // which shape
int pieceRotation;  // which way it is turned
int pieceX;  // where it is left-right (big=right)
int pieceY;  // where it is up-down (big=down)

// this controls the order that pieces appear on screen.  
// guarantees all pieces appear once before the order is changed.
char pieceSequence[NUM_PIECE_TYPES];
char pieceSequenceIndex=NUM_PIECE_TYPES;

// this controls how fast the player can move.
long lastMove;
long moveDelay=MOVE_DELAY;

// this controls when the piece automatically falls.
long lastDrop;
long dropDelay=DROP_DELAY;

// this is how arduino remembers where pieces are on the grid.
int grid[WIDTH*HEIGHT];


//------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------


void setup() {
  Serial.begin(BAUD);
  pinMode(DATA,OUTPUT);
  pinMode(LATCH,OUTPUT);
  pinMode(CLK,OUTPUT);
  pinMode(LED,OUTPUT);
  setupTetris();
}


void loop() {
  //loop1();
  //loop2();
  //findAnodes();
  //lightAnodesSorted();
  //lightEachColumn();
  //lightEverything();
  //lightEachRowUnsorted();
  //lightEachRowSorted();
  //lightOnePixelAtATime();
  //scrollingText();
  //testJoystick();
  //drawEveryPieceAndRotation();
  playTetris();
}


void drawEveryPieceAndRotation() {
  chooseNewPiece();
  pieceY=2;

  int i;
  for(i=0;i<4;++i) {
    addPieceToGrid();
    long t = millis();
    while(millis()-t<250) {
      drawGrid();
    }
    erasePieceFromGrid();
    pieceRotation = ( pieceRotation + 1 ) % 4;
  }
}


// turn on the grid lights in row y
void drawGridRow(int y,int test) {
  // figure out which anode/cathode pairings gives us light(x,y)
  int cathodeY = cathodes[y];
  int b = (y>=MAX_ANODES/2) ? MAX_ANODES/2:0;

  // build a list of the anodes and cathodes that should be high
  int pinStates[MAX_PINS];
  int i;
  
  for(i=0;i<MAX_PINS;++i) {
    // low by default
    pinStates[i]=LOW;
    // high if not a cathode for the column we want
    if(isCathode(i) && i!=cathodeY) {
      pinStates[i]=HIGH;
    }
  }
  for(i=0;i<WIDTH;++i) {
    // high if an anode that we want
    if( grid[y*WIDTH+i] != test ) {
      pinStates[anodes[i+b]] = HIGH;
    }
  }

  // now shift in all the data in a single pass.
  for(i=0;i<MAX_PINS;++i) {
    digitalWrite(DATA,pinStates[i]);
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
  }
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
}


// Grid contains the arduino's memory of the game board, including the piece that is falling.
void drawGrid() {
  /* this is slow and flickers:
  int x, y;
  for(y=0;y<HEIGHT;++y) {
    for(x=0;x<WIDTH;++x) {
      if( grid[y*WIDTH+x] != 0 ) {
        lightXY(x,y);
        digitalWrite(LATCH,HIGH);
        digitalWrite(LATCH,LOW);
      }
    }
  }*/
  
  // this is ~8x faster and doesn't flicker:
  int y;
  for(y=0;y<HEIGHT;++y) {
    drawGridRow(y,0);
  }
}


void drawGridInverted() {
  int y;
  for(y=0;y<HEIGHT;++y) {
    drawGridRow(y,1);
  }
}

// choose a new piece from the sequence.
// the sequence is a random list that contains one of each piece.
// that way you're guaranteed an even number of pieces over time,
// tho the order is random.  it's like picking pieces from a bag 
// without looking.  When the bag is empty, put the pieces back 
// in and pick again.
void chooseNewPiece() {
  // bag empty?
  if( pieceSequenceIndex >= NUM_PIECE_TYPES ) {
    int i,j, k;
    for(i=0;i<NUM_PIECE_TYPES;++i) {
      do {
        // pick a random piece
        j = rand() % NUM_PIECE_TYPES;
        // make sure it isn't already in the bag.
        for(k=0;k<i;++k) {
          if(pieceSequence[k]==j) break;  // already in the bag
        }
      } while(k<i);
      // not in bag.  Add it.
      pieceSequence[i] = j;
    }
    // rewind sequence counter
    pieceSequenceIndex=0;
  }
  
  // get the next piece in the sequence.
  pieceID = pieceSequence[pieceSequenceIndex++];
  // always start the piece top center.
  pieceY=-4;  // -4 squares off the top of the screen.
  pieceX=3;
  // always start in the same orientation.
  pieceRotation=0;
}


void erasePieceFromGrid() {
  int x, y;
  
  const char *piece = pieces[pieceID] + (pieceRotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=pieceX+x;
      int ny=pieceY+y;
      if(ny<0 || ny>HEIGHT) continue;
      if(nx<0 || nx>WIDTH) continue;
      if(piece[y*PIECE_W+x]==1) {
        grid[ny*WIDTH+nx]=0;  // zero erases the grid location.
      }
    }
  }
}


void addPieceToGrid() {
  int x, y;
  
  const char *piece = pieces[pieceID] + (pieceRotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=pieceX+x;
      int ny=pieceY+y;
      if(ny<0 || ny>HEIGHT) continue;
      if(nx<0 || nx>WIDTH) continue;
      if(piece[y*PIECE_W+x]==1) {
        grid[ny*WIDTH+nx]=1;  // zero erases the grid location.
      }
    }
  }
}


// Move everything down 1 space, destroying the old row number y in the process.
void deleteRow(int y) {
  int x;
  for(;y>0;--y) {
    for(x=0;x<WIDTH;++x) {
      grid[y*WIDTH+x] = grid[(y-1)*WIDTH+x];
    }
  }
  // everything moved down 1, so the top row must be empty or the game would be over.
  for(x=0;x<WIDTH;++x) {
    grid[x]=0;
  }
}


void removeFullRows() {
  int x, y, c;
  for(y=0;y<HEIGHT;++y) {
    // count the full spaces in this row
    c = 0;
    for(x=0;x<WIDTH;++x) {
      if( grid[y*WIDTH+x] > 0 ) c++;
    }
    if(c==WIDTH) {
      // row full!
      deleteRow(y);
    }
  }  
}


void tryToMovePieceSideways() {
  // what does the joystick angle say
  int dx = map(analogRead(JOYSTICK_X),0,1023,512*X_AXIS_POSITIVE,512*-X_AXIS_POSITIVE);
  
  int newPx = 0;
  // is the joystick really being pushed?
  if(dx> JOYSTICK_DEAD_ZONE) {
    newPx=1;
  }
  if(dx<-JOYSTICK_DEAD_ZONE) {
    newPx=-1;
  }

  if(newPx!=oldPx && pieceCanFit(pieceX+newPx,pieceY,pieceRotation)==1) {
    pieceX+=newPx;
  }
  oldPx = newPx;
}

int getJoystickX() {
  return map(analogRead(JOYSTICK_X),0,1023,512*X_AXIS_POSITIVE,512*-X_AXIS_POSITIVE);
}

int getJoystickY() {
  return map(analogRead(JOYSTICK_Y),0,1023,512*Y_AXIS_POSITIVE,512*-Y_AXIS_POSITIVE);
}

int getJoystickButton() {
  return digitalRead(JOYSTICK_BUTTON);
}

void tryToRotatePiece() {
  int iWantToTurn=0;
  
  // what does the joystick button say
  int newButton = getJoystickButton();
  // if the button state has just changed AND it is being let go,
  if( newButton > 0 && oldButton != newButton ) {
    iWantToTurn=1;
  }
  oldButton=newButton;
  
  // up on joystick to rotate
  int dy = getJoystickY();
  if(dy<-JOYSTICK_DEAD_ZONE) iWantToTurn=1;
  
  if(iWantToTurn==1 && iWantToTurn != old_iWantToTurn) {
    // figure out what it will look like at that new angle
    int newPieceRotation = ( pieceRotation + 1 ) % 4;
    // if it can fit at that new angle (doesn't bump anything)
    if(pieceCanFit(pieceX,pieceY,newPieceRotation)) {
      // then make the turn.
      pieceRotation = newPieceRotation;
    }
  }
  old_iWantToTurn = iWantToTurn;
}


// can the piece fit in this new location?
int pieceCanFit(int px,int py,int pr) {
  if( pieceOffEdge(px,py,pr) ) return 0;
  if( pieceHitsRubble(px,py,pr) ) return 0;
  return 1;
}


// is the piece out of limits?
int pieceOffEdge(int px,int py,int pr) {
  int x,y;
  const char *piece = pieces[pieceID] + (pr * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=px+x;
      int ny=py+y;
      if(ny<0) continue;  // off top, don't care
      if(piece[y*PIECE_W+x]>0) {
        if(ny>=HEIGHT) return 1;  // yes: goes off bottom of grid  
        if(nx<0) return 1;  // yes: off left side
        if(nx>=WIDTH ) return 1;  // yes: off right side
      }
    }
  }
  
  return 0;  // inside limits
}


// has a piece bumped into stuff left from older pieces?
int pieceHitsRubble(int px,int py,int pr) {
  int x,y;
  const char *piece = pieces[pieceID] + (pr * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {    
    int ny=py+y;
    if(ny<0) continue;
    for(x=0;x<PIECE_W;++x) {
      int nx=px+x;
      if(piece[y*PIECE_W+x]>0) {    
        if(grid[ny*WIDTH+nx]==1 ) return 1;  // yes: grid already full in this space
      }
    }
  }
  
  return 0;  // doesn't hit
}


void gameOver() {
  int x,y;

  while(1) {
    drawGridInverted();
    // click the button?
    if(getJoystickButton()==0) {
      // restart!
      setup();
      return;
    }
  }
}


void tryToDropPiece() {
  erasePieceFromGrid();  
  if(pieceCanFit(pieceX,pieceY+1,pieceRotation)) {
    pieceY++;  // move piece down
    addPieceToGrid();
  } else {
    // hit something!
    // put it back
    addPieceToGrid();
    removeFullRows();
    if(gameIsOver()==1) {
      gameOver();
    }
    // game isn't over, choose a new piece
    chooseNewPiece();
  }
}


void tryToDropFaster() {
  int y = getJoystickY();
  if(y>JOYSTICK_DEAD_ZONE) {
    // player is holding joystick down, drop a little faster.
    tryToDropPiece();
  }
}


void reactToPlayer() {
  erasePieceFromGrid();
  tryToMovePieceSideways();
  tryToRotatePiece();
  addPieceToGrid();
  tryToDropFaster();
}


// can the piece fit in this new location
int gameIsOver() {
  int x,y,nx,ny;
  const char *piece = pieces[pieceID] + (pieceRotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {      
      ny=pieceY+y;
      nx=pieceX+x;
      if(piece[y*PIECE_W+x]>0) {
        if(ny<0) return 1;  // yes: off the top!
      }
    }
  }
  
  return 0;  // not over yet...
}


// called once when arduino reboots
void setupTetris() {
  // set up joystick button
  pinMode(JOYSTICK_BUTTON,INPUT);
  digitalWrite(JOYSTICK_BUTTON,HIGH);
  
  // make sure arduino knows the grid is empty.
  int i;
  for(i=0;i<WIDTH*HEIGHT;++i) {
    grid[i]=0;
  }
  
  // make the game a bit more random - pull a number from space and use it to 'seed' a crop of random numbers.
  randomSeed(analogRead(JOYSTICK_X));
  
  // get ready to start the game.
  chooseNewPiece();
  
  // start the game clock after everything else is ready.
  lastMove = millis();
  lastDrop = lastMove;
}


void playTetris() {
  // the game plays at one speed...
  if(millis() - lastMove > moveDelay ) {
    lastMove = millis();
    reactToPlayer();
  }
  
  // ...and drops the falling block at a different speed.
  if(millis() - lastDrop > dropDelay ) {
    lastDrop = millis();
    tryToDropPiece();
  }
  
  // when it isn't doing those two things, it's redrawing the grid.
  drawGrid();
}


inline int isAnode(int a) {
  // this is slow: for(int i=0;i<NUM_ANODES;++i) if(anodesUnsorted[i]==a) return 1;  return 0;
  // this is fast:
  return anodesFlag[a];
}


inline int isCathode(int a) {
  // if it's not an anode, it must be a cathode.
  return anodesFlag[a]^1;
}


// turn on only the light at column x, row y
void lightXY(int x,int y) {
  // figure out which anode/cathode pair gives us light(x,y)
  int cathodeY = cathodes[y];
  int b = (y>=MAX_ANODES/2) ? MAX_ANODES/2:0;
  int anodeX = anodes[x+b];

  int i;
  for(i=0;i<MAX_PINS;++i) {
    if(i==anodeX) {
      // the anode we want
      digitalWrite(DATA,HIGH);
    } else if(isCathode(i) && i!=cathodeY) {
      // set low only the cathode we want
      digitalWrite(DATA,HIGH);
    }
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    digitalWrite(DATA,LOW);
  }
}


// set only one pin high
void setOneHigh(int n) {
  // loop through the rest
  int i;
  for(i=0;i<MAX_PINS;++i) {
    if(n==i) {
      digitalWrite(DATA,HIGH);
    }
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    digitalWrite(DATA,LOW);
  }
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
}


// shift all the data out of the registers.
// it would be faster to toggle the reset pin.
// this way is easier to build the circuit.
void setAll() {
  int i;
  for(i=0;i<MAX_PINS;++i) {
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
  }
}


// print the joystick position to serial.
void testJoystick() {
  Serial.print(getJoystickX()     ,DEC);  Serial.print('\t');
  Serial.print(getJoystickY()     ,DEC);  Serial.print('\t');
  Serial.print(getJoystickButton(),DEC);  Serial.print('\n');
}


void scrollingText() {
  // for animation
  long t=millis();
  int scroll_step=0;
  int x,y,nx,ny;

  do {
    for(y=0;y<HEIGHT;++y) {
      for(x=0;x<WIDTH;++x) {
          nx = MESSAGE_W * (WIDTH-1-x);
          ny = ( scroll_step + y ) % MESSAGE_W;
          // if it's a 1, turn on the light for a fraction of a second.
          // persistence of vision will create the illusion thath it's solid.
          if( message[ ny + nx ] == 1 ) {
            lightXY(x,y);
            digitalWrite(LATCH,HIGH);
            digitalWrite(LATCH,LOW);
          }
      }
    }

    // animation time - adjust scroll_step, which will push the message to the left. 
    if( millis() - t > SCROLL_DELAY) {
      t=millis();
      ++scroll_step;
    }
  } while(scroll_step < MESSAGE_W * 4);
}


// We have sorted anodes and cathodes, now we can light any pixel we want.  test it.
void lightOnePixelAtATime() {
  int x,y;
  for(y=0;y<HEIGHT;++y) {
    for(x=0;x<WIDTH;++x) {
      lightXY(x,y);
      digitalWrite(LATCH,HIGH);
      digitalWrite(LATCH,LOW);
      delay(WAIT2);
    }
  }
}


// Light all LED rows (horizontal)
// test that cathodes are sorted correctly.
void lightEachRowSorted() {
  int i;
  for(i=0;i<MAX_CATHODES;++i)  {
    Serial.println(cathodes[i],DEC);
    setOneRow(cathodes[i]);
    digitalWrite(LATCH,HIGH);
    digitalWrite(LATCH,LOW);
    delay(WAIT2);
  }
}


void setOneRow(int a) {
  int i;
  for(i=0;i<MAX_PINS;++i) {
    if(isAnode(i)) {
      // all anodes high
      digitalWrite(DATA,HIGH);
    } else if(i!=a) {
      // all cathodes high EXCEPT the row we want to light.
      // (setting cathode high blocks voltage passing through LED)
      digitalWrite(DATA,HIGH);
    }
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    digitalWrite(DATA,LOW);
  }
}


// light all LED rows (horizontal)
// turn ON all the anodes and turn OFF only one cathode for the row we want.
// use this to figure out the sorting of the cathodes.
void lightEachRowUnsorted() {
  int i;
  for(i=0;i<MAX_CATHODES;++i)  {
    Serial.println(cathodesUnsorted[i],DEC);
    setOneRow(cathodesUnsorted[i]);
    digitalWrite(LATCH,HIGH);
    digitalWrite(LATCH,LOW);
    delay(3000);
  }
}


// light all LEDs
void lightEverything() {
  int i;
  for(i=0;i<MAX_PINS;++i) {
    int j;
    for(j=0;j<MAX_ANODES;++j) {
      if(anodes[j]==i) break;
    }
    if(j<MAX_ANODES) {
      digitalWrite(DATA,HIGH);
    }
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    digitalWrite(DATA,LOW);
  }
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
  delay(500);
}


void setTwoHigh(int a,int b) {
  // set two pins high
  // loop through the rest
  int i;
  for(i=0;i<MAX_PINS;++i) {
    if(a==i || b==i) {
      digitalWrite(DATA,HIGH);
    }
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    digitalWrite(DATA,LOW);
  }
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
}


// light entire column (vertical)
void lightEachColumn() {
  int i;
  for(i=0;i<MAX_ANODES/2;++i) {
    setTwoHigh(anodes[i],anodes[i+MAX_ANODES/2]);
    delay(WAIT2);
  }
}


// now check that we've found only anodes and the correct order (right to left,bottom then top)
void lightAnodesSorted() {
  int i;
  for(i=0;i<MAX_ANODES;++i) {
    Serial.println(anodes[i]);
    setOneHigh(anodes[i]);
    delay(500);
  }
}


// shift one high value through all the registers, using a method.
// only columns will light up.  use this to find the anodes.
void findAnodes() {
  int i;
  for(i=0;i<MAX_PINS;++i) {
    Serial.println(i,DEC);
    setOneHigh(i);
    delay(4000);
  }
}


// set one pin high
// shift that one high value through all the registers.
void loop2() {
  digitalWrite(DATA,HIGH);
  digitalWrite(CLK,HIGH);
  digitalWrite(CLK,LOW);
  digitalWrite(DATA,LOW);
  int i;
  for(i=0;i<MAX_PINS;++i) {
    Serial.println(i,DEC);
    digitalWrite(LATCH,HIGH);
    digitalWrite(LATCH,LOW);
    digitalWrite(CLK,HIGH);
    digitalWrite(CLK,LOW);
    delay(WAIT);
  }
}


// high on all pins, wait, low on all pins, wait, repeat
void loop1() {
  digitalWrite(LED,HIGH);
  digitalWrite(DATA,HIGH);
  setAll();
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
  delay(WAIT2);

  digitalWrite(LED,LOW);
  digitalWrite(DATA,LOW);
  setAll();
  digitalWrite(LATCH,HIGH);
  digitalWrite(LATCH,LOW);
  delay(WAIT2);
}
