#include <HTTPClient.h>

#include <Arduino_JSON.h>


String httpGETRequest(const char * serverName) {
    HTTPClient http;
    http.setReuse(true);
    http.setTimeout(99000);
    http.setConnectTimeout(99000);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.begin(serverName);

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode == HTTP_CODE_OK) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        payload = http.getString();

        Serial.print("Error code: ");
        Serial.print(http.getString());
        Serial.print(" ");
        Serial.print(httpResponseCode);
        Serial.print(" - ");
        Serial.print(http.errorToString(httpResponseCode).c_str());
    }
    // Free resources
    http.end();

    return payload;
}

String session_key = "Saj2UmOZRdwtJvU";
String ip = "10.0.4.83";
String create_game_api = "http://10.0.4.83:5000/api/create_game"; //?key={session}&time=10-0&username=test_api
String get_fen_api = "http://10.0.4.83:5000/api/fen"; //?key={session}&gameid={gameid}
String is_gameover_api = "http://10.0.4.83:5000/api/gameover"; //?key={session}&gameid={gameid}
String is_myturn_api = "http://10.0.4.83:5000/api/myturn"; //?key={session}&gameid={gameid}
String make_move_api = "http://10.0.4.83:5000/api/makemove"; //?key={session}&gameid={gameid}&color={color}&start={start}&end={end}
String create_session_api = "http://10.0.4.83:5000/api/create_session";

class ChessCom {
    public:
        String session;
    String username;
    bool loggedin = false;
    void createSession(String username_id, String password);
    void getSession(String username_id, String session_key);
};

void ChessCom::createSession(String username_id, String password) {

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

void ChessCom::getSession(String username_id, String session_key) {
    session = session_key;
    username = username_id;
    loggedin = true;
}

class CreateGame {
    public:
        TaskHandle_t GameSyncTask;

    void startsync() {
        xTaskCreatePinnedToCore(this -> sync, "GameSyncTask", 5000, this, 0, &GameSyncTask, 0);
    }

    void stopsync() {
        vTaskDelete(GameSyncTask);
    }

    static void sync(void * parameter) {
        CreateGame * ChessGame = (CreateGame * ) parameter;
        while (1) {
            ChessGame -> fen();
            ChessGame -> gameover();
            ChessGame -> myturn();
            delay(70);
        }

    }

    String fen_store;
    void fen();

    bool gameover_store;
    void gameover();

    bool myturn_store;
    void myturn();

    bool make_move(String startsquare, String endsquare);

    String gameid;
    String color;
    String session_key;
    ChessCom userinstance;
    CreateGame(String timing, ChessCom instance) {
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

void CreateGame::gameover() {
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

void CreateGame::myturn() {
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

bool CreateGame::make_move(String startsquare, String endsquare) { //?key={session}&gameid={gameid}&color={color}&start={start}&end={end}
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

void CreateGame::fen() {
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

//instance.createSession("Bastien_tmio","pasword123");
