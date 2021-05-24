#define red_lcd 0xF800

bool playing = true; //Variable to unfreeze board when user starts a game. //TESTING ONLY //WILL BE CHANGED to false.
bool board_black = false; //change board perspective if player is the black pieces.

String first_selected = "none"; //first square selected by user's touch
String second_selected = "none"; //second square selected by user's touch

int second_row = 0; //store second square's row clicked by user
int second_col = 0; //store second square's column clicked by user
int first_row = 0;  //store first square's row clicked by user
int first_col = 0;  //store first square's column clicked by user

char board[8][8]= {  //store last shown position to redraw pieces when needed.
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} 
                  };

String numToSan(int n, int m, bool black_pieces)//turns a coordinate to chessmove notation. EG. [1][4] -> 'a4'
{                                               //boolean black_pieces reverse the board's perspective

    if (black_pieces){ 
      n = 9-n;
    }else{
      m = 9-m;
    }
    
    if (!(n >= 1 && n <= 8 && m <= 8 && m >= 1)){ //check if coordinates are in range
      String h;
      return h;
    }

    char tmp = (char)('a' + n - 1);
    
    String row;
    String col;
    row = m;
    col = tmp;
    
    String san = col + row;
    
    return san;
}

void showfen(char * fen,bool black_pieces){ //show fen string position to chessboard.

  board_black = black_pieces;
  int col = 0; //these will increment or decrement (depending on board perspective) and swipe through the whole board while reading fen.
  int row = 8;

  if (black_pieces){
    row = 8;
    col = 8;
  }else{
    row = 1;
    col = 1;
  }
  
  int offsetx = 45 - 56; //set board's offset from coordinates 0,0
  int offsety = 16 - 56; //set board's offset from coordinates 0,0
  int square = 0;

 

  for(int i = 0; fen[i] != '\0'; i++) { //loop through fen string
      if (square >= 64){//stop after processing 64 squares.
       break; 
      }
      String filename; //will be constructed into an image filename for the correct piece.
      filename.concat("/p/"); //pieces' folder
      char entry = fen[i];
      String entryupper;
      entryupper = fen[i];
      if (entry == '-' || entry == ' '){ //skip if special characters
        continue;
      }
      if (entry == '/'){//if slash, it means skip to next row.
          
          if (black_pieces){
            col = 8;
            row -= 1;
          }else{
            col = 1;
            row += 1;
          }
          continue;
      }else{
        if (isDigit(entry)){ //if there are numbers, it means empty squares.
            int toskip = entry - '0'; //convert from char to int. It will be the number of empty squares

            Serial.printf("[*] Blanking for %d times \n",toskip); 
            
            
            for (int j = 1; j <= toskip; j++){ //loop through all empty squares
              square += 1; //increment number of squares processed for each
              board[col-1][row-1] = ' '; //set them as empty strings on chessboard's 2D array

              if ((col + row) % 2 == 0) { //check if it should be a darksquare or lightsquare to draw.
                tft.fillRect(45 + 56 * (col - 1), 16 + 56 * (row - 1), 56, 56, chessboard_white);
              } else {
                tft.fillRect(45 + 56 * (col - 1), 16 + 56 * (row - 1), 56, 56, chessboard_black);
              }
              if (black_pieces){ //increment of decrement column depending on board's perspective
                col -= 1;
              }else{
                col += 1;
              }
            }
            
            
        }else{ //if it is a letter, then it is a piece present on the board. EG. 'Q' -> White Queen. 'q' -> Black Queen. B -> White Bishop. etc...

          if ((col + row) % 2 == 0) { //check if piece is sitting on lightsquare or darksquare
              filename.concat('l');
          } else {
              filename.concat('d');
          }
          entryupper.toLowerCase(); //copy a version of letter as a lowercase
          filename += entryupper;
          
          if (isupper(entry)) { //if letter was uppercase, then piece's color is white. Otherwise, black.
              filename.concat('w');
          } else {
              filename.concat('b');
          }
          filename.concat(".bmp"); //add file extension at the end of string

          if (board[col-1][row-1] != entry){ //if the same piece is not already present there on the board's 2D array, then we must draw it
            bmpDrawPath(const_cast < char * > (filename.c_str()), 45 + 56 * (col - 1), 16 + 56 * (row - 1));
            board[col-1][row-1] = entry; //save the piece's name and location into the board's 2D array for later.
          }
          
          //tft.fillRect(45+56*(col-1), 16+56*(row-1), 56, 56, darkgrey);

          square += 1; //increment the number of square we processed to reach 64.
          if (black_pieces){
             col -= 1;
          }else{
             col += 1;
          }
          
        }
      }
  
  }
}

void restore_block(int col, int row) { //redraw a piece with a given square using pieces saved in the board's 2D array. Copied from `showfen` function. 
    String filename;
    filename.concat("/p/");
    char entry = board[col - 1][row - 1];
    String entryupper;
    entryupper = board[col - 1][row - 1];

    if (entry == ' ') {
        if ((col + row) % 2 == 0) {
            tft.fillRect(45 + 56 * (col - 1), 16 + 56 * (row - 1), 56, 56, chessboard_white);
        } else {
            tft.fillRect(45 + 56 * (col - 1), 16 + 56 * (row - 1), 56, 56, chessboard_black);
        }
    } else {
        if ((col + row) % 2 == 0) {
            filename.concat('l');
        } else {
            filename.concat('d');
        }
        entryupper.toLowerCase();
        filename += entryupper;
        if (isupper(entry)) {
            filename.concat('w');
        } else {
            filename.concat('b');
        }
        filename.concat(".bmp");

        bmpDrawPath(const_cast < char * > (filename.c_str()), 45 + 56 * (col - 1), 16 + 56 * (row - 1));
    }
}


void highlight_square(int col,int row) { //highlight a clicked square

    tft.drawRect(45+56*(col-1), 16+56*(row-1), 55, 55, red_lcd);
    tft.drawRect(45+56*(col-1), 16+56*(row-1), 54, 54, red_lcd);
}

void handle_board_touch(int tx, int ty) { //handle touch events on the `playing` screen
  Serial.println("Handle_board_touch called");
  if (playing){ //if playing a game, then unfreeze board.
    int col = ((tx - 45)/56) + 1; //turn x,y coordinate to board's coordinates
    int row = ((ty - 16)/56) + 1;
    //tft.drawPixel(tx, ty, red_lcd);
    String square = numToSan(col,row,board_black); //get chess square notation given square coordinates
    Serial.printf("checking piece at [%d][%d] \n\n",col-1,row-1);
    Serial.printf("clicked on %s %c \n",square,board[col-1][row-1]);


    if (square == ""){ //if empty, clear selected squares, eg. remove hightlight
      first_selected = "none";
      second_selected = "none";
      if (first_col != 0 && first_row != 0){
        restore_block(first_col,first_row);//erase highlight from past click 
      }
      if (second_col != 0 && second_row != 0){
        restore_block(second_col,second_row);//erase highlight from past click 
      }
    }else if (first_selected == "none"){
      first_selected = square;

      if (first_col != 0 && first_row != 0){
        restore_block(first_col,first_row);//erase highlight from past click 
      }
      if (second_col != 0 && second_row != 0){
        restore_block(second_col,second_row);//erase highlight from past click 
      }
      first_row = row;
      first_col = col;

      highlight_square(col,row); //hightlight square
    
      tft.fillRect(600, 200, 150, 50, background_color); //erase past move
    
      tft.changeMode(TEXT);
      tft.setTextColor(RA8875_WHITE);
      tft.setFontScale(2);
      tft.setCursor(600, 200);
      tft.print(first_selected);
      tft.changeMode(GRAPHIC);

      
    } else if (square == first_selected || square == second_selected){
      
    } else if (second_selected == "none" ){
      second_selected = square;
      
      second_row = row;
      second_col = col;

      highlight_square(col,row);
        
      tft.changeMode(TEXT);
      tft.setTextColor(RA8875_WHITE);
      tft.setFontScale(2);
      tft.setCursor(650, 200);
      tft.print(second_selected);
      tft.changeMode(GRAPHIC);
      
    } else {
      first_selected = "none";
      second_selected = "none";
      first_selected = square;
      
      if (first_col != 0 && first_row != 0){
        //flush them 
        restore_block(first_col,first_row);
        //tft.fillRect(45+56*(first_col-1), 16+56*(first_row-1), 56, 56, background_color); //erase past move 
      }
      if (second_col != 0 && second_row != 0){
        //flush them 
        restore_block(second_col,second_row);
        //tft.fillRect(45+56*(second_col-1), 16+56*(second_row-1), 56, 56, background_color); //erase past move 
      }
      first_row = row;
      first_col = col;

      highlight_square(col,row);

      tft.fillRect(600, 200, 150, 50, background_color); //erase past move
    
      tft.changeMode(TEXT);
      tft.setFontScale(2);
      tft.setTextColor(RA8875_WHITE);
      tft.setCursor(600, 200);
      tft.print(first_selected);
      tft.changeMode(GRAPHIC);
    }
  }//otherwise don't do anything.

}

void chessboard() { //draw the chessboard
  screen = 1;
  tft.fillRect(0, 0, 800, 500, background_color); //bg
  tft.fillRect(0, 0, 40, 500, darkgrey); //left sidebar 
  tft.fillCircle(tft.width() - 10, 10, 5, RA8875_GREEN); //top right signal status dot.

  int squaresize = (480-32)/8;
  int startx = 45;
  int starty = 16;

  for (int c_run = 0; c_run < 8; ++c_run){
    startx = 45;
    for (int c_rise = 0; c_rise < 8; ++c_rise){
      if ((c_rise + c_run) % 2 == 0){
        tft.fillRect(startx, starty, squaresize, squaresize, chessboard_white);
      }else{
        tft.fillRect(startx, starty, squaresize, squaresize, chessboard_black);
      }
      startx += squaresize;
    }
    starty += squaresize;
  }
 
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(640, 0);
  tft.print("Home - Chess.com");
  
  tft.changeMode(GRAPHIC);
  showfen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",false);

  showfen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",false); //TEST.
  
  tft.fillRect(550, 300, 60, 60, ligthgrey);
  tft.fillRect(690, 300, 60, 60, ligthgrey);
}
