#include "ChessDotComLib.h"

ChessCom instance;

void benchmark() {
    int counter = 0;
    while (1) {
        counter += 1;
        Serial.println(counter);
        Serial.println(httpGETRequest("http://10.100.0.73:5000/api/fen?key=D7Ip3ROLKKcMz8M&gameid=14040703649"));
        delay(0);
    }
}

String input(String msg, CreateGame instance) {
    Serial.println(msg);
    bool typed = false;
    String response;
    while (!typed) {
        if (instance.gameover_store) {
            typed = true;
            return "";
        }
        if (Serial.available() > 0) {
            String incomingByte = Serial.readString();
            response = incomingByte;
            response.trim();
            typed = true;
            return response;
        }
    }

}

int ChessComLogin() {
    //instance.createSession("test_api","bastienRES13");
    //instance.getSession("test_api","WJ2cpTpmpIGOODV");
    instance.createSession("Noobist_007", "bastienRES13"); //fake account btw
    Serial.print("Successfully Logged in as: ");
    Serial.println(instance.username);
    Serial.print("Session_ID: ");
    Serial.println(instance.session);

    return 0;
}

int ChessComCreateGame() {
    CreateGame ChessGame = CreateGame("10-0", instance);

    ChessGame.startsync();

    Serial.print("Successfully Created Game [");
    Serial.print(ChessGame.gameid);
    Serial.print("] as: ");
    Serial.println(instance.username);
    Serial.print("The fen is: ");
    Serial.println(ChessGame.fen_store);
    delay(1000);
    if (!ChessGame.gameover_store) {
        Serial.print("FEN: ");
        Serial.println(ChessGame.fen_store);
        Serial.print("MyTurn: ");
        Serial.println(ChessGame.myturn_store);
        Serial.print("Gameover: ");
        Serial.println(ChessGame.gameover_store);
        Serial.println("");

        delay(1000);
        while (!ChessGame.gameover_store) {
            delay(1000);
            if (ChessGame.myturn_store) {
                String startsquare = input("Enter Start Square:", ChessGame);
                String endsquare = input("Enter End Square:", ChessGame);
                if (!ChessGame.gameover_store) {
                    if (ChessGame.make_move(startsquare, endsquare)) {
                        Serial.print("Successfully played ");
                        Serial.print(startsquare);
                        Serial.println(endsquare);
                    } else {
                        Serial.print("Error playing: ");
                        Serial.print(startsquare);
                        Serial.println(endsquare);
                    }
                }
            }

        }
    }

    ChessGame.stopsync();

    Serial.println("Gameover: TRUE");
    return 0;
}
