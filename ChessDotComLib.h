#include <HTTPClient.h>

#include <Arduino_JSON.h>

#include "api_lib.h"

class ChessCom { //main class. Connects to a chess.com account.
  public:
    String session;
    String username;
    bool loggedin = false;
    void createSession(String username_id, String password);
    void getSession(String username_id, String session_key);
};

void ChessCom::createSession(String username_id, String password) { //function which connects user + creates a session_key from username/pwd

  String request;
  String serverPath = create_session_api;
  bool inited;
  String credidentials = "/store/chesscom/" + username_id;
  if (!SPIFFS.exists(credidentials)) {
    inited = false;
  }

  if (!inited) {
    serverPath += "?username=";
    serverPath += username_id;
    serverPath += "&password=";
    serverPath += password;

    const char * serverPathCA = serverPath.c_str();
    request = httpGETRequest(serverPathCA);
    //Serial.println(request);
    JSONVar myObject = JSON.parse(request);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
    }

    Serial.print("\nObject = ");
    Serial.println(myObject);

    // myObject.keys() can be used to get an array of all the keys in the object
    JSONVar keys = myObject.keys();
    for (int i = 0; i < keys.length(); i++) {
      JSONVar value = myObject[keys[i]];
      String key = (const char * ) keys[i];
      if (key == "key") {
        session = (const char * ) value;
      }
      //Serial.print(keys[i]);
      //Serial.print(" = ");
      //Serial.println(value);
      File file = SPIFFS.open(credidentials, "w");
      if (!file) {
        // File not found
        Serial.println("Failed to open creds file");
        return;
      } else {
        file.print(session);
        file.close();
      }
    }
  } else {
    File cred_file = SPIFFS.open(credidentials);
    while (cred_file.available()) {
      session = cred_file.readString();
    }
  }
  username = username_id;
  loggedin = true;
}

void ChessCom::getSession(String username_id, String session_key) { //connects using session_key.
  session = session_key;
  username = username_id;
  loggedin = true;
}

class CreateGame { //class to create a game.
  public:
    TaskHandle_t GameSyncTask; //sync game status

    void startsync() { //start sync job in new core
      xTaskCreatePinnedToCore(this -> sync, "GameSyncTask", 5000, this, 0, &GameSyncTask, 0);
    }

    void stopsync() { //stop syncjob
      vTaskDelete(GameSyncTask);
    }

    static void sync(void * parameter) { //background syncing job
      CreateGame * ChessGame = (CreateGame * ) parameter;
      while (1) {
        ChessGame -> fen();
        ChessGame -> gameover();
        ChessGame -> myturn();
        delay(70);
      }

    }

    String fen_store; //where the retrieved fen is stored
    void fen(); //retrieve fen + saves it

    bool gameover_store; //gameover status
    void gameover(); //retrieve gameover status + saves it

    bool myturn_store; //boolean, which tells if it is player's turn
    void myturn(); //retrieves that value and saves it.

    bool make_move(String startsquare, String endsquare); //functino to make a move from one square to another one.

    String gameid; //initialized game's ID for reference.
    String color; //color of the pieces the player is player: white or black
    String session_key;  //User's session_key
    
    ChessCom userinstance; //Instance of player's account.
    
    CreateGame(String timing, ChessCom instance) { //function to create game
      Serial.print("Creating a game with username: ");
      Serial.println(instance.username);

      session_key = instance.session;
      String request;
      String serverPath = create_game_api;
      serverPath += "?key=";
      serverPath += instance.session;
      serverPath += "&username=0&time=";
      serverPath += timing;
      request = httpGETRequest(serverPath.c_str());
      //Serial.println(request);
      JSONVar myObject = JSON.parse(request);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
      }

      Serial.print("\nObject = ");
      Serial.println(myObject);

      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = myObject[keys[i]];
        String key = (const char * ) keys[i];
        if (key == "gameid") {
          String tmp = (const char * ) value;
          gameid = tmp;
        }
        if (key == "color") {
          color = (const char * ) value;
        }
        //Serial.print(keys[i]);
        //Serial.print(" = ");
        //Serial.println(value);
      }

      //gameid = 2757234;
      if (color == "white") {
        myturn_store == true;
      } else {
        myturn_store == false;
      }
      fen_store = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
      gameover();
    }
};

void CreateGame::gameover() { //check if gameover
  String request;
  String serverPath = is_gameover_api;

  serverPath += "?key=";
  serverPath += session_key;
  serverPath += "&gameid=";
  serverPath += gameid;
  const char * serverPathCA = serverPath.c_str();
  request = httpGETRequest(serverPathCA);
  bool result = false;

  if (request == "true") {
    result = true;
  }
  gameover_store = result;
}

void CreateGame::myturn() { //check turns
  String request;
  String serverPath = is_myturn_api;
  serverPath += "?key=";
  serverPath += session_key;
  serverPath += "&gameid=";
  serverPath += gameid;

  delay(1000);

  const char * serverPathCA = serverPath.c_str();
  request = httpGETRequest(serverPathCA);
  bool result = false;

  if (request == "true") {
    result = true;
  }
  myturn_store = result;
}

bool CreateGame::make_move(String startsquare, String endsquare) { //make a move
  if (gameover_store == true) {
    return false;
  }
  String request;
  String serverPath = make_move_api;
  serverPath += "?key=";
  serverPath += session_key;
  serverPath += "&gameid=";
  serverPath += gameid;
  serverPath += "&color=";
  serverPath += color;
  serverPath += "&start=";
  serverPath += startsquare;
  serverPath += "&end=";
  serverPath += endsquare;
  const char * serverPathCA = serverPath.c_str();
  request = httpGETRequest(serverPathCA);
  bool result = false;

  JSONVar myObject = JSON.parse(request);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return false;
  }

  // myObject.keys() can be used to get an array of all the keys in the object
  JSONVar keys = myObject.keys();
  for (int i = 0; i < keys.length(); i++) {
    JSONVar value = myObject[keys[i]];
    String key = (const char * ) keys[i];
    if (key == "result") {
      String tmp = (const char * ) value;
      if (tmp == "false") {
        return false;
      }
      if (tmp == "true") {
        return true;
      }
    }
    //Serial.print(keys[i]);
    //Serial.print(" = ");
    //Serial.println(value);
  }
}

void CreateGame::fen() {//check fen
  String request;
  String serverPath = get_fen_api;
  serverPath.concat("?key=");
  serverPath.concat(session_key);
  serverPath.concat("&gameid=");
  serverPath.concat(gameid);

  const char * serverPathCA = serverPath.c_str();
  request = httpGETRequest(serverPathCA);
  //Serial.println(request);
  JSONVar myObject = JSON.parse(request);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    fen_store = "";
    return;
  }

  // myObject.keys() can be used to get an array of all the keys in the object
  JSONVar keys = myObject.keys();
  for (int i = 0; i < keys.length(); i++) {
    JSONVar value = myObject[keys[i]];
    String key = (const char * ) keys[i];
    if (key == "fen") {
      fen_store = (const char * ) value;
      return;
    }
    //Serial.print(keys[i]);
    //Serial.print(" = ");
    //Serial.println(value);
  }
  fen_store = "";

}

//ChessCom instance();
